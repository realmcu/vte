#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
####The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

CC = $(CROSS_COMPILER)gcc
CFLAGS += -O3 -I$(KLINUX_SRCDIR)/include -I../../../../../include
LDFLAGS +=  -L../../../../../lib -lltp 

TARGET = imx_fb 
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))


all:$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)
	 
install:
	-ln -f $(TARGET) ../../../../bin

clean:
	-rm -f $(TARGET) *.o

$(OBJS) : $(SRCS)
