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


///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
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


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif

/// @ingroup protocolCodes Protocol Version Codes
/// @{
#define TALLYMARK_MAGIC                0xe2889100 // UTF-8 N-ARY Summation Character
#define TALLYMARK_PORT                 2211       // Unicode Character 'N-ARY SUMMATION' (U+2211)
#define TALLYMARK_PROTO_VERSION        0x00
#define TALLYMARK_PROTO_AGE            0x00
/// }


/// @ingroup responseCodes Response Codes
/// @{
#define TALLYMARK_RES_RESPONSE         0x80 ///< Indicates the message is a response
#define TALLYMARK_RES_EOR              0x40 ///< End of response
#define TALLYMARK_RES_INVALID          0x20 ///< invalid command/hash/service/field/etc
#define TALLYMARK_RES_PERM             0x10 ///< permission denied
/// @}


/// @ingroup requestCodes Request Codes
/// @{
#define TALLYMARK_REQ_SYS_SYNC         0x80000000 ///< request is from another server
#define TALLYMARK_REQ_SYS_CAPABILITIES 0x40000000 ///< send list of capabilities
#define TALLYMARK_REQ_SYS_VERSION      0x20000000 ///< server name & version

#define TALLYMARK_REQ_TALLY_INCREMENT  0x00000001 ///< requests the counter be incremented
#define TALLYMARK_REQ_TALLY_COUNT      0x00000002 ///< requests current tally count
#define TALLYMARK_REQ_TALLY_AVERAGE    0x00000004 ///< requests current tally average
#define TALLYMARK_REQ_TALLY_HISTORY    0x00000008 ///< requests full tally history

#define TALLYMARK_REQ_THRESHOLD_QUERY  0x00000010 ///< request threshold level reached
#define TALLYMARK_REQ_THRESHOLD_SET    0x00000020 ///< mark specified threshold as reached
#define TALLYMARK_REQ_THRESHOLD_UNSET  0x00000040 ///< unset specified threshold
#define TALLYMARK_REQ_THRESHOLD_DEF    0x00000080 ///< request threshold definition

#define TALLYMARK_REQ_HASH_LIST        0x00000100 ///< Send list of hashes
#define TALLYMARK_REQ_HASH_TEXT        0x00000200 ///< Send text value of tallied data
#define TALLYMARK_REQ_HASH_ID          0x00000400 ///< send hash value (only useful with OP_HASH_LIST)
#define TALLYMARK_REQ_HASH_RESET       0x00000800 ///< resets tally and thresholds

#define TALLYMARK_REQ_SERVICE_LIST     0x00001000 ///< send list of services
#define TALLYMARK_REQ_SERVICE_NAME     0x00002000 ///< send service name
#define TALLYMARK_REQ_SERVICE_ID       0x00004000 ///< send service ID (only useful with OP_SERVICE_LIST)

#define TALLYMARK_REQ_FIELD_LIST       0x00010000 ///< send list of fields
#define TALLYMARK_REQ_FIELD_NAME       0x00020000 ///< send field name
#define TALLYMARK_REQ_FIELD_ID         0x00040000 ///< send field ID (only useful with OP_FIELD_LIST)
#define TALLYMARK_REQ_FIELD_WINDOW     0x00080000 ///< request window information for service/field

/// @}


/// @ingroup fieldCodes Data Field Codes
/// @{
#define TALLYMARK_FLD_SYS_CAPABILITIES 0x10000000 ///< window size of tallies
#define TALLYMARK_FLD_SYS_VERSION      0x20000000 ///< ORed list of capabilities
#define TALLYMARK_FLD_SYS_PKG_NAME     0x30000000 ///< ORed list of capabilities

#define TALLYMARK_FLD_TALLY_COUNT      0x00000001 ///< tally count of requested data
#define TALLYMARK_FLD_TALLY_AVERAGE    0x00000002 ///< tally average of requested data
#define TALLYMARK_FLD_TALLY_HISTORY    0x00000003 ///< tally history of requested data

#define TALLYMARK_FLD_THRESHOLD_QUERY  0x00000010 ///< current threshold of requested data
#define TALLYMARK_FLD_THRESHOLD_DEF    0x00000020 ///< threshold definition of requested data

#define TALLYMARK_FLD_HASH_LIST        0x00000100 ///< Send list of hashes
#define TALLYMARK_FLD_HASH_TEXT        0x00000200 ///< Send text value of tallied data
#define TALLYMARK_FLD_HASH_ID          0x00000400 ///< send hash value (only useful with OP_HASH_LIST)

#define TALLYMARK_FLD_SERVICE_LIST     0x00001000 ///< send list of services
#define TALLYMARK_FLD_SERVICE_NAME     0x00002000 ///< send service name
#define TALLYMARK_FLD_SERVICE_ID       0x00004000 ///< send service ID (only useful with OP_SERVICE_LIST)

#define TALLYMARK_FLD_FIELD_LIST       0x00010000 ///< send list of fields
#define TALLYMARK_FLD_FIELD_NAME       0x00020000 ///< send field name
#define TALLYMARK_FLD_FIELD_ID         0x00040000 ///< send field ID (only useful with OP_FIELD_LIST)
/// @}


#define TALLYMARK_MSG_RESET            0x00dead00
#define TALLYMARK_MSG_RECEIVING        0x00000000
#define TALLYMARK_MSG_VALIDATING       0x00000001
#define TALLYMARK_MSG_VALIDATED        0x00000002
#define TALLYMARK_MSG_PARSING          0x00000003
#define TALLYMARK_MSG_PARSED           0x00000004
#define TALLYMARK_MSG_PREPARED         0x00000010
#define TALLYMARK_MSG_BAD              0x11111111


#endif /* end of header */
