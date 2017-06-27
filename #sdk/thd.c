// # thd ######################################################################
// [ref] http://bisqwit.iki.fi/story/howto/openmp/
// [ref] https://github.com/tinycthread/tinycthread/
// [ref] https://stackoverflow.com/questions/12744324/how-to-detach-a-thread-on-windows-c#answer-12746081

#if defined(_OPENMP)
#   include <omp.h>     // #pragma omp
#endif
#ifdef _WIN32
#   include <process.h> // _getpid()
#   include <windows.h> // SwitchToThread()
#   define getpid _getpid
#else
#   include <unistd.h>  // getpid()
#   include <sched.h>   // sched_yield()
#   include <time.h>    // nanosleep()
#endif
int tid() { $
    static THREAD_LOCAL int tid_;
    return (intptr_t)&tid_;
}
int pid() { $
    return (int)getpid();
}
void yield() { $
#ifdef __cplusplus
    std::this_thread::yield();
#elif defined(_WIN32)
    SwitchToThread(); // also, Sleep(0); SleepEx(0, FALSE);
#else
    sched_yield();    // also, nanosleep();
#endif
}
#ifdef _WIN32
static
void nanosleep( int64_t ns ) { $
    LARGE_INTEGER li;      // Windows sleep in 100ns units
    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    li.QuadPart = -ns / 100; // Negative for relative time
    SetWaitableTimer(timer, &li, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#endif
void sleep(unsigned ss, unsigned ms, unsigned us, unsigned ns) { $
    for( unsigned i = 0; i < ss; ++i ) nanosleep(1e9);
    nanosleep(ms * 1e6 + us * 1e3 + ns);
}
int cores() {
#ifdef __cplusplus
    return (int)std::thread::hardware_concurrency();
#elif defined(_OPENMP)
    int cores = 0;
    #pragma omp parallel
    {
        #pragma omp atomic
        ++cores;
    }
    return cores;
#else
    return 1;
#endif
}
#ifdef __cplusplus
    #include <thread>
    static std::thread threads[32];
    bool detach( void (*func)(void *), void *arg ) {
        std::thread( [=]{ func(arg); }).detach();
    }
    bool thread(int thread_id, void (*func)(void *), void *arg) {
        return threads[thread_id] = std::thread( [&]{ func(arg); } ), true;
    }
    bool join(int thread_id) {
        threads[thread_id].join();
        return true;
    }
#else
    static void *threads[32];
    typedef struct thread_args {
        void *func;
        void *args;
    } thread_args;
    #ifdef _WIN32
        #include <windows.h>
        static DWORD WINAPI thread_wrapper(LPVOID opaque) {
            void (*func)(void *) = ((thread_args *)opaque)->func;
            void *arg0 = ((thread_args *)opaque)->args;
            FREE( opaque );
            func( arg0 );
            return 0;
        }
        static void* thread_( void (*func)(void *), void *args ) {
            thread_args *ta = (struct thread_args*)MALLOC( sizeof(struct thread_args));
            ta->func = func;
            ta->args = args;
            return CreateThread(NULL, 0, thread_wrapper, (LPVOID)ta, 0, NULL);
        }
        bool detach( void (*func)(void *), void *arg ) {
            void *thr = thread_(func, arg);
            return CloseHandle(thr) != 0 ? true : false;
        }
        bool thread(int thread_id, void (*func)(void *), void *arg) {
            return !!(threads[thread_id] = thread_(func, arg));
        }
        bool join(int thread_id) {
            if( WaitForSingleObject(threads[thread_id], INFINITE) == WAIT_FAILED ) {
                return false;
            }
            CloseHandle(threads[thread_id]);
            return true;
        }
    #else
        #include <pthread.h>
        static void *thread_wrapper( void *opaque ) {
            void (*func)(void *) = ((thread_args *)opaque)->func;
            void *arg0 = ((thread_args *)opaque)->args;
            FREE( opaque );
            func( arg0 );
            //
            pthread_exit((void**)0);
            return 0;
        }
        static void* thread_( void (*func)(void *), void *args ) {
            thread_args *ta = (struct thread_args*)MALLOC( sizeof(struct thread_args));
            ta->func = func;
            ta->args = args;
            void *ret;
            if( pthread_create(&ret, NULL, thread_wrapper, (void *)ta) != 0 ) {
                *ret = 0;
            }
            return ret;
        }
        bool detach( void (*func)(void *), void *arg ) {
            void *thr = thread_(func, arg);
            return pthread_detach(thr) == 0 ? true : false;
        }
        bool thread(int thread_id, void (*func)(void *), void *arg) {
            return !!(threads[thread_id] = thread_(func, arg));
        }
        bool join(int thread_id) {
            void *nil;
            int err;
            do {
                err = pthread_join(threads[thread_id], &nil );
            } while( err == EINTR );
            return err == 0;
        }
    #endif
#endif
