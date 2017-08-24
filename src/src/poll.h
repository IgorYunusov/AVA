// # cmp

#include "cmp.c"

// api

#include <stdint.h>

#ifndef EVENTS_MAX
#define EVENTS_MAX 16384
#endif

typedef void (*event_handler)(int name, void *data, int size);

void init();
void emit( uint32_t time, uint8_t type, uint32_t name, const void *data, int size );
void pump();

void poll( uint8_t type, event_handler fn );

// impl

#include <stdlib.h>

typedef struct event {
    // 64-bit sortable id: timestamp first, then factory type, then event name id
    struct {
        uint32_t time: 32;
        uint32_t type:  8;
        uint32_t name: 24;
    } sort;
    // event user data
    void *data;
    int   size;
} event;

typedef event_is_sortable_assert[ sizeof(((struct event*)0)->sort) == 8 ];

static int count[256] = {0};
static event_handler handler[256] = {0};
static event *events = 0, *next;

void poll( uint8_t factory, event_handler fn ) {
    handler[factory] = fn;
}

void init() {
    events = (event*)realloc(events, EVENTS_MAX * sizeof(event));
    memset( events, 0, EVENTS_MAX * sizeof(event) );
    memset( count, 0, sizeof(count) );
    next = events;
}

void emit( uint32_t time, uint8_t type, uint32_t name, const void *data, int size ) {
    count[type]++;
    event *e = next++;
    e->sort.time = time;
    e->sort.type = type;
    e->sort.name = name;
    e->data = size ? memcpy( malloc( size ), data, size ) : (void *)0;
    e->size = size;
}

void pump() {
    // high-level consumer: delegates events to low-level consumers.
    int num_events = next - events;

    // - sort events
    // - insert eof stream mark

    qsort( events, num_events, sizeof(event), cmp64u );
    emit(0, 0, 0, 0, 0);

    // - 1) iterate time
    // - 2) iterate factories
    // - 3) advance events

    next = events;
    uint64_t timer = 0 + (1ULL << 32ULL), factory = 0;
    while( next->sort .time < timer ) {
        int factory = next->sort.type; //(next->sort >> 24ULL) & 0xff;
        if( !factory ) break;
        event_handler fn = handler[factory];
        int *c = &count[ factory ];
        while( *c-- ) {
            fn( next->sort.name, next->data, next->size );
            ++next;
        }
    }

    // clean up
    for( int i = 0; i < num_events; ++i ) {
        if( events[i].data ) {
            free( events[i].data );
        }
    }
    init();
}

// demo
#ifdef DEMO

#include <stdio.h>
#include <string.h>

// sorted by priority <<. good idea?
// should be just like 'tags' instead ?
enum { OS = 1, EDIT = 2, MENU = 4, GAME = 8, UIUX = 16, CLIENT = 32, SERVER = 64 };

enum {
    DEAD = 0xDEAD,
    CAFE = 0xCAFE,
    BEEF = 0xBEEF,
    BABE = 0xBABE,
};

void echo_handler( int event, void *data, int size ) {
    printf("1. %x %.*s\n", event, size, data );
}
void echo_handler2( int event, void *data, int size ) {
    printf("2. %x %.*s\n", event, size, data );
}

int main() {
    init();
    poll( UIUX, echo_handler );
    poll( GAME, echo_handler );
    poll( EDIT, echo_handler2 );

    int t = 0;

    emit( t, UIUX, DEAD, 0, 0 );
    emit( t, UIUX, BEEF, 0, 0 );
    emit( t, UIUX, BABE, "hello", strlen("hello") );

    emit( t, GAME, CAFE, 0, 0 );
    emit( t, EDIT, CAFE, 0, 0 );

    pump();
    pump();
    pump();
}

#endif
