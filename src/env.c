// # env ######################################################################

#include <stdlib.h>
void envput( const char *key, const char *value ) { $
#ifdef _WIN32
    char buf[1024];
    sprintf(buf,"%s=%s", key, value ? value : "");
    putenv( buf );
#else
    setenv( key, value ? value : "", 1 );
#endif
}
const char *envget( const char *key ) { $
    return getenv(key);
}
