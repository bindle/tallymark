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

#ifdef HAVE_CONFIG_H
#   include "config.h"
#else
#   include "git-package-version.h"
#endif

#include <tallymark.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "tallymarker"
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
   const char         * tallyurl;
   int                  s;
   //int               opt;
   int                  err;
   size_t               len;
   tallymark_msg      * msg;
   const tallymark_hdr * hdr;
   tallymark_url_desc * tudp;
   uint32_t             inval32;
   const uint8_t      * msg_buff;
   size_t               msg_buff_len;
   char               * str;

   static char   short_opt[] = "H:hV";
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

   tallyurl = "tally://localhost/";

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case 'H':
         tallyurl = optarg;
         break;

         case 'h':
         my_usage();
         return(0);

         case 'V':
         my_version();
         return(0);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", argv[0], c);
         fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
         return(1);
      };
   };

   if ((err = tallymark_msg_alloc(&msg)) != 0)
   {
      fprintf(stderr, "tallymark_msg_alloc(): %s\n",tallymark_strerror(err));
      return(1);
   };

   // parse URL and resolve hostname
   if ((err = tallymark_url_parse(tallyurl, &tudp, 1)) != 0)
   {
      fprintf(stderr, "%s: tallymark_url_parse(): %s\n", argv[0], tallymark_strerror(err));
      return(1);
   };

   if ((s = socket(tudp->tud_family, tudp->tud_socktype, tudp->tud_protocol)) == -1)
   {
      perror("socket()");
      return(1);
   };

   printf("%s: connecting to %s ...\n", argv[0], tudp->tud_strurl);
   if ((err = connect(s, &tudp->tud_addr.sa, tudp->tud_addrlen)) == -1)
   {
      perror("connect()");
      close(s);
      return(1);
   };

   tallymark_msg_create_header(msg, (uint32_t)time(NULL), 25, 1, (const uint8_t *)"0123456789", 10);
   tallymark_msg_get_header(msg, &hdr);
   inval32 = TALLYMARK_REQ_SYS_CAPABILITIES |TALLYMARK_REQ_SYS_VERSION;
   tallymark_msg_set_header(msg, TALLYMARK_HDR_REQUEST_CODES, &inval32, sizeof(inval32));

   tallymark_msg_compile(msg);
   tallymark_msg_get_buffer(msg, &msg_buff, &msg_buff_len);

   if ((err = tallymark_msg_sendto(s, msg, NULL, 0)) != 0)
   {
      fprintf(stderr, "tallymark_msg_sendto(): %s\n", tallymark_strerror(err));
      return(1);
   };

   if ((err = tallymark_msg_recvfrom(s, msg, NULL, 0)) != 0)
   {
      fprintf(stderr, "tallymark_msg_sendto(): %s\n", tallymark_strerror(err));
      return(1);
   };

   len = sizeof(str);
   if ((err = tallymark_msg_get_param(msg, TALLYMARK_PARM_SYS_PKG_NAME, &str, &len)) != 0)
      printf("tallymark_msg_get_param(TALLYMARK_PARM_SYS_PKG_NAME): %s\n", tallymark_strerror(err));
   printf("%s", str);
   free(str);

   len = sizeof(str);
   if ((err = tallymark_msg_get_param(msg, TALLYMARK_PARM_SYS_VERSION, &str, &len)) != 0)
      printf("tallymark_msg_get_param(TALLYMARK_PARM_SYS_VERSION): %s\n", tallymark_strerror(err));
   printf(" (%s)\n", str);
   free(str);

   len = sizeof(inval32);
   inval32 = 0;
   if ((err = tallymark_msg_get_param(msg, TALLYMARK_PARM_SYS_CAPABILITIES, &inval32, &len)) != 0)
      printf("tallymark_msg_get_param(): %s\n", tallymark_strerror(err));
   printf("Capabilities: %08x\n", inval32);

   return(0);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
   printf("Options:\n");
   printf("  -H host                   tallymarked host\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -p port                   tallymarked port\n");
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
