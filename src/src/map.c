// # map
// [ref] http://www.idryman.org/blog/2017/05/03/writing-a-damn-fast-hash-table-with-tiny-memory-footprints/
// @todo: would be nice to have..  patch,diff,load,save ?

#pragma once
#include <stdint.h>
#include <stdbool.h>

// api

typedef struct pair {
    struct pair *next;
    uint64_t hashedkey;
    /* stuff */
    const char *key;
    int value;
} pair;

void  map_new();
void  map_put(const char *key, /*void * */ int value);
void  map_del(const char *key);
pair* map_get(const char *key);
void  map_for(bool (*iter)(pair *iter, void* usrdata), void *usrdata);
 int  map_len();
void  map_trim();
void  map_free();

// impl

#if 0
#include <map>
static std::map< uint64_t, void * > maps[32+32];
void map_new( int id ) { maps[ id ].clear(); }
 int map_len( int id ) { return (int)maps[ id ].size(); }
void map_put( int id, uint64_t key, void *value ) { maps[ id ][ key ] = value; }
void*map_get( int id, uint64_t key ) { return maps[ id ][ key ]; }
void map_del( int id, uint64_t key ) { maps[ id ][ key ]; maps[ id ].erase( key ); }
void map_for( int id, int (*yield)( uint64_t, void * ) ) { for( auto &kv : maps[ id ] ) if( !yield( kv.first, kv.second ) ) break; }
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "shared.h"

// config
#ifndef MAP_HAS_DELAYED_FREE
#define MAP_HAS_DELAYED_FREE 1
#endif

enum { MAP_HASHSIZE = 4096 << 4, MAP_GC_SLOT = MAP_HASHSIZE };
typedef int map_is_hashsize_pow_of_two[ !(MAP_HASHSIZE & (MAP_HASHSIZE - 1)) ];

// compute fast hash. faster than fnv64, a few more collisions though.
// returns buffer len
static int hashkey(const void *key, uint64_t *hash) {
    unsigned char *buf = (unsigned char *)key;
    uint64_t lhash = 0;
    while( *buf ) {
        lhash = (lhash ^ *buf++) * 131;
    }
    *hash = lhash;
    return (buf - (unsigned char *)key);
}

static int get_index(uint64_t hkey1) {
    return hkey1 & (MAP_HASHSIZE-1); // assert(pow_of_two(MAP_HASHSIZE));
    //return hkey1 % MAP_HASHSIZE;
    //~"pseudo quadratic probing", lemire
    //return (uint32_t)(((uint64_t)hkey1 * (uint64_t)MAP_HASHSIZE) >> 32);
}

__thread struct pair **table = 0;

void map_new() {
    bool needs_clear = table == 0;
    table = (struct pair **)realloc( table, sizeof(struct pair *) * (MAP_HASHSIZE+1) );
    if( needs_clear ) memset(table, 0, sizeof(struct pair *) * (MAP_HASHSIZE+1) );
}

void map_put(const char *key, int value) {
    uint64_t keyhash;
    int keylen = hashkey(key, &keyhash);

    struct pair *i = (struct pair*)malloc(sizeof(struct pair) + keylen);
    i->hashedkey = keyhash;
    i->value = value;
    i->key = &((char*)(void*)i)[ sizeof(struct pair) ];
    memcpy((char *)i->key, key, keylen);

    /* Insert onto the beginning of the list */
    int index = get_index(keyhash);
    i->next = table[index];
    table[index] = i;
}

pair*map_get(const char *key) {
    uint64_t keyhash;
    int keylen = hashkey(key, &keyhash);
    int index = get_index(keyhash);
    struct pair *cur;
    for( cur = table[index]; cur; cur = cur->next ) {
        if( cur->hashedkey == keyhash ) {
            if( !memcmp(cur->key, key, keylen) ) {
                return cur; // (void**)&cur->value;
            }
        }
    }
    return 0;
}

void map_for(bool (*iter)(struct pair *, void *usrdata), void *usrdata) {
    struct pair *cur;
    for( int i = 0; i < MAP_HASHSIZE; ++i) {
        for( cur = table[i]; cur; cur = cur->next ) {
            if( true != iter(cur, usrdata) ) {
                return;
            }
        }
    }
}

void map_del(const char *key) {
    uint64_t keyhash;
    int keylen = hashkey(key, &keyhash);
    int index = get_index(keyhash);
    struct pair *cur,*prev = 0;
    for( cur = table[index]; cur; (prev = cur), (cur = cur->next) ) {
        if( cur->hashedkey == keyhash ) {
            if( !memcmp(cur->key, key, keylen) ) {
                if( prev ) prev->next = cur->next; else table[index] = 0;
#if MAP_HAS_DELAYED_FREE
                /* Insert onto the beginning of the GC list */
                cur->next = table[MAP_GC_SLOT];
                table[MAP_GC_SLOT] = cur;
#else
                free(cur);
#endif
                return;
            }
        }
    }
}

