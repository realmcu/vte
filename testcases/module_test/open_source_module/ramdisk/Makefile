#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#        @file   Makefile
#
####################################################################################################
#Revision History:                                                                                 
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
EXTRA_CFLAGS += -Wall -DCONFIG_BLK_DEV_RAM_COUNT=16 -DCONFIG_BLK_DEV_RAM_SIZE=4096 
LOADLIBES +=

obj-m = brd.o
TARGET = brd.ko

all: 
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@echo "***********************************************"
	@echo "** ALL : TEST_RAMDISK_MODULE                 **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(TARGET) *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf .tmp_versions modules.order Module.symvers
	@echo "***********************************************"
	@echo "** CLEAN: TEST_RAMDISK_MODULE                **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : TEST_RAMDISK_MODULE             **"
	@echo "***********************************************"
	@echo
