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
#    @file   Makefile
#
#    @brief  configuration file to compile the DMA test application.
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
