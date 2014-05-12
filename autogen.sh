#!/bin/sh
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
#   autogen.sh - runs GNU Autotools to create build environment
#

AUTOGENNAME="`basename ${0}`" || exit 1
SRCDIR="`dirname ${0}`"
cd $SRCDIR || exit 1


# check for required programs
for TEST_PROG in which autoreconf autoscan find git;do
   which ${TEST_PROG} 2> /dev/null > /dev/null;
   if test $? -ne 0;then
      echo "${AUTOGENNAME}: unable to find \"${TEST_PROG}\""
      exit 1
   fi
done


# updates git repository
if test -d .git || test -f .git;then
   git submodule init           2>&1 | sed -e 's/^/git submodule init: /g'   || exit 1
   git submodule sync           2>&1 | sed -e 's/^/git submodule sync: /g'   || exit 1
   git submodule update --merge 2>&1 | sed -e 's/^/git submodule update: /g' || exit 1
fi


# Performs some useful checks
autoscan . || exit 1


# generates/installs autotools files
autoreconf -v -i -f -Wall \
   -I m4 \
   -I contrib/bindletools/m4 \
   -m \
   . \
   || exit 1


# makes build directory
mkdir -p build


# end of script