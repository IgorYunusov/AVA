// # ios ######################################################################
// @ todo : iosflock, iosfunlock

#if !defined(__MINGW32__) && !defined(_WIN32)
    #include <unistd.h>
    #include <sys/mman.h>
#else
    // mmap() replacement for Windows. Placed into the public domain (Mike Frysinger)
    #include <io.h>
    #include <windows.h>
    #include <sys/types.h>
    enum {  PROT_READ = 0x1, PROT_WRITE = 0x2, PROT_EXEC = 0x4,
            MAP_SHARED = 0x01, MAP_PRIVATE = 0x02, MAP_ANON = 0x20, MAP_ANONYMOUS = MAP_ANON };
    #define MAP_FAILED    ((void *) -1)
    static void* mmap(void* start, size_t length, int prot, int flags, int fd, size_t offset) { $
        DWORD flProtect;
        size_t end = length + offset;
        if( !(prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC)) ) {
            if( ( fd == -1 &&  (flags & MAP_ANON) && !offset ) ||
                ( fd != -1 && !(flags & MAP_ANON)            )) {
                     if( prot & PROT_EXEC  ) flProtect = prot & PROT_READ ? PAGE_EXECUTE_READ : PAGE_EXECUTE_READWRITE;
                else if( prot & PROT_WRITE ) flProtect = PAGE_READWRITE;
                else                         flProtect = PAGE_READONLY;
                HANDLE h = CreateFileMapping( fd == -1 ? INVALID_HANDLE_VALUE : (HANDLE)_get_osfhandle(fd),
                    NULL, flProtect, (end >> 31) >> 1, (uint32_t)end, NULL);
                if( h != NULL ) {
                    DWORD dwDesiredAccess = 0;
                    dwDesiredAccess |= prot & PROT_WRITE ? FILE_MAP_WRITE : FILE_MAP_READ;
                    dwDesiredAccess |= prot & PROT_EXEC ? FILE_MAP_EXECUTE : 0;
                    dwDesiredAccess |= flags & MAP_PRIVATE ? FILE_MAP_COPY : 0;
                    void *ret = MapViewOfFile(h, dwDesiredAccess, (offset >> 31) >> 1, (uint32_t)offset, length);
                    CloseHandle(h); // close the Windows Handle here (we handle the file ourselves with fd)
                    return ret == NULL ? MAP_FAILED : ret;
                }
            }
        }
        return MAP_FAILED;
    }
    static void munmap(void* addr, size_t length) { $
        UnmapViewOfFile(addr);
    }
#endif
#include <fcntl.h> // O_RDONLY
#include <stdio.h>
static
FILE *fopen8( const char *pathfile8, const char *mode8 ) { $
    #ifdef _WIN32
        wchar_t *pf = widen(pathfile8), *md = widen(mode8);
        FILE *fp = _wfopen(pf, md);
        return FREE(md), FREE(pf), fp;
    #else
        return fopen( pathfile8, mode8 );
    #endif
}
char* iofmap( const char *pathfile, size_t offset, size_t len ) { $
    int file = open(pathfile, O_RDONLY);
    if( file < 0 ) {
        return 0;
    }
    void *ptr = mmap(0, len, PROT_READ, MAP_PRIVATE, file, 0);
    if( ptr == MAP_FAILED ) {
        ptr = 0;
    }
    close(file); // close file. mapping held until unmapped
    return (char *)ptr;
}
void iofunmap( char *buf, size_t len ) { $
    munmap( buf, len );
}
char* iofread(const char *pathfile) { $
    size_t len = iofsize(pathfile);
    char *buf = REALLOC( 0, len + 1 ); buf[len] = 0;
    char *map = iofmap( pathfile, 0, len );
    memcpy( buf, map, len );
    iofunmap( map, len );
    return buf;
}
bool iofwrite(const char *pathfile, const void *data, int len) { $
    bool ok = 0;
    FILE *fp = fopen8(pathfile, "wb");
    if( fp ) {
        ok = 1 == fwrite(data, len, 1, fp);
        fclose(fp);
    }
    return ok;
}
bool iofappend(const char *pathfile, const void *data, int len) { $
    bool ok = 0;
    FILE *fp = fopen8(pathfile, "a+b");
    if( fp ) {
        ok = 1 == fwrite(data, len, 1, fp);
        fclose(fp);
    }
    return ok;
}
#include <sys/stat.h>
uint64_t iofstamp( const char *pathfile ) { $
    struct stat st;
    return stat(pathfile, &st) < 0 ? 0ULL : (uint64_t)st.st_mtime;
}
uint64_t iofsize( const char *pathfile ) { $
    struct stat st;
    return stat(pathfile, &st) < 0 ? 0ULL : (uint64_t)st.st_size;
}
bool iofexist( const char *pathfile ) { $
    struct stat st;
    return stat(pathfile, &st) < 0 ? 0 : 1;
}
bool iofisdir( const char *pathfile ) { $
    struct stat st;
    return stat(pathfile, &st) < 0 ? 0 : S_IFDIR == ( st.st_mode & S_IFMT );
}
bool iofisfile( const char *pathfile ) { $
    struct stat st;
    return stat(pathfile, &st) < 0 ? 0 : S_IFREG == ( st.st_mode & S_IFMT );
}
bool iofislink( const char *pathfile ) { $
#ifdef S_IFLNK
    struct stat st;
    return stat(pathfile, &st) < 0 ? 0 : S_IFLNK == ( st.st_mode & S_IFMT );
#else
    return 0;
#endif
}
