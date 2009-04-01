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
#S.Bezrukov/SBAZR1C           31/08/2005     TLSbo52697   Initial version                           
#A.Ozerov/b00320              08/08/2006     TLSbo73745   Review version(in accordance to L26_1_19 release).
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
LTP_DIR = ../../../../

CFLAGS += -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

obj-m = usbtest_module.o
TARGET = usbtest_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL :USBTEST_MODULE                 **"
	@echo "***********************************************"
	@echo

clean:
	@rm -rf *.ko *.mod.ko *.mod.c .*.cmd *.o .tmp_versions .*.o.d
	@echo "***********************************************"
	@echo "** CLEAN:_USBTEST_MODULE                   **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL :_USBTEST_MODULE                **"
	@echo "***********************************************"
	@echo
