###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the systeat_testapp1 test.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#     
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   -----------------------------
#D.Simakov             05/04/2007   ENGR37674    Initial version
#
###################################################################################################

LTP_DIR = ../../../..
BIN_DIR = ../../../bin
MODULE_DIR = ../../security_tests/sahara_module
APIINCDIR = $(KLINUX_SRCDIR)/drivers/mxc/security/sahara2/include

ifeq ($(ARCH_PLATFORM), mxc27530evb)
CFLAGS += -DCONFIG_ARCH_MXC91231
endif

ifeq ($(ARCH_PLATFORM), $(filter mxc30030evb mxc30030ads  mxc30031ads, $(ARCH_PLATFORM)))
CFLAGS += -DCONFIG_ARCH_MXC91321
endif

ifeq ($(ARCH_PLATFORM), imx27ads)
CFLAGS += -DCONFIG_ARCH_MX27
endif

CFLAGS += -Wall -DSAHARA -DLINUX -I$(LTP_DIR)/include -I$(MODULE_DIR) -I$(APIINCDIR) -I$(KLINUX_BLTDIR)/include

CFLAGS     += -DDEBUG_TEST 
LOADLIBES  += -L$(LTP_DIR)/lib -lltp -pthread

PROGRAM = 
LIBRARY =

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = systest_testapp1


all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : systest_testapp1                    **"
	@echo "***********************************************"
	@echo

install:
	cp -f $(TARGET) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : systest_testapp1                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)




