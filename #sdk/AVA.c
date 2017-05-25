#  if !defined(API) && defined(__cplusplus)
#   define API extern "C" __declspec(dllexport)
#elif !defined(API)
#   define API            __declspec(dllexport)
#endif

#include "AVA.h"

#include <stdio.h>  // puts, size_t
#include <stdlib.h> // realloc, getenv, vsnprintf

#if defined(_WIN32)
#include <winsock2.h>
#define $win(...) __VA_ARGS__
#else
#define $win(...)
#endif

#if defined(linux)
#define $lin(...) __VA_ARGS__
#else
#define $lin(...)
#endif

#if defined(__APPLE__)
#define $osx(...) __VA_ARGS__
#else
#define $osx(...)
#endif

#if defined(SHIPPING)
#define $shipping(...)  __VA_ARGS__
#else
#define $shipping(...)
#endif

#if defined(NDEBUG) || defined(_NDEBUG)
#define $release(...)  __VA_ARGS__
#define $debug(...)
#else
#define $debug(...)    __VA_ARGS__
#define $release(...)
#endif

#ifdef _MSC_VER
#define $msc(...)   __VA_ARGS__
#define $clang(...)
#define $gcc(...)
#elif defined(__clang__)
#define $msc(...)
#define $clang(...) __VA_ARGS__
#define $gcc(...) 
#elif defined(__GNUC__)
#define $msc(...)
#define $clang(...)
#define $gcc(...)   __VA_ARGS__
#else
#error
#endif

// # thread local (@todo: move to builtins)

#ifdef __GNUC__
#   define THREAD_LOCAL __thread
#elif __STDC_VERSION__ >= 201112L
#   define THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER) | defined(__INTEL_COMPILER)
#   define THREAD_LOCAL __declspec(thread)
#else
#   error Cannot define thread_local
#endif

// # endian

#if defined(__sgi)      ||  defined (__sparc)        || \
    defined (__sparc__) ||  defined (__PPC__)        || \
    defined (__ppc__)   ||  defined (__BIG_ENDIAN__)
    enum { is_little = 0 };
#else
    enum { is_little = 1 };
#endif

// # arch

#if defined(__x86_64__)     || defined(_M_X64)      || \
    defined(__powerpc64__)  || defined(__alpha__)   || \
    defined(__ia64__)       || defined(__s390__)    || \
    defined(__s390x__)
    enum { is_64bit = 1 };
    typedef char static_assert_64bit[ sizeof(void *) == 8 ];
#else
    enum { is_64bit = 0 };
    typedef char static_assert_32bit[ sizeof(void *) == 4 ];
#endif

// # mem

static void *oom = 0;

// # err

void error( const char *message ) { // $
    if( message[0] == '!' ) {
        //callstack( 128, puts );
    }
    tty( message );
}

void panic( const char *message ) { // $ // NO_RETURN
    if( oom ) free(oom), oom = 0;
    error( message );
#ifndef SHIPPING
    if( debugging() || getenv("DEV") ) {
        breakpoint();
    } else {
        tty( "; press return key to continue..." );
        getchar();
    }
#endif
    // dialog("tmi", "General failure", error, "error");
    die();
}

// # va

static THREAD_LOCAL int vl_index = 0;
static THREAD_LOCAL char *vl_buffer[16] = {0};
char *vl( const char *fmt, va_list lst ) { //$
    int idx = (++vl_index) % 16;
    int sz = 1 + vsnprintf(0, 0, fmt, lst);
    if( sz > 0 ) {
        vl_buffer[idx] = (char *)realloc( vl_buffer[idx], sz );
        if( vl_buffer[idx] ) {
            vsnprintf( vl_buffer[idx], sz, fmt, lst );
        }
    }
    int ok = ( sz > 0 && vl_buffer[idx] );
    if(!ok) panic("!va()/vl(): out of mem");

    static char nil;
    return ok ? vl_buffer[idx] : (nil = 0, &nil);
}
char *va( const char *fmt, ... ) { //$
    va_list lst;
    va_start(lst, fmt);
    char *rc = vl(fmt, lst);
    va_end(lst);
    return rc;
}

// # debug

