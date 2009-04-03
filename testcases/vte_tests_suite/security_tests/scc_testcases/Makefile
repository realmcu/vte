####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the scc testcases.
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
#A.Urusov/NONE                27/09/2005     TLSbo55835  Initial version
#A.Urusov/NONE                14/10/2005     TLSbo55835  Update
#
####################################################################################################
LTP_DIR = ../../../..

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

CFLAGS += -I$(LTP_DIR)/include \
          -I$(KLINUX_SRCDIR)/drivers/mxc/security \
          -I../scc_test_module \
          -I$(KLINUX_SRCDIR)/include/asm-arm/arch-mxc 

LOADLIBES = -L$(LTP_DIR)/lib -lltp

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=scc_test

all: $(TARGET)
	@rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : scc test application                **"
	@echo "***********************************************"
	@echo

install:
	@cp $(TARGET) ../../../bin
	@cp -f scc_scr.sh scc_test.sh ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : scc test application            **"
	@echo "***********************************************"
	@echo
clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN:  scc test application              **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : scc_test.h
