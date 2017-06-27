// # log ######################################################################

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef SHIPPING
void logme( FILE *fp ) {}
#else
static FILE* logfile[32] = {0};
void logme( FILE *fp ) { $
    if( fp ) {
        // find empty slot
        int slot = 0;
        while( logfile[slot] && slot < 32 ) ++slot;
        //
        if( slot < 32 ) {
            logfile[slot] = fp;
            #ifdef NDEBUG
            // Flush automatically every 16 KiB from now
            setvbuf(fp, NULL, _IOFBF, 16 * 1024);
            #else
            // Disable buffering, always fflush()
            setvbuf(fp, NULL, _IOFBF, 2);
            #endif
            // Header
            fprintf(fp, "--- New session [built %s]\n", __TIMESTAMP__);
            fflush(fp);
        }
    }
}
static THREAD_LOCAL FILE *logvl_file = 0;
static int logvl_puts( const char *text ) { return fprintf(logvl_file, "%s\n", text); }
void logvl( const char *file, int line, const char *tags, const char *format, va_list lst ) { $
    char *vastr = 0;
    for( int i = 0; logfile[i]; ++i ) {
        if( !vastr ) {
            vastr = vl( &format[ format[0] == '!' ], lst );
            vastr = va( "%019lld [%s] %s (%s:%d)\n", 0ULL/*date64(ust(),gmt())*/, tags, vastr, file, line );
        }
        fputs(vastr, logfile[i]);
        if( format[0] == '!' ) {
            logvl_file = logfile[i];
            callstack( +16, logvl_puts );
        }
    }
}
void logva( const char *file, int line, const char *tags, const char *format, ... ) { $
    va_list vl;
    va_start( vl, format );
    logvl( file, line, tags, format, vl );
    va_end( vl );
}
#endif
