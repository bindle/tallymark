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

#include "tallymarked.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdlib.h>

#include "memory.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "tallymarked"
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
   int              c;
   int              opt_index;
   tallymarked_cnf * cnf;

   static char   short_opt[] = "46hl:p:V";
   static struct option long_opt[] =
   {
      { "help",          no_argument, 0, 'h'},
      { "version",       no_argument, 0, 'V'},
      { NULL,            0,           0, 0  }
   };

   if ((cnf = tallymarked_alloc()) == NULL)
   {
      fprintf(stderr, "%s: out of virtual memory\n", PROGRAM_NAME);
      return(1);
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, &opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;

         case '4':
         cnf->opts |= TALLYMARKED_IPV4;
         cnf->opts &= ~TALLYMARKED_IPV6;
         break;

         case '6':
         cnf->opts |= TALLYMARKED_IPV6;
         cnf->opts &= ~TALLYMARKED_IPV4;
         break;

         case 'h':
         my_usage();
         tallymarked_free(cnf);
         return(0);

         case 'l':
         if (cnf->address != NULL)
            free(cnf->address);
         if ((cnf->address = strdup(optarg)) == NULL)
         {
            perror("strdup()");
            return(1);
         };
         break;

         case 'p':
         cnf->port = (uint32_t)strtol(optarg, NULL, 10);
         break;

         case 'V':
         my_version();
         tallymarked_free(cnf);
         return(0);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
         tallymarked_free(cnf);
         return(1);

         default:
         fprintf(stderr, "%s: unrecognized option `--%c'\n", argv[0], c);
         fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
         tallymarked_free(cnf);
         return(1);
      };
   };

   tallymarked_free(cnf);

   return(0);
}


void my_usage(void)
{
   printf("Usage: %s [OPTIONS]\n", PROGRAM_NAME);
   printf("Options:\n");
   printf("  -4                        listen on IPv4 addresses only\n");
   printf("  -6                        listen on IPv6 addresses only\n");
   printf("  -l address                listen on address (default: any\n");
   printf("  -p port                   listen on port\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -V, --version             print version number and exit\n");
   printf("  -v, --verbose             print verbose messages\n");
   printf("\n");
   return;
}


void my_version(void)
{
   printf(( "%s (%s) %s\n"
            "libtallymark (%s) %s\n"
            "%s\n"),
            PROGRAM_NAME, PACKAGE_NAME, PACKAGE_VERSION,
            PACKAGE_NAME, tallymark_pkg_version(NULL, NULL, NULL, NULL),
            PACKAGE_COPYRIGHT
   );
   return;
}


/* end of source */
