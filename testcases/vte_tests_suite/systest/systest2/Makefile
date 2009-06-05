###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the systeat_testapp2 test.
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
#D.Simakov             10/04/2007   ENGR37676    Initial version
#
###################################################################################################

LTP_DIR = ../../../..
BIN_DIR = ../../../bin

CFLAGS     += -Wall -I$(LTP_DIR)/include 
CFLAGS     += -DDEBUG_TEST 
#CLFAGS     += -DIPC_EXCLUDE
LOADLIBES  += -L$(LTP_DIR)/lib -lltp -pthread

PROGRAM = 
LIBRARY =

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = systest_testapp2


all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : systest_testapp2                    **"
	@echo "***********************************************"
	@echo

install:
	cp -f $(TARGET) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : systest_testapp2                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)




