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
#include "url.h"

///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
#endif

#include "libtallymark.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <netdb.h>
#include <ctype.h>


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Variables
#endif

static int  tallymark_url_initialized = 0;
regex_t     tallymark_url_regex;


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

void tallymark_url_free(tallymark_url_desc * tudp)
{
   assert(tudp != NULL);

   // frees sub, allocations
   if (tudp->tud_scheme != NULL)
      free(tudp->tud_scheme);
   if (tudp->tud_host != NULL)
      free(tudp->tud_host);
   if (tudp->tud_path != NULL)
      free(tudp->tud_path);
   if (tudp->tud_straddr != NULL)
      free(tudp->tud_straddr);
   if (tudp->tud_strurl != NULL)
      free(tudp->tud_strurl);

   bzero(tudp, sizeof(tallymark_url_desc));

   free(tudp);

   return;
}


int tallymark_url_initialize(void)
{
   int err;
   int cflags;

   if (tallymark_url_initialized != 0)
      return(0);

   cflags = REG_EXTENDED | REG_ICASE;

   if ((err = regcomp(&tallymark_url_regex, TALLYMARK_URL_REGEX, cflags)))
      return(-1);

   tallymark_url_initialized = 1;

   return(0);
}


int tallymark_is_url(const char * urlstr)
{
   assert(urlstr != NULL);
   tallymark_url_initialize();
   if (regexec(&tallymark_url_regex, urlstr, 0, NULL, 0) != 0)
      return(0);
   return(1);
}


int tallymark_url_parse(const char * urlstr, tallymark_url_desc ** tudpp,
   int resolve)
{
   int                  err;
   unsigned             pos;
   regmatch_t           pmatch[9];
   char                 str[160];
   tallymark_url_desc * tudp;
   const char         * ptr;

   assert(urlstr != NULL);
   assert(tudpp  != NULL);

   tallymark_url_initialize();

   if ((err = regexec(&tallymark_url_regex, urlstr, 8, pmatch, 0)) != 0)
      return(EINVAL);

   strncpy(str, urlstr, sizeof(str));

   if ((tudp = malloc(sizeof(tallymark_url_desc))) == NULL)
      return(ENOMEM);
   bzero(tudp, sizeof(tallymark_url_desc));
   tudp->tud_family = PF_UNSPEC;

   // copy "scheme" into struct member "tud_scheme"
   str[pmatch[SUBMATCH_SCHEME].rm_eo] = '\0';
   ptr = &str[pmatch[SUBMATCH_SCHEME].rm_so];
   if ((tudp->tud_scheme = strdup(ptr)) == NULL)
   {
      tallymark_url_free(tudp);
      return(ENOMEM);
   };
   for(pos = 0; pos < strlen(tudp->tud_scheme); pos++)
      tudp->tud_scheme[pos] = (char)tolower(tudp->tud_scheme[pos]);

   // copy "host" into struct member "tud_host"
   ptr = NULL;
   if (pmatch[SUBMATCH_IPV6].rm_eo != pmatch[SUBMATCH_IPV6].rm_so)
   {
      str[pmatch[SUBMATCH_IPV6].rm_eo] = '\0';
      ptr = &str[pmatch[SUBMATCH_IPV6].rm_so];
   }
   else if (pmatch[SUBMATCH_HOST].rm_eo != pmatch[SUBMATCH_HOST].rm_so)
   {
      str[pmatch[SUBMATCH_HOST].rm_eo] = '\0';
      ptr = &str[pmatch[SUBMATCH_HOST].rm_so];
   };
   if (ptr != NULL)
   {
      if ((tudp->tud_host = strdup(ptr)) == NULL)
      {
         tallymark_url_free(tudp);
         return(ENOMEM);
      };
      for(pos = 0; pos < strlen(tudp->tud_host); pos++)
         tudp->tud_host[pos] = (char)tolower(tudp->tud_host[pos]);
   };

   // copy "port" into struct member "tud_port"
   if (pmatch[SUBMATCH_IPV6].rm_eo != pmatch[SUBMATCH_IPV6].rm_so)
   {
      str[pmatch[SUBMATCH_PORT].rm_eo] = '\0';
      ptr = &str[pmatch[SUBMATCH_PORT].rm_so];
      tudp->tud_port = ((int)strtol(ptr, NULL, 10) & 0xFFFF);
      if ((tudp->tud_port < 1) || (tudp->tud_port > 65535))
      {
         tallymark_url_free(tudp);
         return(EINVAL);
      };
   };

   // copy "path" into struct member "tud_path"
   ptr = NULL;
   if (pmatch[SUBMATCH_PATH].rm_eo != pmatch[SUBMATCH_PATH].rm_so)
   {
      str[pmatch[SUBMATCH_PATH].rm_eo] = '\0';
      ptr = &str[pmatch[SUBMATCH_PATH].rm_so];
   };
   if (ptr != NULL)
   {
      if ((tudp->tud_path = strdup(ptr)) == NULL)
      {
         tallymark_url_free(tudp);
         return(ENOMEM);
      };
   };

   // resolve hostname
   if (resolve != 0)
   {
      if ((err = tallymark_url_resolve(tudp)) != 0)
      {
         tallymark_url_free(tudp);
         return((err != ENOMEM) ? EINVAL : ENOMEM);
      };
   };

   *tudpp = tudp;

   return(0);
}


