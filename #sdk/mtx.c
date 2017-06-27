// # mtx ######################################################################

#ifdef __cplusplus
#include <mutex>
    static std::/*recursive_*/mutex mtx[32];
    void mtx_on()  {}
    void mtx_off() {}
    bool try_lock(int mutex_id) { return mtx[mutex_id].try_lock() ? 1 : 0; }
    void lock(int mutex_id) { mtx[mutex_id].lock(); }
    void unlock(int mutex_id) { mtx[mutex_id].unlock(); }
#elif defined(_OPENMP)
    static omp_/*nest_*/lock_t mtx[32];
    void mtx_on()  { for( int i = 0; i < 32; ++i ) omp_init_/*nest_*/lock(&mtx[i]);    }
    void mtx_off() { for( int i = 0; i < 32; ++i ) omp_destroy_/*nest_*/lock(&mtx[i]); }
    bool try_lock(int mutex_id) { return !!omp_test_/*nest_*/lock( &mtx[mutex_id] ); }
    void lock(int mutex_id) { omp_set_/*nest_*/lock( &mtx[mutex_id] ); }
    void unlock(int mutex_id) { omp_unset_/*nest_*/lock( &mtx[mutex_id] ); }
#elif !defined(_WIN32)
    #include <pthread.h>
    static pthread_mutex_t mtx[32]; // = PTHREAD_MUTEX_INITIALIZER;
    void mtx_on()  { for( int i = 0; i < 32; ++i ) pthread_mutex_init(&mtx[i], NULL); }
    void mtx_off() { for( int i = 0; i < 32; ++i ) pthread_mutex_destroy(&mtx[i]); }
    void lock(int mutex_id) { pthread_mutex_lock( &mtx[mutex_id] ); }
    bool try_lock(int mutex_id) { return 0 == pthread_mutex_trylock( &mtx[mutex_id] ); }
    void unlock(int mutex_id) { pthread_mutex_unlock( &mtx[mutex_id] ); }
#else
    #if   defined(_WIN32)
    #   define CAS_LOCK(sl)             _InterlockedExchange((long*)sl, 1)
    #   define CLEAR_LOCK(sl)           _InterlockedExchange((long*)sl, 0)
    #   define PAUSE                    SwitchToThread() // _mm_pause() // SleepEx(50, FALSE)
    #elif defined(__GNUC__)
    #   define CAS_LOCK(sl)             __sync_lock_test_and_set(sl, 1)
    #   define CLEAR_LOCK(sl)           __sync_lock_release(sl)
    #   define PAUSE                    __asm__ __volatile__("pause" ::: "memory") // crossplat: sched_yield() // <sched.h>
    #endif

    #define LOCK_T                volatile int
    #define TRY_LOCK(sl)          !CAS_LOCK(sl)
    #define RELEASE_LOCK(sl)      CLEAR_LOCK(sl)
    #define ACQUIRE_LOCK(sl)      do { if (CAS_LOCK(sl)) while (*sl || CAS_LOCK(sl)) PAUSE; } while (0)
    #define INITIAL_LOCK(sl)      (*sl = 0)
    #define DESTROY_LOCK(sl)      do {} while(0)

    static LOCK_T mtx[32];
    void mtx_on()  { for( int i = 0; i < 32; ++i ) INITIAL_LOCK(&mtx[i]); }
    void mtx_off() { for( int i = 0; i < 32; ++i ) DESTROY_LOCK(&mtx[i]); }
    bool try_lock(int mutex_id) { return !!TRY_LOCK(&mtx[mutex_id]); }
    void lock(int mutex_id) { ACQUIRE_LOCK( &mtx[mutex_id] ); }
    void unlock(int mutex_id) { RELEASE_LOCK( &mtx[mutex_id] ); }
#endif
