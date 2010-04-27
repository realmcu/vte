#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the pmic_rtc test.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/Core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#E.Gromazina/NONE             08/12/2005     TLSbo59968   Initial version
#A.Ozerov/b00320              06/07/2006     TLSbo61903   Code was cast to coding standarts.
#Pradeep K/b01016             09/25/2006     TLSboXXXX    Updated for PMIC drivers
#D.Simakov                    07/09/2006     TLSbo67178   iMX27 compilation issue 
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../../

CFLAGS += -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include
LOADLIBES += -L$(LTP_DIR)/lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=pmic_rtc_testapp

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : $(TARGET)                     **"
	@echo "************************************************"
	@echo
	rm -f $(OBJS)

install:
	@echo "************************************************"
	@echo "** INSTALL : $(TARGET)                 **"
	@echo "************************************************"
	ln -f $(TARGET) $(BIN_DIR)/bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "************************************************"
	@echo "** CLEAN : $(TARGET)                   **"
	@echo "************************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : pmic_rtc_test.h

