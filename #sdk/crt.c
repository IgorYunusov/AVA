// # crt ######################################################################

typedef void (*ring_quit_cb)();
ring_quit_cb ring_list[32] = {0};
static int expected_quit = 0;
static int num_rings = 0;

static
void ring_atexit(void) { // $
    //FREE(oom), oom = 0;
    if( expected_quit ) {
        while( num_rings ) {
            num_rings--;
            if( ring_list[num_rings] ) {
                ring_list[num_rings]();
            }
        }
    } else {
        // try to save game state
        // then,
        // try to get callstack
        // try to display it
        // else try to log it
        // else try to print it
        panic("!ring_atexit(): unexpected exit() call\n");
    }
}

void quit() { // $
    expected_quit = 1;
    exit(0);
}

void ring( bool expr, const char *title, void (*quit)() ) { // $
    if( !num_rings ) atexit(ring_atexit);
    if( !expr ) {
        exit(-1);
    } else {
        ring_list[num_rings++] = quit;
    }
}

static
void ring1() {
    puts("bye ring1");
}
static
void ring2() {
    puts("bye ring2");
}

static
int tests() {
    bool ok = true;
    ok &= TEST(1 < 2);
    ok &= TEST(2 + 2);
//    ok &= dialog("tmbi", "Warning!", "This is a test.\nIs it visible?", 2, "warning");

    ok &= TEST( 0 == strcmp( dirfix( strdup("/A\\b//c")), "/A/b//c") );
    ok &= TEST( 0 == strcmp( dirbase(strdup("/ab/c.e.xt")), "c.e.xt") );
    ok &= TEST( 0 == strcmp( dirbase(strdup("c.e.xt")), "c.e.xt") );
    ok &= TEST( 0 == strcmp( dirname(strdup("/ab/c.e.xt")), "c") );
    ok &= TEST( 0 == strcmp( dirname(strdup("c.e.xt")), "c") );
    ok &= TEST( 0 == strcmp( dirtype(strdup("/ab/c.e.xt")), ".e.xt") );
    ok &= TEST( 0 == strcmp( dirtype(strdup("c.e.xt")), ".e.xt") );
    ok &= TEST( 0 == strcmp( dirpath(strdup("/ab/c.e.xt")), "/ab/") );
    ok &= TEST( 0 == strcmp( dirpath(strdup("c.e.xt")), "") );
    //ASSERT( ls("**.iqm", puts) > 0 );

    ok &= TEST( 1 < 1 );

    const char *text = "私 は ガ";
    ok &= TEST( codepoint(&text) == 31169 );
    ok &= TEST( codepoint(&text) == 32 );
    ok &= TEST( codepoint(&text) == 12399 );
    ok &= TEST( codepoint(&text) == 32 );
    ok &= TEST( codepoint(&text) == 12460 );
    ok &= TEST( codepoint(&text) == 0 );

    return ok;
}

static
bool statics() {
    oom = REALLOC( oom, 32* MiB );
    callstack(0, puts);
    ns();
    return true;
}

#include <string.h>
static
char *build_date() { $
    //                 012345678901234567890123
    //__TIMESTAMP__ -> Sun Mar 26 16:35:27 2017 -> 2017-03-26 16:35:27
    const char *mos = "JanFebMarAprMayJunJulAugSepOctNovDec";
    const char mo[4] = { __TIMESTAMP__[4], __TIMESTAMP__[5], __TIMESTAMP__[6], 0 };
    return va("%.4s-%02d-%.2s %.8s", __TIMESTAMP__+20, 1+( strstr(mos, mo) - mos )/3, __TIMESTAMP__+8, __TIMESTAMP__+11 );
}


void init() { //$

    // init a few kits
    if( !statics() ) {
        exit(-1);
    }

#ifdef SHIPPING
    ttydrop();
    trap();
#else
    tty(";; AVA - Compiled on " __DATE__ " " __TIME__);
    tty(build_date());
    char uuid[37];
    tty(uid4(uuid));

    if(getenv("AVABREAK")) breakpoint();

    // setup a few loggers and channels
    bool append = 1;
    //logme( fopen( va("%s/log.txt", usrgame(), usrbin(), "build_date()"), append ? "a+t" : "wt" ) );
    logme( stderr );

    LOG(AUDIO|STREAMING, "!this is an audio message (with %s)", "callstack");
#endif

    ring( 1+1, "ring1", ring1 );
    ring( 1+1, "ring2", ring2 );

    // did it work?

    tty( va("usrbin:  %s", usrbin() ) );
    tty( va("usrname: %s", usrname() ) );
    tty( va("usrgame: %s", usrgame() ) );
    tty( va("usrdata: %s", usrdata() ) );
    tty( va("usrtemp: %s", usrtemp() ) );
    tty( va("usrwork: %s", usrwork() ) );

/*
    char *readbuf = iofmap(__FILE__, 0, iofsize(__FILE__));
    tty( va("read: [%p]", readbuf) );
    tty( readbuf );
    iofunmap( readbuf, iofsize(__FILE__) );
*/

    dirch( usrgame() );
    if( dllopen(0, "editor") ) {
        void *pfn = dllfind(0, "main");
        printf("editor.dll %p\n", pfn );
        ((int(*)(int, char**))pfn)(0,0);
        dllclose(0);
    }
    if( dllopen(0, "game") ) {
        void *pfn = dllfind(0, "main");
        printf("game.dll %p\n", pfn );
        ((int(*)(int, char**))pfn)(0,0);
        dllclose(0);
    }

    if( getenv("AVATEST") ) {
        bool result = tests();
    }

    tty( va("cfg.port:\t%d", cfgi(8080, "p,port")) );
    tty( va("cfg.client:\t%d", cfgi(1, "c,client")) );
    tty( va("cfg.server:\t%d", cfgi(0, "s,server")) );

    const char *pkg = va("%s/%s.pkg", usrgame(), usrbin());
    bool has_pkg = iofexist(pkg);
    printf("[PKG] package%sfound: %s\n", has_pkg ? " " : " not ", pkg );

    printf("%d\n", (int)ns());
    printf("%d\n", (int)ns());

    if( builtin(likely)( 1 >= 1 ) ) {
        quit();
    }
    builtin(unreachable)();
}
