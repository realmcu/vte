####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the sockets test.
#
####################################################################################################
#
#        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core id                  Date          Number     Description of Changes
#-------------------------   ------------    ----------   -----------------------------------------
#V.Khalabuda/hlbv001          13/08/2005     TLSbo52695   Initial version
#V.Khalabuda/hlbv001          02/09/2005     TLSbo52695   Update for linux-2.6.10-rel-L26_1_14 
#
####################################################################################################
#Portability:    ARM GCC
#
####################################################################################################
ifeq ($(ARCH_PLATFORM),mxc91131evb)
CORE_DIR=$(KLINUX_SRCDIR)/drivers/mxc/sc55112/core/
EXTRA_CFLAGS+=        -DCONFIG_MXC_SC55112_PMIC
else
CORE_DIR=$(KLINUX_SRCDIR)/drivers/mxc/mc13783/core/
EXTRA_CFLAGS+=        -DCONFIG_MXC_PMIC_MC13783
endif

EXTRA_CFLAGS+=	-Wall -I$(LTP_DIR)/include -I$(CORE_DIR) -I$(KLINUX_BLTDIR)/include
LOADLIBES+=	-L$(LTP_DIR)lib -lltp -lm

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

obj-m=pmic_convity_module.o

TARGET=pmic_convity_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : PMIC_CONVITY_TEST_MODULE              **"
	@echo "***********************************************"
	@echo

clean:
	rm -rf *.ko *.mod.ko *.mod.c .*.cmd *.o .tmp_versions .*.o.d
	@echo "***********************************************"
	@echo "** CLEAN: PMIC_CONVITY_TEST_MODULE             **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : PMIC_CONVITY_TEST_MODULE          **"
	@echo "***********************************************"
	@echo
