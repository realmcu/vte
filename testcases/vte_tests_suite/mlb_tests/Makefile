#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the mlb_test.
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

CFLAGS += -Wall -I$(LTP_DIR)include -I$(KLINUX_SRCDIR)/include \
          -I$(KLINUX_BLTDIR)/include
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

