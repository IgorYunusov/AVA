//#ifndef AVA_IMPLEMENTATION
//#error  Internal file. Do not include.
//#endif

#pragma once
#define $shipping(T,F)  F
#define $devel(T,F)     F
#define $release(T,F)   F
#define $debug(T,F)     F
#define $clang(T,F)     F
#define $gcc(T,F)       F
#define $msc(T,F)       F
#define $mingw(T,F)     F
#define $snc(T,F)       F
#define $arm(T,F)       F
#define $mips(T,F)      F
#define $ppc(T,F)       F
#define $x86(T,F)       F
#define $big(T,F)       F
#define $little(T,F)    F
#define $web(T,F)       F
#define $and(T,F)       F
#define $ios(T,F)       F
#define $osx(T,F)       F
#define $xb1(T,F)       F
#define $ps4(T,F)       F
#define $lin(T,F)       F
#define $win(T,F)       F

// build type
#if defined(SHIPPING)
#undef  $shipping
#define $shipping(T,F) T
#else
#undef  $devel
#define $devel(T,F)    T
#endif

// build optimizations
#if defined(NDEBUG) || defined(_NDEBUG)
#undef  $release
#define $release(T,F)  T
#else
#undef  $debug
#define $debug(T,F)    T
#endif

// compilers
#if   defined(__clang__)
#undef  $clang
#define $clang(T,F)  T
#elif defined(__GNUC__)
#undef  $gcc
#define $gcc(T,F)    T
#elif defined(_MSC_VER)
#undef  $msc
#define $msc(T,F)    T
#elif defined(__MINGW32__)
#undef  $mingw
#define $mingw(T,F)  T
#elif defined(__SNC__)
#undef  $snc
#define $snc(T,F)    T
#else
#error Compiler not supported
#endif

// architecture
#if   defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB)
#undef  $arm
#define $arm(T,F)  T
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
#undef  $mips
#define $mips(T,F) T
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(_M_PPC) || defined(_ARCH_PPC) || defined(__PPCGECKO__) || defined(__PPCBROADWAY__) || defined(_XENON)
#undef  $ppc
#define $ppc(T,F)  T
#elif defined(_M_IX86) || defined(_X86_) || defined(__i386__) || defined(i386) || defined(__x86__) || defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__) || defined(_M_X64)
#undef  $x86
#define $x86(T,F)  T
#else
#error Architecture not supported
#endif

// endianness
#if (defined(__BYTE_ORDER__)&&(__BYTE_ORDER__==__ORDER_BIG_ENDIAN__)) || defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN) || $arm(1,0) || $mips(1,0)
#undef  $big
#define $big(T,F)    T
#else
#undef  $little
#define $little(T,F) T
#endif

// target
#if   defined(EMSCRIPTEN)
#undef  $web
#define $web(T,F) T
#elif defined(__ANDROID_API__)
#undef  $and
#define $and(T,F) T
#elif defined(__APPLE__) && defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE)
#undef  $ios
#define $ios(T,F) T
#elif defined(__APPLE__)
#undef  $osx
#define $osx(T,F) T
#elif defined(_DURANGO) // 360: _XBOX_VER
#undef  $xb1
#define $xb1(T,F) T
#elif defined(__ORBIS__)
#undef  $ps4
#define $ps4(T,F) T
#elif defined(__linux) // also: __LINUX__|__linux__|PLATFORM_POSIX|PLATFORM_UNIX ; other oses: __posix, __unix
#undef  $lin
#define $lin(T,F) T
#elif defined(_WIN32) // && !_DURANGO && !_XBOX_VER
#undef  $win
#define $win(T,F) T
#else
#error Target OS not supported
#endif

// builtins

#define builtin(name) builtin_##name

#if $msc(1,0)
#define builtin_const         
#define builtin_forceinline   __forceinline
#define builtin_inline        __inline
#define builtin_likely(x)     (x)
#define builtin_pack(struct_) __pragma(pack(push, 1)) struct_ __pragma(pack(pop))
#define builtin_pure          
#define builtin_unlikely(x)   (x)
#define builtin_unreachable()  __assume(0)
#else
#define builtin_const         __attribute__((const))
#define builtin_forceinline   __attribute__((always_inline))
#define builtin_inline        inline
#define builtin_likely(x)     __builtin_expect(!!(x), 1)
#define builtin_pack(struct_) struct_ __attribute__((__packed__))
#define builtin_pure          __attribute__((pure))
#define builtin_unlikely(x)   __builtin_expect(!!(x), 0)
#define builtin_unreachable() __builtin_unreachable()
#endif

/*
builtin(inline)
int main() {
    if( builtin(likely)( 1 >= 1 ) ) {
        exit(1);
    }
    builtin(unreachable);
}
*/
