#pragma once

// # thread local #############################################################

#ifdef __GNUC__
#   define THREAD_LOCAL __thread
#elif __STDC_VERSION__ >= 201112L
#   define THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER) | defined(__INTEL_COMPILER)
#   define THREAD_LOCAL __declspec(thread)
#else
#   error Cannot define THREAD_LOCAL
#endif

//#define builtin(a, ...)     builtin_#a(__VA_ARGS__)
//#define builtin_thread(...) THREAD_LOCAL __VA_ARGS__

#define thread__            THREAD_LOCAL
