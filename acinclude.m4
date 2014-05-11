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
#   acinclude.m4 - custom m4 macros used by configure.ac
#


# AC_TRENDSONAR_COMPONENTS()
# -----------------------------------
AC_DEFUN([AC_TRENDSONAR_COMPONENTS],[dnl

   enableval=""
   AC_ARG_ENABLE(
      client,
      [AS_HELP_STRING([--enable-client], [enable client [default=yes]])],
      [ ECLIENT=$enableval ],
      [ ECLIENT=$enableval ]
   )
   enableval=""
   AC_ARG_ENABLE(
      daemon,
      [AS_HELP_STRING([--enable-daemon], [enable collector daemon [default=yes]])],
      [ EDAEMON=$enableval ],
      [ EDAEMON=$enableval ]
   )
   enableval=""
   AC_ARG_ENABLE(
      library,
      [AS_HELP_STRING([--enable-library], [enable client library [default=yes]])],
      [ ELIBRARY=$enableval ],
      [ ELIBRARY=$enableval ]
   )

   ENABLE_CLIENT=yes
   if test "x${ECLIENT}" = "xno";then
      ENABLE_CLIENT=no
   fi
   ENABLE_DAEMON=yes
   if test "x${EDAEMON}" = "xno";then
      ENABLE_DAEMON=no
   fi
   ENABLE_LIBRARY=yes
   if test "x${ELIBRARY}" = "xno";then
      ENABLE_LIBRARY=no
   fi

   AM_CONDITIONAL([ENABLE_CLIENT],  [test "${ENABLE_CLIENT}" == "yes"])
   AM_CONDITIONAL([ENABLE_DAEMON],  [test "${ENABLE_DAEMON}" == "yes"])
   AM_CONDITIONAL([ENABLE_LIBRARY], [test "${ENABLE_LIBRARY}" == "yes"])
])dnl


# end of m4 file
