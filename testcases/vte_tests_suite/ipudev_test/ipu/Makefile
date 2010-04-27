#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#CROSS_COMPILE = arm-none-linux-gnueabi-
#KLINUX_SRCDIR = /home/vobs/linux-2.6-imx

CC=$(CROSS_COMPILER)gcc
AR=$(CROSS_COMPILER)ar

OBJS = mxc_ipu_hl_lib.o mxc_ipu_lib.o

LIBNAME=libipu

INCLUDE = -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include


all: $(LIBNAME).so

install:
	echo "Caution !!! need check with unit test library version"

$(LIBNAME).so: $(OBJS)
	-$(CC) -shared -nostartfiles -Wl,-soname,$@ -o $@ $^ -lpthread

%.o: %.c
	-$(CC)  $(INCLUDE) -Wall -O2 -c $^ -o $@

.PHONY: clean
clean:
	@rm -f $(OBJS) $(LIBNAME).*
