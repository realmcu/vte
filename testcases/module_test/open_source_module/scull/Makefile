#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile scull.ko to test udev
#
##############################################################################
#Revision History:
#                            Modification     Tracking
# Author                         Date          Number    Description of Changes
#-------------------------   ------------    ----------  ---------------------
# Spring Zhang                07/02/2008         n/a     Initial version
#
###############################################################################

scull-objs := main.o 

obj-m := scull.o
TARGETS = scull.ko
all:
	$(MAKE) -C $(KERNEL_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules

install:
	@echo "****************************************"
	@echo "** INSTALL : scull.ko                 **"
	@echo "****************************************"
	ln -f $(TARGETS) ../../../bin/
clean:
	@rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers modules.order
	@echo "***********************************************"
	@echo "** CLEAN: UDEV_MODULE                        **"
	@echo "***********************************************"
	@echo


.PHONY: install clean


