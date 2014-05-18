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
#include "socket.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
#endif

#include "libtallymark_util.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int libtmu_alloc(libtmu_sock ** psock)
{
   libtmu_sock * sock;
   assert(psock != NULL);
   if ((sock = malloc(sizeof(libtmu_sock))) == NULL)
      return(ENOMEM);
   memset(sock, 0, sizeof(libtmu_sock));

   sock->s        = -1;
   sock->family   = PF_UNSPEC;
   sock->socktype = SOCK_DGRAM;
   sock->protocol = IPPROTO_UDP;

   *psock = sock;

   return(0);
}


int libtmu_close(libtmu_sock * sock)
{
   assert(sock != NULL);
   if (sock->s == -1)
      return(0);
   close(sock->s);
   sock->s = -1;
   return(0);
}


int libtmu_free(libtmu_sock * sock)
{
   assert(sock != NULL);
   libtmu_close(sock);
   memset(sock, 0, sizeof(libtmu_sock));
   free(sock);
   return(0);
}


//_TALLYMARK_F int libtmu_open(libtmu_sock * sock, const char * host, const char * service);

int libtmu_resolve(libtmu_sock * sock, const char * host, const char * service)
{
   int               err;
   struct addrinfo   hints;
   struct addrinfo * hintsp;
   struct addrinfo * res;

   assert(sock != NULL);

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_V4MAPPED|AI_ALL;
   hints.ai_family   = sock->family;
   hints.ai_socktype = sock->socktype; // 0 for any socket type or SOCK_[STREAM|DGRAM|RAW]
   hints.ai_protocol = sock->protocol; // any protocol (IPPROTO_UDP/IPPROTO_TCP)
   hintsp            = &hints;
   switch(getaddrinfo(host, service, hintsp, &res))
   {
      case 0:              break;
      case EAI_AGAIN:      return(EAGAIN);
      case EAI_BADFLAGS:   return(EINVAL);
      case EAI_BADHINTS:   return(EINVAL);
      case EAI_FAIL:       return(EINVAL);
      case EAI_FAMILY:     return(EPFNOSUPPORT);
      case EAI_MEMORY:     return(ENOMEM);
      case EAI_OVERFLOW:   return(EINVAL);
      case EAI_PROTOCOL:   return(EPROTONOSUPPORT);
      case EAI_SERVICE:    return(EINVAL);
      case EAI_SOCKTYPE:   return(ESOCKTNOSUPPORT);
      case EAI_SYSTEM:     return(errno);

      fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(err));
      return(1);
   };
   memcpy(&sock->addr, res->ai_addr, res->ai_addrlen);
   sock->protocol = res->ai_protocol;
   sock->socktype = res->ai_socktype;
   sock->addrlen  = res->ai_addrlen;
   freeaddrinfo(res);

   return(0);
}


/* end of source */
