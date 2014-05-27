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
#include "random.h"

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
#include <stdarg.h>
#include <time.h>
#include <unistd.h>


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

// The following is based upon the following:
//   http://stackoverflow.com/questions/322938/recommended-way-to-initialize-srand
//   http://burtleburtle.net/bob/hash/doobs.html
uint64_t tallymark_seed(void)
{
   uint64_t a;
   uint64_t b;
   uint64_t c;

   a = (uint64_t)clock();
   b = (uint64_t)time(NULL);
   c = (uint64_t)getpid();

   a=a-b;  a=a-c; a=a^(c >> 26);
   b=b-c;  b=b-a; b=b^(a << 16);
   c=c-a;  c=c-b; c=c^(b >> 13);

   a=a-b;  a=a-c; a=a^(c >> 24);
   b=b-c;  b=b-a; b=b^(a << 32);
   c=c-a;  c=c-b; c=c^(b >> 10);

   a=a-b;  a=a-c; a=a^(c >>  6);
   b=b-c;  b=b-a; b=b^(a << 20);
   c=c-a;  c=c-b; c=c^(b >> 30);

   return(c);
}


/* end of source */
