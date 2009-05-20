###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the mlb_test.
#
###################################################################################################
#
#        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number     Description of Changes
#-------------------------   ------------    ----------   -----------------------------------------
#Hake Huang                    07/11/2008       NA          Initial version

###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../
BIN_DIR = $(LTP_DIR)testcases/bin/

CFLAGS += -Wall -I$(LTP_DIR)include
LIBRARY = -L$(LTP_DIR)lib -lltp

SRCS   = $(wildcard *.c)
OBJS   = $(patsubst %.c,%.o,$(SRCS))

TARGET = mxc_mlb_test
SCRIPT = mlb_test.sh


all:$(TARGET)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo


install:
	ln -f $(TARGET) $(BIN_DIR)
	ln -f $(SCRIPT) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

clean:
	rm -rf $(OBJS) $(TARGET)


$(TARGET): $(OBJS)
	$(CC)  $(OBJS) $(LIBRARY) -o  $@

$(OBJS) :
	$(CC) $(CFLAGS) -c $(SRCS) -o $@

