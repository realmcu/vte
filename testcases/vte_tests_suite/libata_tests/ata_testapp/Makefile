###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the ata_driver_testapp.
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
