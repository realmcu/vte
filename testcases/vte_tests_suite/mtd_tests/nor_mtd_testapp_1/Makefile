###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the nor_mtd_testapp_1 test.
#
###################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
###################################################################################################
# Revision History:
#                             Modification     Tracking
# Author/core ID                  Date          Number    Description of Changes
#-------------------------    ------------    ----------  -----------------------------------------
# V Becker/rc023c              04/05/2004     TLSbo39142  Initial version
# V Becker/rc023c              26/07/2004     TLSbo39142  Update include path
# L.Delaspre/rc149c            08/12/2004     TLSbo40142  Update with Freescale identity
# A.Urusov/NONE                13/02/2006     TLSbo61868  Warnings fixup, code formatting
# Rakesh S Joshi/R65956        25/07/2007     ENGR42510   added Cflags for MXC92323 Arch
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS+= -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
LOADLIBES+=	-L../../../../lib -lltp
include $(KLINUX_BLTDIR)/.config

ifeq "$(CONFIG_ARCH_MXC92323)" "y"
CFLAGS += -DCONFIG_ARCH_MXC92323
endif

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=nor_mtd_testapp_1
COMPRESSED_TARGET=nor_mtd_testapp_1.gz

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET) test application  **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)               **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET) $(COMPRESSED_TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                 **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : nor_mtd_test_1.h
