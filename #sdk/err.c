// # err ######################################################################

static
errorcode error_cb( const char *message ) {
    return tty( message ) ? 0 : -1;
}

void error( const char *message ) { // $
    if( message[0] == '!' ) {
        callstack( 128, error_cb );
    }
    tty( &message[ message[0] == '!' ] );
}

void fatal( const char *message ) { // $
    error( message );
    exit(-1);
}

void panic( const char *message ) { // $ // NO_RETURN
    if( oom ) FREE(oom), oom = 0;
    error( message );
    $devel(
        if( debugging() || getenv("AVADEV") ) {
            breakpoint();
        } else {
            tty( "; press return key to continue..." );
            getchar();
        }
    ,)
    // dialog("tmi", "General failure", error, "error");
    die();
    // abort           causes abnormal program termination (without cleaning up)
    // exit            causes normal program termination with cleaning up
    // _Exit           causes normal program termination without cleaning up (C99)
    // atexit          registers a function to be called on exit() invocation
    // quick_exit      causes normal program termination without cleaning up, but with IO buffers flushed (C11)
    // at_quick_exit   registers a function to be called on quick_exit() invocation
}

