//

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "builtin.h"

#ifndef $
#define $
#endif

#ifndef REALLOC
#define REALLOC   realloc
#endif
#ifndef MALLOC
#define MALLOC(x) REALLOC(0, x)
#endif
#ifndef FREE
#define FREE(p)   REALLOC(p, 0)
#endif

//#ifndef DOUBLE
//#define DOUBLE float
//#endif

#ifdef _MSC_VER
#define __thread __declspec(thread)
#endif
