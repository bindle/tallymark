/*
 *  Tally Mark Daemon - An aggregated time-based counter for disparate systems
 *  Copyright (c) 2014, Bindle Binaries
 *  All rights reserved.
 *
 *  @BINDLE_BINARIES_BSD_LICENSE_START@
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 *     3. Neither the name of the copyright holder nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @BINDLE_BINARIES_BSD_LICENSE_END@
 */
/**
 *   @file tallymark.h
 *   Tally Mark Daemon C/C++ preprocessor macros and definitions.
 */
#ifndef __TALLYMARK_CDEFS_H
#define __TALLYMARK_CDEFS_H 1
#undef  __TALLYMARK_PMARK
#ifndef __TALLYMARK_H
#error "do not include tallymark_cdefs.h directly, include tallymark.h."
#endif


///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
#endif

#ifdef __APPLE__
#  include "TargetConditionals.h"
#  ifdef TARGET_OS_MAC
#     include <libkern/OSAtomic.h>
#  endif
#endif


//////////////
//          //
//  Macros  //
//          //
//////////////
#ifdef CARRDM_PMARK
#pragma mark - Macros
#endif

// Exports function type
#undef TALLYMARK_C_DECLS
#if defined(__cplusplus) || defined(c_plusplus)
#   define _TALLYMARK_I             extern "C" inline
#   define TALLYMARK_C_DECLS        "C"             ///< exports as C functions
#   define TALLYMARK_BEGIN_C_DECLS  extern "C" {    ///< exports as C functions
#   define TALLYMARK_END_C_DECLS    }               ///< exports as C functions
#else
#   define _TALLYMARK_I             inline
#   define TALLYMARK_C_DECLS        /* empty */     ///< exports as C functions
#   define TALLYMARK_BEGIN_C_DECLS  /* empty */     ///< exports as C functions
#   define TALLYMARK_END_C_DECLS    /* empty */     ///< exports as C functions
#endif
#ifdef WIN32
#   ifdef _LIB_LIBTALLYMARK_H
#      define _TALLYMARK_F   extern TALLYMARK_C_DECLS __declspec(dllexport)   ///< used for library calls
#      define _TALLYMARK_V   extern TALLYMARK_C_DECLS __declspec(dllexport)   ///< used for library calls
#   else
#      define _TALLYMARK_F   extern TALLYMARK_C_DECLS __declspec(dllimport)   ///< used for library calls
#      define _TALLYMARK_V   extern TALLYMARK_C_DECLS __declspec(dllimport)   ///< used for library calls
#   endif
#else
#   ifdef _LIB_LIBTALLYMARK_H
#      define _TALLYMARK_F   /* empty */                                      ///< used for library calls
#      define _TALLYMARK_V   extern TALLYMARK_C_DECLS                         ///< used for library calls
#   else
#      define _TALLYMARK_F   extern TALLYMARK_C_DECLS                         ///< used for library calls
#      define _TALLYMARK_V   extern TALLYMARK_C_DECLS                         ///< used for library calls
#   endif
#endif


#endif /* end of header */
