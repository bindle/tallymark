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

#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h>

#include <tallymark_cdefs.h>


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
#ifdef __TALLYMARK_PMARK
#pragma mark - Data Types
#endif

typedef struct tallymark_struct          tallymark;
typedef struct tallymark_message_struct  tallymarkmsg;


struct tallymark_message_struct
{
   uint32_t    magic;
   uint8_t     version_current;
   uint8_t     version_age;
   uint8_t     header_len;
   uint8_t     response_ops;
   uint32_t    request_ops;
   uint32_t    service_id;
   uint32_t    field_id;
   uint8_t     hash_id[20];
   uint32_t    message_len;
   uint8_t     body[128];
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
 *  @defgroup network Network Functions
 *  @brief Functions for assisting with network management.
 */
#ifdef __TALLYMARK_PMARK
#pragma mark Network Prototypes
#endif

/// @ingroup network
/// Polls the set of selected file desriptors for changes in state.
///
/// @param[in]  tmd     Tally mark state data.
/// @param[in]  timeout time out limit in seconds.
/// @return Returns the full version information has a dot delimited string.
/// @see tallymark_add_fd, tallymark_del_fd
_TALLYMARK_F int tallymark_poll(tallymark * tmd, int timeout);


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
