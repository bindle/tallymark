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
#ifndef __TALLYMARK_DEFS_H
#define __TALLYMARK_DEFS_H 1
#undef  __TALLYMARK_PMARK
#ifndef __TALLYMARK_H
#error "do not include tallymark_defs.h directly, include tallymark.h."
#endif


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif

/// @ingroup messageSpec Message Specification
/// @{
#define TM_MSG_MAX_SIZE                2048

#ifdef __TALLYMARK_PMARK
#pragma mark - Header Field Lengths
#endif
#define TM_HDR_LEN_MAGIC                  4UL
#define TM_HDR_LEN_VERSION_CURRENT        1UL
#define TM_HDR_LEN_VERSION_AGE            1UL
#define TM_HDR_LEN_HEADER_LEN             1UL
#define TM_HDR_LEN_MSG_LEN                1UL
#define TM_HDR_LEN_REQUEST_CODES          4UL
#define TM_HDR_LEN_RESPONSE_CODES         1UL
#define TM_HDR_LEN_PARAM_COUNT            1UL
#define TM_HDR_LEN_SERVICE_ID             1UL
#define TM_HDR_LEN_FIELD_ID               1UL
#define TM_HDR_LEN_REQUEST_ID             4UL
#define TM_HDR_LEN_SEQUENCE_ID            4UL
#define TM_HDR_LEN_HASH_ID               20UL
#define TM_HDR_LEN                       44UL
#define TM_MSG_LEN                     1024UL

#ifdef __TALLYMARK_PMARK
#pragma mark - Header Field Offsets
#endif
#define TM_HDR_OFF                        0UL
#define TM_HDR_OFF_MAGIC                  0UL
#define TM_HDR_OFF_VERSION_CURRENT        4UL
#define TM_HDR_OFF_VERSION_AGE            5UL
#define TM_HDR_OFF_HEADER_LEN             6UL
#define TM_HDR_OFF_MSG_LEN                7UL
#define TM_HDR_OFF_REQUEST_CODES          8UL
#define TM_HDR_OFF_RESPONSE_CODES        12UL
#define TM_HDR_OFF_PARAM_COUNT           13UL
#define TM_HDR_OFF_SERVICE_ID            14UL
#define TM_HDR_OFF_FIELD_ID              15UL
#define TM_HDR_OFF_REQUEST_ID            16UL
#define TM_HDR_OFF_SEQUENCE_ID           20UL
#define TM_HDR_OFF_HASH_ID               24UL
#define TM_BDY_OFF                       TM_HDR_LEN

#ifdef __TALLYMARK_PMARK
#pragma mark - Header Field Codes
#endif
#define TALLYMARK_HDR_MAGIC            (TM_HDR_OFF_MAGIC           << 24UL)
#define TALLYMARK_HDR_VERSION_CURRENT  (TM_HDR_OFF_VERSION_CURRENT << 24UL)
#define TALLYMARK_HDR_VERSION_AGE      (TM_HDR_OFF_VERSION_AGE     << 24UL)
#define TALLYMARK_HDR_HEADER_LEN       (TM_HDR_OFF_HEADER_LEN      << 24UL)
#define TALLYMARK_HDR_BODY_LEN         (TM_HDR_OFF_BODY_LEN        << 24UL)
#define TALLYMARK_HDR_PARAM_COUNT      (TM_HDR_OFF_PARAM_COUNT     << 24UL)
#define TALLYMARK_HDR_RESPONSE_CODES   (TM_HDR_OFF_RESPONSE_CODES  << 24UL)
#define TALLYMARK_HDR_REQUEST_CODES    (TM_HDR_OFF_REQUEST_CODES   << 24UL)
#define TALLYMARK_HDR_REQUEST_ID       (TM_HDR_OFF_REQUEST_ID      << 24UL)
#define TALLYMARK_HDR_SEQUENCE_ID      (TM_HDR_OFF_SEQUENCE_ID     << 24UL)
#define TALLYMARK_HDR_SERVICE_ID       (TM_HDR_OFF_SERVICE_ID      << 24UL)
#define TALLYMARK_HDR_FIELD_ID         (TM_HDR_OFF_FIELD_ID        << 24UL)
#define TALLYMARK_HDR_HASH_ID          (TM_HDR_OFF_HASH_ID         << 24UL)
/// @}


/// @ingroup protocolCodes Protocol Version Codes
/// @{
#ifdef __TALLYMARK_PMARK
#pragma mark - Protocol Information
#endif
#define TALLYMARK_MAGIC                0xe2889100U // UTF-8 N-ARY Summation Character
#define TALLYMARK_PORT                 2211U       // Unicode Character 'N-ARY SUMMATION' (U+2211)
#define TALLYMARK_PROTO_VERSION        0x01L
#define TALLYMARK_PROTO_AGE            0x00L
/// @}


/// @ingroup responseCodes Response Codes
/// @{
#ifdef __TALLYMARK_PMARK
#pragma mark - Message Response Codes
#endif
#define TALLYMARK_RES_PERM             0x10U ///< permission denied
#define TALLYMARK_RES_INVALID          0x20U ///< invalid command/hash/service/field/etc
#define TALLYMARK_RES_EOR              0x40U ///< End of response
#define TALLYMARK_RES_RESPONSE         0x80U ///< Indicates the message is a response

