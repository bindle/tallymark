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
#include "conf.h"


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

int tallymarked_getopt(tallymarked_cnf * cnf, int argc,
   char * const * argv, int * opt_index);
void tallymarked_usage(tallymarked_cnf * cnf);
void tallymarked_version(void);

   
/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymarked_getopt(tallymarked_cnf * cnf, int argc,
   char * const * argv, int * opt_index)
{
   int   c;

   static char   short_opt[] = "46hl:p:V";
   static struct option long_opt[] =
   {
      { "help",          no_argument, 0, 'h'},
      { "version",       no_argument, 0, 'V'},
      { NULL,            0,           0, 0  }
   };

   while((c = getopt_long(argc, argv, short_opt, long_opt, opt_index)) != -1)
   {
      switch(c)
      {
         case -1:	/* no more arguments */
         case 0:	/* long options toggles */
         break;


         case '4':
         cnf->family = PF_INET;
         break;

         case '6':
         cnf->family = PF_INET6;
         break;

         case 'h':
         tallymarked_usage(cnf);
         return(1);

         case 'l':
         cnf->urlstr = optarg;
         break;

         case 'V':
         tallymarked_version();
         return(1);

         case '?':
         fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
         return(-1);

         default:
         fprintf(stderr, "%s: unrecognized option `- %c'\n", cnf->prog_name, c);
         fprintf(stderr, "Try `%s --help' for more information.\n", cnf->prog_name);
         return(-1);
      };
   };

   return(0);
}


int tallymarked_init(tallymarked_cnf ** pcnf, int argc, char * argv[])
{
   const char      * prog_name;
   const char      * str;
   int               opt_index;
   int               err;

   assert(pcnf != NULL);
   assert(argv != NULL);

   // determines short program name
   prog_name = argv[0];
   if ((str = rindex(argv[0], '/')) != NULL)
   {
      str++;
      prog_name = str;
   };

   // allocates memory for config struct
   if ((*pcnf = malloc(sizeof(tallymarked_cnf))) == NULL)
   {
      fprintf(stderr, "%s: malloc(): %s\n", prog_name, strerror(errno));
      return(-1);
   };

   // sets initial values
   bzero(*pcnf, sizeof(tallymarked_cnf));
   (*pcnf)->family   = PF_UNSPEC;
   (*pcnf)->urlstr   = "tally://localhost/";
   (*pcnf)->s[0]     = -1;
   (*pcnf)->s[1]     = -1;

   // allocates memory for messages
   if ((err = tallymark_msg_alloc(&(*pcnf)->req)) != 0)
   {
      tallymarked_destroy(*pcnf);
      errno = err;
      return(-1);
   };
   if ((err = tallymark_msg_alloc(&(*pcnf)->res)) != 0)
   {
      tallymarked_destroy(*pcnf);
      errno = err;
      return(-1);
   };

   // parses global CLI options
   if ((err = tallymarked_getopt(*pcnf, argc, argv, &opt_index)) != 0)
   {
      tallymarked_destroy(*pcnf);
      return(err);
   };

   return(0);
}


void tallymarked_destroy(tallymarked_cnf * cnf)
{
   assert(cnf != NULL);

   if (cnf->s[0] != -1)
      close(cnf->s[0]);
   if (cnf->s[1] != -1)
      close(cnf->s[1]);
   cnf->s[0] = -1;
   cnf->s[1] = -1;

   return;
}


void tallymarked_usage(tallymarked_cnf * cnf)
{
   printf("Usage: %s [OPTIONS]\n", cnf->prog_name);
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


void tallymarked_version(void)
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
