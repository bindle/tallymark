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
#include "memory.h"

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

#include "fdpoller.h"


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

void tallymark_destroy(tallymark * tmd)
{
   if (tmd == NULL)
      return;

   // free memory for tracking FD poller state
   tallymark_fdpoll_free(tmd->poller);

   // destroy shared mutex attribute
   pthread_mutexattr_destroy(&tmd->mutexattr);

   // free memory for global tally mark state
   bzero(tmd, sizeof(tallymark));
   free(tmd);

   return;
}


int tallymark_init(tallymark ** ptmd)
{
   int         err;
   size_t      size;
   tallymark * tmd;

   assert(ptmd != NULL);

   // allocate and initialize memory for global tally mark state
   size = sizeof(tallymark);
   if ((tmd = malloc(size)) == NULL)
      return(errno);
   memset(tmd, 0, size);

   // create shared mutex attribute
   if ((err = pthread_mutexattr_init(&tmd->mutexattr)) != 0)
   {
      tallymark_destroy(tmd);
      return(err);
   };
   if ((err = pthread_mutexattr_settype(&tmd->mutexattr, PTHREAD_MUTEX_RECURSIVE)) != 0)
   {
      tallymark_destroy(tmd);
      return(err);
   };

   // initialize memory for tracking network state
   if ((err = tallymark_fdpoll_alloc(tmd, &tmd->poller)) != 0)
   {
      tallymark_destroy(tmd);
      return(err);
   };

   *ptmd = tmd;

   return(0);
}

/* end of source */
