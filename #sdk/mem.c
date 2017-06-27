// # mem ######################################################################

static void *oom = 0;

#ifndef REALLOC
void *REALLOC(void *ptr, int size) {
    ptr = realloc(ptr, size);
    if( !ptr && size ) {
        panic("!out of memory");
    }
    return ptr;
}
#endif
#ifndef MALLOC
void *MALLOC(int size) {
    return REALLOC( 0, size );
}
#endif
#ifndef FREE
void FREE(void *ptr) {
    REALLOC( ptr, 0 );
}
#endif
#ifndef CALLOC
void* CALLOC(int n, int len) {
    return memset( MALLOC( n * len ), 0, n * len );
}
#endif
#ifndef STRDUP
char* STRDUP( const char *str ) {
    heap *mem = MALLOC( strlen(str) + 1 );
    strcpy( mem, str );
    return mem;
}
#endif
#ifndef MSIZE
#include <stdint.h>
#if defined(__GLIBC__)
#  include <malloc.h>
#  define MSIZE malloc_usable_size
#elif defined(__APPLE__) || defined(__FreeBSD__)
#  include <malloc/malloc.h>
#  define MSIZE  malloc_size
#elif defined(__ANDROID_API__)
#  include <malloc.h>
/*extern "C"*/ size_t dlmalloc_usable_size(void*);
#  define MSIZE dlmalloc_usable_size
#elif defined(_WIN32)
#  include <malloc.h>
#  define MSIZE _msize
#else
#  error Unsupported malloc_usable_size()
#endif
#endif

heap *memalloc( int len ) { // $
    return REALLOC( 0, len );
}
heap *memallocf( const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);
    int sz = vsnprintf( 0, 0, fmt, vl );
    void *ptr = memalloc( sz + 1 );
    vsnprintf( (char *)ptr, sz, fmt, vl );
    va_end(vl);
    return (char *)ptr;
}
heap *memdup( const void *src, int len ) {
    void *dst = 0;
    len = len ? len : (strlen((const char *)src) + 1);
    dst = memalloc( len );
    if( dst ) {
        memcpy( dst, src, len );
    }
    return dst;
}

static THREAD_LOCAL uint8_t *stack_mem = 0;
static THREAD_LOCAL uint64_t stack_size = 0, stack_max = 0;
stack *tmpalloc(int bytes) {
    if( bytes < 0 ) {
        if( stack_size > stack_max ) stack_max = stack_size;
        return (stack_size = 0), NULL;
    }
    if( !stack_mem ) stack_mem = memalloc( 4 * MiB );
    return &stack_mem[ (stack_size += bytes) - bytes ];
    /*
    inc_stats('pile', bytes);
    return &(vamem = (uint8_t*)realloc( vamem, vasz += bytes))[ vasz - bytes ];
    */
}
stack *tmpallocf( const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);
    int sz = vsnprintf( 0, 0, fmt, vl );
    void *ptr = tmpalloc( sz + 1 );
    vsnprintf( (char *)ptr, sz, fmt, vl );
    va_end(vl);
    return (char *)ptr;
}

#define free                do_not_use_free
#define malloc              do_not_use_malloc
#define realloc             do_not_use_realloc
#define calloc              do_not_use_calloc
#define malloc_usable_size  do_not_use_malloc_usable_size
