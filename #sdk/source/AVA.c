#  if !defined(API) && defined(__cplusplus)
#   define API extern "C" __declspec(dllexport)
#elif !defined(API)
#   define API            __declspec(dllexport)
#endif

#include <stdio.h>

#include "AVA.h"

void start() {
    puts(";; AVA - " __DATE__ " " __TIME__);
}

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
    typedef char is_64bit_test[ sizeof(void *) == 8 ];
#else
    enum { is_64bit = 0 };
    typedef char is_64bit_test[ sizeof(void *) == 4 ];
#endif
