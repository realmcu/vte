###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the spi_testapp_0 test.
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
#Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490  SPI test development 
#Ludovic DELASPRE/RC149C      11/06/2004     TLSbo40109  Replace hard include path by a variable
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
MOD_DIR = ../spi_test_module
MXC_DIR = $(KLINUX_SRCDIR)/drivers/mxc

CFLAGS    +=    -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include -I$(MXC_DIR) -I$(MOD_DIR)
LOADLIBES +=    -L../../../../lib -lltp

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=spi_testapp_0

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL:      $(TARGET)                   **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                   **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN :   $(TARGET)                   **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : spi_test_0.h
