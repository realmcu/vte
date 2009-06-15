####################################################################################################
#
#        @file   Makefile
#
####################################################################################################
#
#        Copyright (C) 2009, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
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
	@rm -rf .tmp_versions
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
