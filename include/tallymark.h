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
 *   Tally Mark Daemon public API
 */
#ifndef __TALLYMARK_H
#define __TALLYMARK_H 1
#undef __TALLYMARK_PMARK


///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
#endif

#include <sys/types.h>
#include <inttypes.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
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
#      define _TALLYMARK_F   /* empty */                                   ///< used for library calls
#      define _TALLYMARK_V   extern TALLYMARK_C_DECLS                         ///< used for library calls
#   else
#      define _TALLYMARK_F   extern TALLYMARK_C_DECLS                         ///< used for library calls
#      define _TALLYMARK_V   extern TALLYMARK_C_DECLS                         ///< used for library calls
#   endif
#endif


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

_TALLYMARK_F const char * tallymark_lib_version(uint32_t * pcur,
   uint32_t * prev, uint32_t * page);

/**
 *  Retrieve the version of the Tally Mark Daemon client library.
 *  @param  pmaj       Returns reference to major version number.
 *  @param  pmin       Returns reference to minor version number.
 *  @param  ppat       Returns reference to patch level.
 *  @param  pbuild     Returns reference to build commit.
 */
_TALLYMARK_F const char * tallymark_pkg_version(uint32_t * pmaj,
   uint32_t * pmin, uint32_t * ppat, const char ** pbuild);


#endif /* end of header */
