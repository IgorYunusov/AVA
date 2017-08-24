#pragma once

// api

void tlsinit( void (*func)(void *arg), void *arg );
void tlsquit( void (*func)(void *arg), void *arg );
void tlsin();
void tlsout();

// impl

struct tls {
	void (*funs[256])( void *arg );
	void *args[256];
	int nums;
} inits = {0}, quits = {0};

void tlsinit( void (*func)(void *arg), void *arg ) {
	int with = inits.nums++;
	inits.funs[ with ] = func;
	inits.args[ with ] = arg;
}
void tlsquit( void (*func)(void *arg), void *arg ) {
	int with = quits.nums++;
	quits.funs[ with ] = func;
	quits.args[ with ] = arg;
}
void tlsin() {
	for( int i = 0; i < inits.nums; ++i ) {
		inits.funs[i]( inits.args[i] );
	}
}
void tlsout() {
	for( int i = inits.nums; i-- > 0; ) {
		quits.funs[i]( quits.args[i] );
	}
}

// demo

#ifdef DEMO

#include <stdio.h>
#include "thread.c"

void myinit1(void *arg) { puts("hi1!"); }
void myinit2(void *arg) { puts("hi2!"); }
void myquit2(void *arg) { puts("bye2!"); }
void myquit1(void *arg) { puts("bye1!"); }

void hello(void *arg) { puts("hello"); }

int main() {
	tlsinit( myinit1, 0 );
	tlsquit( myquit1, 0 );
	tlsinit( myinit2, 0 );
	tlsquit( myquit2, 0 );

	detach( hello, 0 );
	sleep(1,0,0,0);

	thread( 10, hello, 0 );
	join(10);
}

#endif
