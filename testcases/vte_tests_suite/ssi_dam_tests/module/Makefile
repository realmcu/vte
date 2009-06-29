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
#    @brief  SSI DAM test module configuration file.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Halabuda/hlbv001           27/09/2005     TLsbo55818  Initial version
#
####################################################################################################
MXC_DIR = $(KLINUX_SRCDIR)/drivers/mxc
SSI_DIR = $(KLINUX_SRCDIR)/drivers/mxc/ssi
DAM_DIR = $(KLINUX_SRCDIR)/drivers/mxc/dam

CFLAGS    += -Wall -I$(MXC_DIR) -I$(SSI_DIR) -I$(DAM_DIR)
LOADLIBES +=

ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS += -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS += -DCONFIG_MXC_MC13783_PMIC
endif

obj-m  = testmod_ssi.o
TARGET = testmod_ssi.ko

all: 
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	rm -rf *.mod.ko *.mod.c .*.cmd *.o .*.o.d .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : SSI_TEST_MODULE                     **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(TARGET) *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	rm -rf .tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN : SSI_TEST_MODULE                   **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : SSI_TEST_MODULE                 **"
	@echo "***********************************************"
	@echo
