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
#include "network.h"


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
#include <stdlib.h>
#include <errno.h>
#include <poll.h>

#include "debug.h"


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

int tallymarker_connect(tallymarker_cnf * cnf)
{
   int s;
   int err;
   tallymark_url_desc * tudp;

   assert(cnf != NULL);

   if ((err = tallymark_url_parse(cnf->urlstr, &cnf->tudp, 1)) != 0)
   {
      fprintf(stderr, "%s: invalid URL\n", cnf->prog_name);
      return(1);
   };
   tudp = cnf->tudp;

   if ((s = socket(tudp->tud_family, tudp->tud_socktype, tudp->tud_protocol)) == -1)
   {
      perror("socket()");
      return(1);
   };

   tallymarker_debug(cnf, 1, "connecting to %s ...\n", tudp->tud_strurl);
   if ((err = connect(s, &tudp->tud_addr.sa, tudp->tud_addrlen)) == -1)
   {
      perror("connect()");
      close(s);
      return(1);
   };

   cnf->s = s;

   return(0);
}


int tallymarker_recv(tallymarker_cnf * cnf, tallymark_msg * res)
{
   int               err;
   struct pollfd     fds[1];
   const uint8_t         * msg_buff;
   size_t                  msg_buff_len;
   const tallymark_hdr   * hdr;

   assert(cnf != NULL);
   
   if (res == NULL)
      res = cnf->res;

   if (cnf->timeout != 0)
   {
      fds[0].fd      = cnf->s;
      fds[0].events  = POLLIN;
      tallymarker_debug(cnf, 1, "waiting for data ...\n");
      if ((err = poll(fds, 1, cnf->timeout)) == -1)
      {
         perror("poll()");
         return(-1);
      };
      if (err == 0)
      {
         tallymarker_error(cnf, "connection timed out\n");
         return(-1);
      };
   };

   tallymarker_debug(cnf, 1, "receiving data ...\n");
   if ((err = tallymark_msg_recvfrom(cnf->s, res, NULL, 0)) != 0)
   {
      tallymarker_error(cnf, "tallymark_msg_recvfrom(): %s\n", tallymark_strerror(err));
      return(-1);
   };

   if ((cnf->verbose > 1) || (cnf->hexdump != 0))
   {
      tallymark_msg_get_header(res, &hdr);
      tallymark_msg_get_buffer(res, &msg_buff, &msg_buff_len);
      tallymark_print_hexdump(stdout, msg_buff, (hdr->msg_len*4), "<< ", NULL);
   };

   return(0);
}


int tallymarker_send(tallymarker_cnf * cnf, tallymark_msg * req, uint32_t req_code)
{
   int                     err;
   const uint8_t         * msg_buff;
   size_t                  msg_buff_len;
   const tallymark_hdr   * hdr;

   assert(cnf != NULL);
   if (req == NULL)
      req = cnf->req;

   tallymark_msg_set_header(req, TALLYMARK_HDR_REQUEST_CODES, &req_code, sizeof(req_code));

   tallymarker_debug(cnf, 1, "sending data ...\n");

   if ((cnf->verbose > 1) || (cnf->hexdump != 0))
   {
      tallymark_msg_compile(req);
      tallymark_msg_get_header(req, &hdr);
      tallymark_msg_get_buffer(req, &msg_buff, &msg_buff_len);
      tallymark_print_hexdump(stdout, msg_buff, (hdr->msg_len*4), ">> ", NULL);
   };

   if ((err = tallymark_msg_sendto(cnf->s, req, NULL, 0)) != 0)
   {
      fprintf(stderr, "tallymark_msg_sendto(): %s (%i)\n", tallymark_strerror(err), err);
      return(1);
   };

   return(0);
}


/* end of source */
