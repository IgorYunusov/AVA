// # dir ######################################################################
// @ todo: dirstem(), dirup() { path(path(dir)); }
// @ todo: dirmk(), dirrm(), dirrmrf()

#include <stdlib.h> // realpath
#if _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
#if !defined(PATH_MAX)
# ifdef _MSC_VER
#  define PATH_MAX _MAX_PATH
# else
#  define PATH_MAX 260
# endif
#endif
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
static
void dirch( const char *path ) { $
#ifdef _MSC_VER
    _chdir( path );
#else
    chdir(path);
#endif
}
char *dirfix(char *pathfile) { $
    for( char *p = pathfile; *p; ++p ) {
        if( *p == '\\' ) {
            *p = '/';
        }
    }
    return pathfile;
}
char *dirpath( char *dir ) { $
    char *s = strrchr( dirfix(dir), '/' );
    return s ? (s[1] = '\0', dir) : (dir[0] = '\0', dir);
}
char *dirbase( char *dir ) { $
    char *s = strrchr( dirfix(dir), '/' );
    if( s ) strcpy( dir, s+1 );
    return dir;
}
char *dirname( char *dir ) { $
    char *t = strstr( dirbase( dir ), "." );
    if( t ) *t = 0;
    return dir;
}
char *dirtype( char *dir ) { $
    char *t = strstr( dirbase( dir ), "." );
    if( t ) strcpy( dir, t /*+1*/ );
    return dir;
}
char *dirabs(char **pathfile) { $
    char absolute[PATH_MAX+1];
#ifdef _MSC_VER
    _fullpath(absolute, *pathfile, PATH_MAX);
    dirfix(*pathfile);
#else
    realpath(*pathfile, absolute);
#endif
    int len = strlen(absolute) + 1;
    *pathfile = REALLOC( *pathfile, len );
    memcpy( *pathfile, absolute, len );
    return *pathfile;
}
bool dirisabs(const char *pathfile) { $
    bool equal = 1;
    char *abspath = strdup(pathfile);
    dirabs(&abspath);
    for( int i = 0; pathfile[i] && abspath[i] && equal; ++i ) {
        equal = (pathfile[i] & 32) == (abspath[i] & 32);
    }
    FREE( abspath );
    return equal;
}
// ---
#include <string.h> // strrchr, strstr
#include <stdio.h>  // sprintf
#include <stdlib.h> // realloc
#ifdef _WIN32
#include <winsock2.h>
#else
#include <dirent.h>
#endif
static char *ls_strcat(char **buf, const char *a, const char *b, const char *c) {
    int len = strlen(a) + (b ? strlen(b) : 0) + (c ? strlen(c) : 0);
    sprintf(*buf = (char *)REALLOC( *buf, len + 1 ), "%s%s%s", a, b ? b : "", c ? c : "");
    return *buf;
}
static int ls_strmatch( const char *text, const char *pattern ) { $
    if( *pattern=='\0' ) return !*text;
    if( *pattern=='*' )  return ls_strmatch(text, pattern+1) || (*text && ls_strmatch(text+1, pattern));
    if( *pattern=='?' )  return *text && (*text != '.') && ls_strmatch(text+1, pattern+1);
    return (*text == *pattern) && ls_strmatch(text+1, pattern+1);
}
// requires: source folder to be a path; ie, must end with '/'
// requires: match pattern to be '*' at least
// yields: matching entry, ends with '/' if dir.
// returns: number of matching entries
static int ls_recurse( int recurse, const char *src, const char *pattern, errorcode (*yield)(const char *name) ) {
    char *dir = 0;
    int count = 0;
#ifdef _WIN32
    WIN32_FIND_DATAA fdata;
    for( HANDLE h = FindFirstFileA( ls_strcat(&dir, src, "*", 0), &fdata ); h != INVALID_HANDLE_VALUE; FindClose( h ), h = INVALID_HANDLE_VALUE ) {
        for( int next = 1; next; next = FindNextFileA( h, &fdata ) != 0 ) {
            if( fdata.cFileName[0] != '.' ) {
                int is_dir = (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
                ls_strcat(&dir, src, fdata.cFileName, is_dir ? "/" : "");
                if( ls_strmatch( fdata.cFileName, pattern ) ) {
                    if( yield( dir ) < 0 ) return FREE(dir), count;
                }
                count += recurse && is_dir ? ls_recurse( recurse, dir, pattern, yield ) : 1;
            }
        }
    }
#else
    for( DIR *tmp, *odir = opendir( src ); odir; closedir( odir ), odir = 0 ) {
        for( struct dirent *ep; ep = readdir( odir ); ) {
            if( ep->d_name[0] != '.' ) {
                int is_dir = 0 != (tmp = opendir( ep->d_name)) ? (closedir( tmp ), 1) : 0;
                ls_strcat(&dir, src, fdata.cFileName, is_dir ? "/" : "");
                if( ls_strmatch( ep->d_name, pattern ) ) {
                    if( yield( dir ) < 0 ) return FREE(dir), count;
                }
                count += recurse && is_dir ? ls_recurse( recurse, dir, pattern, yield ) : 1;
            }
        }
    }
#endif
    return FREE(dir), count;
}
int dirls(const char *pathmask, errorcode (*yield)(const char *name) ) {
    if( pathmask[0] == '/' ) return 0; // sandboxed: deny absolute paths
    if( pathmask[1] == ':' ) return 0; // sandboxed: deny absolute paths

    char path_[256] = "./", mask_[256];
    const char *slash = strrchr(pathmask, '/');
    if( slash ) sprintf(path_, "%.*s/", (int)(slash - pathmask), pathmask);
    sprintf(mask_, "%s", slash ? slash + 1 : pathmask);
    if(!mask_[0]) sprintf(mask_, "%s", "*");
    //printf("path=%s mask=%s\n", path_, mask_);

    return ls_recurse( !!strstr(pathmask, "**"), path_, mask_, yield );
}
