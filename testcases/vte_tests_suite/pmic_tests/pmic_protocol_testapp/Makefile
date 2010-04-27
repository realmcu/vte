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
#    @file   Makefile
#
#    @brief  Makefile to compile the pmic_protocol_testapp test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Khoroshev/b00313           07/20/2005     TLSbo52700  Initial version
#D.Khoroshev/b00313           11/17/2005     TLSbo58274  Update for additional DTS document
#D.Khoroshev/b00313           01/13/2006     TLSbo59968  Added config files for MC13783
#D.Khoroshev/b00313           02/15/2006     TLSbo59968  Returned test module for MC13783
#
###################################################################################################
#Portability: ARM GCC
###################################################################################################
LTP_DIR=../../../..
BIN_DIR=../../../bin

CFLAGS +=    -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include 
LOADLIBES+=     -L$(LTP_DIR)/lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=pmic_protocol_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)               **"
	@echo "***********************************************"
	@echo

install:
	cp -f $(TARGET) $(CONFIG_FILES) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)           **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET) 
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)             **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : pmic_test.h
