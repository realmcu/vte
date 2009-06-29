#Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile PMIC audio module.
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
