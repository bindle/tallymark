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


/////////////////
//             //
//  Functions  //
//             //
/////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Functions
#endif

int tallymark_msg_alloc(tallymark_msg ** pmsg)
{
   int             err;
   tallymark_msg * msg;

   assert(pmsg != NULL);

   if ((msg = malloc(sizeof(tallymark_msg))) == NULL)
      return(ENOMEM);
   memset(msg, 0, sizeof(tallymark_msg));

   if ((err = tallymark_msg_reset(msg)) != 0)
   {
      tallymark_msg_free(msg);
      return(err);
   };

   *pmsg = msg;

   return(0);
}


int tallymark_msg_compile(tallymark_msg * msg)
{
   tallymark_hdr   * hdr;
   tallymark_hdr   * buf;

   assert(msg  != NULL);

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

   msg->msg_len   =  TM_HDR_LEN;
   msg->msg_len   += hdr->body_len * 4;
   msg->status    |= TALLYMARK_MSG_COMPILED;

   return(0);
}


int tallymark_msg_create_header(tallymark_msg * msg,
   uint32_t req_id, uint32_t srv_id, uint32_t fld_id,
   const uint8_t * hash, size_t hash_len)
{
   int               err;
   tallymark_hdr   * hdr;

   assert(msg        != NULL);
   assert(hash       != NULL);
   assert(hash_len   >  0);

   hdr  = &msg->header;

   if ((err = tallymark_msg_reset(msg)) != 0)
      return(err);

   // apply header
   hdr->magic              = TALLYMARK_MAGIC;
   hdr->version_current    = TALLYMARK_PROTO_VERSION;
   hdr->version_age        = TALLYMARK_PROTO_AGE;
   hdr->header_len         = TM_HDR_LENGTH/4;
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


int tallymark_msg_errnum(tallymark_msg * msg)
{
   assert(msg != NULL);
   return(msg->error);
}


void tallymark_msg_free(tallymark_msg * msg)
{
   assert(msg  != NULL);

   if (msg->body.version != NULL)
      free(msg->body.version);
   if (msg->body.package_name != NULL)
      free(msg->body.package_name);

   memset(msg, 0, sizeof(tallymark_msg));

   free(msg);

   return;
}


int tallymark_msg_get_string(tallymark_msg * msg, const char * invalue,
   void * outvalue, size_t * outvalue_size)
{
   size_t len;

   assert(msg           != NULL);
   assert(outvalue      != NULL);
   assert(outvalue_size != NULL);

   len = strlen(invalue);
   len = ((*outvalue_size - 1) < len) ? (*outvalue_size - 1) : len;

   if (invalue == NULL)
   {
      ((char *)outvalue)[0] = '\0';
      *outvalue_size = 0;
      return(0);
   };
   strncpy((char *)outvalue, invalue, len);
   ((char *)outvalue)[0] = '\0';
   *outvalue_size = len;

   return(0);
}


int tallymark_msg_get_param(tallymark_msg * msg, int param, void * outvalue,
   size_t * outvalue_size)
{
   assert(msg              != NULL);
   assert(outvalue         != NULL);
   assert(outvalue_size    != NULL);
   assert(*outvalue_size   != 0);

   if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      return(msg->error = EINVAL);

   switch(param)
   {
      case TALLYMARK_PARM_SYS_CAPABILITIES:
      if (*outvalue_size < sizeof(msg->body.capabilities))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->body.capabilities;
      *outvalue_size = sizeof(msg->body.capabilities);
      return(0);

      case TALLYMARK_PARM_SYS_VERSION:
      return(tallymark_msg_get_string(msg, msg->body.version, outvalue, outvalue_size));

      case TALLYMARK_PARM_SYS_PKG_NAME:
      return(tallymark_msg_get_string(msg, msg->body.package_name, outvalue, outvalue_size));

      case TALLYMARK_PARM_TALLY_COUNT:

      case TALLYMARK_PARM_TALLY_HISTORY:
      default:
      return(msg->error = EINVAL);
   };

   return(0);
}


int tallymark_msg_get_header(tallymark_msg * msg, int header, void * outvalue,
   size_t * outvalue_size)
{
   assert(msg              != NULL);
   assert(outvalue         != NULL);
   assert(outvalue_size    != NULL);
   assert(*outvalue_size   != 0);

   if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      return(msg->error = EINVAL);

   switch(header)
   {
      case TALLYMARK_HDR_MAGIC:
      if (*outvalue_size < sizeof(msg->header.magic))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->header.magic;
      *outvalue_size = sizeof(msg->header.magic);
      return(0);

      case TALLYMARK_HDR_VERSION_CURRENT:
      if (*outvalue_size < sizeof(msg->header.version_current))
         return(msg->error = EINVAL);
      *((int8_t *)outvalue) = msg->header.version_current;
      *outvalue_size = sizeof(msg->header.version_current);
      return(0);

      case TALLYMARK_HDR_VERSION_AGE:
      if (*outvalue_size < sizeof(msg->header.version_age))
         return(msg->error = EINVAL);
      *((int8_t *)outvalue) = msg->header.version_age;
      *outvalue_size = sizeof(msg->header.version_age);
      return(0);

      case TALLYMARK_HDR_HEADER_LEN:
      if (*outvalue_size < sizeof(msg->header.header_len))
         return(msg->error = EINVAL);
      *((uint8_t *)outvalue) = msg->header.header_len;
      *outvalue_size = sizeof(msg->header.header_len);
      return(0);

      case TALLYMARK_HDR_BODY_LEN:
      if (*outvalue_size < sizeof(msg->header.body_len))
         return(msg->error = EINVAL);
      *((uint8_t *)outvalue) = msg->header.body_len;
      *outvalue_size = sizeof(msg->header.body_len);
      return(0);

      case TALLYMARK_HDR_PARAM_COUNT:
      if (*outvalue_size < sizeof(msg->header.param_count))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->header.param_count;
      *outvalue_size = sizeof(msg->header.param_count);
      return(0);

      case TALLYMARK_HDR_RESPONSE_CODES:
      if (*outvalue_size < sizeof(msg->header.response_codes))
         return(msg->error = EINVAL);
      *((uint8_t *)outvalue) = msg->header.response_codes;
      *outvalue_size = sizeof(msg->header.response_codes);
      return(0);

      case TALLYMARK_HDR_REQUEST_CODES:
      if (*outvalue_size < sizeof(msg->header.request_codes))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->header.request_codes;
      *outvalue_size = sizeof(msg->header.request_codes);
      return(0);

      case TALLYMARK_HDR_REQUEST_ID:
      if (*outvalue_size < sizeof(msg->header.response_codes))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->header.response_codes;
      *outvalue_size = sizeof(msg->header.response_codes);
      return(0);

      case TALLYMARK_HDR_SEQUENCE_ID:
      if (*outvalue_size < sizeof(msg->header.sequence_id))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->header.sequence_id;
      *outvalue_size = sizeof(msg->header.sequence_id);
      return(0);

      case TALLYMARK_HDR_SERVICE_ID:
      if (*outvalue_size < sizeof(msg->header.service_id))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->header.service_id;
      *outvalue_size = sizeof(msg->header.service_id);
      return(0);

      case TALLYMARK_HDR_FIELD_ID:
      if (*outvalue_size < sizeof(msg->header.field_id))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->header.field_id;
      *outvalue_size = sizeof(msg->header.field_id);
      return(0);

      default:
      return(msg->error = EINVAL);
   };

   return(0);
}


int tallymark_msg_header_errors(int * poffset, int * plen)
{
   int           i;
   int           total;
   tallymark_hdr hdr;

   total = 0;

   i = 0;
   i += (offsetof(tallymark_hdr, magic)            != TM_HDR_OFF_MAGIC);
   i += (offsetof(tallymark_hdr, version_current)  != TM_HDR_OFF_VERSION_CURRENT);
   i += (offsetof(tallymark_hdr, version_age)      != TM_HDR_OFF_VERSION_AGE);
   i += (offsetof(tallymark_hdr, header_len)       != TM_HDR_OFF_HEADER_LEN);
   i += (offsetof(tallymark_hdr, body_len)         != TM_HDR_OFF_BODY_LEN);
   i += (offsetof(tallymark_hdr, reserved)         != TM_HDR_OFF_RESERVED);
   i += (offsetof(tallymark_hdr, param_count)      != TM_HDR_OFF_PARAM_COUNT);
   i += (offsetof(tallymark_hdr, response_codes)   != TM_HDR_OFF_RESPONSE_CODES);
   i += (offsetof(tallymark_hdr, request_codes)    != TM_HDR_OFF_REQUEST_CODES);
   i += (offsetof(tallymark_hdr, request_id)       != TM_HDR_OFF_REQUEST_ID);
   i += (offsetof(tallymark_hdr, sequence_id)      != TM_HDR_OFF_SEQUENCE_ID);
   i += (offsetof(tallymark_hdr, service_id)       != TM_HDR_OFF_SERVICE_ID);
   i += (offsetof(tallymark_hdr, field_id)         != TM_HDR_OFF_FIELD_ID);
   i += (offsetof(tallymark_hdr, hash_id)          != TM_HDR_OFF_HASH_ID);
   if (poffset != NULL)
      *poffset = i;
   total += i;

   i = 0;
   i += (sizeof(tallymark_hdr)                     != TM_HDR_LENGTH);
   i += (sizeof(hdr.magic)                         != TM_HDR_LEN_MAGIC);
   i += (sizeof(hdr.version_current)               != TM_HDR_LEN_VERSION_CURRENT);
   i += (sizeof(hdr.version_age)                   != TM_HDR_LEN_VERSION_AGE);
   i += (sizeof(hdr.header_len)                    != TM_HDR_LEN_HEADER_LEN);
   i += (sizeof(hdr.body_len)                      != TM_HDR_LEN_BODY_LEN);
   i += (sizeof(hdr.reserved)                      != TM_HDR_LEN_RESERVED);
   i += (sizeof(hdr.param_count)                   != TM_HDR_LEN_PARAM_COUNT);
   i += (sizeof(hdr.response_codes)                != TM_HDR_LEN_RESPONSE_CODES);
   i += (sizeof(hdr.request_codes)                 != TM_HDR_LEN_REQUEST_CODES);
   i += (sizeof(hdr.request_id)                    != TM_HDR_LEN_REQUEST_ID);
   i += (sizeof(hdr.sequence_id)                   != TM_HDR_LEN_SEQUENCE_ID);
   i += (sizeof(hdr.service_id)                    != TM_HDR_LEN_SERVICE_ID);
   i += (sizeof(hdr.field_id)                      != TM_HDR_LEN_FIELD_ID);
   i += (sizeof(hdr.hash_id)                       != TM_HDR_LEN_HASH_ID);
   if (plen != NULL)
      *plen = i;
   total += i;

   return(total);
}


int tallymark_msg_parse(tallymark_msg * msg)
{
   tallymark_hdr      * hdr;
   tallymark_hdr      * buf;
   size_t               off;
   uint32_t             param_len;
   uint32_t             param_id;

   assert(msg  != NULL);

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

   // If current version number is negative, assume the protocol
   // version switched to a data type larger than 8 bits.
   if (hdr->version_current < 0)
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
   if (msg->msg_len != ((hdr->header_len * hdr->body_len) * 4U))
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // parses last part of header
   hdr->param_count        = buf->param_count;
   hdr->response_codes     = buf->response_codes;
   hdr->request_codes      = ntohl(buf->request_codes);
   hdr->request_id         = ntohl(buf->request_id);
   hdr->sequence_id        = ntohl(buf->sequence_id);
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
         case TALLYMARK_PARM_SYS_CAPABILITIES:
         msg->body.capabilities   = (((uint32_t)msg->buff.u8[0] & 0xff) << 24);
         msg->body.capabilities  |= (((uint32_t)msg->buff.u8[1] & 0xff) << 16);
         msg->body.capabilities  |= (((uint32_t)msg->buff.u8[2] & 0xff) <<  8);
         msg->body.capabilities  |= (((uint32_t)msg->buff.u8[3] & 0xff) <<  0);
         break;

         case TALLYMARK_PARM_SYS_PKG_NAME:
         if (msg->body.package_name != NULL)
            free(msg->body.package_name);
         if ((msg->body.package_name = malloc(param_len+1)) == NULL)
            return(msg->error = ENOMEM);
         memcpy(msg->body.package_name, &msg->buff.u8[0], param_len);
         msg->body.package_name[param_len] = '\0';
         break;

         case TALLYMARK_PARM_SYS_VERSION:
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

   assert(msg  != NULL);
   assert(s    != -1);

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

   assert(s    != -1);
   assert(msg  != NULL);

   if ((err = tallymark_msg_reset(msg)) != 0)
      return(err);

   if ((err = tallymark_msg_read(msg, s, address, address_len)) != 0)
      return(err);

   return(tallymark_msg_parse(msg));
}


int tallymark_msg_reset(tallymark_msg * msg)
{
   assert(msg  != NULL);

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
   msg->header.header_len          = (TM_HDR_LENGTH / 4);

   // resets body
   msg->body.capabilities           = 0;
   msg->body.version_size           = 0;
   msg->body.package_name_size      = 0;
   if (msg->body.package_name != NULL)
      msg->body.package_name[0]     = '\0';
   if (msg->body.version != NULL)
      msg->body.version[0]          = '\0';

   return(0);
}


ssize_t tallymark_msg_sendto(int s, tallymark_msg * msg,
   const struct sockaddr * dest_addr, socklen_t dest_len)
{
   int     err;
   ssize_t len;

   assert(s    != -1);
   assert(msg  != NULL);

   if ((msg->status & TALLYMARK_MSG_COMPILED) == 0)
   {
      if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      {
         msg->error = EINVAL;
         errno = msg->error;
         return(-1);
      };
      if ((err = tallymark_msg_compile(msg)) != 0)
         return(-1);
   };

   len = sendto(s, msg->buff.u8, msg->msg_len, 0, dest_addr, dest_len);
   if (len == -1)
      msg->error = errno;

   return(len);
}


int tallymark_msg_set_string(tallymark_msg * msg, char ** ptr,
   const void * invalue, size_t invalue_size)
{
   assert(msg              != NULL);
   assert(ptr              != NULL);
   assert(invalue          != NULL);

   if (*ptr != NULL)
   {
      free(*ptr);
      *ptr = NULL;
   };
   if (invalue == NULL)
      return(0);

   if ((*ptr = malloc(invalue_size+1)) == NULL)
      return(msg->error = ENOMEM);
   strncpy(*ptr, invalue, invalue_size);
   (*ptr)[invalue_size] = '\0';

   return(0);
}


int tallymark_msg_set_header(tallymark_msg * msg, int header,
   const void * invalue, size_t invalue_size)
{
   assert(msg              != NULL);
   assert(invalue          != NULL);

   if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      return(msg->error = EINVAL);
   msg->status &= ~TALLYMARK_MSG_COMPILED;

   switch(header)
   {
      case TALLYMARK_HDR_RESPONSE_CODES:
      if (invalue_size != sizeof(msg->header.response_codes))
         return(msg->error = EINVAL);
      msg->header.response_codes = *((uint8_t *)invalue);
      return(0);

      case TALLYMARK_HDR_REQUEST_CODES:
      if (invalue_size != sizeof(msg->header.request_codes))
         return(msg->error = EINVAL);
      msg->header.request_codes = *((uint32_t *)invalue);
      return(0);

      default:
      return(msg->error = EINVAL);
   };

   return(0);
}


int tallymark_msg_set_param(tallymark_msg * msg, int param,
   const void * invalue, size_t invalue_size)
{
   assert(msg              != NULL);
   assert(invalue          != NULL);

   if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      return(msg->error = EINVAL);
   msg->status &= ~TALLYMARK_MSG_COMPILED;

   switch(param)
   {
      case TALLYMARK_PARM_SYS_CAPABILITIES:
      if (invalue_size != sizeof(msg->body.capabilities))
         return(msg->error = EINVAL);
      msg->body.capabilities = *((const uint32_t *)invalue);
      return(0);

      case TALLYMARK_PARM_SYS_VERSION:
      return(tallymark_msg_set_string(msg, &msg->body.version, invalue, invalue_size));

      case TALLYMARK_PARM_SYS_PKG_NAME:
      return(tallymark_msg_set_string(msg, &msg->body.package_name, invalue, invalue_size));

      case TALLYMARK_PARM_TALLY_COUNT:

      case TALLYMARK_PARM_TALLY_HISTORY:
      default:
      return(msg->error = EINVAL);
   };

   return(0);
}


/* end of source */
