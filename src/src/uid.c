// AVA sortable unique ids.
//
// - [x] 64-bit compact (11 chars if base62 encoded).
// - [x] sortable, both integer and base62 encoded versions.
//
// Spec is pretty much based on KSUID: 
//
// - SUID:64b = [timestamp:32b][random:32b]
//   - 32-bit UTC timestamp: number of seconds since July 14, 2017 2:40 (epoch 1970 + 15e8); 136 years max.
//   - 32-bit cryptographic randomness
//

#include <stdint.h>
#include <time.h>

uint64_t entropy() {
    return 0;
}

#define EPOCH 0 // 1 jan, 1970 [.. feb 7, 2106] (32-bit limit)
//#define EPOCH 14e8 // may 13, 2014 [.. 2150]
//#define EPOCH 15e8 // july 14, 2017 [.. 2153]

uint32_t epoch() {
    // Get current UTC time from unix epoch (1970), in seconds
    time_t now;
    time( &now );
    // Adjust to our custom epoch (July 14, 2017 2:40)
    return (uint32_t)(now - EPOCH);
}

const char *epoch_debug( uint32_t epoch ) {
    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    time_t now = (time_t)(epoch + EPOCH);
    struct tm  ts = *localtime(&now);
    static char buf[80];
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    return buf;
}

uint64_t suid() {
    return ((uint64_t)epoch() << 32ULL) | (entropy() >> 32ULL);
}

#ifdef DEMO

#include <stdio.h>

int main(void) {
    uint64_t uid = suid();
    printf("%016llx\n", uid);
    printf("%s\n", epoch_debug( uid >> 32ULL ) );
}

#endif
