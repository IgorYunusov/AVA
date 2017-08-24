#pragma once

#define ON                        +
#define OFF                       -
#define USING(x)                  ((1 x 1) > 0)




#ifdef DEMO

#define BIG_ENDIAN                OFF
#define LITTLE_ENDIAN             ON

int main() {
    #if USING(BIG_ENDIAN)
    puts("big");
    #else
    puts("little");
    #endif

    #if USING(LITTLE_ENDIAN)
    puts("little");
    #else
    puts("big");
    #endif
/*
    #if USING(UNDEFINED)
    puts("??");
    #endif
*/
}

#define  BUILD ( \
    DOCS+ \
    LOGS+ \
    TESTS+ \
    STATS+ \
    CHECKS+ \
    QUARKS+ \
    ASSERTS+ \
    CONSOLES+ \
    PROFILERS+ \
    MEMTRACES+ \
    STACKTRACES+ \
    INFOS+ \
    WARNINGS+ \
    ERRORS+ \
    FATALS+ \
    CRASHES+ \
    INTERNAL+ \
    STATICS+ \
    LITTLE+ \
    BITS64+ MSC + X86+ WINDOWS )

#endif