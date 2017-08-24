#include <stdint.h>
#include <stdbool.h>

// branchless zigzag encoding 32/64
// sign|magnitude to magnitude|sign and back
// [ref] https://developers.google.com/protocol-buffers/docs/encoding
uint32_t to_uint32_t(int32_t n) { return ((n << 1) ^ (n >> 31)); }
uint64_t to_uint64_t(int64_t n) { return ((n << 1) ^ (n >> 63)); }
 int32_t to_int32_t(uint32_t n) { return ((n >> 1) ^  -(n & 1)); }
 int64_t to_int64_t(uint64_t n) { return ((n >> 1) ^  -(n & 1)); }

#include <stdio.h>
#include <windows.h>

#if !defined(ALWAYS_INLINE) && defined(_MSC_VER)
#define ALWAYS_INLINE __forceinline
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE
#endif

    static uint32_t ALWAYS_INLINE popcnt( uint32_t x ) {
        x -= ((x >> 1) & 0x55555555);
        x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return x & 0x0000003f;
    }
    static uint32_t ALWAYS_INLINE clz( uint32_t x ) {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return 32 - popcnt(x);
    }
    static uint32_t ALWAYS_INLINE ctz( uint32_t x ) {
        return popcnt((x & -x) - 1);
    }

// encodes 64-bit `value` into char `buffer`.
// returns number of written bytes (min: 1, max: 9)
uint8_t set_varint64( uint8_t buffer[9], uint64_t value ) {
    uint8_t i = 0;
    while( value && value >= (128ull>>i) ) {
        buffer[++i] = value & 0xff;
        value >>= 8;
    }
    buffer[0] = (uint8_t)(((value << 1) | 1) << i);
    return i + 1ull;
}

// returns decoded char `buffer` as 64-bit `value`.
// optionally returns number of bytes read in `num_bytes_read` (min: 1, max: 9)
uint64_t get_varint64( const uint8_t buf[9], uint8_t *num_bytes_read ) {
#if 0
                                 // ctz()+1 {
    uint8_t len = 9;             // 00000000 64 bits in 9 bytes
    /**/ if(buf[0]&  1) len = 1; // xxxxxxx1  7 bits in 1 byte
    else if(buf[0]&  2) len = 2; // xxxxxx10 14 bits in 2 bytes
    else if(buf[0]&  4) len = 3; // xxxxx100 21 bits in 3 bytes
    else if(buf[0]&  8) len = 4; // xxxx1000 28 bits in 4 bytes
    else if(buf[0]& 16) len = 5; // xxx10000 35 bits in 5 bytes
    else if(buf[0]& 32) len = 6; // xx100000 42 bits in 6 bytes
    else if(buf[0]& 64) len = 7; // x1000000 49 bits in 7 bytes
    else if(buf[0]&128) len = 8; // 10000000 56 bits in 8 bytes
                                 // }
#else
    uint8_t len = ctz(0x100 | buf[0]) + 1;
#endif
    uint64_t out = (uint64_t)(buf[0] >> len);
    for( int i = 1; i < len; ++i ) out = (out << 8ull) | buf[len - i];
    if( num_bytes_read ) *num_bytes_read = len;
    return out;
}

#ifdef DEMO

#include <stdio.h>
#include <assert.h>
#include <omp.h>

void TESTU( uint64_t N ) {
    uint8_t buf[9] = {0};
    set_varint64(buf, (N));
    uint64_t reconstructed = get_varint64(buf, 0);
    if( reconstructed != (N) ) printf("[FAIL] %llu vs %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", (N), buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8] );
    else if( 0xffffff == ((N) & 0xffffff) ) printf("[ OK ] %llx\n", (N));
}

void TESTI( int64_t N ) {
    TESTU( to_uint64_t(N) );
}

int main() {

    #define CMP32(signedN) do { \
        int32_t reconverted = to_int32_t( to_uint32_t(signedN) ); \
        bool equal = signedN == reconverted; \
        printf("[%s] %d vs %d\n", equal ? " OK " : "FAIL", signedN, reconverted ); \
    } while(0)

    #define CMP64(signedN) do { \
        int64_t reconverted = to_int64_t( to_uint64_t(signedN) ); \
        bool equal = signedN == reconverted; \
        printf("[%s] %lld vs %lld\n", equal ? " OK " : "FAIL", signedN, reconverted ); \
    } while(0)

    CMP32( 0);
    CMP32(-1);
    CMP32(+1);
    CMP32(-2);
    CMP32(+2);
    CMP32(INT32_MAX - 1);
    CMP32(INT32_MIN + 1);
    CMP32(INT32_MAX);
    CMP32(INT32_MIN);

    CMP64( 0ll);
    CMP64(-1ll);
    CMP64(+1ll);
    CMP64(-2ll);
    CMP64(+2ll);
    CMP64(INT64_MAX - 1);
    CMP64(INT64_MIN + 1);
    CMP64(INT64_MAX);
    CMP64(INT64_MIN);

    TESTU(0LLU);
    TESTU(1LLU);
    TESTU(2LLU);
    TESTU(UINT64_MAX/8);
    TESTU(UINT64_MAX/4);
    TESTU(UINT64_MAX/2);
    TESTU(UINT64_MAX-2);
    TESTU(UINT64_MAX-1);
    TESTU(UINT64_MAX);

   #pragma omp parallel for  // compile with /openmp
   for( int64_t N = INT64_MIN; N < INT64_MAX; ++N ) {
        TESTU(N);
        TESTI((int64_t)N);
   }

   return 0;
}

#endif
