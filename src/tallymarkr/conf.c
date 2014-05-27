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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>

#include "cmd-count.h"
#include "cmd-debugger.h"
#include "cmd-help.h"
#include "cmd-increment.h"
#include "cmd-info.h"


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
//  Variables  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Variables
#endif

const tallymarker_cmd tallymarker_cmdmap[] =
{
   {
      "checkpoint",                                   // command name
      NULL,                                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "force the server to create a checkpoint"       // command description
   },
   {
      "count",                                        // command name
      tallymarker_cmd_count,                          // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "return the current count of a hash value"      // command description
   },
   {
      "debugger",                                     // command name
      tallymarker_cmd_debugger,                       // entry function
      "abcdef:ghijk:lmnopqr:s:t:uvwxyz"
      "ABCDEFGH:IJK:LMNOPQRSTUVWXYZ"
      "0123456789",                                   // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "debugging tool used by package maintainers"    // command description
   },
   {
      "dump",                                         // command name
      NULL,                                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "dump the server's database"                    // command description
   },
   {
      "field-list",                                   // command name
      NULL,                                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "display the list of fields for a service"      // command description
   },
   {
      "hash",                                         // command name
      NULL,                                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "display the hash value of a string"            // command description
   },
   {
      "help",                                         // command name
      tallymarker_cmd_help,                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "display this message"                          // command description
   },
   {
      "increment",                                    // command name
      tallymarker_cmd_increment,                      // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "increment the count for a hash value"          // command description
   },
   {
      "information",                                  // command name
      tallymarker_cmd_info,                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "display information about server"              // command description
   },
   {
      "record",                                       // command name
      NULL,                                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "display the full record for a hash value"      // command description
   },
   {
      "reset",                                        // command name
      NULL,                                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "reset the count and threshold for a hash"      // command description
   },
   {
      "service-list",                                 // command name
      NULL,                                           // entry function
      TALLYMARKER_GETOPT_SHORT,                       // getopt short options
      0,                                              // number of required arguments
      "",                                             // cli usage
      "display the list of services"                  // command description
   },
   { NULL, NULL, NULL, 0, NULL, NULL }
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymaker_destroy(tallymarker_cnf * cnf)
{
   assert(cnf != NULL);

   if (cnf->tudp != NULL)
      tallymark_url_free(cnf->tudp);
   cnf->tudp = NULL;

   if (cnf->req != NULL)
      tallymark_msg_free(cnf->req);
   cnf->req = NULL;

   if (cnf->res != NULL)
      tallymark_msg_free(cnf->res);
   cnf->res = NULL;

   if (cnf->s != -1)
      close(cnf->s);
   cnf->s = -1;

   bzero(cnf, sizeof(tallymarker_cnf));
   free(cnf);

   return(0);
}


int tallymarker_getopt(tallymarker_cnf * cnf, int argc,
   char * const * argv, const char * short_opt,
   const struct option * long_opt, int * opt_index)
{
   int            c;

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

         case 'H':
         cnf->urlstr = optarg;
         break;

         case 'h':
         tallymarker_usage(cnf);
         return(1);

         case 'q':
         cnf->quiet++;
         break;

         case 'r':
         cnf->request_codes = (uint32_t)strtoll(optarg, NULL, 0);
         break;

         case 't':
         cnf->timeout = (int)strtoll(optarg, NULL, 0);
         break;

         case 'V':
         tallymarker_version();
         return(1);

         case 'v':
         cnf->verbose++;
         break;

         case 'x':
         cnf->hexdump = 1;
         break;

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


int tallymaker_init(tallymarker_cnf ** pcnf, int argc, char * argv[])
{
   const char   * str;
   int            opt_index;
   int            err;
   const char   * cmd_name;
   size_t         cmd_len;
   int            i;

   static char   short_opt[] = "+" TALLYMARKER_GETOPT_SHORT;
   static struct option long_opt[] = { TALLYMARKER_GETOPT_LONG };

   assert(pcnf != NULL);

   // allocates memory for config struct
   if ((*pcnf = malloc(sizeof(tallymarker_cnf))) == NULL)
   {
      fprintf(stderr, "%s: malloc(): %s\n", (*pcnf)->prog_name, strerror(errno));
      return(-1);
   };

   // sets initial values
   bzero(*pcnf, sizeof(tallymarker_cnf));
   (*pcnf)->s        = -1;
   (*pcnf)->timeout  = 10000;

   // allocates memory for messages
   if ((err = tallymark_msg_alloc(&(*pcnf)->res)) != 0)
   {
      fprintf(stderr, "tallymark_msg_alloc(): %s\n", tallymark_strerror(err));
      return(-1);
   };
   if ((err = tallymark_msg_alloc(&(*pcnf)->req)) != 0)
   {
      fprintf(stderr, "tallymark_msg_alloc(): %s\n", tallymark_strerror(err));
      return(-1);
   };

   // determines short program name
   (*pcnf)->prog_name = argv[0];
   if ((str = rindex(argv[0], '/')) != NULL)
   {
      str++;
      (*pcnf)->prog_name = str;
   };

   // sets defaults
   (*pcnf)->urlstr = "tally://localhost/";
   (*pcnf)->family = PF_UNSPEC;

   // parses global CLI options
   if ((err = tallymarker_getopt(*pcnf, argc, argv, short_opt, long_opt, &opt_index)) != 0)
   {
      tallymaker_destroy(*pcnf);
      return(err);
   };

   // verifies a command was specified on the CLI
   if ((argc - optind) < 1)
   {
      fprintf(stderr, "%s: missing command\n", (*pcnf)->prog_name);
      fprintf(stderr, "Try `%s --help' for more information.\n", (*pcnf)->prog_name);
      tallymaker_destroy(*pcnf);
      return(-1);
   };

   // determines which command was specified
   cmd_name = argv[optind];
   cmd_len  = strlen(cmd_name);
   for(i = 0; (tallymarker_cmdmap[i].cmd_name != NULL); i++)
   {
      if ((strncmp(cmd_name, tallymarker_cmdmap[i].cmd_name, cmd_len)))
         continue;

      if (((*pcnf)->cmd))
      {
         fprintf(stderr, "%s: ambiguous command -- \"%s\"\n", (*pcnf)->prog_name, cmd_name);
         fprintf(stderr, "Try `%s --help' for more information.\n", (*pcnf)->prog_name);
         return(-1);
      };

      (*pcnf)->cmd = &tallymarker_cmdmap[i];
   };
   if (!((*pcnf)->cmd))
   {
      fprintf(stderr, "%s: unknown command -- \"%s\"\n", (*pcnf)->prog_name, cmd_name);
      fprintf(stderr, "Try `%s --help' for more information.\n", (*pcnf)->prog_name);
      tallymaker_destroy(*pcnf);
      return(-1);
   };
   if (!((*pcnf)->cmd->cmd_func))
   {
      fprintf(stderr, "%s: command not implemented -- \"%s\"\n", (*pcnf)->prog_name, (*pcnf)->cmd->cmd_name);
      fprintf(stderr, "Try `%s --help' for more information.\n", (*pcnf)->prog_name);
      tallymaker_destroy(*pcnf);
      return(-1);
   };

   // parses command specific CLI options
   if ((err = tallymarker_getopt(*pcnf, argc, argv, (*pcnf)->cmd->cmd_shortopts, long_opt, &opt_index)) != 0)
   {
      tallymaker_destroy(*pcnf);
      return(-1);
   };

   return(0);
}


void tallymarker_usage(tallymarker_cnf * cnf)
{
   int          i;
   const char * cmd_name;
   const char * cmd_help;
   const char * shortopts;

   cmd_name    = "COMMAND";
   cmd_help    = " ...";
   shortopts   = TALLYMARKER_GETOPT_SHORT;

   if ((cnf->cmd))
   {
      shortopts = ((cnf->cmd->cmd_shortopts)) ? cnf->cmd->cmd_shortopts : TALLYMARKER_GETOPT_SHORT;
      cmd_name  = cnf->cmd->cmd_name;
      cmd_help  = ((cnf->cmd->cmd_help)) ? cnf->cmd->cmd_help : "";
   };

   printf("Usage: %s %s [OPTIONS]%s\n", PROGRAM_NAME, cmd_name, cmd_help);
   printf("OPTIONS:\n");
   if ((strchr(shortopts, '4'))) printf("  -4                        force use of IPv4 addresses only\n");
   if ((strchr(shortopts, '6'))) printf("  -6                        force use of IPv6 addresses only\n");
   if ((strchr(shortopts, 'f'))) printf("  -f num                    numeric ID of field type\n");
   if ((strchr(shortopts, 'H'))) printf("  -H uri                    URI of server\n");
   if ((strchr(shortopts, 'h'))) printf("  -h, --help                print this help and exit\n");
   if ((strchr(shortopts, 'K'))) printf("  -K string                 unhashed string of record\n");
   if ((strchr(shortopts, 'k'))) printf("  -k hash                   SHA1 hash of value\n");
   if ((strchr(shortopts, 'q'))) printf("  -q, --quiet, --silent     do not print messages\n");
   if ((strchr(shortopts, 'r'))) printf("  -r code                   numeric request code to send to server\n");
   if ((strchr(shortopts, 's'))) printf("  -s num                    numeric ID of service type\n");
   if ((strchr(shortopts, 't'))) printf("  -t sec                    operation timeout in seconds\n");
   if ((strchr(shortopts, 'V'))) printf("  -V, --version             print version number and exit\n");
   if ((strchr(shortopts, 'v'))) printf("  -v, --verbose             print verbose messages\n");
   if ((strchr(shortopts, 'x'))) printf("  -x                        display raw messages\n");
   if (cnf->cmd == NULL)
   {
      printf("COMMANDS:\n");
      for(i = 0; tallymarker_cmdmap[i].cmd_name != NULL; i++)
         if ( ((tallymarker_cmdmap[i].cmd_desc)) && ((tallymarker_cmdmap[i].cmd_func)) )
            printf("  %-25s %s\n", tallymarker_cmdmap[i].cmd_name, tallymarker_cmdmap[i].cmd_desc);
   };
   printf("\n");
   return;
}


void tallymarker_version(void)
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
