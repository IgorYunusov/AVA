#include <stdarg.h>
char *vl( const char *fmt, va_list lst ); //$
char *va( const char *fmt, ... ); //$

#ifndef THREAD_LOCAL
#define THREAD_LOCAL __declspec(thread)
#endif
#ifndef REALLOC
#define REALLOC realloc
#endif
#ifndef panic
#include <assert.h>
#define panic(str) assert(!str)
#endif

static THREAD_LOCAL int vl_index = 0;
static THREAD_LOCAL char *vl_buffer[16] = {0};
char *vl( const char *fmt, va_list lst ) { //$
    int idx = (++vl_index) % 16;
    int sz = 1 + vsnprintf(0, 0, fmt, lst);
    if( sz > 0 ) {
        vl_buffer[idx] = (char *)REALLOC( vl_buffer[idx], sz );
        if( vl_buffer[idx] ) {
            vsnprintf( vl_buffer[idx], sz, fmt, lst );
        }
    }
    int ok = ( sz > 0 && vl_buffer[idx] );
    if(!ok) panic("!va()/vl(): out of mem");

    static char nil;
    return ok ? vl_buffer[idx] : (nil = 0, &nil);
}
char *va( const char *fmt, ... ) { //$
    va_list lst;
    va_start(lst, fmt);
    char *rc = vl(fmt, lst);
    va_end(lst);
    return rc;
}
