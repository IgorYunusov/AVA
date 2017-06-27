// # dialog ###################################################################

#include <stdarg.h>
#include <string.h>
#pragma comment(lib, "user32.lib")
int dialog(const char *fmt, ...) { $
    int buttons = 2;
    const char *title = "", *icon = "", *message = "";
    va_list vl;
    va_start(vl, fmt);
    do {
        switch(fmt[0]) {
            default:
            break; case 'i': icon = va_arg(vl, const char *); icon = icon ? icon : "";
            break; case 't': title = va_arg(vl, const char *); title = title ? title : "";
            break; case 'm': message = va_arg(vl, const char *); message = message ? message : "";
            break; case 'b': buttons = va_arg(vl, int);
        }
    } while(*fmt++);
    va_end(vl);
#if 0
    if( icon[0] == 'e' || icon[0] == 'w' ) {
        if(window) glfwSetClipboardString(window, message);
    }
#endif
#if _WIN32
    wchar_t *title16 = widen(title);
    wchar_t *message16 = widen(message);
    int rc = MessageBoxW(0, message16, title16, MB_SETFOREGROUND |
        (buttons >= 3 ? MB_YESNOCANCEL : buttons >= 2 ? MB_YESNO : MB_OK) |
        (icon[0] == 'i'/*nfo*/ ? MB_ICONEXCLAMATION : 0) |
        (icon[0] == 'e'/*rror*/ ? MB_ICONERROR : 0) |
        (icon[0] == 'w'/*arning*/ ? MB_ICONWARNING : 0) |
        (icon[0] == 'q'/*uestion*/ ? MB_ICONQUESTION : 0)
    );
    FREE(message16);
    FREE(title16);
    /**/ if( rc == IDYES || rc == IDOK ) return 1;
    else if( rc == IDNO ) return 2;
    else if( rc == IDCANCEL ) return 3;
#else
    puts(title);
    puts(message);
    if( buttons == 1 ) getchar();
    if( buttons == 2 ) { puts("y/n?"); rc = getch() == 'y'; }
    //if( buttons == 3 ) { puts("y/n/r?"); }
#endif
    return 0;
}
