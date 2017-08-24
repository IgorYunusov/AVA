// # win wide strings #########################################################

#ifdef _WIN32
#include <shlobj.h>
static
wchar_t *widen(const char *utf8) { $
    int needed = 2 * /* 6 **/ MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0); // uf8len*6
    void *out = CALLOC( 1, needed + 1 );
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, out, needed);
    return (wchar_t *)out;
}
/*
static
char *shorten(const wchar_t *wstr) { $
    int wlen = wcslen(wstr);
    int needed = WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, NULL, 0, NULL, NULL);
    void *out = CALLOC( 1, needed + 1 );
    WideCharToMultiByte(CP_UTF8, 0, wstr, wlen, out, needed, NULL, NULL);
    return (char *)out;
}*/
#endif

char* strfindl(char *text, const char *substring) { $
    return strstr( text, substring );
}
char* strfindr(char *text, const char *substring) { $
    char *result = 0;
    while(1) {
        char *found = strstr(text, substring);
        if( !found ) break;
        result = found;
        text = found + 1;
    }
    return result;
}
char* strtriml(char *text, const char *substring) { $
    char *found = strfindr(text, substring);
    if( found ) memmove(text, found+1, strlen(found));
    return text;
}
char* strtrimr(char *text, const char *substring) { $
    char *found = strstr(text, substring);
    if( found ) found[0] = '\0';
    return text;
}
int strmatch( const char *text, const char *pattern ) { $
    if( *pattern=='\0' ) return !*text;
    if( *pattern=='*' )  return strmatch(text, pattern+1) || (*text && strmatch(text+1, pattern));
    if( *pattern=='?' )  return *text && (*text != '.') && strmatch(text+1, pattern+1);
    return (*text == *pattern) && strmatch(text+1, pattern+1);
}
char *strlower( char *str ) { $
    for( char *s = str; *s; *s++ ) {
        if( *s >= 'A' && *s <= 'Z' ) *s = *s - 'A' + 'a'; // *s &= 32;
    }
    return str;
}
int strchop(const char **tokens, int *sizes, int avail, const char *src, const char *delim) { $
    while( *src && avail-- > 0 ) {
        int n = strcspn( src += strspn(src, delim), delim );
        *tokens++ = (*sizes++ = n) ? src : "";
        src += n;
    }
    return *tokens = 0, *sizes = 0, avail > 0;
}
bool strbegin( const char *text, const char *substring ) { $
    int s1 = strlen(text), s2 = strlen(substring);
    return s1 >= s2 ? 0 == memcmp( &text[       0 ], substring, s2 ) : false;
}
bool strend( const char *text, const char *substring ) { $
    int s1 = strlen(text), s2 = strlen(substring);
    return s1 >= s2 ? 0 == memcmp( &text[ s1 - s2 ], substring, s2 ) : false;
}