#include <assert.h>
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
    char *out = (char *)realloc( 0, maxlen ), *stream = out;
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
static int backtrace( void **addr, int maxtraces ) {
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
static char **backtrace_symbols(void *const *array,int size) {
    HANDLE process = GetCurrentProcess();
    enum { MAXSYMBOLNAME = 512 - sizeof(IMAGEHLP_SYMBOL64) };
    char symbol64_buf     [ 512 ];
    char symbol64_bufblank[ 512 ] = {0};
    IMAGEHLP_SYMBOL64 *symbol64       = (IMAGEHLP_SYMBOL64*)symbol64_buf;
    IMAGEHLP_SYMBOL64 *symbol64_blank = (IMAGEHLP_SYMBOL64*)symbol64_bufblank;
    symbol64_blank->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    symbol64_blank->MaxNameLength = (MAXSYMBOLNAME-1) / 2; //wchar?

    int symlen = size * sizeof(char *);
    char **symbols = (char **)malloc(symlen);

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

            symbuf = (char **)realloc( symbols, symlen + buflen );
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
static int backtrace(void **, int) { $return 0; }
static char **backtrace_symbols(void *const *,int) { $return 0; }
#endif

void callstack( int traces, int (*yield)( const char *entry ) ) {
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
            free( dmgbuf );
        }
#endif
        sprintf(buf, "%3d %#016p %s", ++L, stack[i], symbols[i]);
        yield(buf);
    }

    free( symbols );
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
void trap() { $// trapc(), trapcpp()
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

// # win wide strings

#ifdef _WIN32
#include <shlobj.h>
static
wchar_t *widen(const char *utf8) { $
    int needed = 2 * /* 6 **/ MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0); // uf8len*6
    void *out = calloc( 1, needed + 1 );
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, out, needed);
    return (wchar_t *)out;
}
/*
static
char *shorten(const wchar_t *wstr) { $
    int wlen = wcslen(wstr);
    int needed = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL);
    void *out = calloc( 1, needed + 1 );
    WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, out, needed, NULL, NULL);
    return (char *)out;
}*/
#endif


// # ios

