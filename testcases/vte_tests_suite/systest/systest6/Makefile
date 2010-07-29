#Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the systeat_testapp6 test.
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
#DUMP_FILES = output_video_yuv420

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
	#cp -f $(DUMP_FILES) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : systest_testapp6                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

