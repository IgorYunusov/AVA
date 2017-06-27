// # rnd ######################################################################

#include <stdint.h>
static uint64_t mix64(uint64_t state) { $
    // splitmix64: Written in 2015 by Sebastiano Vigna (vigna@acm.org) (CC1.0)
    // the state can be seeded with any value.
    uint64_t z = (state += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
static uint64_t next128(uint64_t state[2]) { $
    // xoroshiro128+: Written in 2016 by David Blackman and Sebastiano Vigna (vigna@acm.org) (CC1.0)
    // state must be seeded to not zero. can be seed with splitmix64 generator
    uint64_t s1 = state[1];
    const uint64_t s0 = state[0];
    const uint64_t result = s0 + s1;
    s1 ^= s0;
    state[0] = ((s0 << 55) | (s0 >> (64 - 55))) ^ s1 ^ (s1 << 14);
    state[1] = ((s1 << 36) | (s1 >> (64 - 36)));
    return result;
}
void rndseed(uint64_t state[2], int64_t seed) { $
    state[0] = mix64(seed);
    state[1] = mix64(seed+1);
}
uint64_t rndu64(uint64_t state[2]) { $
    return next128( state );
}
double rnddbl(uint64_t state[2]) { $ // (0, 1]
    uint64_t u64 = rndu64(state);
    // With the exception of generators designed to provide directly double-precision
    // floating-point numbers, the fastest way to convert in C99 a 64-bit unsigned
    // integer x to a 64-bit double is:
    union { uint64_t i; double d; } u; u.i = UINT64_C(0x3FF) << 52 | u64 >> 12;
    double dbl = u.d - 1.0;
    return 2.0 * ((float)(dbl / 2));
}
int64_t rndint(uint64_t state[2], int64_t mini, int64_t maxi) { $ // [mini,maxi]
    assert( mini < maxi );
    return (int64_t)(mini + rnddbl(state) * (maxi + 0.5 - mini));
}
