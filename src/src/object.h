// @todo there are 256 custom types
// @todo 256 arenas too? heap based?
/*
struct pointer {
    uint32_t base;               // if base is ram[], offset could just be uint64 ?
    uint32_t offset;
};
// or...
struct pointer {
    uint32_t base : 8;               // base index for 256 custom types; 256 arenas
    uint32_t slot : 24;              // slot index 2^24 = index max 16M
};
*/
/*
#include <stdint.h>
#include <stddef.h>

#define tag(x) ((ptrdiff_t)&Tag##x)
#define is(x, T) ((x)->tag == tag(T))
#define as(x, T) ((struct T *)variant_cast((void *)(x), tag(T)))

static inline void*
variant_cast(void * variant_ptr, ptrdiff_t desired_tag) {
  ptrdiff_t * variant_tag = (ptrdiff_t *)variant_ptr;
  assert(*variant_tag == desired_tag);
  return (void *)((char *)variant_ptr + sizeof(ptrdiff_t));
}
*/

#include <stdint.h>
#include <stdlib.h>

// # generics

void*   heap( int type_, int num ); // allocate new object
void    drop( void **ptr );         // free allocated object

void*   new( int type, int num );   // allocate then init object
void    del( void **ptr );          // destroy then free object

 int    type( void *ptr );
 int    unit( void *ptr );
 int    size( void *ptr );

void    init( void *ptr );
void    echo( void *ptr );
void    quit( void *ptr );

// # implementation

typedef static_assert_64bit_ptr[ sizeof(void*) == 8 ];

typedef void* any;
any     object( int type, int func, ... );

void *heap( int type_, int num ) {
    uint64_t size = ((uint64_t)object(type_, 'unit')) * (num > 0 ? num : 1);
    uint64_t meta = ((uint64_t)type_) << 32ULL | size;
    uint64_t *ptr = (uint64_t*)realloc( 0, sizeof(meta) + size );
    *ptr++ = meta;
    return ptr;
}

void drop( void **ptr ) {
    uint64_t *ptr64 = *ptr;
    realloc( (void*)(ptr64-1), 0 );
    (*ptr) = 0;
}

void *new( int type_, int num ) {
    void *ptr = heap(type_, num);
    init( ptr );
    return ptr;
}

void del( void **ptr ) {
    quit( *ptr );
    drop( ptr );
}

// INTROSPECTION: size (bytes), size (unit), type4, meta ?,

int size( void *ptr ) {
    uint64_t *ptr64 = ptr;
    return (int)( ptr64[-1] & 0xFFFFFFFFull );
}

int type( void *ptr ) {
    uint64_t *ptr64 = ptr;
    return (int)( ptr64[-1] >> 32ULL );
}

int     unit( void *ptr ) { return (int)object( type(ptr), 'unit', ptr ); }

// IDENTIFICATION: id, name[16], hash[8], meta, version,
int        self( void *ptr ) { return (int)object( type(ptr), 'self', ptr); }
uint64_t   hash( void *ptr ) { return (uint64_t)object( type(ptr), 'hash', ptr); }
const char*name( void *ptr ) { return (const char*)object( type(ptr), 'name', ptr); }
int        head( void *ptr ) { return (int)object( type(ptr), 'head', ptr); }
//
void    echo( void *ptr ) { object( type(ptr), 'echo', ptr ); }
void    init( void *ptr ) { object( type(ptr), 'init', ptr ); }
void    quit( void *ptr ) { object( type(ptr), 'quit', ptr ); }
// SORTING: ==, <,
int     iseq( void *ptr, void *ptr2 ) { return (int)object( type(ptr), 'iseq', ptr, ptr2 ); }
int     islt( void *ptr, void *ptr2 ) { return (int)object( type(ptr), 'islt', ptr, ptr2 ); }

/////


#include <stdio.h>
#include <stdint.h>

#include <stdarg.h>
any object( int type, int func, ... ) {
    union {
        int num, *nump;
        float flt, *fltp;
    } in, out = {0};
    uint64_t meta = ((uint64_t)type << 32ULL) | func;
    va_list vl;
    va_start(vl, func);
    switch( meta ) {
        default:
            // if extended type...
            // if super type...
            // else
#ifdef OBJ_VERBOSE
            printf("ERROR! unknown func %c%c%c%c::%c%c%c%c\n",
                (type >> 24) & 0xff,
                (type >> 16) & 0xff,
                (type >>  8) & 0xff,
                (type >>  0) & 0xff,
                (func >> 24) & 0xff,
                (func >> 16) & 0xff,
                (func >>  8) & 0xff,
                (func >>  0) & 0xff
            );
#endif
        // if built-in type...
#       define DEFINE(a,b) break; case ( ((uint64_t)a << 32ULL) | (b) ):
        //#include "objvec2.inl"
        DEFINE('vec2','init') { in.fltp = va_arg(vl, float*); puts("creating vec2"); in.fltp[0] = -1, in.fltp[1] = -1; }
        DEFINE('vec2','quit') { puts("destroying vec2"); }
        DEFINE('vec2','unit') { out.num = 2 * sizeof(float); }
        DEFINE('vec2','echo') { in.fltp = va_arg(vl, float*);
            printf("%f %f\n", in.fltp[0], in.fltp[1]);
        }
        DEFINE('vec2','iseq') { float *u = va_arg(vl, float*); float *v = va_arg(vl, float*);
            out.num = u[0] == v[0] && u[1] == v[1];
        }
        // vec3
        DEFINE('vec3','unit') { out.num = 3 * sizeof(float); }
    }
    va_end(vl);
    return (any)out.num;
}

// ---

int main() {
    float *ptr = new( 'vec2', 2 );
    float *ptr2 = new( 'vec2', 1 );
    printf("address %p, type %x, size %d, unit %d\n", ptr, type(ptr), size(ptr), unit(ptr));
    printf("address %p, type %x, size %d, unit %d\n", ptr2, type(ptr2), size(ptr2), unit(ptr2));
    if( size(ptr) ) echo( ptr );
    ptr[0] = 1234;
    ptr[1] = 5678;
    if( size(ptr) ) echo( ptr );
    printf("equal %d\n", iseq(ptr,ptr2));
    printf("equal %d\n", iseq(ptr,ptr));
    del( &ptr );
    printf("address %p\n", ptr);

//  extsize =
    ptr = new( 'vec3', 1 );
    printf("address %p, type %x, size %d, unit %d\n", ptr, type(ptr), size(ptr), unit(ptr));
    if( size(ptr) ) echo( ptr );
    del( &ptr );
}
