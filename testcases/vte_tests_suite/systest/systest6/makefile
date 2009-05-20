###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the systeat_testapp6 test.
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
#D.Simakov             06/06/2007   ENGR37682   Initial version
#
###################################################################################################

LTP_DIR = ../../../..
BIN_DIR = ../../../bin
DUMP_FILES = output_video_yuv420

CFLAGS     += -Wall -I$(LTP_DIR)/include
CFLAGS     += -DDEBUG_TEST
LOADLIBES  += -L$(LTP_DIR)/lib -lltp -pthread

PROGRAM =
LIBRARY =

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = systest_testapp6


all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : systest_testapp6                    **"
	@echo "***********************************************"
	@echo

install:
	cp -f $(TARGET) $(BIN_DIR)
	cp -f $(DUMP_FILES) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : systest_testapp6                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)




