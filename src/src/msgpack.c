// Msgpack v5, struct/buffer bitpacking (with schema).
// For schemaless packing, see bufpack.h instead.
// - rlyeh, public domain.

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// @todo: alt api
// return number of bytes written; 0 if not space enough.
// int msgpack( uint8_t *buf, const char *fmt, ... );         // if !buf, bulk size; else pack.
// return number of processed bytes; 0 if parse error.
// int msgunpack( const uint8_t *buf, const char *fmt, ... ); // if !buf, test message; else unpack.

// api

int msgpack_new(uint8_t *w, size_t l);
int msgpack_nil();                                  // write null
int msgpack_chr(bool n);                            // write boolean
int msgpack_flt(double g);                          // write real
int msgpack_int(int64_t n);                         // write integer
int msgpack_uns(uint64_t n);                        // write unsigned integer
int msgpack_str(const char *s);                     // write string
int msgpack_bin(const char *s, size_t n);           // write binary pointer
int msgpack_arr(uint32_t n);                        // write array mark for next N items
int msgpack_map(uint32_t n);                        // write map mark for next N pairs (N keys + N values)
int msgpack_ext(uint8_t key, void *val, size_t n);  // write extension type
int msgpack_eof();                                  // write full?
int msgpack_err();                                  // write error?
int msgpack(const char *fmt, ... );                 // va arg pack "(n),(b),(f/g),(d/i),(u),(s),(p),([),({),(e)"

bool msgunpack_new( const void *opaque, size_t bytes );
bool msgunpack_nil();
bool msgunpack_chr(bool *chr);
bool msgunpack_uns(uint64_t *uns);
bool msgunpack_int(int64_t *sig);
bool msgunpack_flt(double *flt);
bool msgunpack_bin(void **bin, uint64_t *len);
bool msgunpack_str(char **str);
bool msgunpack_ext(uint8_t *key, void **val, uint64_t *len);
bool msgunpack_arr(uint64_t *len);
bool msgunpack_map(uint64_t *len);
bool msgunpack_eof();
bool msgunpack_err();
bool msgunpack(const char *fmt, ... );                 // va arg pack "n,b,f/g,d/i,u,s,p,[,{,e"

// alt unpack api

enum {
    ERR,NIL,BOL,UNS,SIG,STR,BIN,FLT,EXT,ARR,MAP
};
typedef struct variant {
    union {
    uint8_t     chr;
    uint64_t    uns;
    int64_t     sig;
    uint8_t    *str;
    void       *bin;
    double      flt;
    uint32_t    u32;
    };
    uint64_t sz;
    uint16_t ext;
    uint16_t type; //[0..10]={err,nil,bol,uns,sig,str,bin,flt,ext,arr,map}
} variant;
bool msgunpack_var(struct variant *var);

// impl
// ref: https://github.com/msgpack/msgpack/blob/master/spec.md

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pack754.c"
#include "endian.c"
//#include "msgpack.h"

#ifdef _MSC_VER
#define __thread __declspec(thread)
#endif

typedef struct {
    uint8_t *w; // Write pointer into buffer
    size_t len; // Written bytes up to date
    size_t cap; // Buffer capacity
} writer;

typedef struct {
    FILE *fp;
    const void *membuf;
    size_t memsize, offset;
    struct variant v; // tmp
} reader;

static __thread writer out;
static __thread reader in;

static void wrbe(uint64_t n, uint8_t *b) {
#ifndef BIG_ENDIAN
    n = ntoh64(n);
#endif
    memcpy(b, &n, sizeof(uint64_t));
}
static int wr(int len, uint8_t opcode, uint64_t value) {
    uint8_t b[8];
    assert((out.len + (len+1) < out.cap) && "buffer overflow!");
    *out.w++ = (opcode);
    /**/ if(len == 1) *out.w++ = (uint8_t)(value);
    else if(len == 2) wrbe(value, b), memcpy(out.w, &b[6], 2), out.w += 2;
    else if(len == 4) wrbe(value, b), memcpy(out.w, &b[4], 4), out.w += 4;
    else if(len == 8) wrbe(value, b), memcpy(out.w, &b[0], 8), out.w += 8;
    out.len += len+1;
    return len+1;
}
static bool rd(void *buf, size_t len, size_t swap) { // return false any error and/or eof
    bool ret;
    if( in.fp ) {
        assert( !ferror(in.fp) && "invalid file handle (reader)" );
        ret = len == fread((char*)buf, 1, len, in.fp);
    } else {
        assert( in.membuf && "invalid memory buffer (reader)");
        assert( (in.offset + len <= in.memsize) && "memory overflow! (reader)");
        ret = !!memcpy(buf, (char*)in.membuf + in.offset, len);
    }
#ifndef BIG_ENDIAN
    /**/ if( swap && len == 2 ) *((uint16_t*)buf) = ntoh16(*((uint16_t*)buf));
    else if( swap && len == 4 ) *((uint32_t*)buf) = ntoh32(*((uint32_t*)buf));
    else if( swap && len == 8 ) *((uint64_t*)buf) = ntoh64(*((uint64_t*)buf));
#endif
    return in.offset += len, ret;
}
static bool rdbuf(char **buf, size_t len) { // return false on error or out of memory
    char *ptr = realloc(*buf, len+1);
    if( ptr && rd(ptr, len, 0) ) {
        (*buf = ptr)[len] = 0;
    } else {
        free(ptr), ptr = 0;
    }
    return !!ptr;
}