#undef  TALLYMARK_RES_RESERVED      // 0x01U
#undef  TALLYMARK_RES_RESERVED      // 0x02U
#undef  TALLYMARK_RES_RESERVED      // 0x04U
#undef  TALLYMARK_RES_RESERVED      // 0x08U
/// @}


/// @ingroup requestCodes Request Codes
/// @{
#ifdef __TALLYMARK_PMARK
#pragma mark - Message Request Codes
#endif
#define TALLYMARK_REQ_HASH_LIST           0x00000001U ///< Send list of hashes matching service ID and field ID
#define TALLYMARK_REQ_HASH_SET_TEXT       0x00000002U ///< Set text value of tallied data if not already set
#define TALLYMARK_REQ_HASH_RECORD         0x00000004U ///< request full hash record (tally history, value text, etc)
#define TALLYMARK_REQ_HASH_RESET          0x00000008U ///< resets tally history and thresholds

#define TALLYMARK_REQ_HASH_COUNT          0x00000010U ///< requests current tally count
#define TALLYMARK_REQ_HASH_INCREMENT      0x00000020U ///< increment the counter
#define TALLYMARK_REQ_HASH_THRESHOLD      0x00000040U ///< report current threshold level reached
#define TALLYMARK_REQ_HASH_SET_THRESHOLD  0x00000080U ///< mark specified threshold as reached

#define TALLYMARK_REQ_FIELD_LIST          0x00000100U ///< send list of fields matching service ID
#define TALLYMARK_REQ_FIELD_NAME          0x00000200U ///< send field name
#define TALLYMARK_REQ_FIELD_WINDOW        0x00000400U ///< request window size and window slot count
#define TALLYMARK_REQ_FIELD_THRESHOLD     0x00000800U ///< request threshold definition of service and field

#define TALLYMARK_REQ_SERVICE_LIST        0x00008000U ///< send list of services
#define TALLYMARK_REQ_SERVICE_NAME        0x00004000U ///< send service name
#undef  TALLYMARK_REQ_RESERVED         // 0x00002000U
#undef  TALLYMARK_REQ_RESERVED         // 0x00001000U

#undef  TALLYMARK_REQ_RESERVED         // 0x00080000U
#undef  TALLYMARK_REQ_RESERVED         // 0x00040000U
#undef  TALLYMARK_REQ_RESERVED         // 0x00020000U
#undef  TALLYMARK_REQ_RESERVED         // 0x00010000U

#undef  TALLYMARK_REQ_RESERVED         // 0x00100000U
#undef  TALLYMARK_REQ_RESERVED         // 0x00200000U
#undef  TALLYMARK_REQ_RESERVED         // 0x00400000U
#undef  TALLYMARK_REQ_RESERVED         // 0x00800000U

#define TALLYMARK_REQ_DATA_PRUNE          0x01000000U ///< instruct server to prune old data
#define TALLYMARK_REQ_DATA_DUMP           0x02000000U ///< dump records matching service ID and field ID (ID of 0x00 is wild card)
#undef  TALLYMARK_REQ_RESERVED         // 0x04000000U
#undef  TALLYMARK_REQ_RESERVED         // 0x08000000U

#undef  TALLYMARK_REQ_RESERVED         // 0x10000000U
#define TALLYMARK_REQ_SYS_VERSION         0x20000000U ///< send server name & version
#define TALLYMARK_REQ_SYS_CAPABILITIES    0x40000000U ///< send list of capabilities
#define TALLYMARK_REQ_SYS_SYNC            0x80000000U ///< request is from another server
/// @}


/// @ingroup fieldCodes Data Field Codes
/// @{
#ifdef __TALLYMARK_PMARK
#pragma mark - Message Parameter Codes
#endif
#define TALLYMARK_PARM_HASH_COUNT         0x000001U ///< requests current tally count
#define TALLYMARK_PARM_HASH_TEXT          0x000002U ///< request full hash record (tally history, value text, etc)
#undef  TALLYMARK_PARM_HASH_HISTORY    // 0x000003U ///< resets tally history and thresholds

#define TALLYMARK_PARM_THRESHOLD          0x0000010U ///< current threshold of requested data
#undef  TALLYMARK_PARM_THRESHOLD_DEF   // 0x0000020U ///< threshold definition of requested data

#undef  TALLYMARK_PARM_SERVICE_NAME    // 0x000100U ///< send service name

#undef  TALLYMARK_PARM_FIELD_NAME      // 0x001000U ///< send field name
#undef  TALLYMARK_PARM_FIELD_WINDOW    // 0x002000U ///< send field ID (only useful with OP_FIELD_LIST)

#define TALLYMARK_PARM_SYS_CAPABILITIES   0x100000U ///< window size of tallies
#define TALLYMARK_PARM_SYS_VERSION        0x200000U ///< ORed list of capabilities
#define TALLYMARK_PARM_SYS_PKG_NAME       0x300000U ///< ORed list of capabilities
/// @}


#ifdef __TALLYMARK_PMARK
#pragma mark - Message Status Codes
#endif
#define TALLYMARK_MSG_RESET               0xdead0000UL
#define TALLYMARK_MSG_COMPILED            0x00000002UL
#define TALLYMARK_MSG_PARSED              0x00000004UL
#define TALLYMARK_MSG_READY               (TALLYMARK_MSG_COMPILED|TALLYMARK_MSG_PARSED)
#define TALLYMARK_MSG_BAD                 0xbad00000UL


#endif /* end of header */
