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
#include <stddef.h>
#include <arpa/inet.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Macros
#endif

#define TM_HDR_ERRORS tallymark_msg_validate_header_definition()


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymark_msg_alloc(tallymark * tally, tallymark_msg ** pmsg)
{
   tallymark_msg * msg;

   assert(tally         != NULL);
   assert(pmsg          != NULL);
   assert(TM_HDR_ERRORS == 0);

   if ((msg = malloc(sizeof(tallymark_msg))) == NULL)
      return(ENOMEM);
   memset(msg, 0, sizeof(tallymark_msg));

   msg->tally = tally;

   return(tallymark_msg_reset(msg));
}


int tallymark_msg_compile(tallymark_msg * msg)
{
   tallymark_hdr   * hdr;
   tallymark_hdr   * buf;

   assert(msg           != NULL);
   assert(TM_HDR_ERRORS == 0);

   hdr  = &msg->header;
   buf  = &msg->buff.hdr;

   // update message status
   msg->status &= ~TALLYMARK_MSG_COMPILED;
   if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      return(msg->error = EINVAL);
   if ((hdr->response_codes == 0) && (hdr->request_codes == 0))
      return(msg->error = EINVAL);

   // updates header
   hdr->body_len           = 0;

   // compiles header
   buf->magic              = htonl(hdr->magic);
   buf->version_current    = hdr->version_current;
   buf->version_age        = hdr->version_age;
   buf->header_len         = hdr->header_len;
   buf->body_len           = hdr->body_len;
   buf->response_codes     = hdr->response_codes;
   buf->request_id         = htonl(hdr->request_id);
   buf->request_codes      = htonl(hdr->request_codes);
   buf->service_id         = htonl(hdr->service_id);
   buf->field_id           = htonl(hdr->field_id);
   memcpy(buf->hash_id, hdr->hash_id, sizeof(hdr->hash_id));

   msg->status |= TALLYMARK_MSG_COMPILED;

   return(0);
}


int tallymark_msg_create_header(tallymark_msg * msg,
   uint32_t req_id, uint32_t srv_id, uint32_t fld_id,
   const uint8_t * hash, size_t hash_len)
{
   int               err;
   tallymark_hdr   * hdr;

   assert(msg           != NULL);
   assert(hash          != NULL);
   assert(hash_len      >  0);
   assert(TM_HDR_ERRORS == 0);

   hdr  = &msg->header;

   if ((err = tallymark_msg_reset(msg)) != 0)
      return(err);

   // apply header
   hdr->magic              = TALLYMARK_MAGIC;
   hdr->version_current    = TALLYMARK_PROTO_VERSION;
   hdr->version_age        = TALLYMARK_PROTO_AGE;
   hdr->header_len         = TM_HDR_LENGTH;
   hdr->body_len           = 0;
   hdr->request_id         = req_id;
   hdr->service_id         = srv_id;
   hdr->field_id           = fld_id;
   hash_len = (hash_len > sizeof(hdr->hash_id)) ? sizeof(hdr->hash_id) : hash_len;
   memset(hdr->hash_id, 0, sizeof(hdr->hash_id));
   memcpy(hdr->hash_id, hash, hash_len);

   msg->status = TALLYMARK_MSG_PARSED;

   return(0);
}


void tallymark_msg_free(tallymark_msg * msg)
{
   assert(msg           != NULL);
   assert(TM_HDR_ERRORS == 0);

   if (msg->body.version != NULL)
      free(msg->body.version);
   if (msg->body.package_name != NULL)
      free(msg->body.package_name);

   memset(msg, 0, sizeof(tallymark_msg));

   free(msg);

   return;
}


