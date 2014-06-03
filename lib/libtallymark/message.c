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

#include "debug.h"


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
   tallymark_bdy   * bdy;
   uint8_t           pcount;
   size_t            off;
   size_t            len;

   assert(msg  != NULL);

   hdr  = &msg->header;
   buf  = &msg->buff.hdr;
   bdy  = &msg->body;

   // update message status
   msg->status &= ~TALLYMARK_MSG_COMPILED;
   if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      return(msg->error = EINVAL);
   if ((hdr->response_codes == 0) && (hdr->request_codes == 0))
      return(msg->error = EINVAL);

   // updates header
   off      = sizeof(tallymark_hdr);
   pcount   = 0;

   if (bdy->capabilities != 0)
   {
      off += tallymark_msg_compile_param_hdr(msg,  off, 4, TALLYMARK_PARM_SYS_CAPABILITIES);
      off += tallymark_msg_compile_u32(msg,        off, bdy->capabilities);
      pcount++;
   };

   if (bdy->hash_count_set != 0)
   {
      off += tallymark_msg_compile_param_hdr(msg,  off, 16, TALLYMARK_PARM_HASH_COUNT);
      off += tallymark_msg_compile_count(msg,      off, &bdy->hash_count);
      pcount++;
   };

   if (bdy->hash_text.bytes != 0)
   {
      len = tallymark_msg_compiled_len(bdy->hash_text.bytes);
      off += tallymark_msg_compile_param_hdr(msg,  off, len, TALLYMARK_PARM_HASH_TEXT);
      off += tallymark_msg_compile_utf8(msg,       off, &bdy->hash_text);
      pcount++;
   };

   if (bdy->package_name.bytes != 0)
   {
      len = tallymark_msg_compiled_len(bdy->package_name.bytes);
      off += tallymark_msg_compile_param_hdr(msg,  off, len, TALLYMARK_PARM_SYS_PKG_NAME);
      off += tallymark_msg_compile_utf8(msg,       off, &bdy->package_name);
      pcount++;
   };

   if (bdy->threshold_set != 0)
   {
      off += tallymark_msg_compile_param_hdr(msg,  off, 16, TALLYMARK_PARM_THRESHOLD);
      off += tallymark_msg_compile_count(msg,      off, &bdy->threshold);
      pcount++;
   };

   if (bdy->version.bytes != 0)
   {
      len = tallymark_msg_compiled_len(bdy->version.bytes);
      off += tallymark_msg_compile_param_hdr(msg,  off, len, TALLYMARK_PARM_SYS_VERSION);
      off += tallymark_msg_compile_utf8(msg,       off, &bdy->version);
      pcount++;
   };

   // updates header
   hdr->msg_len            = (uint8_t)(off / 4);
   hdr->param_count        = pcount;

   // compiles header
   buf->magic              = htonl(hdr->magic);
   buf->version_current    = hdr->version_current;
   buf->version_age        = hdr->version_age;
   buf->header_len         = hdr->header_len;
   buf->msg_len            = hdr->msg_len;
   buf->request_codes      = htonl(hdr->request_codes);
   buf->response_codes     = hdr->response_codes;
   buf->param_count        = hdr->param_count;
   buf->service_id         = hdr->service_id;
   buf->field_id           = hdr->field_id;
   buf->request_id         = htonl(hdr->request_id);
   buf->sequence_id        = htonl(hdr->sequence_id);
   memcpy(buf->hash, hdr->hash, sizeof(hdr->hash));

   msg->msg_len   += hdr->msg_len * 4;
   msg->status    |= TALLYMARK_MSG_COMPILED;

   return(0);
}


size_t tallymark_msg_compile_count(tallymark_msg * msg, size_t off, tallymark_count * val)
{
   tallymark_msg_compile_u64(msg, off+0, val->count);
   tallymark_msg_compile_u64(msg, off+8, val->seconds);
   return(16);
}


size_t tallymark_msg_compile_param_hdr(tallymark_msg * msg, size_t off, size_t data_len,
   uint32_t id)
{
   data_len += 4;
   msg->buff.u8[off+0] = (uint8_t)(data_len >> 2);
   msg->buff.u8[off+1] = (uint8_t)((id >> 16) & 0xff);
   msg->buff.u8[off+2] = (uint8_t)((id >>  8) & 0xff);
   msg->buff.u8[off+3] = (uint8_t)((id >>  0) & 0xff);
   return(4);
}


