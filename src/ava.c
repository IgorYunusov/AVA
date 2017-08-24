//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// People who worked on this file: @r-lyeh,

#  if !defined(API) && defined(__cplusplus)
#   define API extern "C" __declspec(dllexport)
#elif !defined(API)
#   define API            __declspec(dllexport)
#endif

#include "ava.h"

#include <stdio.h>  // puts, size_t
#include <stdlib.h> // realloc, getenv, vsnprintf

#include "sys.c"
#include "i18.c"
#include "mem.c"
#include "val.c"
#include "err.c"
#include "dbg.c"
#include "str.c"
#include "utf.c"
#include "ios.c"
#include "bin.c"
#include "dll.c"
#include "tty.c"
#include "dir.c"
#include "usr.c"
#include "tst.c"
#include "env.c"
#include "gui.c"
#include "arg.c"
#include "cfg.c"
#include "uid.c"
#include "rnd.c"
#include "crc.c"
#include "clk.c"
#include "thd.c"
#include "mtx.c"
#include "atm.c"
#include "log.c"

#include "crt.c"
