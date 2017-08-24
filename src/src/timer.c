#include <stdbool.h>

// # api

// - exec func every ss seconds
// - exec func after ss seconds (only once)
// - cancel timer

void* every(  float ss, void (*func)(void *usrdata), void *usrdata );
void* delay(  float ss, void (*func)(void *usrdata), void *usrdata );
void  cancel( void *id );

// impl

/* #include "thread.c" */ void sleep(float ss, float ms, float us, float ns);
/* #include "thread.c" */ bool detach( void (*func)(void *), void *arg );
#include <stdlib.h>

typedef struct opaque {
    void (*func)(void *arg);
    void *usrdata;
    float secs;
    int id;
    int counter;
} opaque;

static void every_( void *arg ) {
    for( ;; ) {
        ((opaque*)arg)->func( ((opaque*)arg)->usrdata );
        sleep( ((opaque*)arg)->secs, 0,0,0 );
    }
    free( arg );
}

static void delay_( void *arg ) {
    sleep( ((opaque*)arg)->secs, 0,0,0 );
    ((opaque*)arg)->func( ((opaque*)arg)->usrdata );
    free( arg );
}

void *every( float secs, void (*func)(void *arg), void *arg ) {
    opaque *o = (opaque*)malloc( sizeof(opaque) );
    o->func = func;
    o->usrdata = arg;
    o->secs = secs;
    o->id = 0;
    detach( every_, o );
    return o;
}

void *delay( float secs, void (*func)(void *arg), void *arg ) {
    opaque *o = (opaque*)malloc( sizeof(opaque) );
    o->func = func;
    o->usrdata = arg;
    o->secs = secs;
    o->id = 0;
    detach( delay_, o );
    return o;
}

#ifdef DEMO
#undef DEMO
#include "thread.c"
#include <stdio.h>

void test( void *text ) {
    puts( (const char *)text );
}

int main() {
    delay( 1, test, "delayed hello" );
    sleep( 2, 0,0,0 );

    every( 1, test, "hello every second" );
    sleep( 4, 0,0,0 );
}

#endif
