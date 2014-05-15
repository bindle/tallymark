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
#include "message.h"

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
#include <stdlib.h>
#include <string.h>
#include <errno.h>


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

void tallymark_msg_destroy(tallymark_msg * msg)
{
   assert(msg != NULL);

   if (msg->buff.ptr != NULL)
      free(msg->buff.ptr);
   msg->buff.ptr = NULL;

   memset(msg, 0, sizeof(tallymark_msg));

   free(msg);

   return;
}


int tallymark_msg_init(tallymark * tally, tallymark_msg ** pmsg)
{
   tallymark_msg * msg;

   assert(tally != NULL);
   assert(pmsg  != NULL);

   if ((msg = malloc(sizeof(tallymark_msg))) == NULL)
      return(ENOMEM);
   memset(msg, 0, sizeof(tallymark_msg));

   msg->tally = tally;

   return(tallymark_msg_reset(msg));
}


int tallymark_msg_parse(tallymark_msg * msg)
{
   tallymark_hdr      * hdr;
   tallymark_buff       buff;
   size_t               len;
   uint32_t             param_len;
   uint32_t             param_id;

   assert(msg != NULL);

   hdr  = &msg->header;

   // updates message status
   if (msg->status != TALLYMARK_MSG_VALIDATED)
      return(msg->error = ECANCELED);

   // parses header
   hdr->response_codes  = msg->buff.u8[11];
   hdr->request_id      = ntohl(msg->buff.u32[3]);
   hdr->request_codes   = ntohl(msg->buff.u32[4]);
   hdr->service_id      = ntohl(msg->buff.u32[5]);
   hdr->field_id        = ntohl(msg->buff.u32[6]);
   memcpy(hdr->hash_id, &msg->buff.u8[56], 20);

   // parses body
   len = 0;
   while(len < hdr->body_len)
   {
      buff.u8 = &msg->buff.u8[hdr->header_len + len];

      // verify parameter length
      if ((len + 4) >= hdr->body_len)
      {
         msg->status = TALLYMARK_MSG_BAD;
         return(msg->error = EBADMSG);
      };

      // parses parameter length and ID
      param_len  = msg->buff.u8[0] * 4;
      param_id   = (((uint32_t)msg->buff.u8[1] & 0xff) << 16);
      param_id  |= (((uint32_t)msg->buff.u8[2] & 0xff) <<  8);
      param_id  |= (((uint32_t)msg->buff.u8[3] & 0xff) <<  0);

      // verify parameter's length
      if ((len + param_len) >= hdr->body_len)
      {
         msg->status = TALLYMARK_MSG_BAD;
         return(msg->error = EBADMSG);
      };

      // process parameter
      switch(param_id)
      {
         case TALLYMARK_FLD_SYS_CAPABILITIES:
         msg->body.capabilities = ntohl(buff.u32[1]);
         break;

         case TALLYMARK_FLD_SYS_PKG_NAME:
         if (msg->body.package_name != NULL)
            free(msg->body.package_name);
         if ((msg->body.package_name = malloc(param_len - 3)) == NULL)
            return(msg->error = ENOMEM);
         memcpy(msg->body.package_name, &msg->buff.u8[4], param_len - 4);
         msg->body.package_name[param_len-4] = '\0';
         break;

         case TALLYMARK_FLD_SYS_VERSION:
         if (msg->body.version != NULL)
            free(msg->body.version);
         if ((msg->body.version = malloc(param_len - 3)) == NULL)
            return(msg->error = ENOMEM);
         memcpy(msg->body.version, &msg->buff.u8[4], param_len - 4);
         msg->body.version[param_len-4] = '\0';
         break;

         default:
         break;
      };
   };

   msg->status = TALLYMARK_MSG_PARSED;

   return(0);
}


int tallymark_msg_prepare(tallymark_msg * msg, uint32_t request_id,
   uint32_t service_id, uint32_t field_id, const uint8_t * hash_id,
   size_t hash_len)
{
   int               err;
   size_t            size;
   tallymark_hdr   * hdr;
   tallymark_buff  * buff;

   assert(msg     != NULL);
   assert(hash_id != NULL);

   hdr  = &msg->header;
   buff = &msg->buff;

   if ((err = tallymark_msg_reset(msg)) != 0)
      return(msg->error = err);

   // set magic
   hdr->magic  = TALLYMARK_MAGIC;
   buff->u32[0] = htonl(hdr->magic);

   // set version
   hdr->version_current = TALLYMARK_PROTO_VERSION;
   hdr->version_age     = TALLYMARK_PROTO_AGE;
   buff->u8[4]          = hdr->version_current;
   buff->u8[5]          = hdr->version_age;

   // set header length
   hdr->header_len   = sizeof(tallymark_hdr);
   buff->u8[6]       = hdr->header_len;

   // set request_id
   hdr->request_id   = request_id;
   buff->u32[3]      = htonl(hdr->request_id);

   // set service_id
   hdr->service_id   = service_id;
   buff->u32[5]      = htonl(hdr->service_id);

   // set field_id
   hdr->field_id  = field_id;
   buff->u32[6]   = htonl(hdr->field_id);

   // set hash_id
   memset(&buff->u8[28], 0, sizeof(hdr->hash_id));
   size = (hash_len <= sizeof(hdr->hash_id)) ? hash_len : sizeof(hdr->hash_id);
   memcpy(hdr->hash_id,  hash_id, size);
   memcpy(&buff->u8[28], hash_id, size);

   msg->status = TALLYMARK_MSG_PREPARED;

   return(0);
}


