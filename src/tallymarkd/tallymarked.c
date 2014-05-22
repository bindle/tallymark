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

#include "memory.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "tallymarked"
#endif
#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT "syzdek@bindlebinaries.com"
#endif
#ifndef PACKAGE_COPYRIGHT
#define PACKAGE_COPYRIGHT ""
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME ":-|"
#endif
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION ""
#endif


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

int main(int argc, char * argv[]);
void my_usage(void);
void my_version(void);


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
   int                  c;
   int                  opt_index;
   tallymarked_cnf    * cnf;
   tallymark_url_desc * tudp;
   tallymark_sockaddr   addr;
   const tallymark_hdr  * req_hdr;
   int                  s;
   int                  opt;
   int                  err;
   char               * str;
   char                 straddr[INET6_ADDRSTRLEN];
   socklen_t            addrlen;
   uint32_t             u32;
   uint8_t              u8;
   const char         * constr;

   static char   short_opt[] = "46hl:p:V";
   static struct option long_opt[] =
   {
      { "help",          no_argument, 0, 'h'},
      { "version",       no_argument, 0, 'V'},
      { NULL,            0,           0, 0  }
   };

   if ((str = rindex(argv[0], '/')) != NULL)
   {
      str++;
      argv[0] = str;
   };

   if ((cnf = tallymarked_alloc()) == NULL)
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case '4':
         cnf->opts |= TALLYMARKED_IPV4;
         cnf->opts &= ~TALLYMARKED_IPV6;
         break;

         case '6':
         cnf->opts |= TALLYMARKED_IPV6;
         cnf->opts &= ~TALLYMARKED_IPV4;
         break;

         case 'h':
         my_usage();
         tallymarked_free(cnf);
         return(0);

         case 'l':
         cnf->url = optarg;
         break;

         case 'V':
         my_version();
         tallymarked_free(cnf);
         return(0);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
         tallymarked_free(cnf);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", argv[0], c);
         fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
         tallymarked_free(cnf);
         return(1);
      };
   };

   // parse URL and resolve hostname
   if ((err = tallymark_url_parse(cnf->url, &cnf->tudp, 1)) != 0)
   {
      fprintf(stderr, "%s: tallymark_url_parse(): %s\n", argv[0], tallymark_strerror(err));
      tallymarked_free(cnf);
      return(1);
   };

   // create socket
   tudp = cnf->tudp;
   if ((s = socket(tudp->tud_family, tudp->tud_socktype, tudp->tud_protocol)) == -1)
   {
      perror("socket()");
      return(1);
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
      tallymarked_free(cnf);
      close(s);
      return(-1);
   };

   printf("%s: listening on %s ...\n", argv[0], tudp->tud_strurl);

   while(1)
   {
      addrlen = sizeof(addr);
      err = tallymark_msg_recvfrom(s, cnf->req, &addr.sa, &addrlen);
      strcpy(straddr, "unknown");
      if (addr.sa.sa_family != 0)
         getnameinfo(&addr.sa, addrlen, straddr, sizeof(straddr), NULL, 0, NI_NUMERICHOST);
      if (err != 0)
      {
         printf("%s/%i: error: %s\n", straddr, ntohs(addr.sa_in.sin_port), tallymark_strerror(err));
         continue;
      };
      tallymark_msg_get_header(cnf->req, &req_hdr);
      printf("%s/%i: %u: received request for %08x\n", straddr, ntohs(addr.sa_in.sin_port), req_hdr->request_id, req_hdr->request_codes);

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

      printf("%s/%i: %u: sending response\n", straddr, ntohs(addr.sa_in.sin_port), req_hdr->request_id);
      if ((err = (int)tallymark_msg_sendto(s, cnf->res, &addr.sa, addrlen)) == -1)
         printf("%s/%i: %u: error: %s\n", straddr, ntohs(addr.sa_in.sin_port), req_hdr->request_id, tallymark_strerror(tallymark_msg_errnum(cnf->res)));
   };

   tallymarked_free(cnf);

   return(0);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
   printf("Options:\n");
   printf("  -4                        listen on IPv4 addresses only\n");
   printf("  -6                        listen on IPv6 addresses only\n");
   printf("  -l address                listen on address (default: any\n");
   printf("  -p port                   listen on port\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -V, --version             print version number and exit\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("\n");
   return;
}


void my_version(void)
{
   printf(( "%s (%s) %s\n"
            "libtallymark (%s) %s\n"
            "%s\n"),
            PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION,
            PACKAGE_NAME, tallymark_pkg_version(NULL, NULL, NULL, NULL),
            PACKAGE_COPYRIGHT
   );
   return;
}


/* end of source */
