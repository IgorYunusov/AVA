// # dll ######################################################################

#ifdef _WIN32
#   include <stdlib.h>
#   include <winsock2.h>
#   define DLL                  HMODULE
#   define dlopen(name,mode)    LoadLibraryA( name )
#   define dlsym(handle,symbol) GetProcAddress( handle, symbol )
#   define dlclose(handle)      0
#else
#   include <dlfcn.h>
#   define DLL                  void*
#endif
#if defined(__APPLE__)
#   include <mach-o/dyld.h>
#endif
DLL plugins[32] = {0};
int dllopen(int plug_id, const char *filename) { $
    const char *buf;
    if( iofsize(buf = va("%s.dll", filename)) ||
        iofsize(buf = va("%s.so", filename)) ||
        iofsize(buf = va("lib%s.so", filename)) ||
        iofsize(buf = va("%s.dylib", filename)) ) {
        filename = buf;
    } else {
        return 0;
    }
#if _WIN32 && !SHIPPING
    // hack: dont let windows ever lock our source dll (we want the dll to be monitored and hot-reloaded)
    // we move the dll to the tmp folder and load it from there (this temp dll will get locked instead).
    char outfile[512];
    char *tmpdir = getenv("TMP") ? getenv("TMP") : getenv("TEMP") ? getenv("TEMP") : 0;
    if( tmpdir ) {
        /*
        csystem(0, "mkdir %s\\AVA 1> nul 2> nul", tmpdir);
        csystem(outfile, "echo %s\\AVA\\%%random%%", tmpdir);
        csystem(0, "copy /y \"%s\" \"%s.%s\" 1> nul 2> nul", filename, outfile, filename);
        csystem(outfile, "echo %s.%s", outfile, filename);
        filename = outfile;
        */
    }
#endif
    plugins[plug_id] = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    return plugins[plug_id] != 0;
}
void *dllfind(int plug_id, const char *name) { $
    return dlsym(plugins[plug_id], name);
}
void dllclose(int plug_id) { $
    dlclose(plugins[plug_id]);
}
