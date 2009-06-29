#Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile PMIC RTC test module
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