size_t tallymark_msg_compile_u64(tallymark_msg * msg, size_t off, uint64_t val)
{
   msg->buff.u8[off+0] = (uint8_t)((val >> 56) & 0xff);
   msg->buff.u8[off+1] = (uint8_t)((val >> 48) & 0xff);
   msg->buff.u8[off+2] = (uint8_t)((val >> 40) & 0xff);
   msg->buff.u8[off+3] = (uint8_t)((val >> 32) & 0xff);
   msg->buff.u8[off+4] = (uint8_t)((val >> 24) & 0xff);
   msg->buff.u8[off+5] = (uint8_t)((val >> 16) & 0xff);
   msg->buff.u8[off+6] = (uint8_t)((val >>  8) & 0xff);
   msg->buff.u8[off+7] = (uint8_t)((val >>  0) & 0xff);
   return(8);
}


size_t tallymark_msg_compile_u32(tallymark_msg * msg, size_t off, uint32_t val)
{
   msg->buff.u8[off+0] = (uint8_t)((val >> 24) & 0xff);
   msg->buff.u8[off+1] = (uint8_t)((val >> 16) & 0xff);
   msg->buff.u8[off+2] = (uint8_t)((val >>  8) & 0xff);
   msg->buff.u8[off+3] = (uint8_t)((val >>  0) & 0xff);
   return(4);
}


size_t tallymark_msg_compile_utf8(tallymark_msg * msg, size_t off, const tallymark_blob * blob)
{
   size_t len;
   len = tallymark_msg_compiled_len(blob->bytes);
   bzero(&msg->buff.u8[off], len);
   memcpy(&msg->buff.u8[off], blob->dat.str, blob->bytes);
   return(len);
}


size_t tallymark_msg_compiled_len(size_t len)
{
   if ((len & 0x03) != 0)
   {
      len &= ~0x03U;
      len += 4U;
   };
   return(len);
}


int tallymark_msg_create_header(tallymark_msg * msg,
   uint32_t req_id, uint8_t srv_id, uint8_t fld_id,
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
   hdr->header_len         = sizeof(tallymark_hdr)/4;
   hdr->msg_len            = 0;
   hdr->request_id         = req_id;
   hdr->service_id         = srv_id;
   hdr->field_id           = fld_id;
   hash_len = (hash_len > sizeof(hdr->hash)) ? sizeof(hdr->hash) : hash_len;
   memset(hdr->hash, 0, sizeof(hdr->hash));
   memcpy(hdr->hash, hash, hash_len);

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
   if (msg == NULL)
      return;

   free(msg->body.version.dat.ptr);
   free(msg->body.package_name.dat.ptr);

   bzero(msg, sizeof(tallymark_msg));
   free(msg);

   return;
}


int tallymark_msg_get_buffer(tallymark_msg * msg, const uint8_t ** pbuf,
   size_t * plen)
{
   assert(msg  != NULL);
   assert(pbuf != NULL);
   assert(plen != NULL);
   *pbuf = msg->buff.u8;
   *plen = sizeof(msg->buff.u8);
   return(0);
}


int tallymark_msg_get_utf8(tallymark_msg * msg, const tallymark_blob * blob,
   void * outvalue, size_t * outvalue_size)
{
   assert(msg           != NULL);
   assert(outvalue      != NULL);
   assert(outvalue_size != 0);

   if ((blob->dat.ptr == NULL) || (blob->bytes == 0))
   {
      (*((char **)outvalue)) = NULL;
      if (outvalue_size != NULL)
         *outvalue_size = 0;
      return(0);
   };

   if ((*((char **)outvalue) = malloc(blob->bytes+1)) == NULL)
   {
      if (outvalue_size != NULL)
         *outvalue_size = 0;
      return(msg->error = ENOMEM);
   };

   memcpy(*((char **)outvalue), blob->dat.ptr, blob->bytes);
   (*((uint8_t **)outvalue))[blob->bytes] = 0;
   if (outvalue_size != NULL)
      *outvalue_size = blob->bytes;

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
      case TALLYMARK_PARM_HASH_COUNT:
      if (*outvalue_size != sizeof(tallymark_count))
         return(msg->error = EINVAL);
      ((tallymark_count *)outvalue)->count   = msg->body.hash_count.count;
      ((tallymark_count *)outvalue)->seconds = msg->body.hash_count.seconds;
      return(0);

      case TALLYMARK_PARM_HASH_TEXT:
      return(tallymark_msg_get_utf8(msg, &msg->body.hash_text, outvalue, outvalue_size));

      case TALLYMARK_PARM_SYS_CAPABILITIES:
      if (*outvalue_size < sizeof(msg->body.capabilities))
         return(msg->error = EINVAL);
      *((uint32_t *)outvalue) = msg->body.capabilities;
      *outvalue_size = sizeof(msg->body.capabilities);
      return(0);

      case TALLYMARK_PARM_SYS_VERSION:
      return(tallymark_msg_get_utf8(msg, &msg->body.version, outvalue, outvalue_size));

      case TALLYMARK_PARM_SYS_PKG_NAME:
      return(tallymark_msg_get_utf8(msg, &msg->body.package_name, outvalue, outvalue_size));

      case TALLYMARK_PARM_THRESHOLD:
      if (*outvalue_size != sizeof(tallymark_count))
         return(msg->error = EINVAL);
      ((tallymark_count *)outvalue)->count   = msg->body.threshold.count;
      ((tallymark_count *)outvalue)->seconds = msg->body.threshold.seconds;
      return(0);

      default:
      break;
   };

   return(msg->error = EINVAL);
}


