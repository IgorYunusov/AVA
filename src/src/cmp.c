// # cmp

#pragma once
#include <stdint.h>
#include <string.h>

static int cmp64i(const void *a, const void *b ) { return ( *((  int64_t*)a) - *((  int64_t*)b) ); }
static int cmp64u(const void *a, const void *b ) { return ( *(( uint64_t*)a) - *(( uint64_t*)b) ); }
static int cmp64f(const void *a, const void *b ) { return ( *((   double*)a) - *((   double*)b) ); }
static int cmp32i(const void *a, const void *b ) { return ( *((  int32_t*)a) - *((  int32_t*)b) ); }
static int cmp32u(const void *a, const void *b ) { return ( *(( uint32_t*)a) - *(( uint32_t*)b) ); }
static int cmp32f(const void *a, const void *b ) { return ( *((    float*)a) - *((    float*)b) ); }
static int cmp16i(const void *a, const void *b ) { return ( *((  int16_t*)a) - *((  int16_t*)b) ); }
static int cmp16u(const void *a, const void *b ) { return ( *(( uint16_t*)a) - *(( uint16_t*)b) ); }
static int cmpptr(const void *a, const void *b ) { return ( *((uintptr_t*)a) - *((uintptr_t*)b) ); }
static int cmpstr(const void *a, const void *b ) { return   strcmp((const char*)a,(const char*)b); }
