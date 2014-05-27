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
#include "cmd-debugger.h"


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

uint32_t tallymarker_cmd_debugger_header(tallymark_msg * msg);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymarker_cmd_debugger(tallymarker_cnf * cnf)
{
   tallymark_msg_create_header(cnf->req, (uint32_t)rand(), cnf->service_id, cnf->field_id, cnf->hash_id, sizeof(cnf->hash_id));

   if (cnf->request_codes == 0)
      cnf->request_codes = TALLYMARK_REQ_HASH_COUNT;

   if (tallymarker_send(cnf, cnf->req, cnf->request_codes) != 0)
   {
      tallymarker_cmd_debugger_header(cnf->req);
      return(1);
   };
   tallymarker_cmd_debugger_header(cnf->req);

   while (tallymarker_recv(cnf, cnf->res) == 0)
      if ((tallymarker_cmd_debugger_header(cnf->res) & TALLYMARK_RES_EOR) != 0)
         return(0);

   return(1);
}


uint32_t tallymarker_cmd_debugger_header(tallymark_msg * msg)
{
   int                   i;
   const tallymark_hdr * hdr;

   tallymark_msg_get_header(msg, &hdr);

   printf("Message:\n");
   printf("   magic:                %08x\n",       hdr->magic);
   printf("   version (current):    %02x\n",       hdr->version_current);
   printf("   version (age):        %02x\n",       hdr->version_age);
   printf("   header length:        %u bytes\n",   (hdr->header_len*4));
   printf("   message length:       %u bytes\n",   (hdr->msg_len*4));
   printf("   request codes:        %08x\n",       hdr->request_codes);
   printf("   response codes:       %02x\n",       hdr->response_codes);
   printf("   parameter count:      %02x\n",       hdr->param_count);
   printf("   service ID:           %02x\n",       hdr->service_id);
   printf("   field ID:             %02x\n",       hdr->field_id);
   printf("   request ID:           %08x\n",       hdr->request_id);
   printf("   sequence number:      %u\n",         hdr->sequence_id);
   printf("   value hash:           ");
   for(i = 0; i < 20; i++)
      printf("%02x", hdr->hash_id[i]);
   printf("\n");

   return(hdr->response_codes);
}

/* end of source */