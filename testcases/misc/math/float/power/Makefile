<<<<<<< HEAD
#
#    testcases/misc/math/float/power test suite Makefile.
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
# Garrett Cooper, September 2009
#

top_srcdir		?= ../../../../..

include $(top_srcdir)/include/mk/env_pre.mk

CLEAN_TARGETS		:= *.ref*

LDLIBS			+= -lm

include $(top_srcdir)/include/mk/generic_leaf_target.mk
=======
CFLAGS += -Wall
LOADLIBES = -lm

BIN = gensqrt genpow genceil genfabs genfloor genfmod genpower
REF = 1fmod_inp.ref  ceil_inp.ref  fabs_inp.ref  floor_inp.ref  fmod_inp.ref  pow_inp.ref  sqrt_inp.ref 1pow_inp.ref   ceil_out.ref  fabs_out.ref  floor_out.ref  fmod_out.ref  pow_out.ref  sqrt_out.ref

all:	$(BIN)


clean:
	rm -f $(BIN)
	@set -e; for i in $(BIN); do rm -f ../../../../bin/$$i ; done
	rm -f *.ref
	@set -e; for i in $(REF); do rm -f ../../../../bin/$$i ; done

install:
	@set -e; for i in $(BIN); do ln -f $$i ../../../../bin/$$i ; done
>>>>>>> vte 20080401
