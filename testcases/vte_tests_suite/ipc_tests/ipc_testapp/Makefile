###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the Unified IPC driver test.
#
###################################################################################################
#
#       Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#       THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#       BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#       Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#I.Semenchukov/smng001c       17/02/2005     TLSbo47812  Initial version
#A.Ozerov/b00320              26/04/2006     TLSbo61791  Performs a cast in accordance to coding standarts.
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../..
MOD_DIR = ../ipc_module

CFLAGS    += -Wall -I$(LTP_DIR)/include -I$(MOD_DIR) -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include
LOADLIBES += -L$(LTP_DIR)/lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=ipc_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                         **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) $(BIN_DIR)/bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                     **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                       **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : ipc_test.h
