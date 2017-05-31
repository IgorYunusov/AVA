//#ifndef AVA_IMPLEMENTATION
//#error  Internal file. Do not include.
//#endif

// build type
#if defined(SHIPPING)
#define $shipping(...) __VA_ARGS__
#define $devel(...)
#else
#define $shipping(...)
#define $devel(...)    __VA_ARGS__
#endif

// build optimizations
#if defined(NDEBUG) || defined(_NDEBUG)
#define $release(...)  __VA_ARGS__
#define $debug(...)
#else
#define $debug(...)    __VA_ARGS__
#define $release(...)
#endif

// compilers
#if   defined(__clang__)
#define $clang(...)  __VA_ARGS__
#define $gcc(...) 
#define $mingw(...)
#define $msc(...)
#define $snc(...)
#elif defined(__GNUC__)
#define $clang(...)
#define $gcc(...)    __VA_ARGS__
#define $mingw(...)
#define $msc(...)
#define $snc(...)
#elif defined(_MSC_VER)
#define $clang(...)
#define $gcc(...) 
#define $mingw(...)
#define $msc(...)    __VA_ARGS__
#define $snc(...)
#elif defined(__MINGW32__)
#define $clang(...)
#define $gcc(...) 
#define $mingw(...)  __VA_ARGS__
#define $msc(...)
#define $snc(...)
#elif defined(__SNC__)
#define $clang(...)
#define $gcc(...) 
#define $mingw(...)
#define $msc(...)
#define $snc(...)    __VA_ARGS__
#else
#error Compiler not supported
#endif

// architecture
#if   defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB)
#define $arm(...)  __VA_ARGS__
#define $mips(...)
#define $ppc(...)
#define $x86(...)
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
#define $arm(...)
#define $mips(...) __VA_ARGS__
#define $ppc(...)
#define $x86(...)
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(_M_PPC) || defined(_ARCH_PPC) || defined(__PPCGECKO__) || defined(__PPCBROADWAY__) || defined(_XENON)
#define $arm(...)
#define $mips(...)
#define $ppc(...)  __VA_ARGS__
#define $x86(...)
#elif defined(_M_IX86) || defined(_X86_) || defined(__i386__) || defined(i386) || defined(__x86__) || defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__) || defined(_M_X64)
#define $arm(...)
#define $mips(...)
#define $ppc(...)
#define $x86(...)  __VA_ARGS__
#else
#error Architecture not supported
#endif

// endianness
#if (defined(__BYTE_ORDER__)&&(__BYTE_ORDER__==__ORDER_BIG_ENDIAN__)) || defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN) || ($arm(1)+0) || ($mips(1)+0)
#define $big(...)    __VA_ARGS__
#define $little(...) 
#else
#define $big(...)
#define $little(...) __VA_ARGS__
#endif

// target
#if   defined(EMSCRIPTEN)
#define $and(...)
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...)
#define $web(...) __VA_ARGS__
#define $win(...)
#define $xb1(...)
#elif defined(__ANDROID_API__)
#define $and(...) __VA_ARGS__
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...)
#define $web(...)
#define $win(...)
#define $xb1(...)
#elif defined(__APPLE__) && defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE)
#define $and(...)
#define $ios(...) __VA_ARGS__
#define $lin(...)
#define $osx(...)
#define $ps4(...)
#define $web(...)
#define $win(...)
#define $xb1(...)
#elif defined(__APPLE__)
#define $and(...)
#define $ios(...)
#define $lin(...)
#define $osx(...) __VA_ARGS__
#define $ps4(...)
#define $web(...)
#define $win(...)
#define $xb1(...)
#elif defined(_DURANGO) // 360: _XBOX_VER
#define $and(...)
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...)
#define $web(...)
#define $win(...)
#define $xb1(...) __VA_ARGS__
#elif defined(__ORBIS__)
#define $and(...)
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...) __VA_ARGS__
#define $web(...)
#define $win(...)
#define $xb1(...)
#elif defined(__linux) // also: __LINUX__|__linux__|PLATFORM_POSIX|PLATFORM_UNIX ; other oses: __posix, __unix
#define $and(...)
#define $ios(...)
#define $lin(...) __VA_ARGS__
#define $osx(...)
#define $ps4(...)
#define $web(...)
#define $win(...)
#define $xb1(...)
#elif defined(_WIN32) // && !_DURANGO && !_XBOX_VER
#define $and(...)
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...)
#define $web(...)
#define $win(...) __VA_ARGS__
#define $xb1(...)
#else
#error Target OS not supported
#endif

// builtins

#define builtin(name) builtin_##name

#if $msc(1)+0
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
