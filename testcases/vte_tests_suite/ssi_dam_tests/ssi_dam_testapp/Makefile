#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the SSI DAM test application.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Tony THOMASSIN/RB595C        29/07/2004     TLSbo41151  SSI/DAM test development 
#L.Delaspre/rc149c            09/12/2004     TLSbo40142  update with Freescale identity
#V.Halabuda/hlbv001           05/08/2005     TLSbo53363  update for linux-2.6.10-rel-1.12.arm
#D.Khoroshev/b00313           12/01/2005     TLSbo56844  Add SC55112 support
#D.Simakov                    05/06/2006     TLSbo67103  No sound on SSI 1 and SSI 2
#D.Kardakov                   11/09/2006     TLSbo71015  update for L26_21 release
####################################################################################################
#Portability: ARM GCC 
#
####################################################################################################
BIN_DIR = ../../../bin
CFLAGS += -Wall -I../../../../include -I$(KLINUX_SRCDIR)/drivers/mxc/ssi -I$(KLINUX_SRCDIR)/drivers/mxc/dam 
CFLAGS += -I$(KLINUX_SRCDIR)/include/ -I$(KLINUX_BLTDIR)/include
LOADLIBES += -L../../../../lib -lltp

CFLAGS += -I../pmic_module

ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+= -DCONFIG_MXC_PMIC_MC13783
endif

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=ssi_dam_testapp
#SAMPLES =

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : ssi_dam_testapp test application    **"
	@echo "***********************************************"
	@echo

install:
	cp -f $(TARGET) $(BIN_DIR)
	#cp -f $(SAMPLES) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : ssi_dam_testapp                 **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : ssi_dam_testapp                   **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : ssi_dam_test.h
