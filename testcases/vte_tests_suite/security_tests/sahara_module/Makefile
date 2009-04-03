####################################################################################################
#
#    @file   Makefile
#
#    @brief  Configuration file to compile the module.
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
#Author/Core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Ozerov/NONE                01/12/2005     TLSbo58662  Initial version for linux-2.6.10-rel-L26_1_14
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
EXTRA_CFLAGS += -Wall -DDEBUG -I$(KLINUX_SRCDIR)/drivers/mxc/security/sahara2/include

obj-m = sahara_module.o
TARGET = sahara_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : SAHARA_MODULE                       **"
	@echo "***********************************************"

clean:
	rm -rf *.ko *.mod.ko *.mod.c .*.cmd *.o .tmp_versions .*.o.d
	@echo "***********************************************"
	@echo "** CLEAN: SAHARA_MODULE                      **"
	@echo "***********************************************"

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : SAHARA_MODULE                   **"
	@echo "***********************************************"
