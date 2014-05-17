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

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "udp-client"
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
   int               c;
   int               opt_index;
   int               err;
   int               s;
   char            * str;
   ssize_t           len;
   int               family;
   int               protocol;
   int               socktype;
   const char      * port;
   struct addrinfo   hints;
   struct addrinfo * hintsp;
   struct addrinfo * res;
   char              buff[1024];
   int               timeout;
   size_t            buff_len;
   char              straddr[INET6_ADDRSTRLEN];
   struct pollfd     fds[1];
   union
   {
      struct sockaddr         sa;
      struct sockaddr_in      sa_in;
      struct sockaddr_in6     sa_in6;
      struct sockaddr_storage sa_storage;
   } addr;

   static char   short_opt[] = "46hm:t:TUV";
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

   protocol = 0;
   family   = PF_UNSPEC;
   port     = "2211";
   buff_len = 0;
   timeout  = 0;

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case '4':
         family = PF_INET;
         break;

         case '6':
         family = PF_INET6;
         break;

         case 'h':
         my_usage();
         return(0);

         case 'm':
         strncpy(buff, optarg, sizeof(buff)-1);
         buff[sizeof(buff)-1] = '\0';
         buff_len = strlen(buff);
         break;

         case 'T':
         protocol = IPPROTO_TCP;
         break;

         case 't':
         timeout = (int)strtol(optarg, NULL, 0);
         break;

         case 'U':
         protocol = IPPROTO_UDP;
         break;

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

   // check for hostname from CLI
   if (optind >= argc)
   {
      fprintf(stderr, "%s: missing required argument\n", argv[0]);
      fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
      return(1);
   };

   // check for port from CLI
   if ((optind+2) <= argc)
      port = argv[optind+1];

   // set defaults
   if (timeout == 0)
      timeout = 5000; // timeout is in ms
   if (buff_len == 0)
   {
      strncpy(buff, "Hello World", sizeof(buff)-1);
      buff[sizeof(buff)-1] = '\0';
      buff_len = strlen(buff);
   };

   printf("%s: resolving address ...\n", argv[0]);
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_V4MAPPED|AI_ALL;
   hints.ai_family   = family;
   hints.ai_socktype = 0; // 0 for any socket type or SOCK_[STREAM|DGRAM|RAW]
   hints.ai_protocol = protocol; // any protocol (IPPROTO_UDP/IPPROTO_TCP)
   hintsp            = &hints;
   if ((err = getaddrinfo(argv[optind], port, hintsp, &res)) != 0)
   {
      fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(err));
      return(1);
   };
   memcpy(&addr, res->ai_addr, res->ai_addrlen);
   protocol = res->ai_protocol;
   socktype = res->ai_socktype;
   freeaddrinfo(res);

   // map binary address into human readable form
   if ((err = getnameinfo(&addr.sa, addr.sa.sa_len, straddr, INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST)) != 0)
   {
      fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(err));
      return(1);
   };

   printf("%s: creating socket ...\n", argv[0]);
   if ((s = socket(addr.sa.sa_family, socktype, protocol)) == -1)
   {
      perror("socket()");
      return(1);
   };

   printf("%s: connecting to %s://[%s]:%i ...\n", argv[0], (protocol == IPPROTO_UDP ? "udp" : "tcp"), straddr, ntohs(addr.sa_in6.sin6_port));
   if ((err = connect(s, &addr.sa, addr.sa.sa_len)) == -1)
   {
      perror("connect()");
      close(s);
      return(1);
   };

   printf("%s: sending data ...\n", argv[0]);
   printf("%s: >>> %s\n", argv[0], buff);
   if ((len = sendto(s, buff, buff_len, 0, NULL, 0)) == -1)
   {
      perror("sendto()");
      return(1);
   };

   fds[0].fd      = s;
   fds[0].events  = POLLIN;
   printf("%s: waiting for data ...\n", argv[0]);
   if ((err = poll(fds, 1, timeout)) == -1)
   {
      perror("poll()");
      close(s);
      return(1);
   };
   if (err == 0)
   {
      printf("%s: connection timed out ...\n", argv[0]);
      close(s);
      return(1);
   };

   printf("%s: receiving data ...\n", argv[0]);
   if ((len = recvfrom(s, buff, sizeof(buff)-1, 0, NULL, NULL)) == -1)
   {
      perror("recvfrom()");
      close(s);
      return(1);
   };
   buff[len] = '\0';
   printf("%s: <<< %s\n", argv[0], buff);

   close(s);

   return(0);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS] host [ port ]\n", PROGRAM_NAME);
   printf("Options:\n");
   printf("  -4                        use IPv4\n");
   printf("  -6                        use IPv6\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -m message                message to send to server\n");
   printf("  -T                        use TCP\n");
   printf("   -t ms                    timeout in milliseconds\n");
   printf("  -U                        use UDP\n");
   printf("  -V, --version             print version number and exit\n");
   printf("\n");
   return;
}


void my_version(void)
{
   printf(( "%s (%s) %s\n"
            "%s\n"),
            PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION,
            PACKAGE_COPYRIGHT
   );
   return;
}


/* end of source */
