###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the watchdog_testapp_1 test.
#
###################################################################################################
#
#    Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Gamma Gao/B14842    12/24/2007    design for MX37 platform test
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS += -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
LOADLIBES += -L../../../../lib -lltp

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=watchdog_testapp_3
COMPRESSED_TARGET=watchdog_testapp_3.gz

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                  **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)              **"
	@echo "***********************************************"
	@echo
	@echo "**** bin_dir is $(BIN_DIR) ***"
#	ln -f $(TARGET) $(BIN_DIR)/bin

clean:
	rm -f $(OBJS) $(TARGET) $(COMPRESSED_TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : watchdog_test_3.h
