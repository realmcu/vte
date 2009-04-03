####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile PMIC audio module.
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
#Author/core ID                  Date          Number     Description of Changes
#-------------------------   ------------    ----------   -----------------------------------------
#Rakesh S Joshi             01/03/2007      tlsbo88188      Initial version
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
LTP_DIR = ../../../../

EXTRA_CFLAGS += -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

obj-m = pmic_power_module.o
TARGET = pmic_power_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL :PMIC_AUDIO_MODULE                 **"
	@echo "***********************************************"
	@echo

clean:
	@rm -rf *.ko *.mod.ko *.mod.c .*.cmd *.o .tmp_versions .*.o.d
	@echo "***********************************************"
	@echo "** CLEAN:_PMIC_AUDIO_MODULE                   **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL :_PMIC_AUDIO_MODULE                **"
	@echo "***********************************************"
	@echo
