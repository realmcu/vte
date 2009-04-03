####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the CSPI module driver.
#
####################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Smirnov                    16/06/2005     TLSbo51450  Initial version
#V.Khalabuda/hlbv001          30/06/2005     TLSbo52341  Update version for linux-2.6.11-CVS and fix
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
#CFLAGS    += -Wall -I$(KLINUX_SRCDIR)/drivers/usb/gadget
EXTRA_CFLAGS += -Wall -I$(KLINUX_SRCDIR)/drivers/usb/gadget
LOADLIBES +=

obj-m := usbstillimage.o
usbstillimage-objs := pima.o stillimage.o sys.o driver.o
TARGET = usbstillimage.ko

all:
	make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	rm -rf .*.cmd *.o *.mod.c *.mod.ko .tmp_versions
	@echo "************************************************"
	@echo "** ALL :     $(TARGET)               **"
	@echo "************************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "*************************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "*************************************************"
	@echo

clean:
	rm -f .*.cmd *.o *.mod.c *.mod.ko *.ko
	rm -rf .tmp_versions
	@echo "*************************************************"
	@echo "** CLEAN :   $(TARGET)                **"
	@echo "*************************************************"
	@echo
