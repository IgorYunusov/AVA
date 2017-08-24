#pragma once
#include <stdio.h>

static
void hexdump( FILE *fp, const void *ptr, unsigned len, int width ) {
    unsigned char *data = (unsigned char*)ptr;
    for( unsigned jt = 0; jt < len; jt += width ) {
        printf( "; %p ", (void *)jt );
        for( unsigned it = jt, next = it + width; it < len && it < next; it += 2 ) {
            printf( "%02x%02x %s", (unsigned char)data[it], (unsigned char)data[it+1], &" \n\0...\n"[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
        printf( "; %05d ", jt );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            printf( " %c %s", (signed char)data[it] >= 32 ? (signed char)data[it] : (signed char)'.', &" \n\0...\n"[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
    }
}
static
void hexdump16( FILE *fp, const void *ptr, unsigned len ) {
    hexdump( fp, ptr, len, 16 );
}

#ifdef DEMO
int main() {
    hexdump16( stdout, "hello world", sizeof("hello world"));
}
#endif