int msgpack_new(uint8_t *w, size_t l) {
    out.w = w;
    out.len = 0;
    out.cap = l;
    return w != 0 && l != 0;
}
int msgpack_nil() {
    return wr(0, 0xC0, 0);
}
int msgpack_chr(bool c) {
    return wr(0, c ? 0xC3 : 0xC2, 0);
}
int msgpack_uns(uint64_t n) {
    /**/ if (n < 0x80)           return wr(0, n, 0);
    else if (n < 0x100)          return wr(1, 0xCC, n);
    else if (n < 0x10000)        return wr(2, 0xCD, n);
    else if (n < 0x100000000)    return wr(4, 0xCE, n);
    else                         return wr(8, 0xCF, n);
}
int msgpack_int(int64_t n) {
    /**/ if (n >= 0)             return msgpack_uns(n);
    else if (n >= -32)           return wr(0, n, 0); //wr(0, 0xE0 | n, 0);
    else if (n >= -128)          return wr(1, 0xD0, n + 0xff + 1);
    else if (n >= -32768)        return wr(2, 0xD1, n + 0xffff + 1);
    else if (n >= -2147483648LL) return wr(4, 0xD2, n + 0xffffffffull + 1);
    else                         return wr(8, 0xD3, n + 0xffffffffffffffffull + 1);
}
int msgpack_flt(double g) {
    float f = (float)g;
    double h = f;
    /**/ if(g == h) return wr(4, 0xCA, pack754_32(f));
    else            return wr(8, 0xCB, pack754_64(g));
}
int msgpack_str(const char *s) {
    size_t n = strlen(s), c = n;

    /**/ if (n < 0x20)     c += wr(0, 0xA0 | n, 0);
    else if (n < 0x100)    c += wr(1, 0xD9, n);
    else if (n < 0x10000)  c += wr(2, 0xDA, n);
    else                   c += wr(4, 0xDB, n);

    memcpy(out.w, s, n);
    out.w += n;
    out.len += n;
    return c;
}
int msgpack_bin(const char *s, size_t n) {
    size_t c = n;
    /**/ if (n < 0x100)    c += wr(1, 0xC4, n);
    else if (n < 0x10000)  c += wr(2, 0xC5, n);
    else                   c += wr(4, 0xC6, n);

    memcpy(out.w, s, n);
    out.w += n;
    out.len += n;
    return c;
}
int msgpack_arr(uint32_t numitems) {
    uint32_t n = numitems;
    /**/ if (n < 0x10)    return wr(0, 0x90 | n, 0);
    else if (n < 0x10000) return wr(2, 0xDC, n);
    else                  return wr(4, 0xDD, n);
}
int msgpack_map(uint32_t numpairs) {
    uint32_t n = numpairs;
    /**/ if (n < 0x10)    return wr(0, 0x80 | n, 0);
    else if (n < 0x10000) return wr(2, 0xDE, n);
    else                  return wr(4, 0xDF, n);
}
int msgpack_ext(uint8_t key, void *val, size_t n) {
    uint32_t c = n;
    /**/ if (n == 1)        c += wr(1, 0xD4, key);
    else if (n == 2)        c += wr(1, 0xD5, key);
    else if (n == 4)        c += wr(1, 0xD6, key);
    else if (n == 8)        c += wr(1, 0xD7, key);
    else if (n == 16)       c += wr(1, 0xD8, key);
    else if (n < 0x100)     c += wr(1, 0xC7, n), c += wr(0, key, 0);
    else if (n < 0x10000)   c += wr(2, 0xC8, n), c += wr(0, key, 0);
    else                    c += wr(4, 0xC9, n), c += wr(0, key, 0);

    memcpy(out.w, val, n);
    out.w += n;
    out.len += n;
    return c;
}
int msgpack(const char *fmt, ... ) {
    int count = 0;
    va_list vl;
    va_start(vl, fmt);
    while( *fmt ) {
        char f = *fmt++;
        switch( f ) {
            break; case '{': { int i = va_arg(vl, int64_t); count += msgpack_map( i ); }
            break; case '[': { int i = va_arg(vl, int64_t); count += msgpack_arr( i ); }
            break; case 'b': { bool v = !!va_arg(vl, int64_t); count += msgpack_chr(v); }
            break; case 'e': { uint8_t k = va_arg(vl, uint64_t); void *v = va_arg(vl, void*); size_t l = va_arg(vl, uint64_t); count += msgpack_ext( k, v, l ); }
            break; case 'n': { count += msgpack_nil(); }
            break; case 'p': { void *p = va_arg(vl, void*); size_t l = va_arg(vl, uint64_t); count += msgpack_bin( p, l ); }
            break; case 's': { const char *v = va_arg(vl, const char *); count += msgpack_str(v); }
            break; case 'u': { uint64_t v = va_arg(vl, uint64_t); count += msgpack_uns(v); }
            break; case 'd': case 'i': { int64_t v = va_arg(vl, int64_t); count += msgpack_int(v); }
            break; case 'f': case 'g': { double v = va_arg(vl, double); count += msgpack_flt(v); }
            default: /*count = 0;*/ break;
        }
    }
    va_end(vl);
    return count;
}

