#pragma once
#include <string.h>
#include <stdlib.h>

typedef char heap;

int strlead( const char *string, const char *substr ) {
#if 1
    return strncmp(string, substr, strlen(substr)) == 0;
#else
    int ol = strlen(string);
    int tl = strlen(substr);
    return ol >= tl && 0 == memcmp( string, substr, tl );
#endif
}
int streq( const char *string, const char *substr ) {
    return !strcmp( string, substr );
}
int streqi( const char *string, const char *substr ) { // @todo: test me
    while( *string && *substr ) {
        int eqi = (*string++ | 32) == (*substr++ | 32);
        if( !eqi ) return 0;
    }
    return *string == 0 && *substr == 0;
}
heap *strappend( heap **buf, const char *src ) {
    char *dst = *buf;
    int dlen = (dst ? strlen(dst) : 0);
    int slen = strlen(src);
    dst = (heap *)realloc( dst, dlen + slen + 1 );
    memcpy( &dst[dlen], src, slen + 1 );
    return *buf = dst;
}

/*
int main() {
    printf("%d\n", streqi("hello", "hello"));
    printf("%d\n", streqi("hello", "hellO"));
    printf("%d\n", streqi("hello", "hello2"));
}
*/
