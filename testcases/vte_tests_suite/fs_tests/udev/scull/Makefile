##############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile scull.ko to test udev
#
##############################################################################
#
#   Copyright (C) 2008, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
##############################################################################
#Revision History:
#                            Modification     Tracking
# Author                         Date          Number    Description of Changes
#-------------------------   ------------    ----------  ---------------------
# Spring Zhang                07/02/2008         n/a     Initial version
#
###############################################################################

obj-m := scull.o
TARGETS=scull.ko
all:
	$(MAKE) -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
#$(MAKE) -C $(KERNEL_SRCDIR) M=$(PWD) KBUILD_OUTPUT=$(KERNEL_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules

install:
	@echo "****************************************"
	@echo "** INSTALL : scull.ko                 **"
	@echo "****************************************"
	ln -f $(TARGETS) ../../../../bin/
clean:
	@rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers
	@echo "***********************************************"
	@echo "** CLEAN: UDEV_MODULE                        **"
	@echo "***********************************************"
	@echo


.PHONY: install clean


