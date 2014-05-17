#
#   Tally Mark Daemon - An aggregated time-based counter for disparate systems
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
#   @configure_input@
#
#   Makefile.am - automate compiling on a unix platform
#

# Sub directories
SUBDIRS					=
DIST_SUBDIRS				=


# Global flags
AM_LIBS					=
AM_LDFLAGS				=
AM_CFLAGS				= -O2 $(CFLAGS_WARNINGS)
AM_CXXFLAGS				= -O2 @AM_CXXFLAGS@
AM_OBJCFLAGS				= -O2 @AM_OBJCFLAGS@
AM_CPPFLAGS				= -O2 -UPMARK -DHAVE_CONFIG_H=1 \
					  -I$(top_builddir)/include \
					  -I$(top_srcdir)/include \
					  -DSYSCONFDIR="\"$(sysconfdir)\""
ACLOCAL_AMFLAGS				= -I m4 -W all -W error
AM_MAKEINFOFLAGS                        = --no-split
DEFS					=


# automake targets
check_PROGRAMS				=
doc_DATA				= README COPYING ChangeLog AUTHORS
include_HEADERS				=
lib_LTLIBRARIES				=
man_MANS				=
info_TEXINFOS				=
noinst_LTLIBRARIES			=
noinst_HEADERS				=
noinst_PROGRAMS				= src/socket-client \
					  src/socket-server
bin_PROGRAMS				=
bin_SCRIPTS				=
sbin_SCRIPTS				=


# lists
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT	=
BUILT_SOURCES				=
TESTS					=
XFAIL_TESTS				=
EXTRA_MANS				=
EXTRA_DIST				= lib/libtallymark/libtallymark.sym
CLEANFILES				= \
					  $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  config.h.in~ $(srcdir)/config.h.in~ \
					  $(man_MANS) \
					  $(pkgdata_DATA) \
					  $(bin_SCRIPTS) \
					  $(sbin_SCRIPTS) \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.txz \
					  @PACKAGE_TARNAME@-*.zip
DISTCHECK_CONFIGURE_FLAGS		= --enable-strictwarnings


# macros for lib/libtallymark.la
lib_libtallymark_la_DEPENDENCIES	= Makefile lib/libtallymark/libtallymark.sym
lib_libtallymark_la_LDFLAGS		= -rpath'$(libdir)'\
					  -version-info $(LIB_VERSION_INFO) \
					  $(AM_LDFLAGS) \
					  -export-symbols $(srcdir)/lib/libtallymark/libtallymark.sym
lib_libtallymark_la_SOURCES		= \
					  lib/libtallymark/libtallymark.h \
					  lib/libtallymark/fdpoller.h \
					  lib/libtallymark/error.c \
					  lib/libtallymark/error.h \
					  lib/libtallymark/memory.c \
					  lib/libtallymark/memory.h \
					  lib/libtallymark/message.c \
					  lib/libtallymark/message.h \
					  lib/libtallymark/thread.c \
					  lib/libtallymark/thread.h \
					  lib/libtallymark/version.c \
					  lib/libtallymark/version.h
if USE_POLL
lib_libtallymark_la_SOURCES		+= lib/libtallymark/fdpoller-poll.c
endif
if ENABLE_LIBTALLYMARK
include_HEADERS				+= include/tallymark.h
include_HEADERS				+= include/tallymark_cdefs.h
lib_LTLIBRARIES				+= lib/libtallymark.la
else
noinst_HEADERS				+= include/tallymark.h
noinst_HEADERS				+= include/tallymark_cdefs.h
noinst_LTLIBRARIES			+= lib/libtallymark.la
endif


# macros for src/client
src_socket_client_DEPENDENCIES		= $(lib_LTLIBRARIES) $(noinst_LTLIBRARIES) Makefile
src_socket_client_SOURCES		= $(noinst_HEADERS) src/sockets/socket-client.c


# macros for src/socket-server
src_socket_server_DEPENDENCIES		= $(lib_LTLIBRARIES) $(noinst_LTLIBRARIES) Makefile
src_socket_server_SOURCES		= $(noinst_HEADERS) src/sockets/socket-server.c


# macros for src/tallymarker
bin_tallymarker_DEPENDENCIES		= $(lib_LTLIBRARIES) $(noinst_LTLIBRARIES) Makefile
bin_tallymarker_LDADD			= lib/libtallymark.la
bin_tallymarker_SOURCES			= $(noinst_HEADERS) \
					  src/tallymarker/tallymarker.h \
					  src/tallymarker/main.c
if ENABLE_TALLYMARKER
bin_PROGRAMS				+= bin/tallymarker
endif


# macros for src/tallymarked
bin_tallymarked_DEPENDENCIES		= $(lib_LTLIBRARIES) $(noinst_LTLIBRARIES) Makefile
bin_tallymarked_LDADD			= lib/libtallymark.la
bin_tallymarked_SOURCES			= $(noinst_HEADERS) \
					  src/tallymarked/tallymarked.h \
					  src/tallymarked/main.c \
					  src/tallymarked/memory.c \
					  src/tallymarked/memory.h
if ENABLE_TALLYMARKED
bin_PROGRAMS				+= bin/tallymarked
endif


# Makefile includes
GIT_PACKAGE_VERSION_DIR=include
SUBST_EXPRESSIONS =
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-autotools.am
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-subst.am
include $(srcdir)/@bindletools_srcdir@/build-aux/makefile-version.am


# custom targets
.PHONY:


# local targets
install-exec-local:

install-data-local:

install-data-hook:

uninstall-local:

clean-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache

# end of automake file
