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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <time.h>

#include "conf.h"
#include "daemon.h"
#include "db.h"
#include "network.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

int main(int argc, char * argv[]);


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
   tallymarked_cnf    * cnf;
   int                  err;
   int                  exit_code;

   srand((unsigned)tallymark_seed());

   // initialize daemon configuration
   switch(tallymarked_init(&cnf, argc, argv))
   {
      case -1:
      return(-1);

      case 1:
      return(0);

      default:
      break;
   };

   openlog(cnf->prog_name, LOG_CONS|LOG_PERROR|LOG_PID, LOG_DAEMON);
   syslog(LOG_INFO, "starting %s %s", PACKAGE_NAME, PACKAGE_VERSION);

   // open network sockets
   if ((err = tallymarked_listen(cnf)) == -1)
   {
      tallymarked_destroy(cnf);
      return(1);
   };

   // start daemon functions
   switch((tallymarked_daemon_start(cnf)))
   {
      case -1:
      tallymarked_destroy(cnf);
      return(1);

      case 0:
      break;

      default:
      tallymarked_destroy(cnf);
      return(0);
   };

   // main loop
   exit_code = 0;
   while(tallymaked_sig_exit == 0)
   {
      // processes network requests
      if ((err = tallymarked_loop(cnf)) == -1)
      {
         tallymaked_sig_exit = 1;
         exit_code = 1;
      };
   };

   // clean up resources
   tallymarked_daemon_cleanup(cnf);
   tallymarked_destroy(cnf);

   return(exit_code);
}


/* end of source */
