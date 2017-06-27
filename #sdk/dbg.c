// # debug ####################################################################

#include <assert.h>
static
bool optimized() {
#if defined(NDEBUG) || defined(_NDEBUG)
    return 1;
#else
    return 0;
#endif
}
bool asserting() { $
    int asserting = 0;
    assert( asserting |= 1 );
    return !!asserting;
}
bool debugging() { $
#ifdef _WIN32
    return IsDebuggerPresent() ? 1 : 0;
#else
    FILE *fd = fopen("/tmp", "r");
    int rc = fileno(fd) > 5;
    fclose(fd);
    return !!rc;
#endif
}
void breakpoint() { $
#if _MSC_VER
    __debugbreak();    // msvc
#elif __GNUC__
    __builtin_trap();  // gcc and clang
#else
    raise(SIGTRAP);    // posix
#endif
}
char *hexdump( const void *ptr, unsigned len ) { $
    int width = 16;
    int maxlen = ((len/width)+1) * (8 + (width * 3) + 4);
    char *out = (char *)REALLOC( 0, maxlen ), *stream = out;
    unsigned char *data = (unsigned char*)ptr;
    for( unsigned jt = 0; jt < len; jt += width ) {
        stream += sprintf( stream, "; %05d ", jt );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            stream += sprintf( stream, "%02x %s", (unsigned char)data[it], &" \n\0...\n"[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
        stream += sprintf( stream, "; %05d ", jt );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            stream += sprintf( stream, " %c %s", (signed char)data[it] >= 32 ? (signed char)data[it] : (signed char)'.', &" \n\0...\n"[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
    }
    assert( (stream - out) < maxlen );
    return out;
}
// ---
// Windows users add `/Zi` compilation flags, else add `-g` and/or `-ldl` flags
// If you are linking your binary using GNU ld you need to add --export-dynamic
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#  if defined(__APPLE__)
#include <execinfo.h> // backtrace, backtrace_symbols
#include <dlfcn.h>    // dladdr, Dl_info
#elif defined(_WIN32)
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Kernel32.lib")
static int backtrace( void **addr, int maxtraces ) { //$
    static HANDLE process = 0;
    if( !process ) process = GetCurrentProcess();
    if( !process ) exit( tty( "error: no current process" ) );
    static int init = 0;
    if( !init ) init = SymSetOptions(SYMOPT_UNDNAME), SymInitialize( process, NULL, TRUE );
    if( !init ) exit( tty( "error: cannot initialize DbgHelp.lib" ) );

    typedef USHORT (WINAPI *pFN)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
    static pFN rtlCaptureStackBackTrace = 0;
    if( !rtlCaptureStackBackTrace ) {
        rtlCaptureStackBackTrace = (pFN)GetProcAddress(LoadLibraryA("kernel32.dll"), "RtlCaptureStackBackTrace");
    }
    if( !rtlCaptureStackBackTrace ) {
        return 0;
    }
    return rtlCaptureStackBackTrace(1, maxtraces, (PVOID *)addr, (DWORD *) 0);
}
static char **backtrace_symbols(void *const *array,int size) { //$
    HANDLE process = GetCurrentProcess();
    enum { MAXSYMBOLNAME = 512 - sizeof(IMAGEHLP_SYMBOL64) };
    char symbol64_buf     [ 512 ];
    char symbol64_bufblank[ 512 ] = {0};
    IMAGEHLP_SYMBOL64 *symbol64       = (IMAGEHLP_SYMBOL64*)symbol64_buf;
    IMAGEHLP_SYMBOL64 *symbol64_blank = (IMAGEHLP_SYMBOL64*)symbol64_bufblank;
    symbol64_blank->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    symbol64_blank->MaxNameLength = (MAXSYMBOLNAME-1) / 2; //wchar?

    int symlen = size * sizeof(char *);
    char **symbols = (char **)MALLOC(symlen);

    if( symbols ) {
        for( int i = 0; i < size; ++i ) {
            symbols[ i ] = NULL;
        }

        char begin[1024];
        for( int i = 0; i < size; ++i ) {
            char **symbuf, *buffer = begin;

            DWORD64 dw1 = 0, dw2 = 0;
            *symbol64 = *symbol64_blank;
            if( SymGetSymFromAddr64( process, (DWORD64)array[i], &dw1, symbol64 ) ) {
                IMAGEHLP_LINE64 L64 = {0};
                L64.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                if( SymGetLineFromAddr64( process, (DWORD64)array[i], (DWORD*)&dw2, &L64 ) ) {
                    buffer += sprintf(buffer,"%s (%s:%d)%c", symbol64->Name, L64.FileName, L64.LineNumber, 0);
                } else {
                    buffer += sprintf(buffer,"%s (??)%c", symbol64->Name, 0);
                }
            } else  buffer += sprintf(buffer,"(??)%c", 0);

            size_t buflen = buffer - begin + 1;

            symbuf = (char **)REALLOC( symbols, symlen + buflen );
            if( symbuf ) {
                memcpy( (char *)symbuf + symlen, begin, buflen );
                symbuf[ i ] = (char *)(size_t)symlen;

                symbols = symbuf;
                symlen += buflen;
            } else break;
        }

        for( int i = 0; i < size; ++i ) {
            symbols[ i ] = (char *)symbols + (size_t)symbols[ i ];
        }
    }

    return symbols;
}
#elif defined(__GNUC__)
# include <execinfo.h>  // backtrace, backtrace_symbols
#else
static int backtrace(void **, int) { $ return 0; }
static char **backtrace_symbols(void *const *,int) { $ return 0; }
#endif

void callstack( int traces, errorcode (*yield)( const char *entry ) ) { //$
    enum { skip = 1 }; /* exclude 1 trace from stack (this function) */
    enum { maxtraces = 128 };

    int inc = 1;
    if( traces < 0 ) traces = -traces, inc = -1;
    if( traces == 0 ) return;
    if( traces > maxtraces ) traces = maxtraces;

    void *stack[ maxtraces ];
    traces = backtrace( stack, traces );
    char **symbols = backtrace_symbols( stack, traces );

    char demangled[1024], buf[1024];
    int L = 0, B = inc>0 ? skip - 1 : traces, E = inc>0 ? traces : skip - 1;
    for( int i = B; ( i += inc ) != E; ) {
#ifdef linux
        char *address = strstr( symbols[i], "[" ) + 1; address[strlen(address) - 1] = '\0';
        char *binary = symbols[i]; strstr( symbols[i], "(" )[0] = '\0';
        char command[1024]; sprintf(command, "addr2line -e %s %s", binary, address);
        FILE *fp = popen( command, "rb" );
        if( fp ) {
            char *line_p = fgets(demangled, sizeof(demangled), fp);
            symbols[i] = demangled;
            pclose(fp);
        } else {
            tty( "cannot exec:" );
            tty( command );
        }
#elif defined(__APPLE__)
        struct Dl_info info;
        if( dladdr(stack[i], &info) && info.dli_sname ) {
            char *dmgbuf = info.dli_sname[0] != '_' ? NULL :
                 __cxa_demangle(info.dli_sname, NULL, 0, NULL);
            strcpy( demangled, dmgbuf ? dmgbuf : info.dli_sname );
            symbols[i] = demangled;
            FREE( dmgbuf );
        }
#endif
        sprintf(buf, "%3d %#016p %s", ++L, stack[i], symbols[i]);
        if( yield(buf) < 0 ) break;
    }

    FREE( symbols );
}

// ---
//
#if _MSC_VER && defined __cplusplus
#   include <windows.h>
#   include <eh.h> // _set_se_translator
static void _cdecl se_translator( unsigned int ex, EXCEPTION_POINTERS *p ) { $
    switch( ex ) {
        default: //panic( "Unknown exception" );
        break; case EXCEPTION_ACCESS_VIOLATION: //panic( "Memory access violation" );
        break; case EXCEPTION_ILLEGAL_INSTRUCTION: //panic( "Illegal instruction" );
        break; case EXCEPTION_INT_DIVIDE_BY_ZERO: //panic( "Integer divide by zero" );
        break; case EXCEPTION_STACK_OVERFLOW: //panic( "Stack overflow" );
        {}
    };
    panic("se_translator(): exception caught");
}
#endif
#include <signal.h>
static void catch(int signum) { $
    signal(signum, catch); // reset for next signal
    panic("!catch found");
}
#ifndef _WIN32
#include <sys/resource.h> // setrlimit()
#endif
void trap() { $ // trapc(), trapcpp()
    // install signal handlers
    const int signals[] = { // The C standard defines following 6 signals:
        SIGABRT,      // abort: abnormal termination.
        SIGFPE,       // floating point exception.
        SIGILL,       // illegal: invalid instruction.
#if !SHIPPING
        SIGINT,       // interrupt: interactive attention request sent to the program.
#endif
        SIGSEGV,      // segmentation violation: invalid memory access.
        SIGTERM,      // terminate: termination request sent to the program.
    #ifdef __APPLE__
        SIGPIPE,
    #endif
    #ifdef _WIN32
        SIGBREAK,
    #else
        SIGBUS,       // Bus error
        SIGHUP,       // sent to a process when its controlling terminal is closed
        //SIGQUIT,    //
        //SIGKILL,    // kernel will let go of the process without informing the process of it
        //SIGTRAP,    // debugger
        //SIGSYS,
    #endif
    };
    for( int i = 0; i < sizeof(signals)/sizeof(signals[0]); ++i ) {
        signal(signals[i], SIG_IGN); //catch);
    }
#ifdef __cplusplus
    // C++11
    set_terminate(catch);
#if _MSC_VER
    _set_se_translator( se_translator );
#endif
#endif
#ifndef _WIN32
    // enable coredumps
    rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
    setrlimit( RLIMIT_CORE, &core_limit );
#endif
}
void die() { $
    exit(-1);
}
void crash() { $
    catch(42);
}

