####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile PMIC RTC test module
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
#-------------------------   ------------    ----------  ------------------------------------------
#Pradeep K / b01016          09/25/2006      TLSboxxx    Initial version
#
####################################################################################################
EXTRA_CFLAGS+=	-Wall -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include


SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=pmic_rtc_module.ko
obj-m=pmic_rtc_module.o

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : PMIC_RTC_TEST_MODULE                **"
	@echo "***********************************************"
	@echo

clean:
	@rm -rf *.ko *.mod.ko *.mod.c .*.cmd *.o .tmp_versions .*.o.d
	@echo "***********************************************"
	@echo "** CLEAN: PMIC_RTC_TEST_MODULE               **"
	@echo "***********************************************"
	@echo

install:
	@cp $(TARGET) ../../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : PMIC_RTC_TEST_MODULE            **"
	@echo "***********************************************"
	@echo
