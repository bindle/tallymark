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
#include "fdpoller.h"

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
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "message.h"
#include "thread.h"


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Definitions
#endif


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Data Types
#endif

struct libtallymark_fdpoll_struct
{
   int               mutex_init;
   pthread_mutex_t   mutex;

   struct pollfd   * fds;
   nfds_t            nfds;
   nfds_t            last_polled;
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymark_add_fd(tallymark * tmd, int fd)
{
   size_t   size;
   int      err;
   void   * ptr;

   assert(tmd != NULL);
   assert(fd  != -1);

   if ((err = pthread_mutex_lock(&tmd->poller->mutex)) != 0)
      return(err);

   size = sizeof(struct pollfd) * (tmd->poller->nfds+1);
   if ((ptr = realloc(tmd->poller->fds, size)) == NULL)
   {
      pthread_mutex_unlock(&tmd->poller->mutex);
      return(errno);
   };
   tmd->poller->fds = ptr;
   memset(&tmd->poller->fds[tmd->poller->nfds], 0, sizeof(struct pollfd));
   tmd->poller->fds[tmd->poller->nfds].fd      = fd;
   tmd->poller->fds[tmd->poller->nfds].events  = POLLIN|POLLRDNORM;
   tmd->poller->nfds++;

   pthread_mutex_unlock(&tmd->poller->mutex);

   return(0);
}


int tallymark_del_fd(tallymark * tmd, int fd)
{
   nfds_t   nfds;
   int      err;

   assert(tmd != NULL);
   assert(fd  != -1);

   if ((err = pthread_mutex_lock(&tmd->poller->mutex)) != 0)
      return(err);

   for(nfds = 0; (nfds < tmd->poller->nfds) && (tmd->poller->fds[nfds].fd != fd); nfds++);
   if (nfds != tmd->poller->nfds)
      tmd->poller->nfds--;
   for(nfds += 1; nfds < tmd->poller->nfds; nfds++)
      memcpy(&tmd->poller->fds[nfds-1], &tmd->poller->fds[nfds], sizeof(struct pollfd));

   pthread_mutex_unlock(&tmd->poller->mutex);

   return(0);
}


int tallymark_poll(tallymark * tmd, int timeout, tallymark_msg * msg,
   struct sockaddr * address, socklen_t * address_len)
{
   int                  err;
   int                  rc;
   struct pollfd      * fds;
   tallymark_fdpoll   * plr;
   nfds_t               nfds;
   nfds_t               changed;
   nfds_t               pos;
   struct timespec      abs_timeout;
   struct timeval       start;

   assert(tmd != NULL);
   assert(msg != NULL);


   plr   = tmd->poller;
   fds   = tmd->poller->fds;
   nfds  = tmd->poller->nfds;

   // reset/clear message memory to avoid confusion
   tallymark_msg_reset(msg);

   // used to calulate time used to acquire mutex lock
   gettimeofday(&start, NULL);

   // acquire mutex lock
   if (timeout == 0)
   {
      if ((err = pthread_mutex_lock(&tmd->poller->mutex)) != 0)
         return(err);
   }
   else
   {
      abs_timeout.tv_nsec = timeout;
      abs_timeout.tv_sec  = 0;
      if ((err = pthread_mutex_timedlock(&tmd->poller->mutex, &abs_timeout)) != 0)
         return(err);
   };

   // return cleanly if no file descriptors are set to be polled
   if (plr->nfds < 1)
   {
      pthread_mutex_unlock(&tmd->poller->mutex);
      return(0);
   };

   // poll for changes and return if error is encountered
   if ((rc = poll(fds, nfds, timeout * 0)) == -1)
   {
      pthread_mutex_unlock(&tmd->poller->mutex);
      return(errno);
   };

   // verify changes were found before timeout
   if (rc == 0)
   {
      pthread_mutex_unlock(&tmd->poller->mutex);
      return(0);
   };

   // loops through connections looking for changes
   changed = nfds+1;
   for(pos = plr->last_polled; (pos < nfds) && (changed > nfds); pos++)
      if ((fds[pos].revents | POLLIN|POLLRDNORM) == 0)
         changed = pos;
   for(pos = 0; (pos < plr->last_polled) && (changed > nfds); pos++)
      if ((fds[pos].revents | POLLIN|POLLRDNORM) != 0)
         changed = pos;
   plr->last_polled = changed;

   // read message into buffer
   if ((err = tallymark_msg_read(msg, fds[changed].fd, address, address_len)) != 0)
   {
      err = errno;
      pthread_mutex_unlock(&tmd->poller->mutex);
      return(err);
   };

   pthread_mutex_unlock(&tmd->poller->mutex);

   // parses message
   if ((err = tallymark_msg_parse(msg)) != 0)
      return(err);

   return(0);
}


int tallymark_fdpoll_alloc(tallymark * tmd, tallymark_fdpoll ** ppoller)
{
   int                err;
   size_t             size;
   tallymark_fdpoll * poller;

   // allocate and initialize memory for FD polling wrapper
   size = sizeof(tallymark_fdpoll);
   if ((poller = malloc(size)) == NULL)
      return(errno);
   memset(poller, 0, size);

   // initializes mutex lock
   if ((err = pthread_mutex_init(&poller->mutex, &tmd->mutexattr)) != 0)
   {
      tallymark_fdpoll_free(poller);
      return(err);
   };

   // allocate and initialize memory for poll() file descriptors
   size = sizeof(struct pollfd);
   if ((poller->fds = malloc(size)) == NULL)
   {
      tallymark_fdpoll_free(poller);
      return(errno);
   };
   memset(poller->fds, 0, size);

   // save allocated memory and exit
   *ppoller = poller;
   return(0);
}


void tallymark_fdpoll_free(tallymark_fdpoll * poller)
{
   nfds_t nfds;

   if (poller == NULL)
      return;

   // destroys poller's mutex
   pthread_mutex_destroy(&poller->mutex);

   // closes sockets
   for(nfds = 0; nfds < poller->nfds; nfds++)
      close(poller->fds[nfds].fd);

   // frees memory from tallymark_poll->fds
   free(poller->fds);

   // frees polling struct
   bzero(poller, sizeof(tallymark_fdpoll));
   free(poller);

   return;
}


/* end of source */
