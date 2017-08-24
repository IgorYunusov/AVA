#pragma once

// api

void tlsinit( void (*func)(void *arg), void *arg );
void tlsquit( void (*func)(void *arg), void *arg );
void tlsin();
void tlsout();
