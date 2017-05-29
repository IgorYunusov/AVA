#if 0

/* AVA: Build config */

// # BUILD macro
//
// BUILD macro hosts all compilation flags. User can configure BUILD macro with
// -D compiler option or thru external configuration file (see BUILD_H macro).
//
// ## Examples:
// gcc build.c -DBUILD=XB1+SHIPPING
// gcc build.c -DBUILD=WINDOWS+BITS32+MSC+LOGS+LITTLE
// gcc build.c -DBUILD=PS4+CLANG+STATS+ASSERTS
// gcc build.c -DBUILD=LINUX+ARM+SHIPPING
// gcc build.c -DBUILD=ANDROID+MIPS+BIG+BITS64+SHIPPING
// ...
//
// ## Usage:
// #if  BUILD & LINUX                  // single check
// #if !BUILD & ARM                    // single check
// #if  BUILD & !SHIPPING              // single check
// #if  BUILD & (LINUX+!ARM+!SHIPPING) // combined check
// ...
//

// Optional user-defined header (if any) to configure BUILD macro.
#ifdef   BUILD_H
#include BUILD_H
#endif

// Default BUILD configuration is zero (no configuration).
#ifndef  BUILD
#define  BUILD 0
#endif

// Available behavior flags.
#define DOCS          (BUILDXX <<  0) // Generate documents?
#define LOGS          (BUILDXX <<  1) // Generate logs?
#define TESTS         (BUILDXX <<  2) // Generate tests?
#define STATS         (BUILDXX <<  3) // Generate stats?
#define CHECKS        (BUILDXX <<  4) // Generate bounding checks?
#define QUARKS        (BUILDXX <<  5) // Generate quarks/IDs?
#define ASSERTS       (BUILDXX <<  6) // Generate asserts?
#define CONSOLES      (BUILDXX <<  7) // Generate consoles?
#define PROFILERS     (BUILDXX <<  8) // Generate profilers?
#define MEMTRACES     (BUILDXX <<  9) // Generate memtraces?
#define STACKTRACES   (BUILDXX << 10) // Generate stacktraces?

// Available verbosity flags, in decreasing order.
#define INFOS         (BUILDXX << 11) // Report infos?
#define WARNINGS      (BUILDXX << 12) // Report warnings?
#define ERRORS        (BUILDXX << 13) // Report expected errors?
#define FATALS        (BUILDXX << 14) // Report unexpected errors?
#define CRASHES       (BUILDXX << 15) // Report crashes?

// Available disclosure flags, in decreasing order.
#define INTERNAL      (BUILDXX << 16) // May be equivalent to CHEATS=3, SPEED=0
#define USERTEST      (BUILDXX << 17) // May be equivalent to CHEATS=2, SPEED=1
#define DEMOREEL      (BUILDXX << 18) // May be equivalent to CHEATS=1, SPEED=2
#define SHIPPING      (BUILDXX << 19) // May be equivalent to CHEATS=0, SPEED=3

// Available linkage options.
#define STATICS       (BUILDXX << 20)
#define IMPORTS       (BUILDXX << 21)
#define EXPORTS       (BUILDXX << 22)

// Available architectures.
#define BIG           (BUILDXX << 23)
#define LITTLE        (BUILDXX << 24)
#define BITS32        (BUILDXX << 25)
#define BITS64        (BUILDXX << 26)

// Available compilers.
#define CLANG         (BUILDXX << 27)
#define GCC           (BUILDXX << 28)
#define MINGW         (BUILDXX << 29)
#define MSC           (BUILDXX << 30)
#define SNC           (BUILDXX << 31)

// Available CPUs.
#define ARM           (BUILDXX << 32)
#define MIPS          (BUILDXX << 33)
#define PPC           (BUILDXX << 34)
#define X86           (BUILDXX << 35)

// Available targets.
#define ANDROID       (BUILDXX << 36)
#define IOS           (BUILDXX << 37)
#define LINUX         (BUILDXX << 38)
#define OSX           (BUILDXX << 39)
#define POSIX         (BUILDXX << 40)
#define PS3           (BUILDXX << 41)
#define PS4           (BUILDXX << 42)
#define UNIX          (BUILDXX << 43)
#define WINDOWS       (BUILDXX << 44)
#define XBOX360       (BUILDXX << 45)
#define XBOXONE       (BUILDXX << 46)

