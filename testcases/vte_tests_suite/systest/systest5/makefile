###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the systeat_testapp1 test.
#
###################################################################################################
#
#   Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#     
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   -----------------------------
#D.Kardakov             13/06/2007  ENGR37681    Initial version
#
###################################################################################################

LTP_DIR = ../../../..
BIN_DIR = ../../../bin

CFLAGS += -Wall -DLINUX -I$(LTP_DIR)/include

CFLAGS     += -DDEBUG_TEST 
LOADLIBES  += -L$(LTP_DIR)/lib -lltp -pthread

PROGRAM = 
LIBRARY =

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = systest_testapp5


all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : systest_testapp5                    **"
	@echo "***********************************************"
	@echo

install:
	cp -f $(TARGET) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : systest_testapp5                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)




