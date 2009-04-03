###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sahara_testapp test.
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
#Author/Core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#A.Ozerov/NONE                10/10/2005     TLSbo55834   Initial version
#A.Urusov/NONE                09/11/2005     TLSbo57926   APIINCDIR is added
#A.Ozerov/NONE                01/12/2005     TLSbo58662   Update for linux-2.6.10-rel-L26_1_14
#D.Simakov/smkd001c           21/09/2006     TLSbo76069   Compilation issue
#                                                        
###################################################################################################
#Portability: ARM GCC 
#
###################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../..
MODULE_DIR = ../sahara_module
APIINCDIR = $(KLINUX_SRCDIR)/drivers/mxc/security/sahara2/include

ifeq ($(ARCH_PLATFORM), mxc27530evb)
CFLAGS += -DCONFIG_ARCH_MXC91231
endif

ifeq ($(ARCH_PLATFORM), $(filter mxc30030evb mxc30030ads , $(ARCH_PLATFORM)))
CFLAGS += -DCONFIG_ARCH_MXC91321
endif

ifeq ($(ARCH_PLATFORM), imx27ads)
CFLAGS += -DCONFIG_ARCH_MX27
endif

CFLAGS += -Wall -DSAHARA -DLINUX -I$(LTP_DIR)/include -I$(MODULE_DIR) -I$(APIINCDIR) -I$(KLINUX_BLTDIR)/include

LOADLIBES += -L$(LTP_DIR)/lib -lltp

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = sahara_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "************************************************"
	@echo "** ALL : $(TARGET)                       **"
	@echo "************************************************"
	@echo
install:
	ln -f $(TARGET) $(BIN_DIR)/bin
	@echo "************************************************"
	@echo "** INSTALL : $(TARGET)                   **"
	@echo "************************************************"
	@echo
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "************************************************"
	@echo "** CLEAN : $(TARGET)                     **"
	@echo "************************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS): $(SRCS)

$(SRCS): sahara_test.h
