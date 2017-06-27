// # atomics ##################################################################

void atmset( int *var, const int value ) {
    #ifdef _WIN32
    _InterlockedExchange( (long volatile *)(var), (long)value );
    #else
    __sync_lock_test_and_set( var, value );
    #endif
}
int atmget( int *var ) {
    #ifdef _WIN32
    return _InterlockedExchangeAdd( (long volatile *)(var), 0 );
    #else
    return __sync_fetch_and_add( var, 0 );
    #endif
}
int atmadd( int *var, const int value ) {
    #ifdef _WIN32
    return _InterlockedExchangeAdd( (long volatile *)(var),(long)value );
    #else
    return __sync_fetch_and_add( var, value );
    #endif
}
int atminc( int *var ) {
    #ifdef _WIN32
    return _InterlockedIncrement( (long volatile *)(var) );
    #else
    return __sync_add_and_fetch( var, 1 );
    #endif
}
int atmdec( int *var ) {
    #ifdef _WIN32
    return _InterlockedDecrement( (long volatile *)(var) );
    #else
    return __sync_add_and_fetch( var, -1 );
    #endif
}
