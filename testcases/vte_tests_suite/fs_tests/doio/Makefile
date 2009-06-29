#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

CFLAGS+= -Wall -I../../../../include -D_LARGEFILE64_SOURCE
LDLIBS+= -L../../../../lib -lltp

TARGETS=growfiles

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

all: $(TARGETS)

growfiles: growfiles.o
		 $(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LDLIBS)

install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i ../../../bin/$$i ; done ;
	ln -f growfiles_list.sh ../../../bin/

clean:
	rm -f $(OBJS) $(TARGETS)

