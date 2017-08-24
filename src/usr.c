// # usr ######################################################################
// @ todo: paths must end with slash always

static
char *appfullpath() { $
    static char *t = 0;
    if(t) return t;

    char path[PATH_MAX+1] = {0};

#if defined(__APPLE__)
    uint32_t i = sizeof(path);
    if (_NSGetExecutablePath(path, &i) > -1) {
        return t = strdup(path);
    }
#elif defined(__linux__)
    if (readlink(va("/proc/%d/exe", getpid()), path, sizeof(path)) > -1) {
        return t = strdup(path);
    }
#elif defined(_WIN32)
    if (GetModuleFileNameA(0, path, sizeof(path))) {
        return t = strdup(path);
    }
#endif
    t = strdup( __argv[0] );
    return t = dirabs( &t );
}
char *usrbin() { $
    static char *t = 0;
    if(t) return t;
    t = strdup(__argv[0]);
    // remove extension if present (windows)
    strtrimr(t, ".exe");
    // remove path if present
    strtriml(t, "\\");
    strtriml(t,  "/");
    return t;
}
char *usrname() { $
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("USER");
    t = t ? t : getenv("USERNAME");
    t = strdup( t ? t : "GUEST" );
    return t;
}
char *usrgame() { $
    static char *t = 0;
    if( t ) return t;

    t = strdup( appfullpath() );
    for( size_t i = strlen(t); i-- > 0; ) {
        if (t[i] == '\\' || t[i] == '/') {
            t[i] = '\0';
            break;
        }
    }
    return t;
}
char *usrdata() { $
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("APPDATA");
    t = t ? t : getenv("HOME");
    t = strdup( t ? t : "./" );
    return t;
}
char *usrtemp() { $
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("TMPDIR");
    t = t ? t : getenv("TMP");
    t = t ? t : getenv("TEMP");
#ifndef _WIN32
    t = t ? t : "/tmp";
#endif
    t = strdup( t ? t : "./" );
    return t;
}
THREAD_LOCAL char cwd[PATH_MAX+1];
char *usrwork() { $
    getcwd(cwd, sizeof(cwd));
    return cwd;
}
uint64_t usrspace() { $
#ifdef _WIN32
    DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
    if( GetDiskFreeSpaceA( ".\\", &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters ) ) {
        return ((uint64_t)NumberOfFreeClusters) * SectorsPerCluster * BytesPerSector;
    }
#endif
    return ~0LLU;
}
