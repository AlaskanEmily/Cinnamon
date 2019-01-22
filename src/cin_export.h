/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifndef CIN_EXPORT_H
#define CIN_EXPORT_H
#pragma once

#if ( defined _WIN32 ) && !( defined __GNUC__ )
  
  /* For a DLL build, use dllimport. Since we use a DEF file, we do not need
   * to use dllexport. */
  #if (!defined(CIN_STATIC)) && !defined CIN_INTERNAL

    #define CIN_DLL_EXPORT(X) __declspec(dllimport) X
  
  #else /* CIN_DLL */
    
    #define CIN_DLL_EXPORT(X) X
    
  #endif /* CIN_DLL */
  
  /* Use the C calling convention for all exported symbols. */
  #define CIN_EXPORT(X) CIN_DLL_EXPORT(X) __cdecl
  
  /* We want to use fastcall for non-Watcom on internal functions. */
  #ifdef __WATCOMC__
    
    #define CIN_PRIVATE(X) X
    
  #else
  
    #define CIN_PRIVATE(X) X __fastcall
    
  #endif
  
#elif defined __GNUC__ /* GCC or not WIN32 */

#ifdef __CYGWIN__
  /* Use the C calling convention for all exported symbols, with protected visibility. */
  #define CIN_EXPORT(X) X __attribute__((visibility ("default"), cdecl, used))
  /* Internal functions have hidden visibility. */
  #define CIN_PRIVATE(X) X __attribute__((visibility("hidden"), fastcall))
#else
  #define CIN_EXPORT(X) X __attribute__((visibility ("default")))
  #define CIN_PRIVATE(X) X __attribute__((visibility("hidden")))
#endif
    
#else /* not WIN32 and not GCC */
    
    #define CIN_EXPORT(X) X
    #define CIN_PRIVATE(X) X
    
#endif

#ifdef __GNUC__

#define CIN_PRIVATE_PURE(X) CIN_PRIVATE(X) __attribute__((const))

#elif defined _MSC_VER

#define CIN_PRIVATE_PURE(X) __declspec(noalias) CIN_PRIVATE(X)

#else

#define CIN_PRIVATE_PURE(X) CIN_PRIVATE(X)

#endif

#endif /* CIN_EXPORT_H */
