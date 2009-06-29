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
#        @brief  configuration file to compile the pmic_adc_testapp.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number     Description of Changes
#-------------------------   ------------    ----------   -----------------------------------------
#V. Halabuda/HLBV001          07/25/2005     TLSbo52694   Initial version
#V. HALABUDA/HLBV001          11/21/2005     TLSbo58395   Update for linux-2.6.10-rel-L26_1_14
#E. Gromazina/NONE            12/27/2005     TLSbo59968   Update for MXC91231 and MXC91131 	
#D. Khoroshev/b00313          07/04/2006     TLSbo64235   Added test module compilation
#D. Khoroshev/b00313          07/26/2006     TLSbo64235   Added mc13783 legacy support
#O.DAVARD/b02578		  10/06/2006	   TLSbo80013   mc13783 and sc55112 replaced by pmic

###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../../
BIN_DIR = ../../../

CFLAGS +=      -Wall -I$(LTP_DIR)include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include
LOADLIBES +=	-L$(LTP_DIR)lib -lltp

ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+= -DCONFIG_MXC_PMIC_MC13783
#CFLAGS+= -DCONFIG_MXC_MC13783_LEGACY -I$(KLINUX_SRCDIR)/drivers/mxc/mc13783_legacy/module/ 
endif

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = ts_testapp1

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                  **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : ts_test.h
