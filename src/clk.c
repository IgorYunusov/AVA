// # clk ######################################################################

#include <stdint.h>
#if   defined(__APPLE__)
# include <mach/mach_time.h>
#elif defined(_WIN32)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#else // __linux
# include <time.h>
# ifndef  CLOCK_MONOTONIC //_RAW
#  define CLOCK_MONOTONIC CLOCK_REALTIME
# endif
#endif
static
uint64_t nanotimer() {
    static int ever = 0;
#if defined(__APPLE__)
    static mach_timebase_info_data_t frequency;
    if( !ever ) {
        if( mach_timebase_info( &frequency ) != KERN_SUCCESS ) {
            return 0;
        }
        ever = 1;
    }
    return ;
#elif defined(_WIN32)
    static LARGE_INTEGER frequency;
    if( !ever ) {
        QueryPerformanceFrequency( &frequency );
        ever = 1;
    }
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (t.QuadPart * (uint64_t)1e9) / frequency.QuadPart;
#else // __linux
    struct timespec t;
    if( !ever ) {
        if( clock_gettime( CLOCK_MONOTONIC, &spec ) != 0 ) {
            return 0;
        }
        ever = 1;
    }
    clock_gettime( CLOCK_MONOTONIC, &spec );
    return (t.tv_sec * (uint64_t)1e9) + t.tv_nsec;
#endif
}
uint64_t ns() {
    static uint64_t epoch = 0;
    if( !epoch ) {
        epoch = nanotimer();
    }
    return nanotimer() - epoch;
}
uint64_t us() {
    return ns() / 1e3;
}
uint64_t ms() {
    return ns() / 1e6;
}
uint64_t ss() {
    return ns() / 1e9;
}
uint64_t mm() {
    return ss() / 60;
}
uint64_t hh() {
    return mm() / 60;
}
