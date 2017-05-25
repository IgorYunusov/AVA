//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// People who worked on this file: @r-lyeh, 

#ifndef AVA_VERSION
#define AVA_VERSION "0.0.0" // v0.0.0 YOB-Marrow

//  # std.def : definitions, types and constants.

    #include <stdio.h>
    #include <stdint.h>
    #include <stdarg.h>
    #include <stdbool.h>

    typedef void *any;

//  # std.abi : symbol linkage and visibility.

    #  if !defined(API) && defined(__cplusplus)
    #   define API extern "C" __declspec(dllimport)
    #elif !defined(API)
    #   define API            __declspec(dllimport)
    #endif

//  # std.api : guards, namespaces, enums, macros.

    #ifndef EXPORT
    #   define EXPORT(version, ...) __VA_ARGS__
    #endif

//  # kit.pro : profiler and perf

    #ifndef $
    #   define $
    #endif

//  # api
//

EXPORT( 100,

    // # kit.crt entry point
    // - start engine
    // - drop engine
    // - add subsystem ring

    API void   init();
    API void   quit();
    API void   ring( bool did_init, const char *title, void (*quit)() );

    // # kit.arg arguments
    // - argc, as invoked from main()
    // - argv, as invoked from main()

    API int    argc();
    API char** argv();

    // # kit.cfg configuration options
    // - return value from comma delimited options among all providers below, or defaults if not found.  See:
    //     const char *user = cfg("guest", "u,user,username");
    //     const int port = atoi( cfg("8080", "p,port") );
    //   tip: priorities for cfg() are: 1st) env, 2nd) arg, 3rd) ini.
    // - return value from comma delimited environment variables (set key=VAL), or defaults if not found.
    // - return value from comma delimited arguments (-key=VAL, --key=VAL), or defaults if not found.
    // - return value from comma delimited keys in game.ini file (key=VAL), or defaults if not found.

    API const char* cfg(const char *defaults, const char *csv_vars);
    API const char* env(const char *defaults, const char *csv_sets);
    API const char* arg(const char *defaults, const char *csv_opts);
    API const char* ini(const char *defaults, const char *csv_keys);

    // # kit.err errors
    // - log error and continue
    // - try to recover some memory, then log error, show dialog and die
    // tip: if msg starts with '!' character, callstack/backtrace will be retrieved too. @todo

    API void   error(const char *msg);
    API void   panic(const char *msg);

    // # kit.fmt va arg string formatting
    // - format variable args into temporary ring-buffer. 16 slots per thread.
    // - format variable list into temporary ring-buffer. 16 slots per thread.
    // - format variable args and duplicate temporary buffer. must free() result.
    // - format variable list and duplicate temporary buffer. must free() result.

    API char*  va(const char *format, ...);
    API char*  vl(const char *format, va_list list);
    API char*  vadup( const char *fmt, ... );
    API char*  vldup( const char *fmt, va_list list);

    // # kit.str string manipulation
    // - find first substring, or 0 if not found.
    // - find last substring, or 0 if not found.
    // - trim first substring on the left. returns modified text
    // - trim first substring on the right. returns modified text
    // - search for wildcards patterns (?, *). returns true if found
    // - return true if string begins with substring
    // - return true if string ends with substring
    // - lowercase given string
    // - tokenize string into max tokens by using delimiters. returns 0 if more pairs are needed. See:
    //     const char *str = "There is a lady who's sure, all that glitter is gold...";
    //     const char *tokens[128]; int sizes[128];
    //     if( strchop( tokens, sizes, 128, str, " ,." ) ) for( int i = 0; tokens[i]; ++i ) {
    //         printf("'%.*s'\n", sizes[i], tokens[i]);
    //     }

    API char*  strfindl(char *text, const char *substring);
    API char*  strfindr(char *text, const char *substring);
    API char*  strtriml(char *text, const char *substring);
    API char*  strtrimr(char *text, const char *substring);
    API int    strmatch(const char *text, const char *pattern);
    API bool   strbegin( const char *text, const char *substring );
    API bool   strend( const char *text, const char *substring );
    API char*  strlower( char *str );
    API int    strchop(const char **tokens, int *sizes, int maxtokens, const char *src, const char *delim);

    // # kit.utf utf and unicode
    // - encode codepoint as utf8 string.
    // - decode next codepoint in utf8 string. return 0 if end of string.

    API char*    utf8(uint32_t codepoint);
    API uint32_t codepoint(const char **utf8);

    // # kit.dbg debug
    // - returns true if asserts are enabled
    // - returns true if debugger is present
    // - invokes debugger, if present
    // - install signal/crash handlers
    // - simulate crash
    // - fast exit
    // - hexdumps data content into memory. must free() returned data
    // - yields stacktrace to delegated function. negative maxtraces reverse list. See:
    //     void B() { callstack(-64, puts); puts("--"); callstack(+64, puts); } void A() { B(); } int main() { A(); }

    API bool   asserting();
    API bool   debugging();
    API void   breakpoint();
    API void   trap();
    API void   crash();
    API void   die();
    API char*  hexdump( const void *ptr, unsigned len );
    API void   callstack(int maxtraces, int (*yield)(const char *line));

    // # kit.iof io files
    // - get size of file. 0 if not found.
    // - get file modification date (number of seconds since unix epoch 1970)
    // - get true if file exist
    // - get whole file contents
    //   must free() returned data; return 0 if error; null-ending char silently added
    // - get chunk of data: read len bytes starting from offset
    //   must free() returned data; return 0 if error; null-ending char silently added
    //   tip: if len == ~0ull read as many bytes as possible.
    // - overwrite file with data. returns 0 if error

    API uint64_t iofsize( const char *pathfile );
    API uint64_t iofstamp( const char *pathfile );
    API bool     iofexist( const char *pathfile );
    API char*    iofread( const char *pathfile );
    API char*    iofchunk( const char *pathfile, size_t offset, size_t len );
    API bool     iofwrite( const char *pathfile, const void *ptr, int bytes );
    API bool     iofisdir(const char *pathfile);
    API bool     iofisfile(const char *pathfile);
    API bool     iofislink(const char *pathfile);
    API bool     iofisvirt(const char *pathfile);

    // # kit.bin binary data
    // - type of data: may be NULL, "jpg", "png", "ogg", "mp4", etc...

    API const char *bintype(const char *buf, int len);

    // # kit.env environment
    // - set environment variable. zero value clears variable
    // - get environment variable, or 0 if not found

    API void        envput( const char *key, const char *value );
    API const char *envget( const char *key );

    // # kit.tty terminal
    // - print into terminal(s)
    // - change terminal color
    // - drop terminal window if present. close stdout/err as well.
    // @todo: int ttycols();

    API bool   tty(const char *text);
    API void   ttycolor( uint8_t R, uint8_t G, uint8_t B );
    API void   ttydrop();

    // # kit.usr
    // - get binary name (launcher).
    // - get user name (r-lyeh).
    // - get game directory (D:\prj\#AVA\.bin\debug\).
    // - get data directory (C:\Users\r-lyeh\AppData\Roaming).
    // - get system temp directory (C:\Users\r-lyeh\AppData\Local\Temp).
    // - get current working directory (D:\prj\#AVA).
    // - get free space of working directory.

    API char*    usrbin();
    API char*    usrname();
    API char*    usrgame();
    API char*    usrdata();
    API char*    usrtemp();
    API char*    usrwork();
    API uint64_t usrspace();

    // # kit.dir
    // - convert backslashes to forward slashes.
    // - convert to absolute path. returns converted/resized path.
    // - return true if directory entry is an absolute path.
    // - extract path from directory entry.
    // - extract basename from directory entry.
    // - extract name from directory entry (basename without extension).
    // - extract type/extension from directory entry (basename without name).

    API char*    dirfix(char *pathfile);
    API char*    dirabs(char **pathfile);
    API bool     dirisabs(const char *pathfile);
    API char*    dirpath(char *pathfile);
    API char*    dirbase(char *pathfile);
    API char*    dirname(char *pathfile);
    API char*    dirtype(char *pathfile);

    // # kit.tst unit-tests
    // - define section for next tests.
    // - test case. symbol gets macro'ed at bottom of this header. See:
    //     unit("suite 123"); test(1 < 2); test(2 < 3);

    API bool   unit(const char *name);
    API bool   test(int expression, const char *file, int line);

    // # kit.dll dynamic library loading
    // - open dynamic library (without extension). returns 0 if not found
    // - find symbol in dynamic library
    // - close dynamic library

    API  int   dllopen(int plug_id, const char *pathfile);
    API void*  dllfind(int plug_id, const char *name);
    API void   dllclose(int plug_id);

    // # kit.gui dialog
    // - returns 0 if error, else number of button pressed [1..N]
    //   format = (t)itle, (m)essage, (b)uttons, (w)indow, (i)con, (c)olors. See:
    //     dialog("tmbi", "Warning!", "Files will be overwritten\nContinue?", 2, "warning");

    API int    dialog(const char *format, ...);
)

#define test(expr) test(expr,__FILE__,__LINE__)

#include "ret.inl" // return codes

#endif /* AVA_VERSION */



//{ C++ wrappers for convenience

#ifdef __cplusplus
#pragma once
#include <vector>
#include <map>
static inline
std::vector<std::string> strchop( const char *text, const char *delims ) {
    std::vector<std::string> out; out.reserve(1024);
    std::vector<const char *> tokens(1024);
    std::vector<int>          sizes(1024);
    if( strchop( tokens.data(), sizes.data(), 1024, text, delims ) )
    for( int i = 0; tokens[i]; ++i ) {
        out.push_back( std::string( tokens[i], sizes[i] ) );
    }
    return out;
}
#endif

//} C++ wrappers for convenience
