<<<<<<< HEAD
#
#    misc/crash test suite Makefile.
#
#    Copyright (C) 2009, Cisco Systems Inc.
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Garrett Cooper, July 2009
#

top_srcdir		?= ../../..

include $(top_srcdir)/include/mk/testcases.mk

INSTALL_TARGETS		:= tcore.exp tcore.sh

LDLIBS			+= -lm -lpthread

MAKE_TARGETS		:= tcore

#MODCFLAGS		:= -DLINUX -I$(prefix)/usr/src/linux/include
MODCFLAGS		:= -DLINUX

include $(top_srcdir)/include/mk/generic_leaf_target.mk
=======
CC=gcc
MODCFLAGS := -Wall -DLINUX -I/usr/src/linux/include
EXECS=tcore tcore.sh tcore.exp

all:		tcore

install: 
		@set -e; for i in $(EXECS) ; do ln -f $$i ../../bin/$$i ; done ;	

tcore:		tcore.c
		$(CC)  -o tcore tcore.c -lm -lpthread
clean:
	rm -f tcore

>>>>>>> vte 20080401
