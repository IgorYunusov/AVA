//#ifndef AVA_IMPLEMENTATION
//#error  Internal file. Do not include.
//#endif

#if defined(ANDROID)
#define $and(...) __VA_ARGS__
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...)
#define $web(...)
#define $win(...)
#define $xb1(...)
#elif defined(IOS)
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
#elif defined(WEBGL)
#define $and(...)
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...)
#define $web(...) __VA_ARGS__
#define $win(...)
#define $xb1(...)
#elif defined(linux)
#define $and(...)
#define $ios(...)
#define $lin(...) __VA_ARGS__
#define $osx(...)
#define $ps4(...)
#define $web(...)
#define $win(...)
#define $xb1(...)
#elif defined(XBOXONE)
#define $and(...)
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...)
#define $web(...)
#define $win(...)
#define $xb1(...) __VA_ARGS__
#elif defined(PS4)
#define $and(...)
#define $ios(...)
#define $lin(...)
#define $osx(...)
#define $ps4(...) __VA_ARGS__
#define $web(...)
#define $win(...)
#define $xb1(...)
#elif defined(_WIN32)
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

#if   defined(__clang__)
#define $clang(...)     __VA_ARGS__
#define $gcc(...) 
#define $msc(...)
#elif defined(__GNUC__)
#define $clang(...)
#define $gcc(...)       __VA_ARGS__
#define $msc(...)
#elif defined(_MSC_VER)
#define $clang(...)
#define $gcc(...)
#define $msc(...)       __VA_ARGS__
#else
#error Compiler not supported
#endif

#if defined(NDEBUG) || defined(_NDEBUG)
#define $release(...)  __VA_ARGS__
#define $debug(...)
#else
#define $debug(...)    __VA_ARGS__
#define $release(...)
#endif

#if defined(SHIPPING)
#define $shipping(...) __VA_ARGS__
#define $devel(...)
#else
#define $shipping(...)
#define $devel(...)    __VA_ARGS__
#endif
