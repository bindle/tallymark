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

#include <tallymark.h>


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

   cnf->s[0] = s;

   return(0);
}


/* end of source */
