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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <syslog.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <time.h>

#include <tallymark.h>

#include "db.h"


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

int tallymarked_listen(tallymarked_cnf * cnf)
{
   int s;
   int err;
   int opt;
   tallymark_url_desc * tudp;

   assert(cnf != NULL);

   // parse URL and resolve hostname
   if ((err = tallymark_url_parse(cnf->urlstr, &cnf->tudp, 1)) != 0)
   {
      fprintf(stderr, "%s: tallymark_url_parse(): %s\n", cnf->prog_name, tallymark_strerror(err));
      return(-1);
   };

   // create socket
   tudp = cnf->tudp;
   if ((s = socket(tudp->tud_family, tudp->tud_socktype, tudp->tud_protocol)) == -1)
   {
      perror("socket()");
      return(-1);
   };

   //fcntl(s, F_SETFL, O_NONBLOCK);
   opt = 1;setsockopt(s, SOL_SOCKET,   SO_REUSEADDR, (void *)&opt, sizeof(int));
   opt = 0;setsockopt(s, IPPROTO_IPV6,  IPV6_V6ONLY, (void *)&opt, sizeof(int));
#ifdef SO_NOSIGPIPE
   opt = 1;setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, (void *)&opt, sizeof(int));
#endif

   if (bind(s, &tudp->tud_addr.sa, tudp->tud_addrlen))
   {
      perror("bind()");
      close(s);
      return(-1);
   };

   cnf->fds[0].fd      = s;
   cnf->fds[0].events  = POLLIN;

   syslog(LOG_INFO, "listening on %s", cnf->tudp->tud_strurl);

   return(0);
}


int tallymarked_loop(tallymarked_cnf * cnf)
{
   size_t                  len;
   int                     i;
   int                     s;
   int                     err;
   char                    straddr[INET6_ADDRSTRLEN];
   char                    strhash[41];
   socklen_t               addrlen;
   uint32_t                u32;
   uint8_t                 u8;
   const char            * constr;
   tallymarked_record    * rec;
   tallymark_count         count;
   tallymark_sockaddr      addr;
   const tallymark_hdr   * req_hdr;
   const tallymark_hdr   * res_hdr;

   assert(cnf != NULL);

   s = cnf->fds[0].fd;

   // wait for data
   switch(poll(cnf->fds, 1, 1))
   {
      case -1:
      syslog(LOG_ERR, "poll(): %m");
      return(1);

      case 0:
      return(0);

      default:
      break;
   };

   // read request
   addrlen = sizeof(addr);
   bzero(&addr, addrlen);
   if ((err = tallymark_msg_recvfrom(s, cnf->req, &addr.sa, &addrlen)) != 0)
      return(0);

   // resolve client address
   strcpy(straddr, "unknown");
   if (addr.sa.sa_family != 0)
      getnameinfo(&addr.sa, addrlen, straddr, sizeof(straddr), NULL, 0, NI_NUMERICHOST);
   if (err != 0)
   {
      syslog(LOG_ERR, "client=%s error=%s", straddr, tallymark_strerror(err));
      return(0);;
   };

   // parse header
   tallymark_msg_get_header(cnf->req, &req_hdr);
   for(i = 0; i < 20; i++)
      snprintf(&strhash[i*2], 3, "%02x", req_hdr->hash[i]);
   syslog(LOG_NOTICE, "client=%s, reqid=%08" PRIx32 ", req=%" PRIx32 ", type=%u:%u, hash=%s", straddr, req_hdr->request_id, req_hdr->request_codes, req_hdr->service, req_hdr->field_id, strhash);

   // retrieve requested record
   if ((err = tallymarked_db_record(cnf, cnf->db, req_hdr->service, req_hdr->field_id, req_hdr->hash, &rec)) != 0)
   {
      syslog(LOG_ERR, "client=%s error=%s", straddr, tallymark_strerror(err));
      return(0);
   };
   
   tallymark_msg_create_header(cnf->res, req_hdr->request_id, req_hdr->service, req_hdr->field_id, req_hdr->hash, sizeof(req_hdr->hash));

   u8 = TALLYMARK_RES_RESPONSE|TALLYMARK_RES_EOR;
   tallymark_msg_set_header(cnf->res, TALLYMARK_HDR_RESPONSE_CODES, &u8, sizeof(u8));
   tallymark_msg_set_header(cnf->res, TALLYMARK_HDR_REQUEST_CODES, &req_hdr->request_codes, sizeof(req_hdr->request_codes));

   if ((TALLYMARK_REQ_HASH_SET_TEXT & req_hdr->request_codes) != 0)
   {
      if (rec->hash_text == NULL)
      {
         len = sizeof(rec->hash_text);
         tallymark_msg_get_param(cnf->req, TALLYMARK_PARM_HASH_TEXT, &rec->hash_text, &len);
      };
   };

   if ((TALLYMARK_REQ_HASH_RESET & req_hdr->request_codes) != 0)
   {
      rec->count.count = 0;
      rec->count.seconds = (uint64_t)time(NULL);
   };

   if ((TALLYMARK_REQ_HASH_INCREMENT & req_hdr->request_codes) != 0)
   {
      rec->count.count++;
      if (rec->count.seconds == 0)
         rec->count.seconds = (uint64_t)time(NULL);
   };

   if (  ((TALLYMARK_REQ_HASH_COUNT & req_hdr->request_codes) != 0) ||
         ((TALLYMARK_REQ_HASH_RECORD & req_hdr->request_codes) != 0) )
   {
      count.count   = rec->count.count;
      count.seconds = (uint64_t)time(NULL) - rec->count.seconds;
      count.seconds++;
      tallymark_msg_set_param(cnf->res, TALLYMARK_PARM_HASH_COUNT, &count, sizeof(count));
   };

   if ((TALLYMARK_REQ_HASH_RECORD & req_hdr->request_codes) != 0)
      if ((constr = rec->hash_text) != NULL)
         tallymark_msg_set_param(cnf->res, TALLYMARK_PARM_HASH_TEXT, &constr, strlen(constr));

   if ((TALLYMARK_REQ_SYS_CAPABILITIES & req_hdr->request_codes) != 0)
   {
      u32 =
            TALLYMARK_REQ_HASH_SET_TEXT |
            TALLYMARK_REQ_HASH_RECORD |
            TALLYMARK_REQ_HASH_RESET |
            TALLYMARK_REQ_HASH_COUNT |
            TALLYMARK_REQ_HASH_INCREMENT |
            TALLYMARK_REQ_HASH_THRESHOLD |
            TALLYMARK_REQ_SYS_CAPABILITIES |
            TALLYMARK_REQ_SYS_VERSION;
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

   return(0);
}


/* end of source */
