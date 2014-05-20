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
/**
 *   @file tallymark.h
 *   Tally Mark Daemon public API
 */
#ifndef __TALLYMARK_H
#define __TALLYMARK_H 1
#undef __TALLYMARK_PMARK


///////////////
//           //
//  Headers  //
//           //
///////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Headers
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <inttypes.h>
#include <netinet/in.h>

#include <tallymark_cdefs.h>
#include <tallymark_defs.h>


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Data Types
#endif

typedef struct libtallymark_struct           tallymark;
typedef struct libtallymark_message_struct   tallymark_msg;
typedef struct libtallymark_header_struct    tallymark_hdr;
typedef struct libtallymark_url_desc_struct  tallymark_url_desc;
typedef union  libtallymark_sockaddr_union   tallymark_sockaddr;


struct libtallymark_header_struct
{
                                 //        +----------------+
                                 //        | buffer offsets |
                                 // +------+----+-----+-----+
                                 // | size | u8 | u32 | u64 |
                                 // +------+----+-----+-----+
   uint32_t    magic;            // |    4 |  0 |   0 |   0 |
   int8_t      version_current;  // |    1 |  4 |   1 |     |
   int8_t      version_age;      // |    1 |  5 |     |     |
   uint8_t     header_len;       // |    1 |  6 |     |     |
   uint8_t     body_len;         // |    1 |  7 |     |     |
   uint8_t     reserved[2];      // |    2 |  8 |   2 |   1 |
   uint8_t     param_count;      // |    1 | 10 |     |     |
   uint8_t     response_codes;   // |    1 | 11 |     |     |
   uint32_t    request_codes;    // |    4 | 12 |   3 |     |
   uint32_t    request_id;       // |    4 | 16 |   4 |   2 |
   uint32_t    sequence_id;      // |    4 | 28 |   7 |     |
   uint32_t    service_id;       // |    4 | 20 |   5 |     |
   uint32_t    field_id;         // |    4 | 24 |   6 |   3 |
   uint8_t     hash_id[24];      // |   24 | 32 |   8 |   4 |
   //          body              // | 1024 | 56 |  14 |   7 |
                                 // +------+----+-----+-----+
};


union  libtallymark_sockaddr_union
{
   struct sockaddr            sa;
   struct sockaddr_in         sa_in;
   struct sockaddr_in6        sa_in6;
   struct sockaddr_storage    sa_storage;
   struct sockaddr_un         sa_un;
};


struct libtallymark_url_desc_struct
{
   // URL parsing
   char *               tud_scheme;    // URI scheme
   char *               tud_host;      // host to contact
   int                  tud_port;      // port on host
   char *               tud_path;      // path to IPC Unix Domain Socket

   // string presentations
   char *               tud_straddr;
   socklen_t            tud_straddr_len;
   char *               tud_strurl;
   socklen_t            tud_strurl_len;

