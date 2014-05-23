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


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "udp-server"
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
   int               sec;
   int               s;
   int               opt;
   int               err;
   int               family;
   ssize_t           len;
   socklen_t         socklen;
   char            * str;
   const char      * host;
   const char      * port;
   struct addrinfo   hints;
   struct addrinfo * hintsp;
   struct addrinfo * res;
   char              straddr[INET6_ADDRSTRLEN];
   char              rbuff[128];
   char              sbuff[128];
   union
   {
      struct sockaddr         sa;
      struct sockaddr_in      sa_in;
      struct sockaddr_in6     sa_in6;
      struct sockaddr_storage sa_storage;
   } addr;

   static char   short_opt[] = "46hl:p:V";
   static struct option long_opt[] =
   {
      { "help",          no_argument, 0, 'h'},
      { "version",       no_argument, 0, 'V'},
      { NULL,            0,           0, 0  }
   };

   port   = "2211";
   family = PF_UNSPEC;
   sec    = 0;
   host   = NULL;

   if ((str = rindex(argv[0], '/')) != NULL)
   {
      str++;
      argv[0] = str;
   };

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

         case 'l':
         host = optarg;
         break;

         case 'p':
         sec = (int)strtol(optarg, NULL, 0);
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

   if (host == NULL)
   {
      if (family == AF_INET)
         host = "0.0.0.0";
      if (family == AF_INET6)
         host = "::";
   };

   if ((optind+1) < argc)
      host = argv[optind];
   if ((optind+2) <= argc)
      port = argv[optind+1];

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_V4MAPPED|AI_ALL;
   hints.ai_family   = family;
   hints.ai_socktype = 0; // 0 for any socket type or SOCK_[STREAM|DGRAM|RAW]
   hints.ai_protocol = IPPROTO_UDP; // any protocol (IPPROTO_UDP/IPPROTO_TCP)
   hintsp            = &hints;

   printf("%s: resolving address ...\n", argv[0]);
   if ((err = getaddrinfo(host, port, hintsp, &res)) != 0)
   {
      fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(err));
      return(1);
   };
   socklen  = res->ai_addrlen;
   memcpy(&addr, res->ai_addr, res->ai_addrlen);
   freeaddrinfo(res);

   if ((err = getnameinfo(&addr.sa, socklen, straddr, INET6_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST)) != 0)
   {
      fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(err));
      return(1);
   };

   printf("%s: creating socket ...\n", argv[0]);
   if ((s = socket(addr.sa.sa_family, SOCK_DGRAM, IPPROTO_UDP)) == -1)
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

   printf("%s: binding socket ...\n", argv[0]);
   if ((err = bind(s, &addr.sa, socklen)) == -1)
   {
      perror("bind()");
      close(s);
      return(-1);
   };

   inet_ntop(addr.sa.sa_family, &addr.sa_in6.sin6_addr, straddr, sizeof(straddr));

   printf("%s: listening on [%s]:%i ...\n", argv[0], straddr, ntohs(addr.sa_in6.sin6_port));
   socklen = sizeof(addr);
   while((len = recvfrom(s, rbuff, sizeof(rbuff)-1, 0, (struct sockaddr *)&addr, &socklen)) >= 0)
   {
      inet_ntop(addr.sa.sa_family, &addr.sa_in6.sin6_addr, straddr, sizeof(straddr));

      // logs messages
      rbuff[len] = '\0';
      printf("%s: [%s]:%i >>> %s\n", argv[0], straddr, ntohs(addr.sa_in6.sin6_port), rbuff);

      // pause responses to test timeouts
      if (sec > 0)
      {
         printf("%s: [%s]:%i === pausing for %i seconds\n", argv[0], straddr, ntohs(addr.sa_in6.sin6_port), sec);
         sleep((unsigned)sec);
      };

      // send message
      snprintf(sbuff, sizeof(sbuff)-1, "You sent: %s", rbuff);
      sbuff[sizeof(sbuff)-1] = '\0';
      if ((len = sendto(s, sbuff, strlen(sbuff), 0, &addr.sa, socklen)) == -1)
      {
         perror("sendto()");
         close(s);
         return(1);
      };
      printf("%s: [%s]:%i <<< %s\n", argv[0], straddr, ntohs(addr.sa_in6.sin6_port), sbuff);

      socklen = sizeof(struct sockaddr_storage);
   };

   return(0);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS] [ address [ port ] ]\n", PROGRAM_NAME);
   printf("Options:\n");
   printf("  -4                        listen on IPv4 addresses only\n");
   printf("  -6                        listen on IPv6 addresses only\n");
   printf("  -h, --help                print this help and exit\n");
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
