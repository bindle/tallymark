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
   size_t                len;
   tallymarked_record ** records;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

int tallymarked_db_record_add(tallymarked_cnf * cnf, tallymarked_db * db,
   uint8_t srv, uint8_t fld, const tallymark_hash hash, tallymarked_record ** prec,
   size_t idx);

int tallymarked_db_record_find(tallymarked_cnf * cnf, tallymarked_db * db,
   uint8_t srv, uint8_t fld, const tallymark_hash hash, tallymarked_record ** prec,
   size_t * pidx);

   
/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymarked_db_destroy(tallymarked_db * db)
{
   size_t pos;

   if (db == NULL)
      return(0);

   for(pos = 0; pos < db->len; pos++)
      free(db->records[pos]);

   free(db->records);

   bzero(db, sizeof(tallymarked_db));
   free(db);

   return(0);
}


int tallymarked_db_init(tallymarked_cnf * cnf, tallymarked_db ** pdb)
{
   tallymarked_db  * db;

   assert(cnf != NULL);
   assert(pdb != NULL);

   if ((db = malloc(sizeof(tallymarked_db))) == NULL)
      return(-1);
   bzero(db, sizeof(tallymarked_db));
   db->len     = 0;

   *pdb = db;

   return(0);
}


int tallymarked_db_record(tallymarked_cnf * cnf, tallymarked_db * db,
   uint8_t srv, uint8_t fld, const tallymark_hash hash, tallymarked_record ** prec)
{
   int      err;
   size_t   idx;

   assert(cnf  != NULL);
   assert(db   != NULL);
   assert(prec != NULL);

   *prec = NULL;
   idx   = 0;

   if ((err = tallymarked_db_record_find(cnf, db, srv, fld, hash, prec, &idx)) != 0)
      return(err);

   if (*prec != NULL)
      return(0);

   if ((err = tallymarked_db_record_add(cnf, db, srv, fld, hash, prec, idx)) != 0)
      return(err);

   return(0);
}


int tallymarked_db_record_add(tallymarked_cnf * cnf, tallymarked_db * db,
   uint8_t srv, uint8_t fld, const tallymark_hash hash, tallymarked_record ** prec,
   size_t idx)
{
   size_t               size;
   size_t               pos;
   void               * ptr;
   tallymarked_record * rec;

   assert(cnf  != NULL);
   assert(db   != NULL);
   assert(prec != NULL);

   // increases size of pointer array
   size = sizeof(tallymarked_record *) * (db->len+1);
   if ((ptr = realloc(db->records, size)) == NULL)
      return(errno);
   db->records = ptr;

   // allocates memory for record
   if ((rec = malloc(sizeof(tallymarked_record))) == NULL)
      return(errno);
   bzero(rec, sizeof(tallymarked_record));
   rec->srv = srv;
   rec->fld = fld;
   memcpy(rec->hash, hash, sizeof(rec->hash));

   // inserts record into pointer array at idx
   for(pos = db->len; pos > idx; pos--)
      db->records[pos] = db->records[pos-1];
   db->records[idx] = rec;

   // update state
   db->len++;
   *prec = rec;

   return(0);
}


int tallymarked_db_record_find(tallymarked_cnf * cnf, tallymarked_db * db,
   uint8_t srv, uint8_t fld, const tallymark_hash hash, tallymarked_record ** prec,
   size_t * pidx)
{
   int                  res;
   size_t               idx;
   size_t               high;
   size_t               mid;
   size_t               low;
   size_t               pos;
   tallymarked_record * rec;

   assert(cnf  != NULL);
   assert(db   != NULL);
   assert(prec != NULL);

   *prec = NULL;
   if (pidx == NULL)
      pidx = &idx;

   if (db->len == 0)
   {
      *pidx = 0;
      return(0);
   };

   low   = 0;
   high  = db->len - 1;

   // searches records array
   while (low <= high)
   {
      // determines mid point
      mid  = (low + high) / 2;


      res = 0;
      rec = db->records[mid];

      // compares service
      if (srv < rec->srv)
         res = -1;
      else if (srv > rec->srv)
         res = 1;

      // compares fields
      if (res != 0)
      {
         if (fld < rec->fld)
            res = -1;
         else if (fld > rec->fld)
            res = 1;
      };

      // compares hash
      for(pos = 0; ((pos < 20) && (res == 0)); pos++)
      {
         if (hash[pos] < rec->hash[pos])
            res = -1;
         else if (hash[pos] > rec->hash[pos])
            res = 1;
      };

      // evaluates result
      if (res > 0)
      {
         if (mid == high)
         {
            *pidx = high+1;
            return(0);
         };
         low = mid + 1;
      }
      else if (res < 0)
      {
         if (mid == low)
         {
            *pidx = low;
            return(0);
         };
         high = mid - 1;
      }
      else
      {
         *prec = rec;
         *pidx = mid;
         return(0);
      };
   };

   *pidx = low;

   return(0);
}

/* end of source */
