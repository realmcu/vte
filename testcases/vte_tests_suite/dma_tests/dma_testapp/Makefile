###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE DMA test applications.
#
###################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Simakov/b00236            09/01/2006      TLSbo76801  Initial version
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################

MOD_DIR = ../dma_test_module
MXC_DIR = $(KLINUX_SRCDIR)/drivers/mxc

CFLAGS    +=    -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include -I$(MOD_DIR) -I$(MXC_DIR) -DTEST_USER_SPACE
LOADLIBES +=    -L../../../../lib -lltp -lpthread

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = dma_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL:      $(TARGET)                     **"
	@echo "***********************************************"
	@echo

install:
	cp -rf $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                     **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN :   $(TARGET)                     **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