int tallymark_msg_get_header(tallymark_msg * msg,
  const tallymark_hdr ** phdr)
{
   assert(msg  != NULL);
   assert(phdr != NULL);
   if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      return(msg->error = EINVAL);
   *phdr = &msg->header;
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
   i += (offsetof(tallymark_hdr, msg_len)          != TM_HDR_OFF_MSG_LEN);
   i += (offsetof(tallymark_hdr, param_count)      != TM_HDR_OFF_PARAM_COUNT);
   i += (offsetof(tallymark_hdr, response_codes)   != TM_HDR_OFF_RESPONSE_CODES);
   i += (offsetof(tallymark_hdr, request_codes)    != TM_HDR_OFF_REQUEST_CODES);
   i += (offsetof(tallymark_hdr, request_id)       != TM_HDR_OFF_REQUEST_ID);
   i += (offsetof(tallymark_hdr, sequence_id)      != TM_HDR_OFF_SEQUENCE_ID);
   i += (offsetof(tallymark_hdr, service_id)       != TM_HDR_OFF_SERVICE_ID);
   i += (offsetof(tallymark_hdr, field_id)         != TM_HDR_OFF_FIELD_ID);
   i += (offsetof(tallymark_hdr, hash)             != TM_HDR_OFF_HASH);
   if (poffset != NULL)
      *poffset = i;
   total += i;

   i = 0;
   i += (sizeof(tallymark_hdr)                     != TM_HDR_LEN);
   i += (sizeof(hdr.magic)                         != TM_HDR_LEN_MAGIC);
   i += (sizeof(hdr.version_current)               != TM_HDR_LEN_VERSION_CURRENT);
   i += (sizeof(hdr.version_age)                   != TM_HDR_LEN_VERSION_AGE);
   i += (sizeof(hdr.header_len)                    != TM_HDR_LEN_HEADER_LEN);
   i += (sizeof(hdr.msg_len)                       != TM_HDR_LEN_MSG_LEN);
   i += (sizeof(hdr.param_count)                   != TM_HDR_LEN_PARAM_COUNT);
   i += (sizeof(hdr.response_codes)                != TM_HDR_LEN_RESPONSE_CODES);
   i += (sizeof(hdr.request_codes)                 != TM_HDR_LEN_REQUEST_CODES);
   i += (sizeof(hdr.request_id)                    != TM_HDR_LEN_REQUEST_ID);
   i += (sizeof(hdr.sequence_id)                   != TM_HDR_LEN_SEQUENCE_ID);
   i += (sizeof(hdr.service_id)                    != TM_HDR_LEN_SERVICE_ID);
   i += (sizeof(hdr.field_id)                      != TM_HDR_LEN_FIELD_ID);
   i += (sizeof(hdr.hash)                          != TM_HDR_LEN_HASH);
   if (plen != NULL)
      *plen = i;
   total += i;

   return(total);
}


