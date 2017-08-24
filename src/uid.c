// # uid ######################################################################

#ifdef _WIN32
#include <Wincrypt.h>
#endif
#include <stdio.h>
void uidbuf(char *buffer, int len) { $
#ifdef _WIN32
#   pragma comment(lib, "Advapi32.lib")
    HCRYPTPROV provider;
    if (0 != CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        if (0 != CryptGenRandom(provider, len, (uint8_t*)buffer)) {
            CryptReleaseContext(provider, 0);
            return; // ok
        }
        CryptReleaseContext(provider, 0);
    }
#else
    FILE* fp = fopen("/dev/urandom", "rb");
    fp = fp ? fp : fopen("/dev/random", "rb");
    if (fp) {
        if( len == fread(buffer, 1, len, fp) ) {
            fclose(fp);
            return; // ok
        }
        fclose(fp);
    }
#endif
}
//static THREAD_LOCAL char uuid_[36+1] = {0};
char *uid4( char uuid_[36+1] ) { $
    unsigned char buffer[16];
    uidbuf(buffer, sizeof(buffer));
    sprintf(uuid_,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        buffer[0],buffer[1],buffer[2],buffer[3],
        buffer[4],buffer[5],
        buffer[6],buffer[7],
        buffer[8],buffer[9],
        buffer[10],buffer[11],buffer[12],buffer[13],buffer[14],buffer[15]
    );
    uuid_[14] = '4';
    return uuid_;
}
