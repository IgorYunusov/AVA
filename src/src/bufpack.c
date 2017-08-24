// Bufpack, struct/buffer bitpacking (without schema).
// For schema packing, see msgpack.h instead.
// Based on code by Brian "Beej Jorgensen" Hall (public domain).
// - rlyeh, public domain.

#pragma once

#include <stdint.h>

// @todo: test & doc (x)
// (z) 64-bit zig-zag encoded (@todo) or %V PrefixVarint 0..9 bytes (see: https://github.com/stoklund/varint)

// api
// schema format
//  (<) little endian       (>) big endian (or !)       (=) native endian
//  (c) 8-bit  char         (b) 8-bit  byte
//  (h) 16-bit half         (w) 16-bit word
//  (i) 32-bit integer      (u) 32-bit unsigned         (f) 32-bit float
//  (l) 64-bit long         (q) 64-bit quad             (d) 64-bit double
//  (v) 64-bit varint
//  (s) string  (64-bit variable length is automatically prepended)
//  (p) pointer (64-bit variable length is automatically prepended)
//  (z) zeros

// - save data dictated by the format string from the buffer. return: number of bytes written
// - load data dictated by the format string into the buffer. return: args<<48|bytes processed
//   use BUFUNPACK_NUM_ARGS(retcode) or BUFUNPACK_NUM_BYTES(retcode) as needed.

uint64_t bufpack(uint8_t *buf, const char *format, ...);
uint64_t bufunpack(const uint8_t *buf, const char *format, ...);

#define BUFUNPACK_NUM_ARGS(x)   ((x) >> 48ull)
#define BUFUNPACK_NUM_BYTES(x) (((x) << 16ull) >> 16ull)

// impl

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "pack754.c"
#include "packint.c"
#include "packvar.c"

typedef float  float32_t;
typedef double float64_t;

typedef static_assert_f32[ sizeof(float32_t) == 4 ];
typedef static_assert_f64[ sizeof(float64_t) == 8 ];

uint64_t bufpack(uint8_t *buf, const char *format, ...) {
    int8_t c;
    int16_t h;
    int32_t l;
    int64_t L;
    float32_t f;
    float64_t F;
    char tmp[16], *s;
    uint64_t size = 0, len;
    int le = 0, is_little = (*(uint16_t *)"\0\1") != 1;

    va_list ap;
    va_start(ap, format);

    if(buf) // buffer packing
    for(; *format != '\0'; format++) {
        switch(*format) {
        default:  size = 0; // error
        case '!':
        case '>': le = 0; break;
        case '<': le = 1; break;
        case ' ': le = le; break;
        case '=': le = is_little ? 1 : 0; break;

        case 'c': case 'b': // 8-bit
            size += 1;
            c = (int8_t)va_arg(ap, int); // promoted
            *buf++ = (c>>0)&0xff;
            break;

        case 'h': case 'w': // 16-bit
            size += 2;
            h = (int16_t)va_arg(ap, int); // promoted
            packi16(buf, h, le);
            buf += 2;
            break;

        case 'i': case 'u': // 32-bit
            size += 4;
            l = va_arg(ap, int32_t);
            packi32(buf, l, le);
            buf += 4;
            break;

        case 'l': case 'q': // 64-bit
            size += 8;
            L = va_arg(ap, int64_t);
            packi64(buf, L, le);
            buf += 8;
            break;

        case 'f': // 32-bit float
            size += 4;
            f = (float32_t)va_arg(ap, double); // promoted
            l = pack754_32(f); // convert to IEEE 754
            packi32(buf, l, le);
            buf += 4;
            break;

        case 'd': // 64-bit float (double)
            size += 8;
            F = (float64_t)va_arg(ap, float64_t);
            L = pack754_64(F); // convert to IEEE 754
            packi64(buf, L, le);
            buf += 8;
            break;

        case 'p': // C pointer, 64-bit variable length prepended
            len = va_arg(ap, int);
            s = va_arg(ap, char*);
            L = packi64v(buf, len);
            memcpy(buf + L, s, len);
            buf += L + len;
            size += L + len;
            break;

        case 'x': // C pointer, no 64-bit variable length prepended
            len = va_arg(ap, int);
            s = va_arg(ap, char*);
            L = 0; //packi64v(buf, len);
            memcpy(buf + L, s, len);
            buf += L + len;
            size += L + len;
            break;

        case 's': // C string, 64-bit variable length prepended
            s = va_arg(ap, char*);
            len = strlen(s);
            L = packi64v(buf, len);
            memcpy(buf + L, s, len);
            buf += L + len;
            size += L + len;
            break;

        case 'v': // varint (8,16,32,64 ...)
            L = va_arg(ap, int64_t);
            L = packi64v(buf, L);
            size += L;
            buf += L;
            break;

        case '0': // zeroed mem
            len = va_arg(ap, int);
            memset(buf, 0, len);
            size += len;
            buf += len;
            break;
        }
    }

    if(!buf) // buffer estimation
    for(; *format != '\0'; format++) {
        switch(*format) {
        default: size = 0; // error
        break; case '!': case '>':  case '<':  case '=': case ' ':                //  0-bit endianness
        break; case 'c': case 'b': size += 1; c = (int8_t)va_arg(ap, int);        //  8-bit promoted
        break; case 'h': case 'w': size += 2; h = (int16_t)va_arg(ap, int);       // 16-bit promoted
        break; case 'i': case 'u': size += 4; l = va_arg(ap, int32_t);            // 32-bit
        break; case 'l': case 'q': size += 8; L = va_arg(ap, int64_t);            // 64-bit
        break; case 'f': size += 4; f = (float32_t)va_arg(ap, double);            // 32-bit float promoted
        break; case 'd': size += 8; F = (float64_t)va_arg(ap, float64_t);         // 64-bit float (double)
        break; case 'v': L = va_arg(ap, int64_t); size += packi64v(tmp, L);       // varint (8,16,32,64 ...)
        break; case 's': s = va_arg(ap, char*); len = strlen(s); size += packi64v(tmp, len) + len;       // C string, 64-bit variable length prepended
        break; case 'p': len = va_arg(ap, int); s = va_arg(ap, char*); size += packi64v(tmp, len) + len; // C pointer, 64-bit variable length prepended
        break; case 'x': len = va_arg(ap, int); s = va_arg(ap, char*); size += len; // C pointer, 64-bit variable length prepended
        break; case '0': len = va_arg(ap, int); size += len;                        // zeroed mem
        }
    }

    va_end(ap);
    return size;
}

