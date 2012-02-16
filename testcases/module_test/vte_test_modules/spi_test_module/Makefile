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
#    @brief  configuration file to compile the CSPI module driver.
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

obj-m = spi_test_module.o
obj-m += mxc_spi_testmod.o 
TARGET = spi_test_module.ko
TARGET += mxc_spi_testmod.ko

all:
	make -C $(KERNEL_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
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
	rm -rf .tmp_versions modules.order  Module.symvers
	@echo "*************************************************"
	@echo "** CLEAN :   $(TARGET)                **"
	@echo "*************************************************"
	@echo