#include <stdio.h>
static
FILE *fopen8( const char *pathfile8, const char *mode8 ) { $
#ifdef _WIN32
    wchar_t *pf = widen(pathfile8), *md = widen(mode8);
    FILE *fp = _wfopen(pf, md);
    return free(md), free(pf), fp;
#else
    return fopen( pathfile8, mode8 );
#endif
}
static
void* readfile2(const char *pathfile, void **reptr) { $
    for( FILE *fp = fopen8(pathfile, "rb"); fp; fp = (fclose(fp), NULL) ) {
        fseek(fp, 0L, SEEK_END);
        size_t len = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        // @todo: bring back original code: void *data = reptr ? realloc(*reptr, len + 1) : (void *)stack(len + 1);
        void *data = realloc(reptr ? *reptr : 0, len + 1);
        if( data ) {
            if( reptr ) *reptr = data;
            len[(char*)data] = '\0';
            if( len == 0 || 1 == fread(data, len, 1, fp) ) {
                return fclose(fp), data;
            }
        }
    }
    return NULL;
}
char* readchunk( const char *pathfile, size_t offset, size_t len ) {
    // @todo
    return 0;
}
char* readfile(const char *pathfile) { $
    // return readchunk( pathfile, 0, ~0ull );
    return readfile2(pathfile, 0);
}
bool overwrite(const char *pathfile, const void *data, int len) { $
    bool ok = 0;
    FILE *fp = fopen8(pathfile, "wb");
    if( fp ) {
        ok = 1 == fwrite(data, len, 1, fp);
        fclose(fp);
    }
    return ok;
}
#include <sys/stat.h>
uint64_t filestamp( const char *pathfile ) { $
    struct stat st;
    return stat(pathfile, &st) < 0 ? 0ULL : (uint64_t)st.st_mtime;
}
uint64_t filesize( const char *pathfile ) { $
    struct stat st;
    return stat(pathfile, &st) < 0 ? 0ULL : (uint64_t)st.st_size;
}
bool fileexist( const char *pathfile ) {
    return filestamp( pathfile ) > 0;
}
const char *datatype(const char *buf, int len) { $
    struct type {
        const char *ext; int len; const char *buf; int off;
    } types[] = {
        "opus",     8, "OpusHead", 28, // before 'ogg'
        "mxf",     14, "\x06\x0E\x2B\x34\x02\x05\x01\x01\x0D\x01\x02\x01\x01\x02", 0,
        "m4v",     11, "\x00\x00\x00\x1C\x66\x74\x79\x70\x4D\x34\x56", 0,
        "wmv",     10, "\x30\x26\xB2\x75\x8E\x66\xCF\x11\xA6\xD9", 0,
        "mov",      8, "\x00\x00\x00\x14\x66\x74\x79\x70", 0,
        "msi",      8, "\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1", 0,
        "ar",       7, "!<arch>", 0,
        "m4a",      7, "ftypM4A", 4,
        "7z",       6, "\x37\x7A\xBC\xAF\x27\x1C", 0,
        "xz",       6, "\xFD\x37\x7A\x58\x5A\x00", 0,
        "amr",      5, "#!AMR", 0,
        "rtf",      5, "{\rtf", 0,
        "tar",      5, "ustar", 257,
        "ttf",      5, "\x00\x01\x00\x00\x00", 0,
        "cab",      4, "ISc(", 0,
        "cab",      4, "MSCF", 0,
        "crx",      4, "Cr24", 0,
        "flac",     4, "fLaC", 0,
        "flif",     4, "FLIF", 0,
        "flv",      4, "FLV\1", 0,
        "ico",      4, "\x00\x00\x01\x00", 0,
        "lz",       4, "LZIP", 0,
        "m4a",      4, "M4A ", 0,
        "mid",      4, "MThd", 0,
        "mkv",      4, "\x1A\x45\xDF\xA3", 0,
        "mp4",      4, "3gp5", 0,
        "mp4",      4, "ftyp", 4,
        "mpg",      4, "\x00\x00\x01\x62", 0,
        "nes",      4, "NES\x1A", 0,
        "ogg",      4, "OggS", 0,
        "otf",      4, "OTTO", 0,
        "pdf",      4, "%PDF", 0,
        "png",      4, "\x89PNG", 0,
        "psd",      4, "8BPS", 0,
        "rar",      4, "Rar!", 0,
        "rpm",      4, "\xED\xAB\xEE\xDB", 0,
        "sqlite",   4, "SQLi", 0,
        "svg",      4, "<svg", 0,
        "tiff",     4, "\x49\x49\x2A\x00", 0,
        "tiff",     4, "\x4D\x4D\x00\x2A", 0,
        "wav",      4, "WAVE", 8,
        "webp",     4, "WEBP", 8,
        "woff",     4, "wOFF", 0,
        "woff2",    4, "wOF2", 0,
        "avi",      3, "AVI", 8,
        "bz2",      3, "BZh", 0,
        "gif",      3, "GIF", 0,
        "gz",       3, "\x1F\x8B\x08", 0,
        "jpg",      3, "\xFF\xD8\xFF", 0,
        "jxr",      3, "\x49\x49\xBC", 0,
        "mp3",      3, "ID3", 0,
        "bmp",      2, "BM", 0,
        "dmg",      2, "x\1", 0,
        "exe",      2, "MZ", 0,
        "ps",       2, "%!", 0,
        "swf",      2, "WS", 1,
        "z",        2, "\x1F\x9D", 0,
        "z",        2, "\x1F\xA0", 0,
        "zip",      2, "PK", 0,
        0
    };

    for( int i = 0; types[i].ext; ++i ) {
        if( (types[i].off + types[i].len) < len ) {
            if( 0 == memcmp( buf + types[i].off, types[i].buf, types[i].len ) ) {
                return types[i].ext;
            }
        }
    }
    return 0;
}
uint64_t space() { $
#ifdef _WIN32
    DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
    if( GetDiskFreeSpaceA( ".\\", &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters ) ) {
        return ((uint64_t)NumberOfFreeClusters) * SectorsPerCluster * BytesPerSector;
    }
#endif
    return ~0LLU;
}

// # dll

