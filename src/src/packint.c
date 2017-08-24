#pragma once
#include <stdint.h>

// packi16() -- store a 16-bit int into a char buffer (like htons())
static void packi16(uint8_t *buf, int16_t i, int swap) {
    uint16_t i2 = i;

    if(!swap) {
    buf[0] = i2>>8; buf[1] = i2>>0;
    } else {
    buf[1] = i2>>8; buf[0] = i2>>0;
    }
}

// packi32() -- store a 32-bit int into a char buffer (like htonl())
static void packi32(uint8_t *buf, int32_t i, int swap) {
    uint32_t i2 = i;

    if(!swap) {
    buf[0] = i2>>24; buf[1] = i2>>16;
    buf[2] = i2>>8;  buf[3] = i2>>0;
    } else {
    buf[3] = i2>>24; buf[2] = i2>>16;
    buf[1] = i2>>8;  buf[0] = i2>>0;
    }
}

// packi64() -- store a 64-bit int into a char buffer (like htonl())
static void packi64(uint8_t *buf, int64_t i, int swap) {
    uint64_t i2 = i;

    if(!swap) {
    buf[0] = i2>>56; buf[1] = i2>>48;
    buf[2] = i2>>40; buf[3] = i2>>32;
    buf[4] = i2>>24; buf[5] = i2>>16;
    buf[6] = i2>>8;  buf[7] = i2>>0;
    } else {
    buf[7] = i2>>56; buf[6] = i2>>48;
    buf[5] = i2>>40; buf[4] = i2>>32;
    buf[3] = i2>>24; buf[2] = i2>>16;
    buf[1] = i2>>8;  buf[0] = i2>>0;
    }
}

// unpacki16() -- unpack a 16-bit int from a char buffer (like ntohs())
static int16_t unpacki16(const uint8_t *buf, int swap) {
    uint16_t i2;
    int16_t i;

    if(!swap) {
    i2 = ((uint16_t)buf[0]<<8) | ((uint16_t)buf[1]<<0);
    } else {
    i2 = ((uint16_t)buf[1]<<8) | ((uint16_t)buf[0]<<0);
    }

    // change unsigned numbers to signed
    if (i2 <= 0x7fffu) { i = i2; }
    //else { i = -(int16_t)((uint16_t)0xffff - i2 + (uint16_t)1u); }
    else { i = -1 - (uint16_t)(0xffffu - i2); }

    return i;
}

// unpacki32() -- unpack a 32-bit int from a char buffer (like ntohl())
static int32_t unpacki32(const uint8_t *buf, int swap) {
    uint32_t i2;
    int32_t i;

    if(!swap) {
    i2 = ((uint32_t)buf[0]<<24) | ((uint32_t)buf[1]<<16) |
         ((uint32_t)buf[2]<<8)  | ((uint32_t)buf[3]<<0);
    } else {
    i2 = ((uint32_t)buf[3]<<24) | ((uint32_t)buf[2]<<16) |
         ((uint32_t)buf[1]<<8)  | ((uint32_t)buf[0]<<0);
    }

    // change unsigned numbers to signed
    if (i2 <= 0x7fffffffu) { i = i2; }
    else { i = -1 - (int32_t)(0xffffffffu - i2); }

    return i;
}

// unpacki64() -- unpack a 64-bit int from a char buffer (like ntohl())
static int64_t unpacki64(const uint8_t *buf, int swap) {
    uint64_t i2;
    int64_t i;

    if(!swap) {
    i2 = ((uint64_t)buf[0]<<56) | ((uint64_t)buf[1]<<48) |
         ((uint64_t)buf[2]<<40) | ((uint64_t)buf[3]<<32) |
         ((uint64_t)buf[4]<<24) | ((uint64_t)buf[5]<<16) |
         ((uint64_t)buf[6]<<8)  | ((uint64_t)buf[7]<<0 );
    } else {
    i2 = ((uint64_t)buf[7]<<56) | ((uint64_t)buf[6]<<48) |
         ((uint64_t)buf[5]<<40) | ((uint64_t)buf[4]<<32) |
         ((uint64_t)buf[3]<<24) | ((uint64_t)buf[2]<<16) |
         ((uint64_t)buf[1]<<8)  | ((uint64_t)buf[0]<<0 );
    }

    // change unsigned numbers to signed
    if (i2 <= 0x7fffffffffffffffu) { i = i2; }
    else { i = -1 -(int64_t)(0xffffffffffffffffu - i2); }

    return i;
}
