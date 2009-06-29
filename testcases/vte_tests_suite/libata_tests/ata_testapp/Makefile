#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#        @brief  configuration file to compile the ata_driver_testapp.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Ozerov/b00320              10/09/2006     TLSbo76800  Initial version.
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR=../../../..
BIN_DIR=../../..

CFLAGS+= -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include
LOADLIBES+= -L$(LTP_DIR)/lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=ata_testapp

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : $(TARGET)                 **"
	@echo "************************************************"
	@echo
	rm -f $(OBJS)

install:
	@echo "************************************************"
	@echo "** INSTALL : $(TARGET)             **"
	@echo "************************************************"
	ln -f $(TARGET) $(BIN_DIR)/bin

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "************************************************"
	@echo "** CLEAN : $(TARGET)               **"
	@echo "************************************************"

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : ata_testapp.h
