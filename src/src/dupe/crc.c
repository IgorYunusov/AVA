// # crc ######################################################################

#pragma once
#include <stdint.h>
uint64_t crc64(const void *ptr, size_t size) {
    // based on public domain code by Lasse Collin
    static uint64_t crc64_table[256];
    static uint64_t poly64 = UINT64_C(0xC96C5795D7870F42);
    if( poly64 ) {
        for( int b = 0; b < 256; ++b ) {
            uint64_t r = b;
            for( int i = 0; i < 8; ++i ) {
                r = r & 1 ? (r >> 1) ^ poly64 : r >> 1;
            }
            crc64_table[ b ] = r;
        }
        poly64 = 0;
    }
    const char *buf = (const char *)ptr;
    uint64_t crc = ~0ull; // ~crc;
    while( size != 0 ) {
        crc = crc64_table[*buf++ ^ (crc & 0xFF)] ^ (crc >> 8);
        --size;
    }
    return ~crc;
}

uint64_t ptr64(void *addr) {
    return (uint64_t)addr;
}
uint64_t str64(const char* str) {
   uint64_t hash = 0; // fnv1a: 14695981039346656037ULL;
   while( *str ) {
        hash = ( *str++ ^ hash ) * 131; // fnv1a: 0x100000001b3ULL;
   }
   return hash;
}
uint64_t hash_int(uint64_t key) {
    // Thomas Wang's 64 bit Mix Function (public domain) http://www.cris.com/~Ttwang/tech/inthash.htm
    key += ~(key << 32);
    key ^=  (key >> 22);
    key += ~(key << 13);
    key ^=  (key >>  8);
    key +=  (key <<  3);
    key ^=  (key >> 15);
    key += ~(key << 27);
    key ^=  (key >> 31);
    return key;
/*  also,
    key = (~key) + (key << 21); // key = (key << 21) - key - 1;
    key = key ^ (key >>> 24);
    key = (key + (key << 3)) + (key << 8); // key * 265
    key = key ^ (key >>> 14);
    key = (key + (key << 2)) + (key << 4); // key * 21
    key = key ^ (key >>> 28);
    key = key + (key << 31);
    return key;
*/
}
uint64_t hash_vec3(int pt[3]) {
    uint64_t x = hash_int(pt[0]);
    uint64_t y = hash_int(pt[1]);
    uint64_t z = hash_int(pt[2]);
    return x ^ y ^ z;
}
/* hash_dbl() {
    union { uint64_t i; double d; } u; u.d = dbl;
    return hash_int( u.i );
}
*/

