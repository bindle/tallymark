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


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Data Types
#endif

typedef struct libtallymark_body_struct     tallymark_bdy;


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

_TALLYMARK_F unsigned tallymark_msg_param_header(tallymark_msg * msg, size_t off,
   uint32_t len, uint32_t id);
_TALLYMARK_F unsigned tallymark_msg_param_len(unsigned len);
_TALLYMARK_F unsigned tallymark_msg_param_str(tallymark_msg * msg, size_t off,
   const char * str, size_t len);
_TALLYMARK_F unsigned tallymark_msg_param_u32(tallymark_msg * msg, size_t off,
   uint32_t val);

_TALLYMARK_F int tallymark_msg_parse(tallymark_msg * msg);
_TALLYMARK_F int tallymark_msg_read(tallymark_msg * msg, int s,
   struct sockaddr * address, socklen_t * address_len);
_TALLYMARK_F int tallymark_msg_set_string(tallymark_msg * msg, char ** ptr,
   const void * invalue, size_t invalue_size);
_TALLYMARK_F int tallymark_msg_get_string(tallymark_msg * msg,
   const char * invalue, void * outvalue, size_t * outvalue_size);

#endif /* end of header */
