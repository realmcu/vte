####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sockets test.
#
####################################################################################################
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
#V.Khalabuda/hlbv001          20/07/2004     TLSbo40419  Initial version
#L.Delaspre/rc149c            13/09/2004     TLSbo42022  VTE 1.5 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#V.Khalabuda/hlbv001          28/02/2005     TLSbo45061  I2C updates for L26.1.7
#L.Delaspre/rc149c            19/04/2005     TLSbo48760  VTE 1.9 integration
#V.Khalabuda/b00306           04/07/2006     TLSbo68945  Update testapp for I2C_RDWR ioctl
#                                                        and r/w from user space
#
####################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../..

CFLAGS    +=    -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include
LOADLIBES +=    -L$(LTP_DIR)/lib -lltp

SRCS = i2c_main.c i2c_test.c
OBJS = i2c_main.o i2c_test.o

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET = i2c_testapp_i2c-0_eeprom

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                       **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN: $(TARGET)                      **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) $(BIN_DIR)/bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                   **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : i2c_test.h