bool msgunpack_new( const void *opaque, size_t bytes ) {
    return (memset( &in, 0, sizeof(in)), in.memsize = bytes) ? (in.membuf = opaque) : (in.fp = (FILE*)opaque);
}
bool msgunpack_eof() {
    return in.fp ? !!feof(in.fp) : (in.offset > in.memsize);
}
bool msgunpack_err() {
    return in.fp ? !!ferror(in.fp) : !in.memsize;
}
bool msgunpack_var(struct variant *w) {
    uint8_t tag;
    struct variant v = {0};
    if( rd(&tag, 1, 0) )
    switch(tag) {
        default:
        /**/ if((tag & 0x80) == 0x00) { v.type = UNS; v.sz = 1; v.uns =         tag; }
        else if((tag & 0xe0) == 0xe0) { v.type = SIG; v.sz = 1; v.sig = (int8_t)tag; }
        else if((tag & 0xe0) == 0xa0) { v.type = rdbuf(&v.str, v.sz = tag & 0x1f) ? STR : ERR; }
        else if((tag & 0xf0) == 0x90) { v.type = ARR; v.sz = tag & 0x0f; }
        else if((tag & 0xf0) == 0x80) { v.type = MAP; v.sz = tag & 0x0f; }

        break; case 0xc0: v.type = NIL; v.sz = 0;
        break; case 0xc2: v.type = BOL; v.sz = 1; v.chr = 0;
        break; case 0xc3: v.type = BOL; v.sz = 1; v.chr = 1;

        break; case 0xcc: v.type = rd(&v.uns, v.sz = 1, 0) ? UNS : ERR;
        break; case 0xcd: v.type = rd(&v.uns, v.sz = 2, 1) ? UNS : ERR;
        break; case 0xce: v.type = rd(&v.uns, v.sz = 4, 1) ? UNS : ERR;
        break; case 0xcf: v.type = rd(&v.uns, v.sz = 8, 1) ? UNS : ERR;

        break; case 0xd0: v.type = rd(&v.uns, v.sz = 1, 0) ? (v.sig -= 0xff + 1, SIG) : ERR;
        break; case 0xd1: v.type = rd(&v.uns, v.sz = 2, 1) ? (v.sig -= 0xffff + 1, SIG) : ERR;
        break; case 0xd2: v.type = rd(&v.uns, v.sz = 4, 1) ? (v.sig -= 0xffffffffull + 1, SIG) : ERR;
        break; case 0xd3: v.type = rd(&v.uns, v.sz = 8, 1) ? (v.sig -= 0xffffffffffffffffull + 1, SIG) : ERR;

        break; case 0xca: v.type = rd(&v.u32, v.sz = 4, 1) ? (v.flt = unpack754_32(v.u32), FLT) : ERR;
        break; case 0xcb: v.type = rd(&v.uns, v.sz = 8, 1) ? (v.flt = unpack754_64(v.uns), FLT) : ERR;

        break; case 0xd9: v.type = rd(&v.sz, 1, 0) && rdbuf(&v.str, v.sz) ? STR : ERR;
        break; case 0xda: v.type = rd(&v.sz, 2, 1) && rdbuf(&v.str, v.sz) ? STR : ERR;
        break; case 0xdb: v.type = rd(&v.sz, 4, 1) && rdbuf(&v.str, v.sz) ? STR : ERR;

        break; case 0xc4: v.type = rd(&v.sz, 1, 0) && rdbuf(&v.str, v.sz) ? BIN : ERR;
        break; case 0xc5: v.type = rd(&v.sz, 2, 1) && rdbuf(&v.str, v.sz) ? BIN : ERR;
        break; case 0xc6: v.type = rd(&v.sz, 4, 1) && rdbuf(&v.str, v.sz) ? BIN : ERR;

        break; case 0xdc: v.type = rd(&v.sz, 2, 1) ? ARR : ERR;
        break; case 0xdd: v.type = rd(&v.sz, 4, 1) ? ARR : ERR;

        break; case 0xde: v.type = rd(&v.sz, 2, 1) ? MAP : ERR;
        break; case 0xdf: v.type = rd(&v.sz, 4, 1) ? MAP : ERR;

        break; case 0xd4: v.type = rd(&v.ext, 1, 0) && rd(&v.uns, 1, 0) && rdbuf(&v.str, v.sz = v.uns) ? EXT : ERR;
        break; case 0xd5: v.type = rd(&v.ext, 1, 0) && rd(&v.uns, 2, 1) && rdbuf(&v.str, v.sz = v.uns) ? EXT : ERR;
        break; case 0xd6: v.type = rd(&v.ext, 1, 0) && rd(&v.uns, 4, 1) && rdbuf(&v.str, v.sz = v.uns) ? EXT : ERR;
        break; case 0xd7: v.type = rd(&v.ext, 1, 0) && rd(&v.uns, 8, 1) && rdbuf(&v.str, v.sz = v.uns) ? EXT : ERR;
        break; case 0xd8: v.type = rd(&v.ext, 1, 0) && rd(&v.uns,16, 1) && rdbuf(&v.str, v.sz = v.uns) ? EXT : ERR;

        break; case 0xc7: v.type = rd(&v.sz, 1, 0) && rd(&v.ext, 1, 0) && rdbuf(&v.str,v.sz) ? EXT : ERR;
        break; case 0xc8: v.type = rd(&v.sz, 2, 1) && rd(&v.ext, 1, 1) && rdbuf(&v.str,v.sz) ? EXT : ERR;
        break; case 0xc9: v.type = rd(&v.sz, 4, 1) && rd(&v.ext, 1, 1) && rdbuf(&v.str,v.sz) ? EXT : ERR;
    }
    return *w = v, v.type != ERR;
}
bool msgunpack_nil() {
    return msgunpack_var(&in.v) && (in.v.type == NIL);
}
bool msgunpack_chr(bool *chr) {
    return msgunpack_var(&in.v) && (*chr = in.v.chr, in.v.type == BOL);
}
bool msgunpack_uns(uint64_t *uns) {
    return msgunpack_var(&in.v) && (*uns = in.v.uns, in.v.type == UNS);
}
bool msgunpack_int(int64_t *sig) {
    return msgunpack_var(&in.v) && (*sig = in.v.sig, in.v.type == SIG);
}
bool msgunpack_flt(double *flt) {
    return msgunpack_var(&in.v) && (*flt = in.v.flt, in.v.type == FLT);
}
bool msgunpack_bin(void **bin, uint64_t *len) {
    return msgunpack_var(&in.v) && (*bin = in.v.bin, *len = in.v.sz, in.v.type == BIN);
}
bool msgunpack_str(char **str) {
    return msgunpack_var(&in.v) && (*str = in.v.str, in.v.type == STR);
}
bool msgunpack_ext(uint8_t *key, void **val, uint64_t *len) {
    return msgunpack_var(&in.v) && (*key = in.v.ext, *val = in.v.bin, *len = in.v.sz, in.v.type == EXT);
}
bool msgunpack_arr(uint64_t *len) {
    return msgunpack_var(&in.v) && (*len = in.v.sz, in.v.type == ARR);
}
bool msgunpack_map(uint64_t *len) {
    return msgunpack_var(&in.v) && (*len = in.v.sz, in.v.type == MAP);
}

