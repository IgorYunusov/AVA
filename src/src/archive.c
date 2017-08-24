// @todo -patchpaddingalign=65536

// AVA archive format
//
// - [x] no file number limit
// - [x] no file size limit
// - [x] append only archive creation
// - [x] data is always zero padded to pow^2 multiples: 1,2,4,8,16.. KiB
// - [x] cheap preload of initial bytes for all archived files.
// - [x] layered toc
//
// spec:
// rom.x00 contains the toc: every line is a new file entry whose format is %u %u %s,mtime,fsize,pathfile
// rom.x01 contains the first  1024 bytes for every indexed file.
// rom.x02 contains the next   2048 bytes for every indexed file.
// rom.x03 contains the next   4096 bytes for every indexed file.
// rom.x04 contains the next   8192 bytes for every indexed file.
// rom.x05 contains the next  16384 bytes for every indexed file.
// rom.x06 contains the next  32768 bytes for every indexed file.
// rom.x07 contains the next  65536 bytes for every indexed file.
// rom.x08 contains the next 131072 bytes for every indexed file.
// etc.
// tip: when mounting archives, scan toc and keep only latest file if duplicated.
// tip: when shipping reorder toc according GameOpenOrder.log to outer ring.
// tip: rom.x00, then 001.x00 (first update), 002.x00 (second update), etc


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
static
size_t get_mtime( const char *path ) {
    struct stat attr;
    stat(path, &attr);
    printf("Last modified time: %s\n", ctime(&attr.st_mtime));
    return attr.st_mtime;
}

bool arc_new() {
    int errors = 0;
    for( int i = 0; i < 0xff; ++i ) {
        char name[16];
        sprintf( name, "rom.x%02x", i );
        unlink( name );
        FILE *fp = fopen(name, "rb");
        if( fp ) { ++errors; fclose(fp); }
    }
    return errors ? false : true;
}

bool arc_add( const char *filename ) {
    FILE *in = fopen( filename, "rb" );
    fseek( in, 0L, SEEK_END );
    size_t size = ftell( in );
    fseek( in, 0L, SEEK_SET );

    char *buffer = 0;
    size_t block = 1;
    size_t bytes = 1024;
    while( !feof(in) ) {
        buffer = (char *)realloc( buffer, bytes );
        memset( buffer, 0, bytes );

        fread( buffer, 1, size > bytes ? bytes : size, in );

        char name[16];
        sprintf( name, "rom.x%02x", block );
        for( FILE *fp = fopen(name, "a+b"); fp; fclose(fp), fp = 0) {
            fwrite( buffer, 1, bytes, fp );
        }

        block++;
        bytes *= 2; // += 64 * 1024; // += bytes / 2; //*= 2; // 64 * 1024; //
    }
    free( buffer );

    for( FILE *fp = fopen("rom.x00", "a+b"); fp; fclose(fp), fp = 0) {
        size_t mtime = get_mtime( filename );
        fprintf( fp, "%u %u %s\n", (unsigned)mtime, (unsigned)size, filename );
    }

    fclose( in );
    return true;
}

#ifdef DEMO
int main( int argc, char **argv ) {
    if( argc > 1 && !strcmp(argv[1], "-new") ) {
        arc_new();
        argc--;
        argv++;
    }

    for( int i = 1; i < argc; ++i ) {
        arc_add( argv[i] );
    }
}
#endif