uint64_t bufunpack(const uint8_t *buf, const char *format, ...) {
    char *s;
    int8_t *c;
    int16_t *h;
    int32_t *l;
    int64_t *L;
    int32_t pf;
    int64_t pF;
    float32_t *f;
    float64_t *F;
    uint64_t args = 0;
    const uint8_t *buf0 = buf;
    int32_t len, count, maxstrlen=0;
    int le = 0, is_little = (*(uint16_t *)"\0\1") != 1;

    va_list ap;
    va_start(ap, format);

    if(buf) // buffer unpacking
    for(; *format != '\0'; format++) {
        switch(*format) {
        case ' ': break;
        case '!':
        case '>': le = 0; break;
        case '<': le = 1; break;
        case '=': le = is_little ? 1 : 0; break;

        case 'c': case 'b': // 8-bit
            c = va_arg(ap, int8_t*);
            if (*buf <= 0x7f) { *c = *buf;}
            else { *c = -1 - (uint8_t)(0xffu - *buf); }
            buf++;
            ++args;
            break;

        case 'h': case 'w': // 16-bit
            h = va_arg(ap, int16_t*);
            *h = unpacki16(buf, le);
            buf += 2;
            ++args;
            break;

        case 'i': case 'u': // 32-bit
            l = va_arg(ap, int32_t*);
            *l = unpacki32(buf, le);
            buf += 4;
            ++args;
            break;

        case 'l': case 'q': // 64-bit
            L = va_arg(ap, int64_t*);
            *L = unpacki64(buf, le);
            buf += 8;
            ++args;
            break;


        case 'f': // 32-bit float
            f = va_arg(ap, float32_t*);
            pf = unpacki32(buf, le);
            buf += 4;
            *f = unpack754_32(pf);
            ++args;
            break;

        case 'd': // 64-bit float (double)
            F = va_arg(ap, float64_t*);
            pF = unpacki64(buf, le);
            buf += 8;
            *F = unpack754_64(pF);
            ++args;
            break;

        case 'p': // C pointer, 64-bit variable length prepended
            s = va_arg(ap, char*);
            {   int64_t vlen, read;
                read = unpacki64v(buf, &vlen);
                count = vlen; //(maxstrlen > 0 && vlen > maxstrlen ? maxstrlen - 1 : vlen);
                memcpy(s, buf + read, count);
                //s[count] = '\0';
                buf += read + vlen;
            }
            ++args;
            break;

        case 'x': // C pointer, no 64-bit variable length prepended
            { int64_t vlen = va_arg(ap, int);
            s = va_arg(ap, char*);
               int64_t read = 0;
                count = vlen; //(maxstrlen > 0 && vlen > maxstrlen ? maxstrlen - 1 : vlen);
                memcpy(s, buf + read, count);
                //s[count] = '\0';
                buf += read + vlen;
            }
            ++args;
            break;

        case 's': // C string, 64-bit variable length prepended
            s = va_arg(ap, char*);
            {   int64_t vlen, read;
                read = unpacki64v(buf, &vlen);
                count = (maxstrlen > 0 && vlen > maxstrlen ? maxstrlen - 1 : vlen);
                memcpy(s, buf + read, count);
                s[count] = '\0';
                buf += read + vlen;
            }
            ++args;
            break;

        case 'v': // varint (8,16,32,64 ...)
            L = va_arg(ap, int64_t*);
            buf += unpacki64v(buf, L);
            ++args;
            break;

        case '0': // varint (8,16,32,64 ...)
            l = va_arg(ap, int*);
            *l = 0;
            while( *buf == 0 ) ++buf, *l++;
            ++args;
            break;

        default:
            if (isdigit(*format)) { // track max str len
                maxstrlen = maxstrlen * 10 + (*format-'0');
            }
        }

        if (!isdigit(*format)) maxstrlen = 0;
    }

    if(!buf) // @todo: message verification
    ;

    va_end(ap);
    return ( args << 48ull ) | ( buf - buf0 );
}

