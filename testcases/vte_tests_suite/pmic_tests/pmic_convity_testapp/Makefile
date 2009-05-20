###################################################################################################
#
#        @file   Makefile
#
#        @brief  Configuration file to compile the pmic_convity_testapp.
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
#Author/core ID                  Date          Number     Description of Changes
#-------------------------   ------------    ----------   -------------------------------------------
#V.Halabuda/hlbv001           08/16/2005     TLSbo52695   Initial version
#V.Halabuda/hlbv001           02/09/2005     TLSbo58397   Update for linux-2.6.10-rel-L26_1_14
#A.Ozerov/b00320              15/05/2006     TLSbo64237   Code was cast in accordance to coding conventions.
#A.Ozerov/b00320              10/08/2006     TLSbo74269   Conditional compilation was added.
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../../
BIN_DIR = ../../../
MC13783_LEGACY_DIR = $(KLINUX_SRCDIR)/drivers/mxc/mc13783_legacy

ifeq ($(ARCH_PLATFORM), mxc91131evb)
CFLAGS += -DCONFIG_MXC_PMIC_SC55112 -Wall -I$(LTP_DIR)include -I$(KLINUX_BLTDIR)/include
else
CFLAGS += -DCONFIG_MXC_PMIC_MC13783 -Wall -I$(LTP_DIR)include -I$(KLINUX_BLTDIR)/include
endif

LOADLIBES += -L$(LTP_DIR)lib -lltp

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = pmic_convity_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***************************************************"
	@echo "** ALL : pmic_convity_testapp                    **"
	@echo "***************************************************"
	@echo

install:
	ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***************************************************"
	@echo "** INSTALL : pmic_convity_testapp                **"
	@echo "***************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***************************************************"
	@echo "** CLEAN : pmic_convity_testapp                  **"
	@echo "***************************************************"
	@echo
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : pmic_convity_test.h