   // host resolution
   int                  tud_family;
   int                  tud_socktype;
   int                  tud_protocol;
   socklen_t            tud_addrlen;
   tallymark_sockaddr   tud_addr;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark -
#endif

/**
 *  @defgroup core Core Functions
 *  @brief Core functions for managing the state of tally mark.
 */
#ifdef __TALLYMARK_PMARK
#pragma mark Core Prototypes
#endif

/// @ingroup core
/// Allocates and initializes memory for storing tallymark state.
///
/// @param[out] ptmd    Reference to pointer to store the initialized memory.
/// @return Returns 0 on success and the value of errno if an error is
///         encountered.
/// @see tallymark_destroy
_TALLYMARK_F int tallymark_init(tallymark ** ptmd);


/// @ingroup core
/// Frees resources used by tallymark instance.
///
/// @param[in]  tmd     Pointer to the tallymark state data.
/// @see tallymark_init
_TALLYMARK_F void tallymark_destroy(tallymark * tmd);


/**
 *  @defgroup debug Debug Functions
 *  @brief Functions for debugging errors.
 */
#ifdef __TALLYMARK_PMARK
#pragma mark Debug Prototypes
#endif

_TALLYMARK_F void tallymark_print_hexdump(FILE * fs, uint8_t * buff,
   size_t len, const char * prefix, const char * fmt, ...);


/**
 *  @defgroup error Error Functions
 *  @brief Functions for reporting errors.
 */
#ifdef __TALLYMARK_PMARK
#pragma mark Error Prototypes
#endif

/// @ingroup error
/// Prints current error string to STDERR.
///
/// @param[in]  s  String prefix for error string
/// @see tallymark_strerror, tallymark_strerror_r
_TALLYMARK_F void tallymark_perror(const char * s);

/// @ingroup error
/// Returns a pointer to the corresponding message string.
///
/// @param[in]  errnum  Error number.
/// @return Returns error string.
/// @see tallymark_perror, tallymark_strerror_r
_TALLYMARK_F char * tallymark_strerror(int errnum);

/// @ingroup error
/// Returns a pointer to the corresponding message string.
///
/// @param[in]  errnum  Error number.
/// @return Returns 0 if success, otherwise returns error code.
/// @see tallymark_perror, tallymark_strerror
_TALLYMARK_F int tallymark_strerror_r(int errnum, char * strerrbuf,
   size_t buflen);


/**
 *  @defgroup network Network Functions
 *  @brief Functions for assisting with network management.
 */
#ifdef __TALLYMARK_PMARK
#pragma mark Network Prototypes
#endif

/// @ingroup network
/// Adds a file descriptor to the set of file descriptors to be polled.
///
/// @param[in]  tmd     Tally mark state data.
/// @param[in]  fd      file descriptor to add to polling set.
/// @return  Returns 0 on success and the value of errno if an error is encountered.
/// @see tallymark_poll, tallymark_del_fd
_TALLYMARK_F int tallymark_add_fd(tallymark * tmd, int fd);


/// @ingroup network
/// Polls the set of selected file desriptors for changes in state.
///
/// @param[in]  tmd     Tally mark state data.
/// @param[in]  fd      file descriptor to add to polling set.
/// @return  Returns 0 on success and the value of errno if an error is encountered.
/// @see tallymark_poll, tallymark_add_fd
_TALLYMARK_F int tallymark_del_fd(tallymark * tmd, int fd);


/// @ingroup network
/// Polls the set of selected file desriptors for changes in state.
///
/// @param[in]  tmd     Tally mark state data.
/// @param[in]  timeout time out limit in seconds.
/// @param[out] msg     parsed received message
/// @return Returns the full version information has a dot delimited string.
/// @see tallymark_add_fd, tallymark_del_fd
_TALLYMARK_F int tallymark_poll(tallymark * tmd, int timeout,
   tallymark_msg * msg, struct sockaddr * address,
   socklen_t * address_len);

/**
 *  @defgroup message Protocol Message Functions
 *  @brief Functions for parsing protocol messages.
 */
#ifdef __TALLYMARK_PMARK
#pragma mark Protocol Message Prototypes
#endif

_TALLYMARK_F int tallymark_msg_alloc(tallymark_msg ** pmsg);

_TALLYMARK_F int tallymark_msg_create_header(tallymark_msg * msg,
   uint32_t req_id, uint32_t srv_id, uint32_t fld_id,
   const uint8_t * hash, size_t hash_len);

_TALLYMARK_F int tallymark_msg_errnum(tallymark_msg * msg);

_TALLYMARK_F void tallymark_msg_free(tallymark_msg * msg);

_TALLYMARK_F int tallymark_msg_get_header(tallymark_msg * msg, int header,
   void * outvalue, size_t * outvalue_size);

_TALLYMARK_F int tallymark_msg_get_param(tallymark_msg * msg, int param,
   void * outvalue, size_t * outvalue_size);

_TALLYMARK_F int tallymark_msg_header_errors(int * poffset, int * plen);

_TALLYMARK_F int tallymark_msg_recvfrom(int s, tallymark_msg * msg,
   struct sockaddr * address, socklen_t * address_len);

_TALLYMARK_F int tallymark_msg_reset(tallymark_msg * msg);

_TALLYMARK_F ssize_t tallymark_msg_sendto(int s, tallymark_msg * msg,
   const struct sockaddr * dest_addr, socklen_t dest_len);

_TALLYMARK_F int tallymark_msg_set_header(tallymark_msg * msg, int header,
   const void * invalue, size_t invalue_size);

_TALLYMARK_F int tallymark_msg_set_param(tallymark_msg * msg, int param,
   const void * invalue, size_t invalue_size);

/**
 *  @defgroup url URL Functions
 *  @brief Functions for parsing URLs
 */
#ifdef __TALLYMARK_PMARK
#pragma mark URL Prototypes
#endif

_TALLYMARK_F void tallymark_url_free(tallymark_url_desc * tudp);
_TALLYMARK_F int tallymark_url_initialize(void);
_TALLYMARK_F int tallymark_is_url(const char * urlstr);
_TALLYMARK_F int tallymark_url_parse(const char * urlstr,
   tallymark_url_desc ** tudpp, int resolve);
_TALLYMARK_F int tallymark_url_resolve(tallymark_url_desc * tudp);


/**
 *  @defgroup version Version Functions
 *  @brief Functions for determine version and capabilities of library.
 */
#ifdef __TALLYMARK_PMARK
#pragma mark Version Prototypes
#endif

/// @ingroup version
/// Retrieve the API version implemented by the tally mark library.
///
/// @param[out] pcur    Pointer for storing the current API version.
/// @param[out] prev    Pointer for storing the current revision of the API
///                     implementation.
/// @param[out] page    Pointer for storing the number of previous API versions
///                     which are compatible with the current API version.
/// @return Returns an ASCII string of the API version for use by libtool.
/// @see tallymark_pkg_version
_TALLYMARK_F const char * tallymark_lib_version(uint32_t * pcur,
   uint32_t * prev, uint32_t * page);


/// @ingroup version
/// Retrieve the version of the Tally Mark Daemon client library.
///
/// @param[out] pmaj    Returns reference to major version number.
/// @param[out] pmin    Returns reference to minor version number.
/// @param[out] ppat    Returns reference to patch level.
/// @param[out] pbuild  Returns reference to build commit.
/// @return Returns the full version information has a dot delimited string.
/// @see tallymark_lib_version
_TALLYMARK_F const char * tallymark_pkg_version(uint32_t * pmaj,
   uint32_t * pmin, uint32_t * ppat, const char ** pbuild);

#endif /* end of header */
