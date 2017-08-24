#pragma once

typedef int (*streamer)( void *dst, int dstlen, const void *src, int srclen, void *usrdata );
typedef int (*pipeline)( void *dst, int dstlen, const void *src, int srclen, void *usrdata, int numstreams, streamer *streamers );

static int is_stream_reading( void *dst, int dstlen, const void *src, int srclen ) {
    return src && srclen;
}
static int is_stream_writing( void *dst, int dstlen, const void *src, int srclen ) {
    return dst && dstlen;
}
static int is_stream_testing( void *dst, int dstlen, const void *src, int srclen ) {
    return src && !srclen;
}
static int is_stream_bulking( void *dst, int dstlen, const void *src, int srclen ) {
    return dst && !dstlen;
}

#include <stdlib.h>

static int pipeliner( void *dst, int dstlen, const void *src, int srclen, void *usrdata, int numstreams, streamer *streamers ) {
    int ret = 0;
    char *cpy1 = numstreams > 0 ? (char*)malloc(dstlen) : (char*)0;
    char *cpy2 = numstreams > 1 ? (char*)malloc(dstlen) : (char*)0;
    for( int i = 0; i < numstreams; ++i ) {
        void *wr = (i & 1 ? cpy2 : cpy1);
        const void *rd = (const void *)(i & 1 ? cpy1 : cpy2);
        ret = streamers[i]( i == (numstreams-1) ? dst : wr, dstlen, i == 0 ? src : rd, srclen, usrdata );
        srclen = ret;
    }
    free( cpy1 );
    free( cpy2 );
    return ret;
}


#ifdef DEMO
#include <stdio.h>
#include <stdlib.h>

int my_printer( void *dst, int dstlen, const void *src, int srclen, void *usdata ) {
    return printf("%.*s\n", srclen, (const char*)src );
}
int my_copier( void *dst, int dstlen, const void *src, int srclen, void *usdata ) {
    int written = 0;
    if( srclen < dstlen ) memcpy( dst, src, written = srclen );
    return written;
}
int my_lowercaser( void *dst, int dstlen, const void *src, int srclen, void *usdata ) {
    char *dst8 = (char*)dst;
    const char *src8 = (const char*)src;
    while( *src8 ) {
        *dst8++ = *src8++ | 32;
    }
    return dst8 - (char *)dst;
}
int my_l33ter( void *dst, int dstlen, const void *src, int srclen, void *usdata ) {
    char *dst8 = (char*)dst;
    const char *src8 = (const char*)src;
    while( *src8++ ) {
        *dst8++ = src8[-1] == 'e' ? '3' : src8[-1];
    }
    return dst8 - (char *)dst;
}
int my_appender( void *dst, int dstlen, const void *src, int srclen, void *usdata ) {
    memcpy( dst, src, srclen );
    ((char*)dst)[ srclen ] = '+';
    return srclen + 1;
}
int my_pipeline( void *dst, int dstlen, const void *src, int srclen, void *usrdata, int numstreams, streamer *streamers ) {
    int ret = 0;
    char *cpy1 = numstreams > 0 ? (char*)malloc(dstlen) : (char*)0;
    char *cpy2 = numstreams > 1 ? (char*)malloc(dstlen) : (char*)0;
    for( int i = 0; i < numstreams; ++i ) {
        void *wr = (i & 1 ? cpy2 : cpy1);
        const void *rd = (const void *)(i & 1 ? cpy1 : cpy2);
        ret = streamers[i]( i == (numstreams-1) ? dst : wr, dstlen, i == 0 ? src : rd, srclen, usrdata );
        srclen = ret;
    }
    free( cpy1 );
    free( cpy2 );
    return ret;
}

int main() {
    streamer worker = 0;
    streamer reader = 0;
    streamer writer = 0;

    char src[128] = "Hello world", dst[256] = {0};
    streamer pipe1[] = { my_printer };
    my_pipeline( 0, 0, src, 128, 0, 1, pipe1 );

    streamer pipe2[] = { my_copier, my_lowercaser, my_l33ter, my_appender, my_appender, my_printer };
    my_pipeline( dst, 256, src, 128, 0, 6, pipe2 );
}

#endif
