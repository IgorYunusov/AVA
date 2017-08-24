#include <stdint.h>

int64_t strint(const char *s ) {
    int64_t v = 0, n = 1;
    if( s ) {
        while( *s == '-' ) n *= -1, s++;
        while( *s >= '0' && *s <= '9') v = (v * 10) + *s++ - '0';
    }
    return n * v;
};

int strpos( int pos, int size ) {
    if( size ) {
        return pos >= 0 ? pos % size : size - 1 + ((pos+1) % size);
    }
    return -1; // will crash if accessed
}


