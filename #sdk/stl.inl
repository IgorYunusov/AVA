EXPORT( STL100,
    // # kit.vec vectors
    // - clear container
    // - size of container
    // - access element at container
    // - insert element into back of container
    // - get last element from container
    // - erase last element from container
    // - iterate container

    API void vec_clr( int id );
    API int  vec_len( int id );
    API void vec_get( int id, int slot );
    API void vec_push( int id, void * );
    API void*vec_back( int id );
    API void vec_pop( int id );
    API void vec_for( int id, int (*yield)( void *value ) );

    // # kit.map dictionaries
    // - clear container
    // - size of container
    // - insert element into container
    // - access element at container
    // - erase element from container
    // - iterate container

    API void map_clr( int id );
    API int  map_len( int id );
    API void map_put( int id, uint64_t key, void *value );
    API void*map_get( int id, uint64_t key );
    API void map_del( int id, uint64_t key );
    API void map_for( int id, int (*yield)( uint64_t key, void *value ) );

    // # kit.mtx mutex
    // - try to lock a mutex. returns 0 if cannot lock
    // - lock a mutex
    // - unlock a mute

    API int  try_lock(int mutex_id);
    API void lock(int mutex_id);
    API void unlock(int mutex_id);

    // # kit.thd thread
    // - system process id
    // - thread id
    // - number of max concurrent hardware threads (cores)
    // - detach thread
    // - start thread
    // - join thread

    API int pid();
    API int tid();
    API int cores();
    API void detach(void(*fn)(void *), void *arg);
    API void thread(int thread_id, void(*fn)(void *), void *arg);
    API void join(int thread_id);

    // # time
    // # kit.clk yield and sleep
    // - yield current thread
    // - sleep current thread

    API void yield();
    API void sleep_ns( unsigned ns );
    API void sleep_us( unsigned us );
    API void sleep_ms( unsigned ms );
    API void sleep_ss( unsigned ss );
)