#ifdef DEMO
#include "hexdump.c"
#include <limits.h>
#include <float.h>
#include <assert.h>
void tests() {
    uint8_t buf[1024];
    int x;

    int64_t k, k2;
    int64_t test64[14] = { 0, -0, 1, 2, -1, -2, LLONG_MAX>>1, LLONG_MAX-1, LLONG_MAX, LLONG_MIN+1, LLONG_MIN, 9007199254740991, 9007199254740992, 9007199254740993 };

    int32_t i, i2;
    int32_t test32[14] = { 0, -0, 1, 2, -1, -2, INT_MAX>>1, INT_MAX-1, INT_MAX, INT_MIN+1, INT_MIN, 0, 0, 0 };

    int16_t j, j2;
    int16_t test16[14] = { 0, -0, 1, 2, -1, -2, SHRT_MAX>>1, SHRT_MAX-1, SHRT_MAX, SHRT_MIN+1, SHRT_MIN, 0, 0, 0 };

    for(x = 0; x < 14; x++) {
        k = test64[x];
        bufpack(buf, "l", k);
        bufunpack(buf, "l", &k2);

        if (memcmp( &k, &k2, sizeof(k))) {
            printf("[FAIL] 64: %llu != %llu\n", k, k2);
            hexdump( stdout, buf, 8, 16 );
        } else {
            printf("[ OK ] 64: %" PRId64 " == %" PRId64 "\n", k, k2);
        }

        i = test32[x];
        bufpack(buf, "i", i);
        bufunpack(buf, "i", &i2);

        if (i2 != i) {
            printf("[FAIL] 32: %d != %d\n", (int)i, (int)i2);
        } else {
            printf("[ OK ] 32: %d == %d\n", (int)i, (int)i2);
        }

        j = test16[x];
        bufpack(buf, "h", j);
        bufunpack(buf, "h", &j2);

        if (j2 != j) {
            printf("[FAIL] 16: %d != %d\n", (int)j, (int)j2);
        } else {
            printf("[ OK ] 16: %d == %d\n", (int)j, (int)j2);
        }
    }

    {
        float64_t testf64[7] = { 0.0, 1.0, -1.0, DBL_MIN*2, DBL_MAX/2, DBL_MIN, DBL_MAX };
        float64_t f,f2;

        for (i = 0; i < 7; i++) {
            f = testf64[i];
            bufpack(buf, "d", f);
            bufunpack(buf, "d", &f2);

            if (memcmp( &f, &f2, sizeof(f))) {
                printf("[FAIL] f64: %g != %g\n", f, f2);
                hexdump( stdout, buf, 8, 16 );
            } else {
                printf("[ OK ] f64: %g == %g\n", f, f2);
            }
        }
    }
}

int main()
{
    tests();

    uint8_t buf[1024];
    int8_t magic;
    int16_t monkeycount;
    int32_t altitude;
    float32_t absurdityfactor;
    char *s = "Great unmitigated Zot!  You've found the Runestaff!";
    char p[] = {'a','b','c'};
    char s2[96];
    char exclamation;
    int16_t packetsize, ps2;
    int64_t large_but_small;

    packetsize = bufpack(0/*buf*/, ">chhisfvcp", (int8_t)'\x01', (int16_t)0, (int16_t)37,
            (int32_t)-5, s, (float32_t)-3490.6677, -123LL, '!', sizeof(p), p);

    printf("packet is %d bytes\n", (int)packetsize);

    packetsize = bufpack(buf, ">chhisfvcp", (int8_t)'\x01', (int16_t)0, (int16_t)37,
            (int32_t)-5, s, (float32_t)-3490.6677, -123LL, '!', sizeof(p), p);

    bufpack(buf+1, ">h", packetsize); // store packet size in packet for kicks

    printf("packet is %d bytes\n", (int)packetsize);
    hexdump( stdout, buf, packetsize, 16 );

    uint64_t retcode =
    bufunpack(buf, ">chhi96sfvcp", &magic, &ps2, &monkeycount, &altitude, s2,
        &absurdityfactor, &large_but_small, &exclamation, &p);

    assert( BUFUNPACK_NUM_ARGS(retcode) == 9 );
    printf("unpacked %d bytes\n", (int)BUFUNPACK_NUM_BYTES(retcode));

    printf("v%d %d-bytes %d %d \"%s\" %f %lld %c %c%c%c\n", magic, ps2, monkeycount,
            altitude, s2, absurdityfactor, large_but_small, exclamation, p[0], p[1], p[2]);

    // typical msgpack sample
    packetsize = bufpack(buf, "scsc", "compact", 1, "schema", 0);
    printf("packet is %d bytes\n", (int)packetsize);
    hexdump( stdout, buf, packetsize, 16 );
}
#endif
