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
#    @brief  configuration file to compile the scc module.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#hake huang/b20222             20080815       
####################################################################################################
EXTRA_CFLAGS += -I$(KERNEL_SRCDIR)/drivers/mxc/security \
          -I$(KERNEL_SRCDIR)/include/linux \
          -I$(KERNEL_SRCDIR)/include/asm-arm/arch-mxc \
          -DSCC2_MODULE \
          -DSCC2_DEBUG

LOADLIBES =

TARGET = scc2_test_driver.ko
obj-m= scc2_test_driver.o

all:
	@make -C $(KERNEL_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules
	@rm -f *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** ALL : SCC2_TEST_MODULE                    **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f *.ko *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN: SCC2_TEST_MODULE                    **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : SCC2_TEST_MODULE                 **"
	@echo "***********************************************"
	@echo
