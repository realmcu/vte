#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
CC=$(CROSS_COMPILER)gcc
AR=$(CROSS_COMPILER)ar

INCLUDE = -I$(KLINUX_SRCDIR)/include
INC = $(INCLUDE) -I../ipu

OBJS = ScreenLayer.o

LIBNAME=libIpuScreenLayer

all: $(LIBNAME).so

install:
	echo "Caution !!! need check with unit test library version"

$(LIBNAME).so: $(OBJS)
	$(CC) -shared -nostartfiles -Wl,-soname,$@ -o $@ $^ -L../ipu -lipu

%.o: %.c
	$(CC) $(INC) -Wall -O2 -c $^ -o $@

.PHONY: clean
clean:
	@rm -f $(OBJS) $(LIBNAME).*
