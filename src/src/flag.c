#pragma once
#include <stdint.h>

typedef uint8_t  flag; // 1 byte, 0xf0 mode [0..15]<<4, 0x0f level { 0(default)..1(min)..9(max) | 10..15 (extra tuned) }

#ifdef DEMO

// demo

enum { MEMCPY = 0<<4, LZ4 = 1<<4, ZIP = 2<<4, RAR = 3<<4, ZSTD = 4<<4 };

enum {
    none = MEMCPY|0,
    fastc = ZSTD|1,
    fastd = ZSTD|15,
    bestc = RAR|9,
    autoc = 0, // per platform or per game
};

// more examples
    #define DEFAULT_COMPRESSION_DSK (ZIP|9)
    #define DEFAULT_COMPRESSION_MEM (MEM|0)
    #define TEXTURE_COMPRESSION_DSK (LZ4|9)
    #define TEXTURE_COMPRESSION_MEM (ETC|4) // also, AUDIO, SPEECH, MESH, ANIMATION,
    #define TEXTURE_D_DIFFUSE_COMPRESSION_DSK (LZ4|9)
    #define TEXTURE_L_LIGHTMAP_COMPRESSION_MEM (ETC|4)
//  etc..

int compressable( int tag4_mode, int tag4_class, flag on_disk, flag on_mem );

int main()
{}

#endif
