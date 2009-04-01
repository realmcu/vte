####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the pmic_rtc test.
#
####################################################################################################
#
#        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
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

CFLAGS += -Wall -I$(LTP_DIR)/include -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/include
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

