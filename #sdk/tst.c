// # tst ######################################################################

#include <stdlib.h>
#include <stdio.h>
static THREAD_LOCAL int right = 0, wrong = 0;
static THREAD_LOCAL const char *suite = "";
bool unit(const char *name) { $
    suite = name;
    right = wrong = 0;
    return true;
}
bool test(const char *file, int line, int expr) { $
    right = right + !!expr;
    wrong = wrong +  !expr;
    printf("; [%s] when testing (%s L%d) [unit %s: %d/%d] %s\n", !expr ? "FAIL":" OK ", file, line, suite, right, right+wrong, wrong ? "FAILED":"" );
    if( !expr ) { callstack(8, puts); }
    return !!expr;
}
