<<<<<<< HEAD
#
#    misc/math/float test suite Makefile.
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

top_srcdir	?= ../../../..

include $(top_srcdir)/include/mk/testcases.mk

LDLIBS		+= -lpthread -lm

# main.c doesn't compile...
MAKE_TARGETS	:= $(patsubst $(abs_srcdir)/%.c,%,$(wildcard $(abs_srcdir)/float*.c))

include $(top_srcdir)/include/mk/generic_trunk_target.mk
=======
CFLAGS+= -I../../../../include -Wall -g 
LOADLIBES+= -lpthread -lm -L../../../../lib -lltp

SRCS=$(wildcard float*.c)
TARGETS=$(patsubst %.c,%,$(SRCS))

SUBDIRS=bessel exp_log iperb power trigo

all: $(TARGETS)
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@set -e; for i in $(TARGETS); do ln -f $$i ../../../bin/$$i; done

clean: 
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	rm -f $(TARGETS) 
	@set -e; for i in $(TARGETS); do rm -rf ../../../bin/$$i ; done

>>>>>>> vte 20080401
