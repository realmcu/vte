###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the drawrectangle_testapp_X test.
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
#Developer Name/ID            DD/MM/YYYY     TLSboXXXXX   BRIEF description of changes made
# Ludovic Delaspre/rc149c     27/10/2004     TLSbo44018   make GTK compilation available
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=drawrectangle

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : drawrectangle test application      **"
	@echo "***********************************************"
	@echo

install:
	@echo "***********************************************"
	@echo "** INSTALL : drawrectangle test application  **"
	@echo "***********************************************"
	ln -f $(TARGET) ../../../bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : drawrectangle_test_X.h
