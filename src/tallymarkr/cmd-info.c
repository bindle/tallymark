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
#include "cmd-info.h"


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
#include <time.h>
#include <stdlib.h>

#include <tallymark.h>

#include "network.h"


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Prototypes
#endif

void tallymarker_server_caps(const char * str,
   uint32_t capabilities, uint32_t request);


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymarker_cmd_info(tallymarker_cnf * cnf)
{
   size_t                  len;
   uint32_t                caps;
   char                  * pkg_name;
   char                  * pkg_version;
   const tallymark_hdr   * hdr;

   pkg_name       = NULL;
   pkg_version    = NULL;
   caps   = 0;

   tallymark_msg_create_header(cnf->req, (uint32_t)rand(), cnf->service_id, cnf->field_id, cnf->hash_id, sizeof(cnf->hash_id));

   if (tallymarker_send(cnf, cnf->req, TALLYMARK_REQ_SYS_CAPABILITIES |TALLYMARK_REQ_SYS_VERSION) != 0)
      return(1);

   while (tallymarker_recv(cnf, cnf->res) == 0)
   {
      tallymark_msg_get_header(cnf->res, &hdr);

      if (pkg_name == NULL)
      {
         len = sizeof(pkg_name);
         tallymark_msg_get_param(cnf->res, TALLYMARK_PARM_SYS_PKG_NAME, &pkg_name, &len);
      };

      if (pkg_version == NULL)
      {
         len = sizeof(pkg_version);
         tallymark_msg_get_param(cnf->res, TALLYMARK_PARM_SYS_VERSION, &pkg_version, &len);
      };

      if (caps == 0)
      {
         len = sizeof(caps);
         tallymark_msg_get_param(cnf->res, TALLYMARK_PARM_SYS_CAPABILITIES, &caps, &len);
      };

      if ((hdr->response_codes & TALLYMARK_RES_EOR) != 0)
      {
         if ((pkg_name))
         {
            printf("Server Name:              %s\n", pkg_name);
            free(pkg_name);
         };
         if ((pkg_version))
         {
            printf("Server Version:           %s\n", pkg_version);
            free(pkg_version);
         };
         if (caps == 0)
            return(0);
         printf("Capabilities:\n");
         tallymarker_server_caps("List Hashes",          caps, TALLYMARK_REQ_HASH_QUERY);
         tallymarker_server_caps("Set Hash Text:",       caps, TALLYMARK_REQ_HASH_SET_TEXT);
         tallymarker_server_caps("Report Count:",        caps, TALLYMARK_REQ_HASH_COUNT);
         tallymarker_server_caps("Increment Count:",     caps, TALLYMARK_REQ_HASH_INCREMENT);
         tallymarker_server_caps("Threshold Query:",     caps, TALLYMARK_REQ_THRESHOLD_QUERY);
         tallymarker_server_caps("Report Version:",      caps, TALLYMARK_REQ_SYS_VERSION);
         tallymarker_server_caps("Report Capabilities:", caps, TALLYMARK_REQ_SYS_CAPABILITIES);
         return(0);
      };
   };

   return(1);
}


void tallymarker_server_caps(const char * str, uint32_t capabilities,
   uint32_t request)
{
   if ((capabilities & request) == 0)
   {
      printf("   %-23sno  (0x%08x)\n", str, request);
      return;
   };
   printf("   %-23syes (0x%08x)\n", str, request);
   return;
}

/* end of source */
