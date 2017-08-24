#pragma once
#include <stdint.h>

// vbyte, varint (signed)
static uint64_t packi64v( uint8_t *buffer, int64_t value_ ) {
    /* convert sign|magnitude to magnitude|sign */
    uint64_t value = (uint64_t)((value_ >> 63) ^ (value_ << 1));
    /* encode unsigned : 7-bit packing. MSB terminates stream */
    const uint8_t *buffer0 = buffer;
    do {
        *buffer++ = (uint8_t)( 0x80 | (value & 0x7f) );
        value >>= 7;
    } while( value > 0 );
    *(buffer-1) ^= 0x80;
    return buffer - buffer0;
}
static uint64_t unpacki64v( const uint8_t *buffer, int64_t *value ) {
    /* decode unsigned : 7-bit unpacking. MSB terminates stream */
    const uint8_t *buffer0 = buffer;
    uint64_t out = 0, j = -7;
    do {
        out |= (( ((uint64_t)(*buffer)) & 0x7f) << (j += 7) );
    } while( ((uint64_t)(*buffer++)) & 0x80 );
    /* convert magnitude|sign to sign|magnitude */
    *value = ((out >> 1) ^ -(out & 1));
    return buffer - buffer0;
}
