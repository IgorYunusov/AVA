// # endianness

#pragma once
#include <stdint.h>

#ifndef LITTLE
#define LITTLE 1
#endif

static int little() { return LITTLE; } // (!*(const char*)(L"\0\1")); }

static uint16_t swap16( uint16_t x ) { return (x << 8) | (x >> 8); }
static uint32_t swap32( uint32_t x ) { return (x << 24) | (x >> 24) | ((x & 0xff00) << 8) | ((x >> 8) & 0xff00); }
static uint64_t swap64( uint64_t x ) { return (x << 56) | (x >> 56) | ((x & 0xff00) << 40) | ((x >> 40) & 0xff00) | ((x & 0xff0000) << 24) | ((x >> 24) & 0xff0000) | ((x & 0xff000000) << 8) | ((x >> 8) & 0xff000000); }

static uint16_t ntoh16( uint16_t x ) { return little() ? swap16(x) : x; }
static uint32_t ntoh32( uint32_t x ) { return little() ? swap32(x) : x; }
static uint64_t ntoh64( uint64_t x ) { return little() ? swap64(x) : x; }

static uint16_t hton16( uint16_t x ) { return little() ? swap16(x) : x; }
static uint32_t hton32( uint32_t x ) { return little() ? swap32(x) : x; }
static uint64_t hton64( uint64_t x ) { return little() ? swap64(x) : x; }

static uint16_t conv16( uint16_t x ) { return little() ? swap16(x) : x; }
static uint32_t conv32( uint32_t x ) { return little() ? swap32(x) : x; }
static uint64_t conv64( uint64_t x ) { return little() ? swap64(x) : x; }
