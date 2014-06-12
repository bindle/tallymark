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
#include "daemon.h"


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
#include <syslog.h>
#include <signal.h>

#include <tallymark.h>


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

void tallymarked_signal_exit(int sig);


/////////////////
//             //
//  Variables  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Variables
#endif

int tallymaked_sig_exit = 0;


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

void tallymarked_signal_exit(int sig)
{
   tallymaked_sig_exit = 1;
   signal(sig, SIG_IGN);
   return;
}


int tallymarked_daemon_start(tallymarked_cnf * cnf)
{
   int      fd;
   int      pid;
   char     pidtmp[1024];
   FILE   * fs;

   assert(cnf != NULL);

   signal(SIGINT,    tallymarked_signal_exit);
   signal(SIGQUIT,   tallymarked_signal_exit);
   signal(SIGTERM,   tallymarked_signal_exit);
   signal(SIGHUP,    tallymarked_signal_exit);

   signal(SIGVTALRM, SIG_IGN);
   signal(SIGPIPE,   SIG_IGN);
   signal(SIGALRM,   SIG_IGN);
   signal(SIGUSR1,   SIG_IGN);
   signal(SIGUSR2,   SIG_IGN);

   // changes user and group
   if (cnf->gid != getgid())
   {
      syslog(LOG_DEBUG, "changing to gid \"%i\"", cnf->gid);
      if (setgid(cnf->gid) == -1)
      {
         syslog(LOG_ERR, "setgid(%i): %m", cnf->gid);
         return(-1);
      };
   };
   if (cnf->uid != getuid())
   {
      syslog(LOG_DEBUG, "changing to uid \"%i\"", cnf->uid);
      if (setuid(cnf->uid) == -1)
      {
         syslog(LOG_ERR, "setuid(%i): %m", cnf->uid);
         return(-1);
      };
   };

   // create PID file
   snprintf(pidtmp, 1024, "%s.XXXXXXXX", cnf->pidfile);
   if ((fd = mkstemp(pidtmp)) == -1)
   {
      syslog(LOG_ERR, "mkstemp(%s): %m", cnf->pidfile);
      return(-1);
   };
   if (link(pidtmp, cnf->pidfile))
   {
      syslog(LOG_ERR, "PID file exists, exiting to avoid conflicts");
      close(fd);
      unlink(pidtmp);
      return(-1);
   };
   unlink(pidtmp);

   // forks to background
   if (cnf->foreground != 1)
   {
      switch (pid = fork())
      {
         case -1:
         perror("fork()");
         close(fd);
         unlink(cnf->pidfile);
         return(-1);

         case 0:
         closelog();
         openlog(cnf->prog_name, LOG_CONS|LOG_PID, LOG_DAEMON);
         close(STDERR_FILENO);
         close(STDIN_FILENO);
         close(STDOUT_FILENO);
         syslog(LOG_NOTICE, "running in background");
         if (setsid() == -1)
         {
            syslog(LOG_ERR, "setsid(): %m");
            close(fd);
            unlink(cnf->pidfile);
            return(-1);
         };
         break;

         default:
         syslog(LOG_INFO, "switching from pid \"%i\" to pid \"%i\"", getpid(), pid);
         close(fd);
         return(pid);
      };
   }
   else
   {
      syslog(LOG_INFO, "running in foreground");
   };

   // write PID to file
   fs = fdopen(fd, "w");
   fprintf(fs, "%i\n", getpid());
   fclose(fs);

   return(0);
}


int tallymarked_daemon_cleanup(tallymarked_cnf * cnf)
{
   assert(cnf != NULL);

   unlink(cnf->pidfile);

   return(0);
}


/* end of source */
