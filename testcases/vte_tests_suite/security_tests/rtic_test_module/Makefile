####################################################################################################
#                                                                                                  #
#    @file   Makefile                                                                              #
#                                                                                                  #
#    @brief  configuration file to compile the sockets test.                                       #
#                                                                                                  #
####################################################################################################
#                                                                                                  #
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved                          #
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT                                   #
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF                                       #
#   Freescale Semiconductor, Inc.                                                                  #
#                                                                                                  #
####################################################################################################
#Revision History:                                                                                 #
#                            Modification     Tracking                                             #
#Author                          Date          Number    Description of Changes                    #
#-------------------------   ------------    ----------  ------------------------------------------#
#S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version                          #
#L.DELASPRE/rc149c            25/11/2004     TLSbo44180   VTE 1.6 integration                      #
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity           #
#A.URUSOV                     13/09/2005     TLSbo55076   Fix compilation issue and warnings       #
#A.URUSOV                     01/11/2005     TLSbo57063   CFLAGS and LOADLIBES changed             #
####################################################################################################

BIN_DIR = ../../../

CFLAGS+= -Wall \
        -I$(KLINUX_SRCDIR)/drivers/mxc/security/ \
        -DCONFIG_PM
LOADLIBES=

TARGET = rtic_test_module.ko
obj-m= rtic_test_module.o

all: 
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -f *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** ALL : rtic test module                    **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : rtic test module                **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f rtic_test_module.ko *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN : rtic test module                  **"
	@echo "***********************************************"
	@echo
