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
#include <sys/time.h>


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

#ifndef USE_CUSTOM_PTHREAD_MUTEX_TIMEDLOCK
// replica of pthread_mutex_timedlock() for systems missing this function
int pthread_mutex_timedlock(pthread_mutex_t * mutex,
   const struct timespec * abs_timeout)
{
   int             result;
   struct timeval  end;
   struct timeval  cur;

   // try an immediate lock per IEEE Std 1003.1, 2004 Edition
   if ((result = pthread_mutex_trylock(mutex)) != EBUSY)
      return(result);

   // calculate expiration time
   // 1 second ==     1,000,000 microsecond (usec)
   // 1 second == 1,000,000,000 nanosecond  (nsec)
   gettimeofday(&end, NULL);
   end.tv_sec  += abs_timeout->tv_sec;
   end.tv_usec += abs_timeout->tv_nsec / 1000;
   if (end.tv_usec >= 1000000)
   {
      end.tv_sec++;
      end.tv_usec -= 1000000;
   };

   // loop until locked or expired, pausing between each interation.
   while (result == EBUSY)
   {
      if ((result = pthread_mutex_trylock(mutex)) == EBUSY)
      {
         gettimeofday(&cur, NULL);
         if ((cur.tv_sec == end.tv_sec) && (cur.tv_usec > end.tv_usec))
            return(ETIMEDOUT);
         if (cur.tv_sec > end.tv_sec)
            return(ETIMEDOUT);
         usleep(1); // sleep length was arbitrarily chosen
      };
   };

   return(result);
}
#endif

/* end of source */
