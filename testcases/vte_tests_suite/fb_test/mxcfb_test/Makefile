#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################
#KLINUX_SRCDIR=/home/vobs/linux-2.6-imx
#KLINUX_BLTDIR=/home/vobs/linux-2.6-imx

CFLAGS+=	-Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=mxcfb_test
 
all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : mxcfb test application       **"
	@echo "***********************************************"
	@echo

install:
	@echo "***********************************************"
	@echo "** INSTALL : lcd_testapp_power               **"
	@echo "***********************************************"
	ln -f $(TARGET) ../../../bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : mxcfb_test.h
