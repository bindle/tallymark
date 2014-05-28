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


///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
#endif

#include "tallymarked.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <syslog.h>

#include "conf.h"
#include "daemon.h"
#include "network.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

int main(int argc, char * argv[]);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int main(int argc, char * argv[])
{
   tallymarked_cnf    * cnf;
   tallymark_sockaddr   addr;
   const tallymark_hdr  * req_hdr;
   const tallymark_hdr  * res_hdr;
   char                   hash[21];
   int                  i;
   int                  s;
   int                  err;
   char                 straddr[INET6_ADDRSTRLEN];
   socklen_t            addrlen;
   uint32_t             u32;
   uint8_t              u8;
   const char         * constr;

   srand((unsigned)tallymark_seed());

   switch(tallymarked_init(&cnf, argc, argv))
   {
      case -1:
      return(-1);

      case 1:
      return(0);

      default:
      break;
   };

   openlog(cnf->prog_name, LOG_CONS|LOG_PERROR|LOG_PID, LOG_DAEMON);
   syslog(LOG_INFO, "starting %s %s", PACKAGE_NAME, PACKAGE_VERSION);

   if ((err = tallymarked_listen(cnf)) == -1)
   {
      tallymarked_destroy(cnf);
      return(1);
   };
   s = cnf->s[0];

   switch((tallymarked_daemon(cnf)))
   {
      case -1:
      tallymarked_destroy(cnf);
      return(1);

      case 0:
      break;

      default:
      tallymarked_destroy(cnf);
      return(0);
   };

   while(1)
   {
      addrlen = sizeof(addr);
      err = tallymark_msg_recvfrom(s, cnf->req, &addr.sa, &addrlen);
      strcpy(straddr, "unknown");
      if (addr.sa.sa_family != 0)
         getnameinfo(&addr.sa, addrlen, straddr, sizeof(straddr), NULL, 0, NI_NUMERICHOST);
      if (err != 0)
      {
         syslog(LOG_ERR, "client=%s error=%s", straddr, tallymark_strerror(err));
         continue;
      };
      tallymark_msg_get_header(cnf->req, &req_hdr);
      for(i = 0; i < 20; i++)
         snprintf(&hash[i*2], 3, "%02x", req_hdr->hash_id[i]);
      syslog(LOG_NOTICE, "client=%s, reqid=%08" PRIx32 ", req=%" PRIx32 ", type=%u:%u, hash=%s", straddr, req_hdr->request_id, req_hdr->request_codes, req_hdr->service_id, req_hdr->field_id, hash);

      tallymark_msg_create_header(cnf->res, req_hdr->request_id, req_hdr->service_id, req_hdr->field_id, req_hdr->hash_id, sizeof(req_hdr->hash_id));

      u8 = TALLYMARK_RES_RESPONSE|TALLYMARK_RES_EOR;
      tallymark_msg_set_header(cnf->res, TALLYMARK_HDR_RESPONSE_CODES, &u8, sizeof(u8));
      tallymark_msg_set_header(cnf->res, TALLYMARK_HDR_REQUEST_CODES, &req_hdr->request_codes, sizeof(req_hdr->request_codes));

      if ((TALLYMARK_REQ_SYS_CAPABILITIES & req_hdr->request_codes) != 0)
      {
         u32 = TALLYMARK_REQ_SYS_CAPABILITIES|TALLYMARK_REQ_SYS_VERSION;
         tallymark_msg_set_param(cnf->res, TALLYMARK_PARM_SYS_CAPABILITIES, &u32, sizeof(u32));
      };
      if ((TALLYMARK_REQ_SYS_VERSION & req_hdr->request_codes) != 0)
      {
         constr = PACKAGE_NAME;
         tallymark_msg_set_param(cnf->res, TALLYMARK_PARM_SYS_PKG_NAME, &constr, strlen(constr));
         constr = PACKAGE_VERSION;
         tallymark_msg_set_param(cnf->res, TALLYMARK_PARM_SYS_VERSION,  &constr, strlen(constr));
      };

      tallymark_msg_compile(cnf->res);
      tallymark_msg_get_header(cnf->res, &res_hdr);

      syslog(LOG_NOTICE, "client=%s, reqid=%08" PRIx32 ", seq=%08" PRIu32 ", res=%02x", straddr, res_hdr->request_id, res_hdr->sequence_id, res_hdr->response_codes);
      if ((err = (int)tallymark_msg_sendto(s, cnf->res, &addr.sa, addrlen)) == -1)
         syslog(LOG_NOTICE, "client=%s, reqid=%08" PRIx32 ", seq=%08" PRIu32 ", error=%s", straddr, res_hdr->request_id, res_hdr->sequence_id, tallymark_strerror(tallymark_msg_errnum(cnf->res)));
   };

   tallymarked_destroy(cnf);

   return(0);
}


/* end of source */
