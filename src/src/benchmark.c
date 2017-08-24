#pragma once
#include <omp.h>
#define BENCHMARK(x) 0; for( x = -omp_get_wtime(); x < 0; x += omp_get_wtime() )
// usage:
//    double t = BENCHMARK(t) {
//        for( int i = 0; i < 1000000; ++i );
//    }
//    printf("%fs\n", t);

#ifdef DEMO
#include <stdio.h>
#include <stdint.h>

void nop(){ _ReadWriteBarrier(); }

void empty6(int a, int b, int c, int d, int e, int f)
{nop();}
void emptyN(int a, ...)
{nop();}

int main() {
    #define N 1000000000ULL
    double t1 = BENCHMARK(t1) {
        for( uint64_t i = 0; i < N; ++i) empty6(0,1,2,3,4,5);
    }
    double t2 = BENCHMARK(t2) {
        for( uint64_t i = 0; i < N; ++i) emptyN(0,1,2,3,4,5);
    }

    printf("variadic is x%1.6f times slower\n", t2/t1);
}
#endif
