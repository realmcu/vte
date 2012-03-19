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
#        @file   Makefile
#
#        @brief  configuration file to compile PMIC audio module.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number     Description of Changes
#-------------------------   ------------    ----------   -----------------------------------------
#S.Bezrukov/SBAZR1C           31/08/2005     TLSbo52697   Initial version                           
#A.Ozerov/b00320              08/08/2006     TLSbo73745   Review version(in accordance to L26_1_19 release).
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
LTP_DIR = ../../../../

EXTRA_CFLAGS += -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

obj-m = pmic_audio_module.o
TARGET = pmic_audio_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules
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