#ifdef DEMO

#include <inttypes.h>

bool vardump( struct variant *w ) {
    static int tabs = 0;
    struct variant v = *w;
    printf("%.*s", tabs, "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
    switch( v.type ) {
    default: case ERR:
         if( !msgunpack_eof() ) printf("ERROR: unknown tag type (%02X)\n", (int)v.type);
         return false;
    break; case NIL: printf("(%s)\n", "null");
    break; case BOL: printf("bool: %d\n", v.chr);
    break; case SIG: printf("int: %"PRId64"\n", v.sig);
    break; case UNS: printf("uint: %"PRIu64"\n", v.uns);
    break; case FLT: printf("float: %g\n", v.flt);
    break; case STR: printf("string: '%s'\n", v.str);
    break; case BIN: { for( size_t n = 0; n < v.sz; n++ ) printf("%s%02x(%c)", n > 0 ? " ":"binary: ", v.str[n], v.str[n] >= 32 ? v.str[n] : '.'); puts(""); }
    break; case EXT: { printf("ext: [%02X (%d)] ", v.ext, v.ext); for( size_t n = 0; n < v.sz; n++ ) printf("%s%02x(%c)", n > 0 ? " ":"", v.str[n], v.str[n] >= 32 ? v.str[n] : '.'); puts(""); }
    break; case ARR: {
        ++tabs; puts("[");
        for( size_t n = v.sz; n-- > 0; ) {
            if( !msgunpack_var(&v) || !vardump(&v) ) return false;
        }
        --tabs; puts("]");
    }
    break; case MAP: {
        ++tabs; puts("{");
        for( size_t n = v.sz; n-- > 0; ) {
            if( !msgunpack_var(&v) || !vardump(&v) ) return false;
            if( !msgunpack_var(&v) || !vardump(&v) ) return false;
        }
        --tabs; puts("}");
    }}
    return true;
}

void hexdump( const void *ptr, unsigned len, int width ) {
    unsigned char *data = (unsigned char*)ptr;
    for( unsigned jt = 0; jt < len; jt += width ) {
        printf( "; %05d ", jt );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            printf( "%02x %s", (unsigned char)data[it], &" \n\0...\n"[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
        printf( "; %05d ", jt );
        for( unsigned it = jt, next = it + width; it < len && it < next; ++it ) {
            printf( " %c %s", (signed char)data[it] >= 32 ? (signed char)data[it] : (signed char)'.', &" \n\0...\n"[ (1+it) < len ? 2 * !!((1+it) % width) : 3 ] );
        }
    }
}

void testdump( const char *fname ) {
    FILE *fp = fopen(fname, "rb");
    if( !fp ) {
        fputs("Cannot read input stream", stderr);
    } else {
        if( msgunpack_new(fp, 0) ) {
            struct variant v;
            while( msgunpack_var(&v) ) {
                vardump(&v);
            }
            if( msgunpack_err() ) {
                fputs("Error while unpacking", stderr);
            }
        }
        fclose(fp);
    }
}

void testwrite(const char *outfile) {
    char buf[256];
    msgpack_new(buf, 256);
    int len = msgpack("ddufs [dddddddd-dddddddd {sisi bne"/*bp0*/,
        -123LL, 123LL, 123456ULL, 3.14159f, "hello world",
        16ULL,
         -31LL, -32LL, -127LL, -128LL, -255LL, -256LL, -511LL, -512LL,  // ,121, 3, "hi",
         +31LL, +32LL, +127LL, +128LL, +255LL, +256LL, +511LL, +512LL,  // ,121, 3, "hi",
        2ULL,
          "hello", -123LL,
          "world", -456LL,
        1ULL,
        0xeeULL, "this is an EXT type", sizeof("this is an EXT type")-1
    );
    hexdump(buf, len, 16);

    FILE *fp = fopen(outfile, "wb");
    if( fp ) {
        fwrite( buf, len, 1, fp );
        fclose(fp);
    }
}

void testunits() {

#   define unit(title_) do { \
        title = title_; \
    } while(0)
#   define data(data) do { \
        test_data = data; \
        test_len = sizeof(data) - 1; \
        msgunpack_new(test_data, test_len); \
    } while(0)
#   define test(expr) do { \
        const char *blank = "\r                                                                                   \r"; \
        printf("[    ] %s%s", title, #expr); \
        int ok = msgunpack_var(&obj) && !!(expr); \
        right += ok; wrong += !ok; \
        printf("\r[%s]%s", ok ? " OK " : "FAIL", ok ? "\r" : "\n"); \
    } while(0)

    int test_len, right = 0, wrong = 0;
    const char *test_data = 0, *title = "";
    struct variant obj;

    /*
     * Test vectors are derived from
     * `https://github.com/ludocode/mpack/blob/v0.8.2/test/test-write.c`.
     */

    unit("(minposfixint)");
    data("\x00");
    test(obj.type == UNS && obj.sz == 1 && obj.uns == 0);

    unit("(maxposfixint)");
    data("\x7f");
    test(obj.type == UNS && obj.sz == 1 && obj.uns == 127);

    unit("(maxnegfixint)");
    data("\xe0");
    test(obj.type == SIG && obj.sz == 1 && obj.uns == -32);

    unit("(minnegfixint)");
    data("\xff");
    test(obj.type == SIG && obj.sz == 1 && obj.uns == -1);

    unit("(uint8)");
    data("\xcc\0");
    test(obj.type == UNS && obj.sz == 1 && obj.uns == 0);

    unit("(uint16)");
    data("\xcd\0\0");
    test(obj.type == UNS && obj.sz == 2 && obj.uns == 0);

    unit("(uint32)");
    data("\xce\0\0\0\0");
    test(obj.type == UNS && obj.sz == 4 && obj.uns == 0);

    unit("(uint64)");
    data("\xcf\0\0\0\0\0\0\0\0");
    test(obj.type == UNS && obj.sz == 8 && obj.uns == 0);

    unit("(float32)");
    data("\xca\0\0\0\0");
    test(obj.type == FLT && obj.sz == 4 && obj.uns == 0);

    unit("(float64)");
    data("\xcb\0\0\0\0\0\0\0\0");
    test(obj.type == FLT && obj.sz == 8 && obj.uns == 0);

    unit("(string)");
    data("\xa5Hello");
    test(obj.type == STR && obj.sz == 5 && !strcmp(obj.str, "Hello"));

    unit("(str8)");
    data("\xd9\x05Hello");
    test(obj.type == STR && obj.sz == 5 && !strcmp(obj.str, "Hello"));

    unit("(str16)");
    data("\xda\0\x05Hello");
    test(obj.type == STR && obj.sz == 5 && !strcmp(obj.str, "Hello"));

    unit("(str32)");
    data("\xdb\0\0\0\x05Hello");
    test(obj.type == STR && obj.sz == 5 && !strcmp(obj.str, "Hello"));

    unit("(array)");
    data("\x91\x01");
    test(obj.type == ARR && obj.sz == 1);

    unit("(array8)");
    data("\x91\x01");
    test(obj.type == ARR && obj.sz == 1);

    unit("(array16)");
    data("\xdc\0\x01\x01");
    test(obj.type == ARR && obj.sz == 1);

    unit("(map8)");
    data("\x81\x01\x01");
    test(obj.type == MAP && obj.sz == 1);
    test(obj.type == UNS && obj.sz == 1 && obj.uns == 1);
    test(obj.type == UNS && obj.sz == 1 && obj.uns == 1);

    unit("(map32)");
    data("\xdf\0\0\0\x01\xa5Hello\x01");
    test(obj.type == MAP && obj.sz == 1);
    test(obj.type == STR && obj.sz == 5 && !strcmp(obj.str, "Hello"));
    test(obj.type == UNS && obj.sz == 1 && obj.uns == 1);

    unit("(+fixnum)");
    data("\x00"); test(obj.type == UNS && obj.uns == 0);
    data("\x01"); test(obj.type == UNS && obj.uns == 1);
    data("\x02"); test(obj.type == UNS && obj.uns == 2);
    data("\x0f"); test(obj.type == UNS && obj.uns == 0x0f);
    data("\x10"); test(obj.type == UNS && obj.uns == 0x10);
    data("\x7f"); test(obj.type == UNS && obj.uns == 0x7f);

    unit("(-fixnum)");
    data("\xff"); test(obj.type == SIG && obj.sig == -1);
    data("\xfe"); test(obj.type == SIG && obj.sig == -2);
    data("\xf0"); test(obj.type == SIG && obj.sig == -16);
    data("\xe0"); test(obj.type == SIG && obj.sig == -32);

    unit("(+int)");
    data("\xcc\x80"); test(obj.type == UNS && obj.uns == 0x80);
    data("\xcc\xff"); test(obj.type == UNS && obj.uns == 0xff);
    data("\xcd\x01\x00"); test(obj.type == UNS && obj.uns == 0x100);
    data("\xcd\xff\xff"); test(obj.type == UNS && obj.uns == 0xffff);
    data("\xce\x00\x01\x00\x00"); test(obj.type == UNS && obj.uns == 0x10000);
    data("\xce\xff\xff\xff\xff"); test(obj.type == UNS && obj.uns == 0xffffffffull);
    data("\xcf\x00\x00\x00\x01\x00\x00\x00\x00"); test(obj.type == UNS && obj.uns == 0x100000000ull);
    data("\xcf\xff\xff\xff\xff\xff\xff\xff\xff"); test(obj.type == UNS && obj.uns == 0xffffffffffffffffull);

    unit("(-int)");
    data("\xd0\xdf"); test(obj.type == SIG && obj.sig == -33);
    data("\xd0\x80"); test(obj.type == SIG && obj.sig == -128);
    data("\xd1\xff\x7f"); test(obj.type == SIG && obj.sig == -129);
    data("\xd1\x80\x00"); test(obj.type == SIG && obj.sig == -32768);
    data("\xd2\xff\xff\x7f\xff"); test(obj.type == SIG && obj.sig == -32769);
    data("\xd2\x80\x00\x00\x00"); test(obj.type == SIG && obj.sig == -2147483648ll);
    data("\xd3\xff\xff\xff\xff\x7f\xff\xff\xff"); test(obj.type == SIG && obj.sig == -2147483649ll);
    data("\xd3\x80\x00\x00\x00\x00\x00\x00\x00"); test(obj.type == SIG && obj.sig == INT64_MIN);

    unit("(misc)");
    data("\xc0"); test(obj.type == NIL && obj.chr == 0);
    data("\xc2"); test(obj.type == BOL && obj.chr == 0);
    data("\xc3"); test(obj.type == BOL && obj.chr == 1);

    data("\x90"); test(obj.type == ARR && obj.sz == 0);

    data("\x91\xc0");
    test(obj.type==ARR && obj.sz==1);
    test(obj.type==NIL);

    data("\x9f\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e");
    test(obj.type==ARR && obj.sz==15);
    for(int i = 0; i < 15; ++i) {
        test(obj.type==UNS && obj.sig==i);
    }

    data("\xdc\x00\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c"
         "\x0d\x0e\x0f");
    test(obj.type==ARR && obj.sz==16);
    for(unsigned i = 0; i < 16; ++i) {
        test(obj.type == UNS && obj.uns == i);
    }

    data("\x80");
    test(obj.type == MAP && obj.sz == 0);

    data("\x81\xc0\xc0");
    test(obj.type == MAP && obj.sz == 1);
    test(obj.type == NIL);
    test(obj.type == NIL);

    data("\x82\x00\x00\x01\x01");
    test(obj.type == MAP && obj.sz == 2);
    test(obj.type == UNS && obj.sig == 0);
    test(obj.type == UNS && obj.sig == 0);
    test(obj.type == UNS && obj.sig == 1);
    test(obj.type == UNS && obj.sig == 1);

    data("\x8f\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e"
         "\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d");
    test(obj.type == MAP && obj.sz == 15);
    for(unsigned i = 0; i < 15; ++i) {
        test(obj.type == UNS && obj.uns == i*2+0);
        test(obj.type == UNS && obj.uns == i*2+1);
    }

    data("\xde\x00\x10\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c"
         "\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c"
         "\x1d\x1e\x1f");
    test(obj.type == MAP && obj.sz == 16);
    for(unsigned i = 0; i < 16; ++i) {
        test(obj.type == UNS && obj.uns == i*2+0);
        test(obj.type == UNS && obj.uns == i*2+1);
    }

    data("\x91\x90");
    test( obj.type == ARR && obj.sz == 1 );
    test( obj.type == ARR && obj.sz == 0 );

    data("\x93\x90\x91\x00\x92\x01\x02");
    test( obj.type == ARR && obj.sz == 3 );
        test( obj.type == ARR && obj.sz == 0 );
        test( obj.type == ARR && obj.sz == 1 );
            test( obj.type == UNS && obj.uns == 0 );
        test( obj.type == ARR && obj.sz == 2 );
            test( obj.type == UNS && obj.uns == 1 );
            test( obj.type == UNS && obj.uns == 2 );

    data("\x95\x90\x91\xc0\x92\x90\x91\xc0\x9f\x00\x01\x02\x03\x04\x05\x06"
         "\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\xdc\x00\x10\x00\x01\x02\x03\x04"
         "\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f");
    test( obj.type == ARR && obj.sz == 5 );
        test( obj.type == ARR && obj.sz == 0 );
        test( obj.type == ARR && obj.sz == 1 );
            test( obj.type == NIL );
        test( obj.type == ARR && obj.sz == 2 );
            test( obj.type == ARR && obj.sz == 0 );
            test( obj.type == ARR && obj.sz == 1 );
                test( obj.type == NIL );
        test( obj.type == ARR && obj.sz == 15 );
            for( unsigned i = 0; i < 15; ++i ) {
                test( obj.type == UNS && obj.uns == i );
            }
        test( obj.type == ARR && obj.sz == 16 );
            for( unsigned i = 0; i < 15; ++i ) {
                test( obj.type == UNS && obj.uns == i );
            }

    data("\x85\x00\x80\x01\x81\x00\xc0\x02\x82\x00\x80\x01\x81\xc0\xc0\x03"
         "\x8f\x00\x00\x01\x01\x02\x02\x03\x03\x04\x04\x05\x05\x06\x06\x07"
         "\x07\x08\x08\x09\x09\x0a\x0a\x0b\x0b\x0c\x0c\x0d\x0d\x0e\x0e\x04"
         "\xde\x00\x10\x00\x00\x01\x01\x02\x02\x03\x03\x04\x04\x05\x05\x06"
         "\x06\x07\x07\x08\x08\x09\x09\x0a\x0a\x0b\x0b\x0c\x0c\x0d\x0d\x0e"
         "\x0e\x0f\x0f");
    test(obj.type == MAP && obj.sz == 5);
        test(obj.type == UNS && obj.uns == 0);
        test(obj.type == MAP && obj.sz == 0);
        test(obj.type == UNS && obj.uns == 1);
        test(obj.type == MAP && obj.sz == 1);
            test(obj.type == UNS && obj.uns == 0);
            test(obj.type == NIL);
        test(obj.type == UNS && obj.uns == 2);
        test(obj.type == MAP && obj.sz == 2);
            test(obj.type == UNS && obj.uns == 0);
            test(obj.type == MAP && obj.sz == 0);
            test(obj.type == UNS && obj.uns == 1);
            test(obj.type == MAP && obj.sz == 1);
                test(obj.type == NIL);
                test(obj.type == NIL);
        test(obj.type == UNS && obj.uns == 3);
        test(obj.type == MAP && obj.sz == 15);
            for( unsigned i = 0; i < 15; ++i ) {
                test(obj.type == UNS && obj.uns == i);
                test(obj.type == UNS && obj.uns == i);
            }
        test(obj.type == UNS && obj.uns == 4);
        test(obj.type == MAP && obj.sz == 16);
            for( unsigned i = 0; i < 16; ++i ) {
                test(obj.type == UNS && obj.uns == i);
                test(obj.type == UNS && obj.uns == i);
            }

    data("\x85\xd0\xd1\x91\xc0\x90\x81\xc0\x00\xc0\x82\xc0\x90\x04\x05\xa5"
         "\x68\x65\x6c\x6c\x6f\x93\xa7\x62\x6f\x6e\x6a\x6f\x75\x72\xc0\xff"
         "\x91\x5c\xcd\x01\x5e");
    test(obj.type == MAP && obj.sz == 5);
        test(obj.type == SIG && obj.sig == -47);
        test(obj.type == ARR && obj.sz == 1);
            test(obj.type == NIL);
        test(obj.type == ARR && obj.sz == 0);
        test(obj.type == MAP && obj.sz == 1);
            test(obj.type == NIL);
            test(obj.type == UNS && obj.uns == 0);
        test(obj.type == NIL);
        test(obj.type == MAP && obj.sz == 2);
            test(obj.type == NIL);
            test(obj.type == ARR && obj.sz == 0);
            test(obj.type == UNS && obj.uns == 4);
            test(obj.type == UNS && obj.uns == 5);
        test(obj.type == STR && !strcmp(obj.str, "hello"));
        test(obj.type == ARR && obj.sz == 3);
            test(obj.type == STR && !strcmp(obj.str, "bonjour"));
            test(obj.type == NIL);
            test(obj.type == SIG && obj.sig == -1);
        test(obj.type == ARR && obj.sz == 1);
            test(obj.type == UNS && obj.uns == 92);
        test(obj.type == UNS && obj.uns == 350);

    data("\x82\xa7" "compact" "\xc3\xa6" "schema" "\x00");
    test(obj.type == MAP && obj.sz == 2);
    test(obj.type == STR && obj.sz == 7 && !strcmp(obj.str, "compact"));
    test(obj.type == BOL && obj.chr == 1);
    test(obj.type == STR && obj.sz == 6 && !strcmp(obj.str, "schema"));
    test(obj.type == UNS && obj.sz == 1 && obj.uns == 0);

    printf("\r%d/%d tests passed", right, right + wrong);
    for(int i = 0; i < 60; ++i) printf(" "); puts("");
}

int main() {
    testunits();
    testwrite("out.mp");
    testdump("out.mp");
}

#endif