#ifdef _WIN32
#   include <stdlib.h>
#   include <winsock2.h>
#   define DLL                  HMODULE
#   define dlopen(name,mode)    LoadLibraryA( name )
#   define dlsym(handle,symbol) GetProcAddress( handle, symbol )
#   define dlclose(handle)      0
#else
#   include <dlfcn.h>
#   define DLL                  void*
#endif
#if defined(__APPLE__)
#   include <mach-o/dyld.h>
#endif
DLL plugins[32] = {0};
int dllopen(int plug_id, const char *filename) { $
    const char *buf;
    if( filesize(buf = va("%s.dll", filename)) ||
        filesize(buf = va("%s.so", filename)) ||
        filesize(buf = va("lib%s.so", filename)) ||
        filesize(buf = va("%s.dylib", filename)) ) {
        filename = buf;
    } else {
        return 0;
    }
#if _WIN32 && !SHIPPING
    // hack: dont let windows ever lock our source dll (we want the dll to be monitored and hot-reloaded)
    // we move the dll to the tmp folder and load it from there (this temp dll will get locked instead).
    char outfile[512];
    char *tmpdir = getenv("TMP") ? getenv("TMP") : getenv("TEMP") ? getenv("TEMP") : 0;
    if( tmpdir ) {
        /*
        csystem(0, "mkdir %s\\AVA 1> nul 2> nul", tmpdir);
        csystem(outfile, "echo %s\\AVA\\%%random%%", tmpdir);
        csystem(0, "copy /y \"%s\" \"%s.%s\" 1> nul 2> nul", filename, outfile, filename);
        csystem(outfile, "echo %s.%s", outfile, filename);
        filename = outfile;
        */
    }
#endif
    plugins[plug_id] = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    return plugins[plug_id] != 0;
}
void *dllfind(int plug_id, const char *name) { $
    return dlsym(plugins[plug_id], name);
}
void dllclose(int plug_id) { $
    dlclose(plugins[plug_id]);
}

// # tty

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>

bool tty(const char *text) {
#ifdef _MSC_VER
    OutputDebugStringA( text );
#endif
    return !!puts( text );
}
void ttycolor( uint8_t r, uint8_t g, uint8_t b ) {
#if _WIN32
    const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    auto color = ( r > 127 ? FOREGROUND_RED : 0 ) |
                 ( g > 127 ? FOREGROUND_GREEN : 0 ) |
                 ( b > 127 ? FOREGROUND_BLUE : 0 ) |
                 FOREGROUND_INTENSITY;
    SetConsoleTextAttribute(stdout_handle, color);
#else
    // 24-bit console ESC[ … 38;2;<r>;<g>;<b> … m Select RGB foreground color
    // 256-color console ESC[38;5;<fgcode>m
    // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
    // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
    // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
    // 0xE8-0xFF:  grayscale from black to white in 24 steps
    r /= 51; // [0..5]
    g /= 51; // [0..5]
    b /= 51; // [0..5]
    printf("\033[38;5;%dm", r*36+g*6+b+16); // "\033[0;3%sm", color_code);
#endif
}
void ttydrop() {
#ifdef _WIN32
    //static FILE* fcon = 0; if(!fcon) fcon = fopen("CON", "w" );            // open console
    //static FILE* fout = 0; if(!fout) fout = freopen( "CON", "w", stdout ); // redirect stdout to console
    //static FILE* ferr = 0; if(!ferr) ferr = freopen( "CON", "w", stderr ); // redirect stderr to console
#endif
    // hide stdout
    freopen( tmpnam(0), "wt", stdout );
    // hide stderr
    freopen( tmpnam(0), "wt", stderr );
#ifdef _WIN32
    FreeConsole();
#endif
}



// # dir

char *exedir() {
#   ifdef _WIN32
    static char s_path[MAX_PATH] = { 0 };
    const size_t len = MAX_PATH;

    HMODULE mod = 0;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&exedir, &mod);
    DWORD size = GetModuleFileNameA(mod, s_path, (DWORD)len);

    while (size > 0) {
        if (s_path[size] == '\\') {
            //name = &s_path[size + 1];
            s_path[size + 1] = '\0';
            //path = s_path;
            return s_path;
        }
        --size;
    }
    return "";
#   else
    char arg1[20];
    static char exepath[PATH_MAX + 1] = { 0 };

    sprintf(arg1, "/proc/%d/exe", getpid());
    readlink(arg1, exepath, 1024);
    //path = exepath;
    //name = std::string();
    return path;
#   endif
}

static
void cd( const char *path ) {
#ifdef _MSC_VER
    _chdir( path );
#else
    chdir(path);
#endif
}

// # crt

void start() { $
    //breakpoint();
    //ttycolor(64,255,64);
    tty(";; AVA - Compiled on " __DATE__ " " __TIME__);

    // init a few kits
    oom = realloc( oom, 32 * 1024 * 1024 );
    callstack(0, puts); // tty);

    // did it work?
    tty(exedir());
    cd(exedir());
    if( dllopen(0, "editor") ) {
        void *pfn = dllfind(0, "main");
        printf("editor.dll %p\n", pfn );
        ((int(*)(int, char**))pfn)(0,0);
        dllclose(0);
    }
    if( dllopen(0, "game") ) {
        void *pfn = dllfind(0, "main");
        printf("game.dll %p\n", pfn );
        ((int(*)(int, char**))pfn)(0,0);
        dllclose(0);
    }
}
