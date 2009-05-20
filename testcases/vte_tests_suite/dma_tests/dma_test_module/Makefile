####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the DMA test application.
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
#D.Simakov                    30/08/2006     TLSbo76801  Initial version
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
MXC_DIR = $(KLINUX_SRCDIR)/drivers/mxc

CFLAGS    += -Wall -I$(MXC_DIR)
LOADLIBES +=

obj-m  = dma_test_module.o
TARGET = dma_test_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	rm -rf *.mod.ko *.mod.c .*.cmd *.o .*.o.d .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : DMA_TEST_MODULE                     **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(TARGET) *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	rm -rf .tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN : DMA_TEST_MODULE                   **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : DMA_TEST_MODULE                 **"
	@echo "***********************************************"
	@echo
