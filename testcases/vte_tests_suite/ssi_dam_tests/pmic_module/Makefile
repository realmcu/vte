####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the module fot the SSI DAM test.
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
#D.Khoroshev/b00313           11/11/2005     TLsbo56844  Initial version for Roadrunner support
#
####################################################################################################
MXC_DIR = $(KLINUX_SRCDIR)/drivers/mxc
SSI_DIR = $(KLINUX_SRCDIR)/drivers/mxc/ssi
DAM_DIR = $(KLINUX_SRCDIR)/drivers/mxc/dam
SC55112_CORE_DIR = $(KLINUX_SRCDIR)/drivers/mxc/sc55112/core

CFLAGS += -Wall -I$(MXC_DIR) -I$(SSI_DIR) -I$(DAM_DIR) -I$(SC55112_CORE_DIR) -I$(KLINUX_SRCDIR) 
LOADLIBES+=

ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+= -DCONFIG_MXC_PMIC_MC13783
endif

obj-m=testmod_ssi.o
TARGET=testmod_ssi.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -rf *.mod.ko *.mod.c .*.cmd *.o .*.o.d .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                     **"
	@echo "***********************************************"
	@echo

clean:
	@rm -rf $(TARGET) *.mod.ko *.mod.c .*.cmd *.o .*.o.d *.ko .tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                   **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                 **"
	@echo "***********************************************"
	@echo