int tallymark_msg_read(tallymark_msg * msg, int s,
   struct sockaddr * restrict address, socklen_t * restrict address_len)
{
   ssize_t         len;
   uint32_t        msg_len;
   uint32_t        u32;
   tallymark_hdr * hdr;
   void          * ptr;

   assert(msg != NULL);
   assert(s   != -1);

   if (msg->status != TALLYMARK_MSG_RESET)
      return(msg->error = EINVAL);

   msg->status = TALLYMARK_MSG_RECEIVING;
   hdr         = &msg->header;

   // allocates memory
   if (msg->buff.u8 == NULL)
   {
      if ((msg->buff.ptr = malloc(sizeof(tallymark_hdr))) == NULL)
         return(msg->error = ENOMEM);
      msg->buff_size = sizeof(tallymark_hdr);
   };

   // read header from socket
   len = recvfrom(s, &msg->buff, 8, MSG_PEEK, NULL, NULL);
   if (len == -1)
      return(msg->error = errno);
   if (len < 8)
   {
      recvfrom(s, &msg->buff, 8, 0, NULL, NULL);
      return(msg->error = EBADMSG);
   };

   // calculate message length and allocate memory
   hdr->header_len = msg->buff.u8[6];
   hdr->body_len   = msg->buff.u8[7];
   msg_len         = ((hdr->header_len + hdr->body_len) * 4);
   if (msg->buff_size < msg_len)
   {
      if ((ptr = realloc(msg->buff.u8, msg_len)) == NULL)
         return(msg->error = ENOMEM);
      msg->buff.ptr  = ptr;
      msg->buff_size = msg_len;
   };

   // read whole message from socket
   len = recvfrom(s, &msg->buff, msg_len, 0, address, address_len);
   if (len == -1)
      return(msg->error = errno);
   if (len != (ssize_t)msg_len)
      return(msg->error = EBADMSG);
   msg->msg_len = msg_len;

   // update message status
   msg->status = TALLYMARK_MSG_VALIDATING;

   // check message size
   if (len < (ssize_t)sizeof(tallymark_hdr))
      return(msg->error = EBADMSG);

   // validates header: magic
   if ((u32 = ntohl(msg->buff.u32[0])) != TALLYMARK_MAGIC)
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };
   msg->header.magic = u32;

   // validates header: protocol version
   hdr->version_current = msg->buff.u8[4];
   hdr->version_age     = msg->buff.u8[5];
   if (hdr->version_current < (TALLYMARK_PROTO_VERSION - TALLYMARK_PROTO_AGE))
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };
   if (TALLYMARK_PROTO_VERSION < (hdr->version_current - hdr->version_age))
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   msg->s      = s;
   msg->status = TALLYMARK_MSG_VALIDATED;

   return(0);
}


int tallymark_msg_recvfrom(int s, tallymark_msg * msg,
   struct sockaddr * restrict address, socklen_t * restrict address_len)
{
   int err;

   assert(s            != -1);
   assert(msg          != NULL);

   if ((err = tallymark_msg_reset(msg)) != 0)
      return(err);

   if ((err = tallymark_msg_read(msg, s, address, address_len)) != 0)
      return(err);

   return(tallymark_msg_parse(msg));
}


int tallymark_msg_reset(tallymark_msg * msg)
{
   assert(msg != NULL);

   // saves time if message is already reset
   if (msg->status == TALLYMARK_MSG_RESET)
      return(0);

   // resets message
   memset(&msg->header,    0, sizeof(tallymark_hdr));
   memset(&msg->body,      0, sizeof(tallymark_bdy));
   msg->status   = TALLYMARK_MSG_RESET;
   msg->s        = -1;
   msg->msg_len  = 0;

   return(0);
}


ssize_t tallymark_msg_sendto(int s, tallymark_msg * msg,
   const struct sockaddr * dest_addr, socklen_t dest_len)
{
   ssize_t len;

   assert(s         != -1);
   assert(msg       != NULL);

   if (msg->status != TALLYMARK_MSG_PREPARED)
   {
      msg->error = EINVAL;
      errno = msg->error;
      return(msg->error = EINVAL);
   };

   len = sendto(s, msg->buff.ptr, msg->msg_len, 0, dest_addr, dest_len);
   if (len == -1)
      msg->error = errno;

   return(len);
}


/* end of source */
