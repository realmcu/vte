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

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = v4l_output_testapp


all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : V4L output testapp                  **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin/
	ln -f config/* ../../../bin/
	@echo "***********************************************"
	@echo "** INSTALL : V4L output testapp              **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : v4l_output_test.h