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
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <uuid/uuid.h>

#include <tallymark.h>

#include "db.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

int tallymarked_defaults(tallymarked_cnf * cnf);
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

int tallymarked_defaults(tallymarked_cnf * cnf)
{
   assert(cnf != NULL);

   if (cnf->pidfile == NULL)
      cnf->pidfile = "/var/run/tallymarked/tallymarked.pid";

   if (cnf->urlstr == NULL)
      cnf->urlstr = "tally://localhost/";

   if (cnf->foreground == 0)
      cnf->foreground = -1;

   return(0);
}


int tallymarked_getopt(tallymarked_cnf * cnf, int argc,
   char * const * argv, int * opt_index)
{
   int               c;
   struct passwd   * pw;
   struct group    * gr;

   static char   short_opt[] = "46fG:hl:P:U:V";
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

         case 'f':
         cnf->foreground = 1;
         break;

         case 'G':
         if ((gr = getgrnam(optarg)) == NULL)
         {
            fprintf(stderr, "%s: invalid group \"%s\"", cnf->prog_name, optarg);
            return(-1);
         };
         cnf->gid = gr->gr_gid;;
         break;

         case 'h':
         tallymarked_usage(cnf);
         return(1);

         case 'l':
         cnf->urlstr = optarg;
         break;

         case 'P':
         cnf->pidfile = optarg;
         break;

         case 'U':
         if ((pw = getpwnam(optarg)) == NULL)
         {
            fprintf(stderr, "%s: invalid user \"%s\"", cnf->prog_name, optarg);
            return(-1);
         };
         cnf->uid = pw->pw_uid;
         if (cnf->gid == getgid())
            cnf->gid = pw->pw_gid;
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
   (*pcnf)->fds[0].fd     = -1;
   (*pcnf)->fds[1].fd     = -1;
   (*pcnf)->uid         = getuid();
   (*pcnf)->gid         = getgid();

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

   // allocates memory for backend
   if ((err = tallymarked_db_init(*pcnf, &(*pcnf)->db)) != 0)
   {
      tallymarked_destroy(*pcnf);
      return(-1);
   };

   // parses global CLI options
   if ((err = tallymarked_getopt(*pcnf, argc, argv, &opt_index)) != 0)
   {
      tallymarked_destroy(*pcnf);
      return(err);
   };

   // applies default options
   if ((err = tallymarked_defaults(*pcnf)) != 0)
   {
      tallymarked_destroy(*pcnf);
      return(err);
   };

   return(0);
}


void tallymarked_destroy(tallymarked_cnf * cnf)
{
   if (cnf == NULL)
      return;

   tallymarked_db_destroy(cnf->db);
   tallymark_url_free(cnf->tudp);
   tallymark_msg_free(cnf->req);
   tallymark_msg_free(cnf->res);

   // closes sockets
   close(cnf->fds[0].fd);

   free(cnf);

   return;
}


void tallymarked_usage(tallymarked_cnf * cnf)
{
   printf("Usage: %s [OPTIONS]\n", cnf->prog_name);
   printf("Options:\n");
   printf("  -4                        listen on IPv4 addresses only\n");
   printf("  -6                        listen on IPv6 addresses only\n");
   printf("  -f                        run in foreground\n");
   printf("  -G group                  group to run process\n");
   printf("  -l address                listen on address (default: any\n");
   printf("  -P file                   PID file\n");
   printf("  -h, --help                print this help and exit\n");
   printf("  -U user                   user to run process\n");
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
