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
#    @brief  configuration file to compile PMIC ADC test module
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#D.Khoroshev/b00313           06/28/2006     TLSbo64235  Initial version
#
####################################################################################################
CFLAGS+=	-Wall -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include

ifeq ($(ARCH_PLATFORM), mxc91131evb)
CFLAGS+=	-DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+=	-DCONFIG_MXC_PMIC_MC13783
endif

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

obj-m=pmic_adc_module.o
TARGET=pmic_adc_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules
	@rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : PMIC_ADC_TEST_MODULE                **"
	@echo "***********************************************"
	@echo

clean:
	@rm -rf *.ko *.mod.ko *.mod.c .*.cmd *.o .tmp_versions .*.o.d
	@echo "***********************************************"
	@echo "** CLEAN: PMIC_ADC_TEST_MODULE               **"
	@echo "***********************************************"
	@echo

install:
	@cp $(TARGET) ../../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : PMIC_ADC_TEST_MODULE            **"
	@echo "***********************************************"
	@echo
