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
#ifndef __LIBTALLYMARK_MESSAGE_H
#define __LIBTALLYMARK_MESSAGE_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
#endif

#include "libtallymark.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif

#define TM_HDR_LENGTH                    56
#define TM_MSG_MAX_SIZE                2048

#define TM_BDY_OFF                        TM_HDR_LENGTH

#define TM_HDR_OFF_MAGIC                  0
#define TM_HDR_OFF_VERSION_CURRENT        4
#define TM_HDR_OFF_VERSION_AGE            5
#define TM_HDR_OFF_HEADER_LEN             6
#define TM_HDR_OFF_BODY_LEN               7
#define TM_HDR_OFF_RESERVED               8
#define TM_HDR_OFF_PARAM_COUNT           10
#define TM_HDR_OFF_RESPONSE_CODES        11
#define TM_HDR_OFF_REQUEST_CODES         12
#define TM_HDR_OFF_REQUEST_ID            16
#define TM_HDR_OFF_SEQUENCE_ID           20
#define TM_HDR_OFF_SERVICE_ID            24
#define TM_HDR_OFF_FIELD_ID              28
#define TM_HDR_OFF_HASH_ID               32
#define TM_HDR_OFF_PARAMETERS            56

#define TM_HDR_LEN_MAGIC                  4
#define TM_HDR_LEN_VERSION_CURRENT        1
#define TM_HDR_LEN_VERSION_AGE            1
#define TM_HDR_LEN_HEADER_LEN             1
#define TM_HDR_LEN_BODY_LEN               1
#define TM_HDR_LEN_RESERVED               2
#define TM_HDR_LEN_PARAM_COUNT            1
#define TM_HDR_LEN_RESPONSE_CODES         1
#define TM_HDR_LEN_REQUEST_CODES          4
#define TM_HDR_LEN_REQUEST_ID             4
#define TM_HDR_LEN_SEQUENCE_ID            4
#define TM_HDR_LEN_SERVICE_ID             4
#define TM_HDR_LEN_FIELD_ID               4
#define TM_HDR_LEN_HASH_ID               24
#define TM_HDR_LEN_PARAMETERS          1024


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Data Types
#endif

typedef struct libtallymark_header_struct   tallymark_hdr;
typedef struct libtallymark_body_struct     tallymark_bdy;


struct libtallymark_header_struct
{
                                 //        +----------------+
                                 //        | buffer offsets |
                                 // +------+----+-----+-----+
                                 // | size | u8 | u32 | u64 |
                                 // +------+----+-----+-----+
   uint32_t    magic;            // |    4 |  0 |   0 |   0 |
   int8_t      version_current;  // |    1 |  4 |   1 |     |
   int8_t      version_age;      // |    1 |  5 |     |     |
   uint8_t     header_len;       // |    1 |  6 |     |     |
   uint8_t     body_len;         // |    1 |  7 |     |     |
   uint8_t     reserved[2];      // |    2 |  8 |   2 |   1 |
   uint8_t     param_count;      // |    1 | 10 |     |     |
   uint8_t     response_codes;   // |    1 | 11 |     |     |
   uint32_t    request_codes;    // |    4 | 12 |   3 |     |
   uint32_t    request_id;       // |    4 | 16 |   4 |   2 |
   uint32_t    sequence_id;      // |    4 | 28 |   7 |     |
   uint32_t    service_id;       // |    4 | 20 |   5 |     |
   uint32_t    field_id;         // |    4 | 24 |   6 |   3 |
   uint8_t     hash_id[24];      // |   24 | 32 |   8 |   4 |
   //          body              // | 1024 | 56 |  14 |   7 |
                                 // +------+----+-----+-----+
};


struct libtallymark_body_struct
{
   // TALLYMARK_FLD_SYS_CAPABILITIES
   uint32_t    capabilities;

   // TALLYMARK_FLD_SYS_VERSION
   char      * version;
   uint32_t    version_size;
   char      * package_name;
   uint32_t    package_name_size;

};


struct libtallymark_message_struct
{
   union
   {
      tallymark_hdr           hdr;
      uint8_t                 u8[TM_MSG_MAX_SIZE];
      uint32_t                u32[TM_MSG_MAX_SIZE/4];
      uint64_t                u64[TM_MSG_MAX_SIZE/8];
      char                    str[TM_MSG_MAX_SIZE];
   } buff;
   tallymark                * tally;
   tallymark_hdr              header;
   tallymark_bdy              body;
   uint32_t                   status;
   int                        error;
   int                        s;
   size_t                     msg_len;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

_TALLYMARK_F int tallymark_msg_compile(tallymark_msg * msg);
_TALLYMARK_F int tallymark_msg_parse(tallymark_msg * msg);
_TALLYMARK_F int tallymark_msg_read(tallymark_msg * msg, int s,
   struct sockaddr * address, socklen_t * address_len);
_TALLYMARK_F int tallymark_msg_validate_header_definition(void);

#endif /* end of header */
