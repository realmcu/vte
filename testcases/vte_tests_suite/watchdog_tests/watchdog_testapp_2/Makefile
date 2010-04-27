#Copyright (C) 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the watchdog_testapp_1 test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Khalabuda/b00306           06/07/2006     TLSbo69495  Update for MX27 platform in accordance with changes in time.c file
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
TARGET=watchdog_testapp_2
COMPRESSED_TARGET=watchdog_testapp_2.gz

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

clean:
	rm -f $(OBJS) $(TARGET) $(COMPRESSED_TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : watchdog_test_2.h
