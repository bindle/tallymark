#
#   Trend Sonar
#   Copyright (c) 2014, Bindle Binaries
#   All rights reserved.
#
#   @BINDLE_BINARIES_BSD_LICENSE_START@
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#      1. Redistributions of source code must retain the above copyright
#         notice, this list of conditions and the following disclaimer.
#
#      2. Redistributions in binary form must reproduce the above copyright
#         notice, this list of conditions and the following disclaimer in the
#         documentation and/or other materials provided with the distribution.
#
#      3. Neither the name of the copyright holder nor the names of its
#         contributors may be used to endorse or promote products derived from
#         this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#   @BINDLE_BINARIES_BSD_LICENSE_END@
#
#   configure.in - automate compiling on a unix platform
#

AC_PREREQ([2.65])

AC_COPYRIGHT([Copyright (c) 2014, Bindle Binaries])
AC_REVISION(0.0)
AC_INIT([Trend Sonar],[0.0],[syzdek@bindlebinaries.com],[trendsonar])

AC_SUBST([RELEASE_MONTH],["May 2014"])

# Version Info:
#    Current    -- the current version number of this API
#    Revision   -- the revision of the implementation of the API version
#    Age        -- How many seqential past API versions is supported by
#                  this implementation
# Format => Current:Revision:Age
# Convenience macro:
#    AC_BINDLE_LIBTOOL_VERSION_INFO(current, revision, age)
AC_BINDLE_LIBTOOL_VERSION_INFO(0, 0, 0)

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([build-aux/git-tar-name.txt])
#AC_CONFIG_LIBOBJ_DIR(compat)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# determines host type
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# configures for automake
AM_INIT_AUTOMAKE(gnu std-options subdir-objects)

# Compilers
AC_PROG_CC
#AC_PROG_OBJC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_USE_SYSTEM_EXTENSIONS # replaces AC_AIX AC_GNU_SOURCE AC_MINIX
AC_C_BIGENDIAN

# binary locations
AC_PROG_INSTALL
AC_PATH_PROG([SHELL],   sh bash,        [AC_MSG_ERROR([missing "sh"])], [$PATH])
AC_CHECK_TOOLS(AR,      ar gar,         [AC_MSG_ERROR([missing binutil "ar"])])
AC_CHECK_TOOLS(LD,      ld gld,         [AC_MSG_ERROR([missing binutil "ld"])])
AC_CHECK_TOOLS(NM,      nm gnm,         [AC_MSG_ERROR([missing binutil "nm"])])
AC_CHECK_TOOLS(RANLIB,  ranlib granlib, [AC_MSG_ERROR([missing binutil "ranlib"])])
AC_CHECK_TOOLS(STRIP,   strip gstrip,   [AC_MSG_ERROR([missing binutil "strip"])])

# shell programs
AC_PROG_AWK
AC_PROG_EGREP
AC_PROG_FGREP
AM_PROG_LEX
AC_PROG_LN_S

# GNU Libtool Support
LT_INIT(dlopen disable-fast-install win32-dll)

# initiates bindle tools macros
AC_BINDLE(contrib/bindletools)

# Type checks
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# custom configure options
AC_BINDLE_ENABLE_WARNINGS
AC_TRENDSONAR_COMPONENTS

# enables getopt_long if header and functions were found
AC_CHECK_HEADERS([getopt.h], [AC_DEFINE_UNQUOTED(USE_GETOPT_LONG, 1, [Use GNU getopt_long])])

# determine PACKAGE_VERSION via Git
AC_BINDLE_GIT_PACKAGE_VERSION([contrib/bindletools/build-aux/git-package-version.sh])

# Creates outputs
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

# Show local config
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   Trend Sonar $GIT_PACKAGE_VERSION ($GIT_PACKAGE_BUILD)])
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   Features:])
AC_MSG_NOTICE([      Collector Daemon:         ${ENABLE_DAEMON}])
AC_MSG_NOTICE([      CLI Client:               ${ENABLE_CLIENT}])
AC_MSG_NOTICE([      Client Library:           ${ENABLE_LIBRARY}])
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   Please send suggestions to:   $PACKAGE_BUGREPORT])
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   run 'make all'])
AC_MSG_NOTICE([ ])

# end of configure.ac