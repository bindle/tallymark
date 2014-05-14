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
#include "thread.h"

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
#include <pthread.h>


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymark_mutex_timedlock(pthread_mutex_t * restrict mutex,
   const struct timespec * restrict abs_timeout)
{
   int             result;
   int             expired;
   struct timespec ts;

   result = pthread_mutex_trylock(mutex);

   ts.tv_sec  = abs_timeout->tv_sec;
   ts.tv_nsec = abs_timeout->tv_nsec;
   expired    = 0;
   result     = EBUSY;

   while ((result == EBUSY) && (expired == 0))
   {
      if ((result = pthread_mutex_trylock(mutex)) == EBUSY)
      {
         if (ts.tv_nsec < 1000000)
         {
            if (abs_timeout->tv_sec == 0)
               return(result);
            ts.tv_sec--;
            ts.tv_nsec += 1000000000;
         };
         ts.tv_nsec -= 1000000;

         // 1 second ==         1,000 millisecond (msec)
         // 1 second ==     1,000,000 microsecond (usec)
         // 1 second == 1,000,000,000 nanosecond  (nsec)
         usleep(1000);
      };
   };
   return(result);
}

/* end of source */
