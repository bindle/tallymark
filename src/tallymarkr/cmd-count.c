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
#include "cmd-count.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include <tallymark.h>

#include "network.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymarker_cmd_count(tallymarker_cnf * cnf)
{
   uint32_t                req_codes;
   size_t                  len;
   tallymark_count         count;
   const tallymark_hdr   * hdr;

   bzero(&count, sizeof(count));
   req_codes = TALLYMARK_REQ_HASH_COUNT;

   tallymark_msg_create_header(cnf->req, (uint32_t)rand(), cnf->service, cnf->field_id, cnf->hash, sizeof(cnf->hash));
   if (cnf->hash_txt != NULL)
   {
      tallymark_msg_set_param(cnf->req, TALLYMARK_PARM_HASH_TEXT, &cnf->hash_txt, strlen(cnf->hash_txt));
      req_codes |= TALLYMARK_REQ_HASH_SET_TEXT;
   };

   if (tallymarker_send(cnf, cnf->req, req_codes) != 0)
      return(1);

   while (tallymarker_recv(cnf, cnf->res) == 0)
   {
      tallymark_msg_get_header(cnf->res, &hdr);
      if ((count.count == 0) && (count.seconds == 0))
      {
         len = sizeof(count);
         tallymark_msg_get_param(cnf->res, TALLYMARK_PARM_HASH_COUNT, &count, &len);
      };
      if ((hdr->response_codes & TALLYMARK_RES_EOR) != 0)
      {
         printf("%" PRIu64 "\n", count.count);
         return(0);
      };
   };

   return(1);
}

/* end of source */
