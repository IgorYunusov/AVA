#  if !defined(API) && defined(__cplusplus)
#   define API extern "C" __declspec(dllexport)
#elif !defined(API)
#   define API            __declspec(dllexport)
#endif

#include "AVA.h"

void start() {
    puts(";; AVA - " __DATE__ " " __TIME__);
}
