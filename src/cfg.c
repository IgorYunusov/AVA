// # cfg ######################################################################

const char *env( const char *defaults, const char *csv_sets ) { $
    int tksizes[128];
    const char *tokens[128];
    if( strchop( tokens,tksizes,128,csv_sets, "," ) ) {
        for( int t = 0; tokens[t]; ++t ) {
            const char *val = getenv( va("%.*s", tksizes[t], tokens[t]) );
            if( val ) return val;
        }
    }
    return defaults;
}

const char *arg( const char *defaults, const char *csv_opts ) { $
    int tksizes[128];
    const char *tokens[128];
    if( strchop( tokens,tksizes,128,csv_opts, "," ) ) {
        int argc = __argc-1;
        const char **argv = (const char **)__argv+1;
        for( int t = 0; tokens[t]; ++t ) {
            for( int i = 0; i < argc; ++i ) {
                int s = argv[i][0] == '-';
                if( argv[i][s] == '-' ) ++s;
                if( strlen(&argv[i][s]) >= tksizes[t] && !memcmp( &argv[i][s], tokens[t], tksizes[t] ) ) {
                    s += tksizes[t];
                    if( argv[i][s] == '=' || argv[i][s] == 0 ) {
                        if( argv[i][s] == '=') ++s;
                        return argv[i][s] != 0 ? &argv[i][s] : 0;
                    }
                }
            }
        }
    }
    return defaults;
}

const char *ini( const char *defaults, const char *csv_keys ) { $
    static char *keys[128] = {0}, *values[128];
    if( !keys[0] ) {
        char *inibuf = (char *)iofread( va("%s/%s.ini", usrgame(), usrbin()) );
        if( inibuf ) {
            int sizes[128];
            char *lines[128];
            if( strchop( (const char **)lines, sizes, 128, inibuf, "\r\n" ) ) {
                for( int t = 0; lines[t]; ++t ) {
                    keys[t]   = strtrimr( lines[t], "=" );
                    values[t] = &lines[t][ strlen(keys[t]) + 1 ];
                    values[t] = strtrimr( values[t], "\r" );
                    values[t] = strtrimr( values[t], "\n" );
                }
            }
        }
        // FREE(inibuf); // controlled leak
    }
    if( keys[0] ) {
        int tksizes[128];
        const char *tokens[128];
        if( strchop( tokens,tksizes,128,csv_keys, "," ) ) {
            for( int k = 0; keys[k]; ++k ) {
                int lkey = strlen(keys[k]);
                for( int t = 0; tokens[t]; ++t ) {
                    if( lkey >= tksizes[t] && !memcmp( keys[k], tokens[t], tksizes[t] ) ) {
                        return values[k];
                    }
                }
            }
        }
    }
    return defaults;
}

const char* cfg(const char *defaults, const char *csv_vars) { $
    const char *found;
    if( 0 != (found = env(0, csv_vars))) return found;
    if( 0 != (found = arg(0, csv_vars))) return found;
    if( 0 != (found = ini(0, csv_vars))) return found;
    return defaults;
}

static
int cfgi( int defaults, const char *key ) { $
    const char *found = cfg(0, key);
    return found ? atoi( found ) : defaults;
}
