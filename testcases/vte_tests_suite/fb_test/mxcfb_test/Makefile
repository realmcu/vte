###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sleep test that checks SLCDC driver by
#            putting device to the various VESA blanking levels.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#I. Semenchukov/smng001c      21/09/2004     TLSbo41672   Initial version
#L. Delaspre/rc149c           08/10/2004     TLSbo42821   VTE 1.5.1 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################
#KLINUX_SRCDIR=/home/vobs/linux-2.6-imx
#KLINUX_BLTDIR=/home/vobs/linux-2.6-imx

CFLAGS+=	-Wall -I../../../../include -I$(KLINUX_BLTDIR)/include
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
