#pragma once
#include <string.h>

// - Split 'src' string into/up to 'avail' pair of tokens ({begin,end}) delimited by 'delim' characters.
//     Returns zero if more pairs are needed.

static
int strsplit(const char **tokens, int avail, const char *src, char *delim) {
    while( *src && avail > 2 ) {
        int n = strcspn( src += strspn(src, delim), delim );
        *tokens++ = n ? src : (const char *)0;
        *tokens++ = n + ( n ? src : (const char *)0 );
        avail -= 2;
        src += n;
    }
    *tokens = 0;
    return avail > 2;
}

#ifdef DEMO

int main() {
    const char *str = "There is a lady who's sure, all that glitter is gold...";
    const char *tokens[128];
    if( strsplit( tokens, 128, str, " ,." ))
    for( int i = 0; tokens[i]; i += 2 ) {
        printf("'%.*s'\n", tokens[i+1] - tokens[i+0], tokens[i]);
    }
    if( strsplit( tokens, 128, str, "$" ))
    for( int i = 0; tokens[i]; i += 2 ) {
        printf("'%.*s'\n", tokens[i+1] - tokens[i+0], tokens[i]);
    }
}

#endif
