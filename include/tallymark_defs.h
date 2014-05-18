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

#pragma mark - Header Field Lengths
#define TM_HDR_LEN_MAGIC                  4UL
#define TM_HDR_LEN_VERSION_CURRENT        1UL
#define TM_HDR_LEN_VERSION_AGE            1UL
#define TM_HDR_LEN_HEADER_LEN             1UL
#define TM_HDR_LEN_BODY_LEN               1UL
#define TM_HDR_LEN_RESERVED               2UL
#define TM_HDR_LEN_PARAM_COUNT            1UL
#define TM_HDR_LEN_RESPONSE_CODES         1UL
#define TM_HDR_LEN_REQUEST_CODES          4UL
#define TM_HDR_LEN_REQUEST_ID             4UL
#define TM_HDR_LEN_SEQUENCE_ID            4UL
#define TM_HDR_LEN_SERVICE_ID             4UL
#define TM_HDR_LEN_FIELD_ID               4UL
#define TM_HDR_LEN_HASH_ID               24UL
#define TM_HDR_LEN                       56UL
#define TM_BDY_LEN                     1024UL

#pragma mark - Header Field Offsets
#define TM_HDR_OFF                        0UL
#define TM_HDR_OFF_MAGIC                  0UL
#define TM_HDR_OFF_VERSION_CURRENT        4UL
#define TM_HDR_OFF_VERSION_AGE            5UL
#define TM_HDR_OFF_HEADER_LEN             6UL
#define TM_HDR_OFF_BODY_LEN               7UL
#define TM_HDR_OFF_RESERVED               8UL
#define TM_HDR_OFF_PARAM_COUNT           10UL
#define TM_HDR_OFF_RESPONSE_CODES        11UL
#define TM_HDR_OFF_REQUEST_CODES         12UL
#define TM_HDR_OFF_REQUEST_ID            16UL
#define TM_HDR_OFF_SEQUENCE_ID           20UL
#define TM_HDR_OFF_SERVICE_ID            24UL
#define TM_HDR_OFF_FIELD_ID              28UL
#define TM_HDR_OFF_HASH_ID               32UL
#define TM_HDR_LENGTH                    56UL
#define TM_BDY_OFF                       56UL

#pragma mark - Header Field Codes
#define TALLYMARK_HDR_MAGIC            (TM_HDR_OFF_MAGIC       << 24UL)
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
/// }


/// @ingroup protocolCodes Protocol Version Codes
/// @{
#pragma mark - Protocol Information
#define TALLYMARK_MAGIC                0xe2889100 // UTF-8 N-ARY Summation Character
#define TALLYMARK_PORT                 2211       // Unicode Character 'N-ARY SUMMATION' (U+2211)
#define TALLYMARK_PROTO_VERSION        0x00
#define TALLYMARK_PROTO_AGE            0x00
/// }


/// @ingroup responseCodes Response Codes
/// @{
#pragma mark - Message Response Codes
#define TALLYMARK_RES_RESPONSE         0x80 ///< Indicates the message is a response
#define TALLYMARK_RES_EOR              0x40 ///< End of response
#define TALLYMARK_RES_INVALID          0x20 ///< invalid command/hash/service/field/etc
#define TALLYMARK_RES_PERM             0x10 ///< permission denied
/// @}


/// @ingroup requestCodes Request Codes
/// @{
#pragma mark - Message Request Codes
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
#pragma mark - Message Parameter Codes
#define TALLYMARK_PARM_SYS_CAPABILITIES 0x10000000 ///< window size of tallies
#define TALLYMARK_PARM_SYS_VERSION      0x20000000 ///< ORed list of capabilities
#define TALLYMARK_PARM_SYS_PKG_NAME     0x30000000 ///< ORed list of capabilities

#define TALLYMARK_PARM_TALLY_COUNT      0x00000001 ///< tally count of requested data
#define TALLYMARK_PARM_TALLY_AVERAGE    0x00000002 ///< tally average of requested data
#define TALLYMARK_PARM_TALLY_HISTORY    0x00000003 ///< tally history of requested data

#define TALLYMARK_PARM_THRESHOLD_QUERY  0x00000010 ///< current threshold of requested data
#define TALLYMARK_PARM_THRESHOLD_DEF    0x00000020 ///< threshold definition of requested data

#define TALLYMARK_PARM_HASH_LIST        0x00000100 ///< Send list of hashes
#define TALLYMARK_PARM_HASH_TEXT        0x00000200 ///< Send text value of tallied data
#define TALLYMARK_PARM_HASH_ID          0x00000400 ///< send hash value (only useful with OP_HASH_LIST)

#define TALLYMARK_PARM_SERVICE_LIST     0x00001000 ///< send list of services
#define TALLYMARK_PARM_SERVICE_NAME     0x00002000 ///< send service name
#define TALLYMARK_PARM_SERVICE_ID       0x00004000 ///< send service ID (only useful with OP_SERVICE_LIST)

#define TALLYMARK_PARM_FIELD_LIST       0x00010000 ///< send list of fields
#define TALLYMARK_PARM_FIELD_NAME       0x00020000 ///< send field name
#define TALLYMARK_PARM_FIELD_ID         0x00040000 ///< send field ID (only useful with OP_FIELD_LIST)
/// @}


#pragma mark - Message Status Codes
#define TALLYMARK_MSG_RESET            0xDead0000UL
#define TALLYMARK_MSG_COMPILED         0x00000002UL
#define TALLYMARK_MSG_PARSED           0x00000004UL
#define TALLYMARK_MSG_READY            (TALLYMARK_MSG_COMPILED|TALLYMARK_MSG_PARSED)
#define TALLYMARK_MSG_BAD              0xBADC0DE0UL


#endif /* end of header */
