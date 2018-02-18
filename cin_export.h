/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifndef CIN_EXPORT_H
#define CIN_EXPORT_H
#pragma once

/*
 * The build should have generated this file. It just contains a define for
 * CIN_DLL on a DLL build, or nothing for a static build. If you are importing
 * Cinnamon into another project, you can simply remove this include.
 */
#include "cin_conf.inc"

#if ( defined _WIN32 ) && !( defined __GNUC__ )
  
  /* For a DLL build, use dllimport and dllexport. */
  #ifdef CIN_DLL
    
    #ifdef CIN_INTERNAL
      
      /* TODO: Apparently __export is the right thing here with Watcom? */
      #define CIN_DLL_EXPORT(X) __declspec(dllexport) X
      
    #else /* CIN_INTERNAL */
      
      #define CIN_DLL_EXPORT(X) __declspec(dllimport) X
      
    #endif /* CIN_INTERNAL */
    
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
    
  /* Use the C calling convention for all exported symbols, with protected visibility. */
  #define CIN_EXPORT(X) X __attribute__((visibility ("default"), cdecl, used))
  
  /* Internal functions have hidden visibility. */
  #define CIN_PRIVATE(X) X __attribute__((visibility("hidden"), fastcall))
    
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
