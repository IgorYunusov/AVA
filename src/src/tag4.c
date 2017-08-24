// AVA - TAG4 specification.
//
// - [x] compact 32-bit encoding.
// - [x] name: char[3] factory name string (human-readable)
// - [x] head: char[1] factory version number (human-readable)
// - [x] swap: endian mismatch if first byte is digit.
// - [x] spec: spec revision encoded into 7 uppercase bits of 'factory' field (obfuscated)
//     AAA -> v000, aaa -> v111, aAa -> v101, aaA -> v110, etc...
//
// Examples:
// '2XET' -> wrong endian, first char is digit (byte-swap required)
// 'TEX2' -> endian ok, factory 'TEX' v2, spec v0 (0 == TEX b000 lettercase)
// 'TEx3' -> endian ok, factory 'TEX' v3, spec v1 (1 == TEx b001 lettercase)
// 'teX4' -> endian ok, factory 'TEX' v4, spec v6 (6 == teX b110 lettercase)
//
// Note:
// uint32_t readable_way = 'geo4';
// uint32_t portable_way = TAG4(geo4);

#pragma once
#include <stdint.h>

typedef uint32_t tag4;

static uint32_t tagtype( uint32_t tag4 ) {
    return (tag4 | (0x20202020)) >> 8;
}

static uint32_t taghead( uint32_t tag4 ) {
    return (tag4 & 0xff) - '0';
}

static uint32_t tagswap( uint32_t tag4 ) {
    return !(((tag4 & 0xff) - '0') <= 9);
}

static uint32_t tagspec( uint32_t tag4 ) {
    return ((tag4 >> 27) & 0x4) | ((tag4 >> 20) & 0x2) | ((tag4 >> 13) & 0x1);
}

// debug only. not thread-safe.
static const char *const tagdump( uint32_t tag4 ) {
    static char local[5] = {0};
    local[0] = tag4 >> 24;
    local[1] = tag4 >> 16;
    local[2] = tag4 >>  8;
    local[3] = tag4 >>  0;
    return local;
}

// util
#define TAG4(abcd) (((#abcd[0])<<24)|((#abcd[1])<<16)|((#abcd[2])<<8)|(#abcd[3]))
#define TAG3(abc)  TAG4(abc##0)



#ifdef DEMO
#include <stdio.h>
#include <assert.h>
int main() {
    assert( tagtype('tEx3') == 'tex' );    // is type a texture?
    assert( taghead('tEx3') == 3 );        // is type in 3rd head?
    assert( tagspec('tEx3') == (4|0|1) );  // is spec v5 being used?
    assert( tagswap('tEx3') == 0 );        // is endian swapped?
    assert( tagswap('3xEt') == 1 );        // is endian swapped?

    printf( "view: %s\n", tagdump(TAG4(tEx3)));
    printf( "view: %s\n", tagdump(TAG3(tEx)|3));
    printf( "view: %s\n", tagdump('tEx3') );
}
#endif
