####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the ipc_module.
#
####################################################################################################
#
#        Copyright (C) 2005, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Ozerov/b00320              26/04/2006     TLSbo61791  Initial version
#
####################################################################################################
CFLAGS += -Wall 

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

obj-m := ipc_module.o
TARGET = ipc_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                       **"
	@echo "***********************************************"

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                   **"
	@echo "***********************************************"

clean:
	@rm -rf *.ko *.mod.ko *.mod.c .*.cmd *.o .tmp_versions .*.o.d
	@echo "***********************************************"
	@echo "** CLEAN: $(TARGET)                      **"
	@echo "***********************************************"
