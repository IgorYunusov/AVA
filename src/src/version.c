// # AVA semantic versioning (octal)
// - single octal byte that represents semantic versioning (major.minor.patch).
// - maximum value: major:3, minor:7, patch:7.

#pragma once

int ver( int major, int minor, int patch ) {
    return 0100 * major + 010 * minor + patch;
}
int vercmp( int v1, int v2 ) {
    return (v1 & 0100) == (v2 & 0100);
}
const char *verschema() {
    return "%03o";
}

#ifdef DEMO

unsigned char version = 0300; // [0..255] decimal <-> [0000..0377] octal

void test( int a, int b ) {
    printf("are compatible %03o vs %03o ? %s\n", a, b, vercmp(a,b) ? "yes" : "no");
}

int main() {
    for( int i= 0; i <= 255; ++i) {
        printf("%03o\n", i);
    }

    printf("%03o\n", ver(3,7,7));
    printf("%03o\n", ver(2,7,7));
    printf("%03o\n", ver(1,7,7));
    printf("%03o\n", ver(0,7,7));

    printf("%03o\n", ver(3,7,1));
    printf("%03o\n", ver(2,5,3));
    printf("%03o\n", ver(1,3,5));
    printf("%03o\n", ver(0,1,7));

    test( 0357, 0300 );
    test( 0277, 0300 );
    test( 0277, 0200 );
    test( 0277, 0100 );
    test( 0277, 0270 );
    test( 0277, 0275 );
}

#endif