int map_len() {
    int counter = 0;
    for( int i = 0; i < MAP_HASHSIZE; ++i) {
        struct pair *cur;
        for( cur = table[i]; cur; cur = cur->next ) {
            ++counter;
        }
    }
    return counter;
}

void map_trim() {
#if MAP_HAS_DELAYED_FREE
    struct pair *cur, *next;
    for( cur = table[MAP_GC_SLOT]; cur; cur = next ) {
        next = cur->next;
        free(cur);
    }
    table[MAP_GC_SLOT] = 0;
#endif
}

void map_free() {
    for( int i = 0; i <= MAP_HASHSIZE; ++i) {
        struct pair *cur, *next;
        for( cur = table[i]; cur; cur = next ) {
            next = cur->next;
            free(cur);
        }
    }
    free(table), table = 0;
}



#if defined(DEMO) && defined(__cplusplus) // && defined(TEST)
#include <stdio.h>
#include <omp.h>
#include <unordered_map>
#include <map>
#include <string>
#ifdef HASHMAP
#include "hashmap/hashmap.h"
#include "hashmap/hashmap.c"
#endif

typedef void *any;

bool iter2(any userdata, any data) {
  return true;
}
bool iter3(pair *iter, void *userdata) {
  return true;
}

#define BENCH(NEW,SIZE,PUT,GET,EACH,DEL,FREE) do \
{ \
    enum { J = 1000 }; \
    enum { N = 50000 }; \
    static char **bufs = 0; \
    if(!bufs) { \
        bufs = (char **)malloc( sizeof(char*) * N ); \
        for( int i = 0; i < N; ++i ) { \
            bufs[i] = (char*)malloc(16); \
            sprintf(bufs[i], "%d", i); \
        } \
    } \
    double t = -omp_get_wtime(); \
    for( int j = 0; j < J; ++j ) { \
        NEW; \
        if(j==0) printf("N:%d ", (int)SIZE), fflush(stdout); \
        for( int i = 0; i < N; ++i ) { \
            char *buf = bufs[i]; \
            PUT; \
        } \
        if(j==0) printf("P:%d ", (int)SIZE), fflush(stdout); \
        for( int i = 0; i < N; ++i ) { \
            char *buf = bufs[i]; \
            GET; \
        } \
        if(j==0) printf("G:%d ", (int)SIZE), fflush(stdout); \
        EACH; \
        if(j==0) printf("E:%d ", (int)SIZE), fflush(stdout); \
        for( int i = 0; i < N; ++i ) { \
            char *buf = bufs[i]; \
            DEL; \
        } \
        if(j==0) printf("D:%d ", (int)SIZE), fflush(stdout); \
        FREE; \
        if(j==0) printf("FREE ", 0), fflush(stdout); \
    } \
    t += omp_get_wtime(); \
    printf("%fs " #NEW /*"C map(char*,int)"*/ "\n", t); \
} while(0)

#define XX(...) __VA_ARGS__
#include <assert.h>

int main ()
{
    {
        map_new();
        assert( map_len() == 0 );
        map_put("123", 123);
        map_put("456", 456);
        assert( map_len() == 2 );
        assert( map_get("123") );
        assert( map_get("456") );
        assert(!map_get("789") );
        assert( map_len() == 2 );
        assert( map_get("123")->value == 123);
        assert( map_get("456")->value == 456);
        map_del("123");
        assert( map_get("456")->value == 456);
        assert( map_len() == 1 );
        map_del("456");
        assert( map_len() == 0 );
        map_free();
    }

    BENCH(
        XX(map_new()),
        XX(map_len()),
        XX(map_put(buf, i)),
        XX(map_get(buf)),
        XX(map_for(iter3, (any)0)),
        XX(map_del(buf)),
        XX(map_free())
    );

#ifdef HASHMAP
    int unused = 0;
    BENCH(
        XX(map_t M = hashmap_new()),
        XX(hashmap_length(M)),
        XX(hashmap_put(M, buf, (any)i)),
        XX(hashmap_get(M, buf, (any*)&unused)),
        XX(hashmap_iterate(M, (PFany)iter2, (any)0)),
        XX(hashmap_remove(M, buf)),
        XX(hashmap_free(M))
    );
#endif

    BENCH(
        XX(std::map<const char *, int> v),
        XX(v.size()),
        XX(v.insert(std::make_pair(buf, i))),
        XX(v.find(buf)),
        XX(for( auto &kv : v ) { iter2( (any)0, (any)&kv ); }),
        XX(v.erase(buf)),
        XX({})
    );

    BENCH(
        XX(std::unordered_map<const char *, int> v),
        XX(v.size()),
        XX(v.insert(std::make_pair(buf, i))),
        XX(v.find(buf)),
        XX(for( auto &kv : v ) { iter2( (any)0, (any)&kv ); }),
        XX(v.erase(buf)),
        XX({})
    );
}
#endif

