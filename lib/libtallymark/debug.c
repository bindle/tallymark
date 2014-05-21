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
#include "debug.h"

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


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

void tallymark_print_hexdump(FILE * fs, const uint8_t * buff, size_t len,
   const char * prefix, const char * fmt, ...)
{
   size_t x;
   size_t y;
   va_list ap;

   if (fmt != NULL)
   {
      va_start(ap, fmt);
      vprintf(fmt, ap);
      va_end(ap);
   };

   prefix = (prefix == NULL) ? "" : prefix;

   fprintf(fs, "%s offset    0  1  2  3   4  5  6  7   8  9  a  b   c  d  e  f  0123456789abcdef\n", prefix);
   len = (len > (~0LLU - 16LLU)) ? (~0LLU - 16LLU) : len;
   for(y = 0; y < len; y += 16)
   {
      fprintf(fs, "%s%08zx", prefix, y);
      for(x = 0; x < 16; x++)
      {
         if ((x & 0x03) == 0)
            fprintf(fs, " ");
         if ((x+y) < len)
            fprintf(fs, " %02x", buff[x+y]);
         else
            fprintf(fs, "   ");
      };
      fprintf(fs, "  ");
      for(x = y; (x < (y+16)) && (x < len); x++)
         fprintf(fs, "%c", ((buff[x] >= 0x20) && (buff[x] <= 0x7e)) ? buff[x] : '.');
      fprintf(fs, "\n");
   };
   return;
}


/* end of source */