// Others.
// (Maximum for MSVC15 is 47 (included) as 2^63=47<<16)
#define IMPLEMENTATION (BUILDXX << 47)

// Internal macros. Do not edit.
#define VERSION       (1ULL) // 0xFFFFULL)
#define BUILDXX       (VERSION+1)





#if BUILD & TESTS
#include <stdio.h>
int build_tests() {
    printf("isbig:%d\n", (*(short *)"\0\0\0\1" == 1));
    printf("[%c] DOCS\n", BUILD & DOCS ? 'x' : ' ' );
    printf("[%c] LOGS\n", BUILD & LOGS ? 'x' : ' ' );
    printf("[%c] TESTS\n", BUILD & TESTS ? 'x' : ' ' );
    printf("[%c] STATS\n", BUILD & STATS ? 'x' : ' ' );
    printf("[%c] CHECKS\n", BUILD & CHECKS ? 'x' : ' ' );
    printf("[%c] QUARKS\n", BUILD & QUARKS ? 'x' : ' ' );
    printf("[%c] ASSERTS\n", BUILD & ASSERTS ? 'x' : ' ' );
    printf("[%c] CONSOLES\n", BUILD & CONSOLES ? 'x' : ' ' );
    printf("[%c] STACKTRACES\n", BUILD & STACKTRACES ? 'x' : ' ' );
    printf("[%c] INFOS\n", BUILD & INFOS ? 'x' : ' ' );
    printf("[%c] WARNINGS\n", BUILD & WARNINGS ? 'x' : ' ' );
    printf("[%c] ERRORS\n", BUILD & ERRORS ? 'x' : ' ' );
    printf("[%c] FATALS\n", BUILD & FATALS ? 'x' : ' ' );
    printf("[%c] CRASHES\n", BUILD & CRASHES ? 'x' : ' ' );
    printf("[%c] INTERNAL\n", BUILD & INTERNAL ? 'x' : ' ' );
    printf("[%c] USERTEST\n", BUILD & USERTEST ? 'x' : ' ' );
    printf("[%c] DEMOREEL\n", BUILD & DEMOREEL ? 'x' : ' ' );
    printf("[%c] SHIPPING\n", BUILD & SHIPPING ? 'x' : ' ' );
    printf("[%c] STATICS\n", BUILD & STATICS ? 'x' : ' ' );
    printf("[%c] IMPORTS\n", BUILD & IMPORTS ? 'x' : ' ' );
    printf("[%c] EXPORTS\n", BUILD & EXPORTS ? 'x' : ' ' );
    printf("[%c] BIG\n", BUILD & BIG ? 'x' : ' ' );
    printf("[%c] LITTLE\n", BUILD & LITTLE ? 'x' : ' ' );
    printf("[%c] BITS32\n", BUILD & BITS32 ? 'x' : ' ' );
    printf("[%c] BITS64\n", BUILD & BITS64 ? 'x' : ' ' );
    printf("[%c] CLANG\n", BUILD & CLANG ? 'x' : ' ' );
    printf("[%c] GCC\n", BUILD & GCC ? 'x' : ' ' );
    printf("[%c] MINGW\n", BUILD & MINGW ? 'x' : ' ' );
    printf("[%c] MSC\n", BUILD & MSC ? 'x' : ' ' );
    printf("[%c] SNC\n", BUILD & SNC ? 'x' : ' ' );
    printf("[%c] ARM\n", BUILD & ARM ? 'x' : ' ' );
    printf("[%c] MIPS\n", BUILD & MIPS ? 'x' : ' ' );
    printf("[%c] PPC\n", BUILD & PPC ? 'x' : ' ' );
    printf("[%c] X86\n", BUILD & X86 ? 'x' : ' ' );
    printf("[%c] ANDROID\n", BUILD & ANDROID ? 'x' : ' ' );
    printf("[%c] IOS\n", BUILD & IOS ? 'x' : ' ' );
    printf("[%c] LINUX\n", BUILD & LINUX ? 'x' : ' ' );
    printf("[%c] OSX\n", BUILD & OSX ? 'x' : ' ' );
    printf("[%c] PS4\n", BUILD & PS4 ? 'x' : ' ' );
    printf("[%c] WINDOWS\n", BUILD & WINDOWS ? 'x' : ' ' );
    printf("[%c] XB1\n", BUILD & XB1 ? 'x' : ' ' );
    return 0;
}
#endif

#endif