int tallymark_msg_parse(tallymark_msg * msg)
{
   tallymark_hdr      * hdr;
   tallymark_hdr      * buf;
   size_t               off;
   uint32_t             param_len;
   uint32_t             param_id;

   assert(msg           != NULL);
   assert(TM_HDR_ERRORS == 0);

   hdr  = &msg->header;
   buf  = &msg->buff.hdr;

   // updates message status
   if ((msg->status & TALLYMARK_MSG_COMPILED) == 0)
      return(msg->error = ECANCELED);
   msg->status &= ~TALLYMARK_MSG_PARSED;

   // parses first part of header
   hdr->magic              = ntohl(buf->magic);
   hdr->version_current    = buf->version_current;
   hdr->version_age        = buf->version_age;
   hdr->header_len         = buf->header_len;
   hdr->body_len           = buf->body_len;

   // validates header magic number
   if (hdr->magic != TALLYMARK_MAGIC)
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // If current version number is greater than 127, assume the protocol
   // version switched to a data type larger than 8 bits.
   if (hdr->version_current > 127)
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // Verify the protocol version implemented by the remote side is supported
   // by the local library
   if (hdr->version_current < (TALLYMARK_PROTO_VERSION - TALLYMARK_PROTO_AGE))
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // Verify the protocol version implemented by the local library is
   // supported by the remote side
   if (TALLYMARK_PROTO_VERSION < (hdr->version_current - hdr->version_age))
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // Verify the header's claimed message size matched the amount of data
   // received.
   if (msg->msg_len != ((hdr->header_len * hdr->body_len) * 4))
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // parses last part of header
   hdr->response_codes     = buf->response_codes;
   hdr->request_id         = ntohl(buf->request_id);
   hdr->request_codes      = ntohl(buf->request_codes);
   hdr->service_id         = ntohl(buf->service_id);
   hdr->field_id           = ntohl(buf->field_id);
   memcpy(hdr->hash_id, &msg->buff.u8[TM_HDR_OFF_HASH_ID], TM_HDR_LEN_HASH_ID);

   // parses body
   off = TM_BDY_OFF;
   while(off < msg->msg_len)
   {
      // verify message is long enough to contain a parameter header
      if ((off + 4) >= msg->msg_len)
      {
         msg->status = TALLYMARK_MSG_BAD;
         return(msg->error = EBADMSG);
      };

      // parses parameter length and ID
      param_len  = msg->buff.u8[off] * 4;
      param_id   = (((uint32_t)msg->buff.u8[1] & 0xff) << 16);
      param_id  |= (((uint32_t)msg->buff.u8[2] & 0xff) <<  8);
      param_id  |= (((uint32_t)msg->buff.u8[3] & 0xff) <<  0);

      // verify message is long enough to contain the specified parameter
      if ((off + param_len) >= msg->msg_len)
      {
         msg->status = TALLYMARK_MSG_BAD;
         return(msg->error = EBADMSG);
      };

      // adjust offset and parameter lengths
      off       += 4;
      param_len -= 4;

      // process parameter
      switch(param_id)
      {
         case TALLYMARK_FLD_SYS_CAPABILITIES:
         msg->body.capabilities   = (((uint32_t)msg->buff.u8[0] & 0xff) << 24);
         msg->body.capabilities  |= (((uint32_t)msg->buff.u8[1] & 0xff) << 16);
         msg->body.capabilities  |= (((uint32_t)msg->buff.u8[2] & 0xff) <<  8);
         msg->body.capabilities  |= (((uint32_t)msg->buff.u8[3] & 0xff) <<  0);
         break;

         case TALLYMARK_FLD_SYS_PKG_NAME:
         if (msg->body.package_name != NULL)
            free(msg->body.package_name);
         if ((msg->body.package_name = malloc(param_len+1)) == NULL)
            return(msg->error = ENOMEM);
         memcpy(msg->body.package_name, &msg->buff.u8[0], param_len);
         msg->body.package_name[param_len] = '\0';
         break;

         case TALLYMARK_FLD_SYS_VERSION:
         if (msg->body.version != NULL)
            free(msg->body.version);
         if ((msg->body.version = malloc(param_len+1)) == NULL)
            return(msg->error = ENOMEM);
         memcpy(msg->body.version, &msg->buff.u8[0], param_len);
         msg->body.version[param_len] = '\0';
         break;

         default:
         break;
      };

      off += param_len;
   };

   msg->status |= TALLYMARK_MSG_PARSED;

   return(0);
}


int tallymark_msg_read(tallymark_msg * msg, int s,
   struct sockaddr * address, socklen_t * address_len)
{
   ssize_t         len;

   assert(msg           != NULL);
   assert(s             != -1);
   assert(TM_HDR_ERRORS == 0);

   if (msg->status != TALLYMARK_MSG_RESET)
      return(msg->error = EINVAL);

   // read header from socket
   len = recvfrom(s, &msg->buff, TM_MSG_MAX_SIZE, 0, address, address_len);
   if (len == -1)
      return(msg->error = errno);
   if (len < 8)
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // update message state
   msg->status    = TALLYMARK_MSG_COMPILED;
   msg->msg_len   = (size_t)len;
   msg->s         = s;

   return(0);
}


int tallymark_msg_recvfrom(int s, tallymark_msg * msg,
   struct sockaddr * address, socklen_t * address_len)
{
   int err;

   assert(s             != -1);
   assert(msg           != NULL);
   assert(TM_HDR_ERRORS == 0);

   if ((err = tallymark_msg_reset(msg)) != 0)
      return(err);

   if ((err = tallymark_msg_read(msg, s, address, address_len)) != 0)
      return(err);

   return(tallymark_msg_parse(msg));
}


