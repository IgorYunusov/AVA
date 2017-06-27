//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// People who worked on this file: @r-lyeh,

#ifndef AVA_VERSION
#define AVA_VERSION "0.0.0" // v0.0.0 YOB-Marrow

//  # std.int : types and constants.

    #include <stdio.h>
    #include <stdint.h>
    #include <stdarg.h>
    #include <stdbool.h>

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

    // # kit.def definitions
    // - opaque pointer.
    // - pointer from heap. user must memfree() after use.
    // - pointer from stack. user must memcpy() before next frame.
    // - errorcode: C style errocode (positive if ok, negative if error)

    typedef void *  any;
    typedef char    heap;
    typedef char    stack;
    typedef int     errorcode;

    // # kit.crt entry point
    // - add subsystem ring
    // - start engine
    // - drop engine

    API void        ring        (bool did_init, const char *title, void (*quit)());
    API void        init        ();
    API void        quit        ();

    // # kit.arg arguments
    // - argc, as invoked from main()
    // - argv, as invoked from main()

    API int         argc        ();
    API char**      argv        ();

    // ## kit.mem memory
    // - allocate bytes from heap.
    // - allocate bytes from heap. formatted string w/ null-terminator.
    // - allocate bytes from heap, and memcpy src into it. if len is 0, strlen(src) is used instead.
    // - liberate bytes from heap.
    // - allocate bytes from stack.
    // - allocate bytes from stack. formatted string w/ null-terminator.

    API heap*       memalloc    (int size);
    API heap*       memallocf   (const char *fmt, ...);
    API heap*       memdup      (const void *src, int len);
    API heap*       memfree     (void **ptr);

    API stack*      tmpalloc    (int bytes);
    API stack*      tmpallocf   (const char *fmt, ...);

    // # kit.cfg configuration options
    // - return value from comma delimited options among all providers below, or defaults if not found.  See:
    //     const char *user = cfg("guest", "u,user,username");
    //     const int port = atoi( cfg("8080", "p,port") );
    //   tip: priorities for cfg() are: 1st) env, 2nd) arg, 3rd) ini.
    // - return value from comma delimited environment variables (set key=VAL), or defaults if not found.
    // - return value from comma delimited arguments (-key=VAL, --key=VAL), or defaults if not found.
    // - return value from comma delimited keys in game.ini file (key=VAL), or defaults if not found.

    API const char* cfg         (const char *defaults, const char *csv_vars);
    API const char* env         (const char *defaults, const char *csv_sets);
    API const char* arg         (const char *defaults, const char *csv_opts);
    API const char* ini         (const char *defaults, const char *csv_keys);

    // # kit.err errors
    // - log error and continue
    // - log error and die gracefully
    // - try to { recover reserved memory, log error, show dialog and die } gracefully
    // tip: if msg starts with '!' character, callstack/backtrace will be retrieved too. @todo

    API void        error       (const char *msg);
    API void        fatal       (const char *msg);
    API void        panic       (const char *msg);

    // # kit.fmt va arg string formatting
    // - format variable args into temporary ring-buffer. 16 slots per thread.
    // - format variable list into temporary ring-buffer. 16 slots per thread.
    // - format variable args and duplicate temporary buffer.
    // - format variable list and duplicate temporary buffer.

    API stack*      va          (const char *format, ...);
    API stack*      vl          (const char *format, va_list list);
    API heap*       vadup       (const char *fmt, ... );
    API heap*       vldup       (const char *fmt, va_list list);

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

    API char*       strfindl    (char *text, const char *substring);
    API char*       strfindr    (char *text, const char *substring);
    API char*       strtriml    (char *text, const char *substring);
    API char*       strtrimr    (char *text, const char *substring);
    API int         strmatch    (const char *text, const char *pattern);
    API bool        strbegin    (const char *text, const char *substring);
    API bool        strend      (const char *text, const char *substring);
    API char*       strlower    (char *str);
    API int         strchop     (const char **tokens, int *sizes, int maxtokens, const char *src, const char *delim);

    // # kit.utf utf and unicode
    // - encode codepoint as utf8 string.
    // - decode next codepoint in utf8 string. return 0 if end of string.

    API char*       utf8        (uint32_t codepoint);
    API uint32_t    codepoint   (const char **utf8);

    // # kit.dbg debug
    // - returns true if asserts are enabled
    // - returns true if debugger is present
    // - invokes debugger, if present
    // - install signal/crash handlers
    // - simulate crash
    // - fast exit
    // - hexdumps data content into memory.
    // - yields stacktrace to delegated function. negative maxtraces reverse list. See:
    //     void B() { callstack(-64, puts); puts("--"); callstack(+64, puts); } void A() { B(); } int main() { A(); }

    API bool        asserting   ();
    API bool        debugging   ();
    API void        breakpoint  ();
    API void        trap        ();
    API void        crash       ();
    API void        die         ();
    API heap*       hexdump     (const void *ptr, unsigned len);
    API void        callstack   (int maxtraces, errorcode (*yield)(const char *line));

    // # kit.iof io files
    // - get true if path exists.
    // - get true if path is a directory.
    // - get true if path is a file.
    // - get true if path is a link.

    // - get file size of file. 0 may indicate file not found.
    // - get file modification date (number of seconds since unix epoch 1970)
    // - get file contents. return 0 if error; null-ending char silently added
    // - set file contents (overwrites file). returns 0 if error
    // - set file contents (appends to file). returns 0 if error
    // - map chunk of file: filename, initial offset and read size in bytes.
    // - unmap memory from iofmap().

    API bool        iofexist    (const char *pathfile);
    API bool        iofisdir    (const char *pathfile);
    API bool        iofisfile   (const char *pathfile);
    API bool        iofislink   (const char *pathfile);

    API uint64_t    iofsize     (const char *pathfile);
    API uint64_t    iofstamp    (const char *pathfile);
    API heap*       iofread     (const char *pathfile);
    API bool        iofwrite    (const char *pathfile, const void *data, int bytes);
    API bool        iofappend   (const char *pathfile, const void *data, int bytes);
    API char*       iofmap      (const char *pathfile, size_t offset, size_t size);
    API void        iofunmap    (char *buf, size_t len);

    // # kit.bin binary data
    // - type of data: may be NULL, "jpg", "png", "ogg", "mp4", etc...

    API const char* bintype     (const char *buf, int len);

    // # kit.env environment
    // - set environment variable. zero value clears variable
    // - get environment variable, or 0 if not found

    API void        envput      (const char *key, const char *value);
    API const char* envget      (const char *key);

    // # kit.tty terminal
    // - print into terminal(s)
    // - change terminal color
    // - number of columns in the terminal window or 0 on error.
    // - drop terminal window if present. close stdout/err as well.

    API bool        tty         (const char *text);
    API void        ttycolor    (uint8_t R, uint8_t G, uint8_t B);
    API int         ttycolumns  ();
    API void        ttydrop     ();

    // # kit.usr
    // - get binary name (launcher).
    // - get user name (r-lyeh).
    // - get game directory (D:\prj\#AVA\.bin\debug\).
    // - get data directory (C:\Users\r-lyeh\AppData\Roaming).
    // - get system temp directory (C:\Users\r-lyeh\AppData\Local\Temp).
    // - get current working directory (D:\prj\#AVA).
    // - get free space of working directory.

    API char*       usrbin      ();
    API char*       usrname     ();
    API char*       usrgame     ();
    API char*       usrdata     ();
    API char*       usrtemp     ();
    API char*       usrwork     ();
    API uint64_t    usrspace    ();

    // # kit.dir
    // - convert backslashes to forward slashes.
    // - convert to absolute path. returns converted/resized path.
    // - return true if directory entry is an absolute path.
    // - extract path from directory entry.
    // - extract basename from directory entry.
    // - extract name from directory entry (basename without extension).
    // - extract type/extension from directory entry (basename without name).
    // - list files and folders in path, according to mask. accepts *, ?, and ** for subdirs
    //   returns number of proccessed entries; yields matching entries, that end with '/' if dir
    //      dirls("./textures/**.*p*", puts);

    API char*       dirfix      (char *pathfile);
    API char*       dirabs      (char **pathfile);
    API bool        dirisabs    (const char *pathfile);
    API char*       dirpath     (char *pathfile);
    API char*       dirbase     (char *pathfile);
    API char*       dirname     (char *pathfile);
    API char*       dirtype     (char *pathfile);
    API int         dirls       (const char *pathmask, errorcode (*yield)(const char *name));

    // # kit.uid unique ids
    // - fill buffer with human readable (UUID v4: 16 byte, 128-bit)
    // - fill buffer with entropy/random data

    API char*       uid4        (char uuid4[36+1]);
    API void        uidbuf      (char *buffer, int len);

    // # kit.rnd pseudo-random
    // - seed state with given argument
    // - get next integer from state
    // - get next double from state (0,1]
    // - get next integer within range from state [mini, maxi]

    API void        rndseed     (uint64_t state[2], int64_t seed);
    API uint64_t    rndu64      (uint64_t state[2]);
    API double      rnddbl      (uint64_t state[2]);
    API int64_t     rndint      (uint64_t state[2], int64_t mini, int64_t maxi);

    // # kit.crc 64-bit crc and hashing functions
    // - hash of data blob (crc64)
    // - hash of pointer (address of pointer)
    // - hash of string (custom hashing)

    API uint64_t    crc64       (const void *ptr, size_t len);
    API uint64_t    ptr64       (void *addr);
    API uint64_t    str64       (const char *str);

    // # kit.log logging
    // - init logging on given file stream. many loggers are allowed.
    // - deploy message to all loggers (using va_list).
    // - deploy message to all loggers. See:
    //     logme( stderr ); LOG(AUDIO|STREAMING, "!this is an audio message (with %s)", "callstack");
    //   tip: LOG(tags, ...) macro adds FILE and LINE.
    //   tip: callstack/backtraces will be printed if message starts with '!' character.

    API void        logme       (FILE *fp);
    API void        logvl       (const char *file, int line, const char *tag, const char *format, va_list);
    API void        logva       (const char *file, int line, const char *tag, const char *format, ...);

    // # kit.tst unit-tests
    // - define section for next tests.
    // - test case. See:
    //     unit("suite 123"); TEST(1 < 2); TEST(2 < 3);
    //   tip: TEST(expression) macro adds FILE and LINE.

    API bool        unit        (const char *name);
    API bool        test        (const char *file, int line, int expression);

    // # kit.dll dynamic library loading
    // - open dynamic library (without extension). returns 0 if not found
    // - find symbol in dynamic library
    // - close dynamic library

    API  int        dllopen     (int plug_id, const char *pathfile);
    API void*       dllfind     (int plug_id, const char *name);
    API void        dllclose    (int plug_id);

    // # kit.clk clock
    // - nanoseconds since app initialization
    // - microseconds since app initialization
    // - milliseconds since app initialization
    // - seconds since app initialization
    // - minutes since app initialization
    // - hours since app initialization

    API uint64_t    ns          ();
    API uint64_t    us          ();
    API uint64_t    ms          ();
    API uint64_t    ss          ();
    API uint64_t    mm          ();
    API uint64_t    hh          ();

    // # kit.thd threading
    // - get process id
    // - get thread id
    // - get number of max concurrent hardware threads (cores)
    // - start detached thread. return true if started
    // - start thread. return true if started
    // - join thread. returns true if joined
    // - yield current thread
    // - sleep current thread

    API int         pid         ();
    API int         tid         ();
    API int         cores       ();
    API bool        detach      (void(*function)(void *), void *arg);
    API bool        thread      (int thread_id, void(*function)(void *), void *arg);
    API bool        join        (int thread_id);
    API void        yield       ();
    API void        sleep       (unsigned ss, unsigned ms, unsigned us, unsigned ns);

    // # kit.mtx mutex
    // - lock a mutex
    // - unlock a mute
    // - try to lock a mutex. returns 0 if cannot lock

    API void        lock(int mutex_id);
    API void        unlock(int mutex_id);
    API bool        try_lock(int mutex_id);

    // # kit.atm atomics
    // - set value into atomic variable
    // - get atomic variable value
    // - add value into atomic variable
    // - increase atomic variable
    // - decrease atomic variable

    API void        atmset(int *var, const int value);
    API int         atmget(int *var);
    API int         atmadd(int *var, const int value);
    API int         atminc(int *var);
    API int         atmdec(int *var);

    // # kit.gui dialog
    // - returns 0 if error, else number of button pressed [1..N]
    //   format = (t)itle, (m)essage, (b)uttons, (w)indow, (i)con, (c)olors. See:
    //     dialog("tmbi", "Warning!", "Files will be overwritten\nContinue?", 2, "warning");

    API int         dialog(const char *format, ...);
)

// rest of api

#include "ret.inl" // return codes
#include "stl.inl" // containers
#include "sys.inl" // system headers and detection macros
#include "bld.inl" // build macros

// a few convenient macros. they just add FILE:LINE most of the times.

#ifdef SHIPPING
#define LOG(TAGS, ...)
#define TEST(EXPR)
#else
#define LOG(TAGS, ...) logva(__FILE__, __LINE__, #TAGS, __VA_ARGS__)
#define TEST(EXPR)     test(__FILE__,__LINE__, EXPR)
#endif

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
