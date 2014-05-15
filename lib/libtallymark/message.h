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
typedef union  libtallymark_buffer_union    tallymark_buff;


struct libtallymark_header_struct
{
   uint32_t    magic;            // offset 0 - 3

   uint8_t     version_current;  // offset 4
   uint8_t     version_age;      // offset 5
   uint8_t     header_len;       // offset 6
   uint8_t     body_len;         // offset 7

   uint8_t     reserved[3];      // offset 8 - 10
   uint8_t     response_codes;   // offset 11

   uint32_t    request_id;       // offset 12 - 15
   uint32_t    request_codes;    // offset 16 - 19
   uint32_t    service_id;       // offset 20 - 23
   uint32_t    field_id;         // offset 24 - 27

   uint8_t     hash_id[20];      // offset 28 - 47
};


union libtallymark_buffer_union
{
   void     * ptr;
   uint8_t  * u8;
   uint32_t * u32;
   uint64_t * u64;
   char     * string;
};


struct libtallymark_body_struct
{
   uint32_t    capabilities;     // TALLYMARK_FLD_SYS_CAPABILITIES
   char      * version;          // TALLYMARK_FLD_SYS_VERSION
   char      * package_name;     // TALLYMARK_FLD_SYS_VERSION
};


struct libtallymark_message_struct
{
   tallymark                * tally;
   tallymark_hdr              header;
   tallymark_bdy              body;
   tallymark_buff             buff;
   uint32_t                   status;
   int                        error;
   int                        s;
   size_t                     msg_len;
   size_t                     buff_size;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

_TALLYMARK_F int tallymark_msg_assert_header(void);
_TALLYMARK_F int tallymark_msg_parse(tallymark_msg * msg);
_TALLYMARK_F int tallymark_msg_read(tallymark_msg * msg, int s,
   struct sockaddr * address, socklen_t * address_len);

#endif /* end of header */
