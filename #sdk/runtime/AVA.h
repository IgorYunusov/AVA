//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// People who worked on this file: @r-lyeh, 

#ifndef AVA_VERSION
#define AVA_VERSION "0.0.0" // v0.0.0 YOB-Marrow

//  ## std.def : definitions, types and constants.

    #include <stdio.h>
    #include <stdint.h>
    #include <stdarg.h>

//  ## std.abi : symbol linkage and visibility.

    #  if !defined(API) && defined(__cplusplus)
    #   define API extern "C" __declspec(dllimport)
    #elif !defined(API)
    #   define API            __declspec(dllimport)
    #endif

//  ## std.api : guards, namespaces, enums, macros.

    #ifndef EXPORT
    #   define EXPORT(version, ...) __VA_ARGS__
    #endif

//  ## api
//

EXPORT( 100,

//  ## std.crt entry point

    API void start();

)

#endif /* AVA_VERSION */
