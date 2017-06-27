// # utf8 and unicode #########################################################
//
#include <wctype.h>
#include <locale.h>
static
void putlocale() { $
    if( !setlocale( LC_CTYPE, "en_US.UTF-8" ) ){
        setlocale( LC_CTYPE, "" );
    }
}
uint32_t codepoint(const char **p) { $
    if ((**p & 0x80) == 0x00) {
        int a = *((*p)++);
        return a;
    }
    if ((**p & 0xE0) == 0xC0) {
        int a = *((*p)++) & 0x1F;
        int b = *((*p)++) & 0x3F;
        return (a << 6) | b;
    }
    if ((**p & 0xF0) == 0xE0) {
        int a = *((*p)++) & 0x0F;
        int b = *((*p)++) & 0x3F;
        int c = *((*p)++) & 0x3F;
        return (a << 12) | (b << 6) | c;
    }
    if ((**p & 0xF8) == 0xF0) {
        int a = *((*p)++) & 0x07;
        int b = *((*p)++) & 0x3F;
        int c = *((*p)++) & 0x3F;
        int d = *((*p)++) & 0x3F;
        return (a << 18) | (b << 12) | (c << 8) | d;
    }
    return 0;
}
static THREAD_LOCAL uint8_t utf[8] = {0};
char *utf8(uint32_t cp) { $
    int n = 0;
    /**/ if (cp < 0x80) n = 1;
    else if (cp < 0x800) n = 2;
    else if (cp < 0x10000) n = 3;
    else if (cp < 0x200000) n = 4;
    else if (cp < 0x4000000) n = 5;
    else if (cp <= 0x7fffffff) n = 6;
    switch (n) {
        case 6: utf[5] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0x4000000;
        case 5: utf[4] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0x200000;
        case 4: utf[3] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0x10000;
        case 3: utf[2] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0x800;
        case 2: utf[1] = 0x80 | (cp & 0x3f); cp = (cp >> 6) | 0xc0;
        case 1: utf[0] = cp; default:;
    }
    return utf[n] = '\0', (char *)utf;
}
