###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the spi_testapp test.
#
###################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Kazachkov/e1403c           16/06/2005     TLSbo51450  Initial version
#V.Khalabuda/hlbv001          30/06/2005     TLSbo52341  Update version for linux-2.6.11-CVS and fix
#V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
MOD_DIR = ../spi_test_module
MXC_DIR = $(KLINUX_SRCDIR)/drivers/mxc

CFLAGS    +=    -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include -I$(MOD_DIR) -I$(MXC_DIR)
LOADLIBES +=    -L../../../../lib -lltp -lpthread

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = spi_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL:      $(TARGET)                     **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
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

$(SRCS) : spi_test.h
