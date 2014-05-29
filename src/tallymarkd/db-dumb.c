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
#include "db.h"


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
#include <syslog.h>

#include <tallymark.h>


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Data Types
#endif

struct tallymarked_database_struct
{
   size_t               len;
   size_t               size;
   tallymarked_record * records;
};


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
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymarked_backend_destroy(tallymarked_db * db)
{
   if (db == NULL)
      return(0);

   free(db->records);

   bzero(db, sizeof(tallymarked_db));
   free(db);

   return(0);
}


int tallymarked_backend_init(tallymarked_cnf * cnf)
{
   tallymarked_db  * db;
   size_t            size;
   void            * ptr;

   assert(cnf != NULL);

   if ((db = malloc(sizeof(tallymarked_db))) == NULL)
      return(-1);
   bzero(db, sizeof(tallymarked_db));
   db->len     = 0;
   db->size    = 10 * 1024 * 1024;

   size = sizeof(tallymarked_record) * db->size;
   if ((ptr = realloc(db->records, size)) == NULL)
   {
      tallymarked_backend_destroy(db);
      return(-1);
   };
   bzero(ptr, size);
   db->records = ptr;

   cnf->db = db;

   return(0);
}


int tallymarked_backend_record(tallymarked_cnf * cnf, uint8_t srv,
   uint8_t fld, const uint8_t * hash, tallymarked_record ** prec)
{
   tallymarked_record * rec;
   size_t               x;
   tallymarked_db     * db;

   assert(cnf  != NULL);
   assert(prec != NULL);

   db    = cnf->db;
   *prec = NULL;

   for(x = 0; ((x < db->len) && (*prec == NULL)); x++)
   {
      rec = &db->records[x];
      if ( (rec->srv == srv) && (rec->fld == fld) && (!(memcmp(hash, rec->hid, 20))) )
         *prec = rec;
   };

   if (*prec == NULL)
   {
      if (db->len >= db->size)
         return(errno = ENOMEM);
      rec = &db->records[x];
      *prec = rec;
      rec->srv = srv;
      rec->fld = fld;
      memcpy(rec->hid, hash, sizeof(rec->hid));
      db->len++;
   };

   return(0);
}

/* end of source */
