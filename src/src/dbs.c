#include <stdbool.h>
#include "3rd/@adamierymenko/kissdb.h"
#include "3rd/@adamierymenko/kissdb.c"
#include "hexdump.c"

//#include "lib/va.c"
#include <stdarg.h>
char *vl( const char *fmt, va_list lst ); //$
char *va( const char *fmt, ... ); //$


typedef char key_t[256];
typedef char val_t[65536];

static KISSDB kdb = {0};

bool dbopen( const char *fmt, ... ) {
    va_list lst;
    va_start(lst, fmt);
    char *dbfile = vl(fmt, lst);
    va_end(lst);

    int HASH_BUCKET = 1024;
    int ok = 0 == KISSDB_open( &kdb, dbfile, KISSDB_OPEN_MODE_RWCREAT, HASH_BUCKET, sizeof(key_t), sizeof(val_t));
    return !!ok;
}
bool dbclose() {
    KISSDB_close( &kdb );
    return true;
}
bool dbget( val_t *val, const char *fmt, ... ) {
    va_list lst;
    va_start(lst, fmt);
    char *keystr = vl(fmt, lst);
    va_end(lst);

    key_t key = {0};
    strcpy(key, keystr);

    int result;
    int not_found;
    int ok = ( result = KISSDB_get( &kdb, key, val ) ) >= 0;
    not_found = result == 1;
    printf("rd %s=%s (rc: %d)\n", key, val, result);

    return result == 0 ? val : 0;
}
bool dbput( const val_t *val, const char *fmt, ... ) {
    va_list lst;
    va_start(lst, fmt);
    char *keystr = vl(fmt, lst);
    //void *val = va_arg(fmt, void *);
    va_end(lst);

    key_t key = {0};
    strcpy(key, keystr);

    int result;
    int ok = (result = KISSDB_put( &kdb, key, val )) >= 0;
    printf("wr (rc: %d)\n", result);

    return !!ok;
}

val_t *dbget2( const char *keystr ) {
    static val_t values[4];
    static int counter = 0;
    val_t *value = &values[ (counter++) % 4 ];
    if( !dbget( value, "%s", keystr ) ) memset( value, 0, sizeof(val_t));
    return value;
}
void dbput2( const char *keystr, const char *value ) {
    val_t dst = {0};
    strcpy( dst, value );
    dbput( &dst, "%s", keystr );
}
void dbput2i( const char *keystr, int value ) {
    val_t val = {0};
    sprintf(val, "%d", value);
    dbput( &val, "%s", keystr );
}

#if 0
int main( int argc, char **argv ) {
    if( dbopen("kissdb.db") ) {
        val_t val = "0"; // default value, in case not found
        dbget( &val, "/%s", "hello");

        val[0] ++; if( val[0] > '9' ) val[0] = '0';
        dbput( &val, "/%s", "hello");

        //for( int i = 0 ; i < 1000; ++i ) dbput(&val, "/test%d", i);

        // flush db
        dbclose();
    }
}
#endif