int tallymark_url_resolve(tallymark_url_desc * tudp)
{
   char              port[8];
   struct addrinfo   hints;
   struct addrinfo * hintsp;
   struct addrinfo * res;

   assert(tudp             != NULL);
   assert(tudp->tud_scheme != NULL);

   if (tudp->tud_family != PF_UNSPEC)
      return(0);

   // tally:// specific options
   if (!(strncasecmp(tudp->tud_scheme, "tally", 6)))
   {
      tudp->tud_socktype = SOCK_DGRAM;
      tudp->tud_protocol = IPPROTO_UDP;
   };

   // tallyi:// specific options
   if (!(strcmp(tudp->tud_scheme, "tallyi")))
   {
      tudp->tud_family     = PF_UNIX;
      tudp->tud_socktype   = SOCK_STREAM;
      tudp->tud_protocol   = IPPROTO_TCP;
      tudp->tud_addrlen    = sizeof(tudp->tud_addr.sa_un);

      // copies path
      if (tudp->tud_path == NULL)
         if ((tudp->tud_path = strdup("/var/run/tallymark/tallymark.sock")) == NULL)
            return(ENOMEM);
      strncpy(tudp->tud_addr.sa_un.sun_path, tudp->tud_path, sizeof(tudp->tud_addr.sa_un.sun_path)-1);
      tudp->tud_addr.sa_un.sun_path[sizeof(tudp->tud_addr.sa_un.sun_path)-1] = '\0';

      // generates numeric URL
      if (tudp->tud_strurl != NULL)
         free(tudp->tud_strurl);
      if (asprintf(&tudp->tud_strurl, "tallyi://%s", tudp->tud_path) == -1)
         return(ENOMEM);
      tudp->tud_strurl_len = (socklen_t)strlen(tudp->tud_strurl);

      return(EPROTONOSUPPORT);
   };

   // tallys:// specific options
   if (!(strncasecmp(tudp->tud_scheme, "tallys", 6)))
   {
      tudp->tud_socktype = SOCK_STREAM;
      tudp->tud_protocol = IPPROTO_TCP;
      return(EPROTONOSUPPORT);
   };

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags    = AI_V4MAPPED|AI_ALL;
   hints.ai_family   = tudp->tud_family;
   hints.ai_socktype = tudp->tud_socktype; // 0 for any socket type or SOCK_[STREAM|DGRAM|RAW]
   hints.ai_protocol = tudp->tud_protocol; // any protocol (IPPROTO_UDP/IPPROTO_TCP)
   hintsp            = &hints;

   // fills in defaults if missing
   if (tudp->tud_host == NULL)
      if ((tudp->tud_host = strdup("localhost")) == NULL)
         return(ENOMEM);
   if (tudp->tud_port == 0)
      tudp->tud_port = TALLYMARK_PORT;
   snprintf(port, sizeof(port), "%u", tudp->tud_port);

   // resolves hosts
   if (getaddrinfo(tudp->tud_host, port, hintsp, &res) != 0)
      return(EINVAL);

   // copies data
   tudp->tud_family   = res->ai_family;
   tudp->tud_socktype = res->ai_socktype;
   tudp->tud_protocol = res->ai_protocol;
   tudp->tud_addrlen  = res->ai_addrlen;
   memcpy(&tudp->tud_addr, res->ai_addr, res->ai_addrlen);

   freeaddrinfo(res);

   // maps binary address into string address
   if (tudp->tud_straddr == NULL)
   {
      if ((tudp->tud_straddr = malloc(INET6_ADDRSTRLEN)) == NULL)
         return(ENOMEM);
      bzero(tudp->tud_straddr, INET6_ADDRSTRLEN);
      tudp->tud_straddr_len = INET6_ADDRSTRLEN;
   };
   getnameinfo(&tudp->tud_addr.sa, tudp->tud_addrlen, tudp->tud_straddr, tudp->tud_straddr_len, NULL, 0, NI_NUMERICHOST);

   // generates numeric URL
   if (tudp->tud_strurl != NULL)
      free(tudp->tud_strurl);
   if (tudp->tud_family == AF_INET6)
      asprintf(&tudp->tud_strurl, "%s://[%s]:%i/", tudp->tud_scheme, tudp->tud_straddr, tudp->tud_port);
   else
      asprintf(&tudp->tud_strurl, "%s://%s:%i/", tudp->tud_scheme, tudp->tud_straddr, tudp->tud_port);
   if (tudp->tud_strurl == NULL)
      return(ENOMEM);
   tudp->tud_strurl_len = (socklen_t)strlen(tudp->tud_strurl);

   return(0);
}


/* end of source */