int tallymark_msg_reset(tallymark_msg * msg)
{
   assert(msg           != NULL);
   assert(TM_HDR_ERRORS == 0);

   // saves time if message is already reset
   if (msg->status == TALLYMARK_MSG_RESET)
      return(0);

   // resets message
   memset(&msg->header, 0, sizeof(tallymark_hdr));
   msg->status   = TALLYMARK_MSG_RESET;
   msg->s        = -1;
   msg->msg_len  = 0;

   // resets static header information
   msg->header.magic               = TALLYMARK_MAGIC;
   msg->header.version_current     = TALLYMARK_PROTO_VERSION;
   msg->header.version_age         = TALLYMARK_PROTO_AGE;
   msg->header.header_len          = TM_HDR_LENGTH;

   // resets body
   msg->body.capabilities           = 0;
   msg->body.version[0]             = '\0';
   msg->body.version_size           = 0;
   msg->body.package_name[0]        = '\0';
   msg->body.package_name_size      = 0;

   return(0);
}


ssize_t tallymark_msg_sendto(int s, tallymark_msg * msg,
   const struct sockaddr * dest_addr, socklen_t dest_len)
{
   int     err;
   ssize_t len;

   assert(s             != -1);
   assert(msg           != NULL);
   assert(TM_HDR_ERRORS == 0);

   if ((msg->status & TALLYMARK_MSG_COMPILED) == 0)
   {
      if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      {
         msg->error = EINVAL;
         errno = msg->error;
         return(msg->error = EINVAL);
      };
      if ((err = tallymark_msg_compile(msg)) != 0)
         return(err);
   };

   len = sendto(s, msg->buff.u8, msg->msg_len, 0, dest_addr, dest_len);
   if (len == -1)
      msg->error = errno;

   return(len);
}


int tallymark_msg_validate_header_definition(void)
{
   int           i;
   tallymark_hdr hdr;

   i = 0;

   i += (sizeof(tallymark_hdr)                     != TM_HDR_LENGTH);

   i += (offsetof(tallymark_hdr, magic)            != TM_HDR_OFF_MAGIC);
   i += (offsetof(tallymark_hdr, version_current)  != TM_HDR_OFF_VERSION_CURRENT);
   i += (offsetof(tallymark_hdr, version_age)      != TM_HDR_OFF_VERSION_AGE);
   i += (offsetof(tallymark_hdr, header_len)       != TM_HDR_OFF_HEADER_LEN);
   i += (offsetof(tallymark_hdr, body_len)         != TM_HDR_OFF_BODY_LEN);
   i += (offsetof(tallymark_hdr, reserved)         != TM_HDR_OFF_RESERVED);
   i += (offsetof(tallymark_hdr, response_codes)   != TM_HDR_OFF_RESPONSE_CODES);
   i += (offsetof(tallymark_hdr, request_id)       != TM_HDR_OFF_REQUEST_ID);
   i += (offsetof(tallymark_hdr, request_codes)    != TM_HDR_OFF_REQUEST_CODES);
   i += (offsetof(tallymark_hdr, service_id)       != TM_HDR_OFF_SERVICE_ID);
   i += (offsetof(tallymark_hdr, field_id)         != TM_HDR_OFF_FIELD_ID);
   i += (offsetof(tallymark_hdr, pad0)             != TM_HDR_OFF_PAD0);
   i += (offsetof(tallymark_hdr, hash_id)          != TM_HDR_OFF_HASH_ID);

   i += (sizeof(hdr.magic)                         != TM_HDR_LEN_MAGIC);
   i += (sizeof(hdr.version_current)               != TM_HDR_LEN_VERSION_CURRENT);
   i += (sizeof(hdr.version_age)                   != TM_HDR_LEN_VERSION_AGE);
   i += (sizeof(hdr.header_len)                    != TM_HDR_LEN_HEADER_LEN);
   i += (sizeof(hdr.body_len)                      != TM_HDR_LEN_BODY_LEN);
   i += (sizeof(hdr.reserved)                      != TM_HDR_LEN_RESERVED);
   i += (sizeof(hdr.response_codes)                != TM_HDR_LEN_RESPONSE_CODES);
   i += (sizeof(hdr.request_id)                    != TM_HDR_LEN_REQUEST_ID);
   i += (sizeof(hdr.request_codes)                 != TM_HDR_LEN_REQUEST_CODES);
   i += (sizeof(hdr.service_id)                    != TM_HDR_LEN_SERVICE_ID);
   i += (sizeof(hdr.field_id)                      != TM_HDR_LEN_FIELD_ID);
   i += (sizeof(hdr.pad0)                          != TM_HDR_LEN_PAD0);
   i += (sizeof(hdr.hash_id)                       != TM_HDR_LEN_HASH_ID);

   return(i);
}



/* end of source */
