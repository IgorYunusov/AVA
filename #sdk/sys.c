#if $win(1,0)
#include <winsock2.h>
#include <windows.h>
#pragma warning (push)
#pragma warning (disable:4091) // microsoft header with warnings. wonderful.
#include <dbghelp.h>
#pragma warning (pop)
#endif

// # thread local (@todo: move to builtins) ###################################

#ifdef __GNUC__
#   define THREAD_LOCAL __thread
#elif __STDC_VERSION__ >= 201112L
#   define THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER) | defined(__INTEL_COMPILER)
#   define THREAD_LOCAL __declspec(thread)
#else
#   error Cannot define thread_local
#endif

// # endian ###################################################################

#if defined(__BIG_ENDIAN__) || defined(__sgi)     || \
    defined(__PPC__)        || defined(__ppc__)   || \
    defined(__sparc)        || defined(__sparc__)
    enum { is_little = 0 };
#else
    enum { is_little = 1 };
#endif

// # arch #####################################################################

#if defined(__x86_64__) || defined(_M_X64)        || \
    defined(__ia64__)   || defined(__powerpc64__) || \
    defined(__s390__)   || defined(__s390x__)     || \
    defined(__alpha__)
    enum { is_64bit = 1 };
    typedef char static_assert_64bit[ sizeof(void *) == 8 ];
#else
    enum { is_64bit = 0 };
    typedef char static_assert_32bit[ sizeof(void *) == 4 ];
#endif

int bits() {
    if( INTPTR_MAX >= INT64_MAX ) return 64;
    if( INTPTR_MAX >= INT32_MAX ) return 32;
    return 0;
}