int tallymark_msg_parse(tallymark_msg * msg)
{
   tallymark_hdr      * hdr;
   tallymark_hdr      * buf;
   int                  err;
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
   hdr->msg_len            = buf->msg_len;

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
   if (msg->msg_len != (hdr->msg_len * 4U))
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // parses rest of header
   hdr->request_codes      = ntohl(buf->request_codes);
   hdr->response_codes     = buf->response_codes;
   hdr->param_count        = buf->param_count;
   hdr->service_id         = buf->service_id;
   hdr->field_id           = buf->field_id;
   hdr->request_id         = ntohl(buf->request_id);
   hdr->sequence_id        = ntohl(buf->sequence_id);
   memcpy(hdr->hash, &msg->buff.u8[TM_HDR_OFF_HASH], sizeof(hdr->hash));

   // parses body
   off = hdr->header_len * 4;
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
      param_id   = (((uint32_t)msg->buff.u8[off+1] & 0xff) << 16);
      param_id  |= (((uint32_t)msg->buff.u8[off+2] & 0xff) <<  8);
      param_id  |= (((uint32_t)msg->buff.u8[off+3] & 0xff) <<  0);

      // verify message is long enough to contain the specified parameter
      if ((off + param_len) > msg->msg_len)
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
         tallymark_msg_parse_u32(msg, off, &msg->body.capabilities);
         break;

         case TALLYMARK_PARM_HASH_COUNT:
         tallymark_msg_parse_count(msg, off, &msg->body.hash_count);
         msg->body.hash_count_set = 1;
         break;

         case TALLYMARK_PARM_HASH_TEXT:
         if ((err = tallymark_msg_parse_utf8(msg, off, param_len, &msg->body.hash_text)) != 0)
            return(err);
         break;

         case TALLYMARK_PARM_SYS_PKG_NAME:
         if ((err = tallymark_msg_parse_utf8(msg, off, param_len, &msg->body.package_name)) != 0)
            return(err);
         break;

         case TALLYMARK_PARM_SYS_VERSION:
         if ((err = tallymark_msg_parse_utf8(msg, off, param_len, &msg->body.version)) != 0)
            return(err);
         break;

         case TALLYMARK_PARM_THRESHOLD:
         tallymark_msg_parse_count(msg, off, &msg->body.threshold);
         msg->body.threshold_set = 1;
         break;

         default:
         break;
      };

      off += param_len;
   };

   msg->status |= TALLYMARK_MSG_PARSED;

   return(0);
}


int tallymark_msg_parse_count(tallymark_msg * msg, size_t off, tallymark_count * count)
{
   tallymark_msg_parse_u64(msg, off+0, &count->count);
   tallymark_msg_parse_u64(msg, off+8, &count->seconds);
   return(0);
}


int tallymark_msg_parse_u32(tallymark_msg * msg, size_t off, uint32_t * u32)
{
   *u32  = (((uint32_t)msg->buff.u8[off+0] & 0xff) << 24);
   *u32 |= (((uint32_t)msg->buff.u8[off+1] & 0xff) << 16);
   *u32 |= (((uint32_t)msg->buff.u8[off+2] & 0xff) <<  8);
   *u32 |= (((uint32_t)msg->buff.u8[off+3] & 0xff) <<  0);
   return(0);
}


int tallymark_msg_parse_u64(tallymark_msg * msg, size_t off, uint64_t * u64)
{
   *u64  = (((uint64_t)msg->buff.u8[off+0] & 0xff) << 56LLU);
   *u64 |= (((uint64_t)msg->buff.u8[off+1] & 0xff) << 48LLU);
   *u64 |= (((uint64_t)msg->buff.u8[off+2] & 0xff) << 40LLU);
   *u64 |= (((uint64_t)msg->buff.u8[off+3] & 0xff) << 32LLU);
   *u64 |= (((uint64_t)msg->buff.u8[off+4] & 0xff) << 24LLU);
   *u64 |= (((uint64_t)msg->buff.u8[off+5] & 0xff) << 16LLU);
   *u64 |= (((uint64_t)msg->buff.u8[off+6] & 0xff) <<  8LLU);
   *u64 |= (((uint64_t)msg->buff.u8[off+7] & 0xff) <<  0LLU);
   return(0);
}


int tallymark_msg_parse_utf8(tallymark_msg * msg, size_t off,
   size_t param_len, tallymark_blob * blob)
{
   void * ptr;

   if ((ptr = realloc(blob->dat.ptr, (param_len+1))) == NULL)
      return(msg->error = ENOMEM);
   blob->dat.ptr = ptr;

   memcpy(ptr, &msg->buff.u8[off], param_len);
   blob->dat.str[param_len] = '\0';
   blob->bytes = param_len;

   return(0);
}



int tallymark_msg_read(tallymark_msg * msg, int s,
   struct sockaddr * address, socklen_t * address_len)
{
   ssize_t  bufflen;
   size_t   len;

   assert(msg  != NULL);
   assert(s    != -1);

   if (msg->status != TALLYMARK_MSG_RESET)
      return(msg->error = EINVAL);

   if ((address != NULL) && (address_len != NULL))
      bzero(address, *address_len);

