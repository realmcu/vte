####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the scc module.
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
#A.Urusov/NONE                26/09/2005     TLSbo55835  Initial version
#
####################################################################################################
#EXTRA_CFLAGS += -I$(KLINUX_SRCDIR)/drivers/net/sk98lin \
          -I$(KLINUX_SRCDIR)/drivers/net/sk98lin/h \
          -I$(KLINUX_SRCDIR)/drivers/mxc/security \
          -I$(KLINUX_SRCDIR)/include/linux \
          -I$(KLINUX_SRCDIR)/include/asm-arm/arch-mxc \
          -DSCC_MODULE \
          -DSCC_DEBUG
EXTRA_CFLAGS += -I$(KLINUX_SRCDIR)/drivers/mxc/security \
          -I$(KLINUX_SRCDIR)/include/linux \
          -I$(KLINUX_SRCDIR)/include/asm-arm/arch-mxc \
          -DSCC_MODULE \
          -DSCC_DEBUG


LOADLIBES =

TARGET = scc_test_module.ko
obj-m= scc_test_module.o

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -f *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** ALL : SCC_TEST_MODULE                    **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f scc_test_module.ko *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN: SCC_TEST_MODULE                    **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : SCC_TEST_MODULE                 **"
	@echo "***********************************************"
	@echo
