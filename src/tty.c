// # tty ######################################################################

#ifdef _WIN32
#   define NOMINMAX
#   include <winsock2.h>
#   include <conio.h>
#else
#   include <sys/ioctl.h>
#   include <termios.h>
#   include <unistd.h>
#endif
#include <stdio.h>
bool tty(const char *text) { $
#ifdef _MSC_VER
    OutputDebugStringA( text );
#endif
    return !!puts( text );
}
void ttycolor( uint8_t r, uint8_t g, uint8_t b ) { $
#if _WIN32
    const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    auto color = ( r > 127 ? FOREGROUND_RED : 0 ) |
                 ( g > 127 ? FOREGROUND_GREEN : 0 ) |
                 ( b > 127 ? FOREGROUND_BLUE : 0 ) |
                 FOREGROUND_INTENSITY;
    SetConsoleTextAttribute(stdout_handle, color);
#else
    // 24-bit console ESC[ … 38;2;<r>;<g>;<b> … m Select RGB foreground color
    // 256-color console ESC[38;5;<fgcode>m
    // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
    // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
    // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
    // 0xE8-0xFF:  grayscale from black to white in 24 steps
    r /= 51; // [0..5]
    g /= 51; // [0..5]
    b /= 51; // [0..5]
    printf("\033[38;5;%dm", r*36+g*6+b+16); // "\033[0;3%sm", color_code);
#endif
}
int ttycolumns() { $
#if _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return csbi.srWindow.Right - csbi.srWindow.Left + 1; // Window width
#elif defined(TIOCGSIZE)
    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    return ts.ts_cols;
#elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
    return ts.ws_col;
#endif
    return 0;
}
void ttydrop() { $
#ifdef _WIN32
    FreeConsole();
    //static FILE* fcon = 0; if(!fcon) fcon = fopen("CON", "w" );            // open console
    //static FILE* fout = 0; if(!fout) fout = freopen( "CON", "w", stdout ); // redirect stdout to console
    //static FILE* ferr = 0; if(!ferr) ferr = freopen( "CON", "w", stderr ); // redirect stderr to console
#endif
    // hide stdin
    freopen( tmpnam(0), "rt", stdin ); // also "/dev/null" (linux/osx), "/dev/console" (linux/osx)
    // hide stdout
    freopen( tmpnam(0), "wt", stdout );
    // hide stderr
    freopen( tmpnam(0), "wt", stderr );
}