   // read first 8 bytes of header from socket
   bufflen = recvfrom(s, &msg->buff, TM_MSG_MAX_SIZE, MSG_PEEK, address, address_len);
   if (bufflen == -1)
      return(msg->error = errno);
   msg->msg_len = (size_t)bufflen;
   if (bufflen < 8)
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // read full message from socket
   len = msg->buff.hdr.msg_len * 4;
   bufflen = recvfrom(s, &msg->buff, TM_MSG_MAX_SIZE, 0, NULL, NULL);
   if (bufflen == -1)
      return(msg->error = errno);
   if ((msg->msg_len = (size_t)bufflen) != len)
   {
      msg->status = TALLYMARK_MSG_BAD;
      return(msg->error = EBADMSG);
   };

   // update message state
   msg->status    = TALLYMARK_MSG_COMPILED;
   msg->msg_len   = (size_t)bufflen;
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
   msg->header.header_len          = (sizeof(tallymark_hdr) / 4);

   // resets body
   msg->body.hash_count.count             = 0;
   msg->body.hash_count.seconds           = 0;
   msg->body.hash_count_set               = 0;
   msg->body.capabilities                 = 0;
   msg->body.version.bytes                = 0;
   msg->body.package_name.bytes           = 0;
   if (msg->body.package_name.dat.str != NULL)
      msg->body.package_name.dat.str[0]   = '\0';
   if (msg->body.version.dat.str != NULL)
      msg->body.version.dat.str[0]        = '\0';

   return(0);
}


int tallymark_msg_sendto(int s, tallymark_msg * msg,
   const struct sockaddr * dest_addr, socklen_t dest_len)
{
   int err;

   assert(s    != -1);
   assert(msg  != NULL);

   if ((msg->status & TALLYMARK_MSG_COMPILED) == 0)
   {
      if ((msg->status & TALLYMARK_MSG_PARSED) == 0)
      {
         msg->error = EINVAL;
         errno = msg->error;
         return(msg->error);
      };
      if ((err = tallymark_msg_compile(msg)) != 0)
         return(err);
   };

   if (sendto(s, msg->buff.u8, msg->msg_len, 0, dest_addr, dest_len) == -1)
      return(msg->error = errno);

   return(0);
}


int tallymark_msg_set_utf8(tallymark_msg * msg, tallymark_blob * pout,
   const void * invalue, size_t invalue_size)
{
   char * ptr;
   assert(msg              != NULL);
   assert(pout             != NULL);

   // clears value if missing
   if (invalue == NULL)
   {
      free(pout->dat.ptr);
      pout->dat.ptr  = NULL;
      pout->bytes    = 0;
      return(0);
   };

   // allocates memory
   if ((ptr = realloc(pout->dat.ptr, invalue_size+1)) == NULL)
      return(msg->error = ENOMEM);
   pout->dat.ptr  = ptr;
   pout->bytes    = invalue_size;

   // copy memory
   memcpy(ptr, *((const uint8_t * const * const)invalue), invalue_size);
   ptr[invalue_size] = '\0';

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
      break;
   };

   return(msg->error = EINVAL);
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
      case TALLYMARK_PARM_HASH_COUNT:
      if (invalue_size != sizeof(tallymark_count))
         return(msg->error = EINVAL);
      msg->body.hash_count.count   = ((const tallymark_count *)invalue)->count;
      msg->body.hash_count.seconds = ((const tallymark_count *)invalue)->seconds;
      msg->body.hash_count_set     = 1;
      return(0);

      case TALLYMARK_PARM_HASH_TEXT:
      return(tallymark_msg_set_utf8(msg, &msg->body.hash_text, invalue, invalue_size));

      case TALLYMARK_PARM_SYS_CAPABILITIES:
      if (invalue_size != sizeof(msg->body.capabilities))
         return(msg->error = EINVAL);
      msg->body.capabilities = *((const uint32_t *)invalue);
      return(0);

      case TALLYMARK_PARM_SYS_VERSION:
      return(tallymark_msg_set_utf8(msg, &msg->body.version, invalue, invalue_size));

      case TALLYMARK_PARM_SYS_PKG_NAME:
      return(tallymark_msg_set_utf8(msg, &msg->body.package_name, invalue, invalue_size));

      case TALLYMARK_PARM_THRESHOLD:
      if (invalue_size != sizeof(tallymark_count))
         return(msg->error = EINVAL);
      msg->body.threshold.count   = ((const tallymark_count *)invalue)->count;
      msg->body.threshold.seconds = ((const tallymark_count *)invalue)->seconds;
      msg->body.threshold_set     = 1;
      return(0);

      default:
      break;
   };

   return(msg->error = EINVAL);
}


/* end of source */
