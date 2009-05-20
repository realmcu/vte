###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the v4l_output test.
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
#-------------------------   ------------    ----------  ------------------------------------------
#KHOROSHEV.D                 09/29/2005      TLSbo55077  Initial version
#
###################################################################################################
#Portability: ARM GCC
###################################################################################################

CFLAGS       += -Wall -I ../../../../include
LOADLIBES    += -L../../../../lib -lltp -lm
PROGRAM =
LIBRARY =

#SRCS = $(wildcard *.c)
#OBJS = $(patsubst %.c,%.o,$(SRCS))

#TARGET = mxc_v4l2_overlay mxc_v4l2_capture mxc_v4l2_output mxc_v4l2_still
TARGET = mxc_v4l2_output.sh mxc_v4l2_overlay.sh

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : V4L2 unit test                  **"
	@echo "***********************************************"
	@echo

install:
	@set -e ; for i in $(TARGET) ; do ln -f $$i ../../../bin ; done
	@echo "***********************************************"
	@echo "** INSTALL : V4L2 Basic testapp              **"
	@echo "***********************************************"
	@echo

clean:


#$(OBJS) : $(SRCS)
