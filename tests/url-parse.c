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

#include <tallymark.h>
#include <stdio.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

int main(void);


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Variables
#endif

const char * tallymark_valid_resolv_urls[] =
{
   "tally://",
   "tally:///",
   "tally://:2211",
   "tally://:2211/",
   "tally://localhost",
   "tally://localhost/",
   "tally://localhost:2211",
   "tally://localhost:2211/",
   "tally://127.0.0.1",
   "tally://127.0.0.1/",
   "tally://127.0.0.1:2211",
   "tally://127.0.0.1:2211/",
   "tally://0.0.0.0",
   "tally://0.0.0.0/",
   "tally://0.0.0.0:2211",
   "tally://0.0.0.0:2211/",
   "tally://[::]",
   "tally://[::]/",
   "tally://[::]:2211",
   "tally://[::]:2211/",
   "tally://[::1]",
   "tally://[::1]/",
   "tally://[::1]:2211",
   "tally://[::1]:2211/",
   "tally://[fe80::]",
   "tally://[fe80::]/",
   "tally://[fe80::]:2211",
   "tally://[fe80::]:2211/",
   NULL
};

const char * tallymark_valid_urls[] =
{
   "tally://[fe80::6a5b:35ff:fe94:d491%en4]",
   "tally://[fe80::6a5b:35ff:fe94:d491%en4]/",
   "tally://[fe80::6a5b:35ff:fe94:d491%en4]:2211",
   "tally://[fe80::6a5b:35ff:fe94:d491%en4]:2211/",
   NULL
};

const char * tallymark_invalid_urls[] =
{
   "tally://local_host",
   "tally://local_host/",
   "tally://local_host:2211",
   "tally://local_host:2211/",
   NULL
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int main(void)
{
   int                  rc;
   size_t               pos;
   tallymark_url_desc * tudp;

   rc = 0;

   printf("URL Regex:\n");
   printf("  %s\n", tallymark_url_regex_string());

   printf("Testing Invalid URLs:\n");
   for(pos = 0; tallymark_invalid_urls[pos] != NULL; pos++)
   {
      if (tallymark_url_parse(tallymark_invalid_urls[pos], &tudp, 0) != 0)
      {
         fprintf(stdout, "  Failed: %s\n", tallymark_invalid_urls[pos]);
         continue;
      };
      rc = 1;
      fprintf(stdout, "! Passed: %s\n", tallymark_invalid_urls[pos]);
      tallymark_url_free(tudp);
   };

   printf("Testing Valid URLs:\n");
   for(pos = 0; tallymark_valid_urls[pos] != NULL; pos++)
   {
      if (tallymark_url_parse(tallymark_valid_urls[pos], &tudp, 0) != 0)
      {
         fprintf(stdout, "! Failed: %s\n", tallymark_valid_urls[pos]);
         rc = 1;
         continue;
      };
      fprintf(stdout, "  Passed: %s\n", tallymark_valid_urls[pos]);
      tallymark_url_free(tudp);
   };

   printf("Testing Resolvable URLs:\n");
   for(pos = 0; tallymark_valid_resolv_urls[pos] != NULL; pos++)
   {
      if (tallymark_url_parse(tallymark_valid_resolv_urls[pos], &tudp, 1) != 0)
      {
         fprintf(stdout, "! Failed: %s\n", tallymark_valid_resolv_urls[pos]);
         rc = 1;
         continue;
      };
      fprintf(stdout, "  Passed: %s\n", tallymark_valid_resolv_urls[pos]);
      tallymark_url_free(tudp);
   };

   return(rc);
}


/* end of source */
