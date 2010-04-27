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
#    @file   Makefile
#
#    @brief  Rules for compile the fec_testapp test.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Khoroshev/b00313           10/05/2006     TLSbo76803  Initial version
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
BIN_DIR = ../../bin

CFLAGS+= -Wall -I../../../include -I$(KLINUX_SRCDIR)/include \
         -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/drivers/net \
		 -I$(KLINUX_SRCDIR)/arch/arm/include
LOADLIBES+=     -L../../../lib -lltp

ifeq ($(ARCH_PLATFORM), mxc91131evb)
CFLAGS+=        -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+=        -DCONFIG_MXC_MC13783_LEGACY
endif

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=fec_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "****************************************************"
	@echo "** ALL : $(TARGET) test application             **"
	@echo "****************************************************"
	@echo

install:
	cp -f $(TARGET) $(BIN_DIR)
	@echo "****************************************************"
	@echo "** INSTALL : $(TARGET)                          **"
	@echo "****************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "****************************************************"
	@echo "** CLEAN : $(TARGET)                            **"
	@echo "****************************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)
