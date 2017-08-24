EXPORT( STL100,
    // # kit.vec vectors
    // - clear container
    // - size of container
    // - access element at container
    // - insert element into back of container
    // - get last element from container
    // - erase last element from container
    // - iterate container

    API void vecclr( int id );
    API int  veclen( int id );
    API void vecget( int id, int slot );
    API void vecpush( int id, void * );
    API void*vecback( int id );
    API void vecpop( int id );
    API void vecfor( int id, int (*yield)( void *value ) );

    // # kit.map dictionaries
    // - clear container
    // - size of container
    // - insert element into container
    // - access element at container
    // - erase element from container
    // - iterate container

    API void mapclr( int id );
    API int  maplen( int id );
    API void mapput( int id, uint64_t key, void *value );
    API void*mapget( int id, uint64_t key );
    API void mapdel( int id, uint64_t key );
    API void mapfor( int id, int (*yield)( uint64_t key, void *value ) );
)
