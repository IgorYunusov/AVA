//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// People who worked on this file: @r-lyeh, 

#ifndef AVA_VERSION
#define AVA_VERSION "0.0.0" // v0.0.0 YOB-Marrow

//  ## std.def : definitions, types and constants.

    #include <stdio.h>
    #include <stdint.h>
    #include <stdarg.h>
    #include <stdbool.h>

//  ## std.abi : symbol linkage and visibility.

    #  if !defined(API) && defined(__cplusplus)
    #   define API extern "C" __declspec(dllimport)
    #elif !defined(API)
    #   define API            __declspec(dllimport)
    #endif

//  ## std.api : guards, namespaces, enums, macros.

    #ifndef EXPORT
    #   define EXPORT(version, ...) __VA_ARGS__
    #endif

//  ## std.pro : profiler and perf

    #ifndef $
    #   define $
    #endif

//  ## api
//

EXPORT( 100,

    // # std.crt entry point

    API void   start();

    // # kit.err errors
    // - log error and continue
    // - try to recover some memory, then log error, show dialog and die
    // tip: if msg starts with '!' character, callstack/backtrace will be retrieved too. @todo

    API void   error(const char *msg);
    API void   panic(const char *msg);

    // # kit.debug
    // - returns true if asserts are enabled
    // - returns true if debugger is present
    // - invokes debugger, if present
    // - install signal/crash handlers
    // - simulate crash
    // - fast exit
    // - hexdumps data content into memory. must free() returned data
    // - yields stacktrace to delegated function. negative maxtraces reverse list.
    //   Usage: void B() { callstack(-64, puts); puts("--"); callstack(+64, puts); } void A() { B(); } int main() { A(); }

    API bool   asserting();
    API bool   debugging();
    API void   breakpoint();
    API void   trap();
    API void   crash();
    API void   die();
    API char*  hexdump( const void *ptr, unsigned len );
    API void   callstack(int maxtraces, int (*yield)(const char *line));

    // # kit.ios
    // - returns size of file. 0 if not found.
    // - file modification date (number of seconds since unix epoch 1970)
    // - returns if file exist
    // - reads whole file (*)
    // - reads file chunk of length bytes starting from offset (*)
    //   (*) return 0 if error, else data must be free(). hint: null-ending char silently added
    //   tip: if len == ~0u read as many bytes as possible.
    // - overwrite file with data. returns 0 if error
    // - returns free space

    API uint64_t filesize( const char *pathfile );
    API uint64_t filestamp( const char *pathfile );
    API bool     fileexist( const char *pathfile );
    API char*    readfile( const char *pathfile );
    API char*    readchunk( const char *pathfile, size_t offset, size_t len );
    API bool     overwrite( const char *pathfile, const void *ptr, int bytes );
    API uint64_t space();

    // # kit.data
    // - type of data: may be NULL, "jpg", "png", "ogg", "mp4", etc...

    API const char *datatype(const char *buf, int len);

    // # kit.terminal
    // - print into terminal(s)
    // - change terminal color
    // - drop terminal window if present. close stdout/err as well.
    // @todo: int ttycols();

    API bool   tty(const char *text);
    API void   ttycolor( uint8_t R, uint8_t G, uint8_t B );
    API void   ttydrop();

    // # kit.dir

    API char*  exedir();

    // # kit.test unit-tests
    // - define section for next tests
    // - test case. symbol gets macro'ed at end of file
    // usage: unit("suite 123"); test(1 < 2); test(2 < 3);

    API int unit(const char *name);
    API int test(int expression, const char *file, int line);

    // # std.str string manipulation
    // - format variable args into temporary ring-buffer. 16 slots per thread.
    // - format variable list into temporary ring-buffer. 16 slots per thread.

    API char*  va(const char *format, ...);
    API char*  vl(const char *format, va_list vl);

    // # std.dll dynamic library loading
    // - open dynamic library (without extension). returns 0 if not found
    // - find symbol in dynamic library
    // - close dynamic library

    API  int   dllopen(int plug_id, const char *pathfile);
    API void*  dllfind(int plug_id, const char *name);
    API void   dllclose(int plug_id);
)

#define test(expr) test(expr, __FILE__, __LINE__)

#endif /* AVA_VERSION */
