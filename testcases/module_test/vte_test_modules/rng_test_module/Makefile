#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#                                                                                                  #
#    @file   Makefile                                                                              #
#                                                                                                  #
#    @brief  configuration file to compile the sockets test.                                       #
#                                                                                                  #
####################################################################################################
#Revision History:                                                                                 #
#                            Modification     Tracking                                             #
#Author                          Date          Number    Description of Changes                    #
#-------------------------   ------------    ----------  ------------------------------------------#
#S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version                          #
#L.DELASPRE/rc149c            25/11/2004     TLSbo44180   VTE 1.6 integration                      #
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity           #
#A.URUSOV                     06/09/2005     TLSbo53754   CFLAGS and LOADLIBES changed             #
#A.URUSOV                     01/11/2005     TLSbo57925   CFLAGS and LOADLIBES changed             #
#Blake/b19411                 31/07/2008                  Create rng_test_module
####################################################################################################

BIN_DIR = ../../../
EXTRA_CFLAGS += -DLINUX_KERNEL -I$(KERNEL_SRCDIR)/drivers/mxc \
                -I$(KERNEL_SRCDIR)/drivers/mxc/security/rng/include \
		-I$(KERNEL_SRCDIR)/drivers/mxc/security/sahara2/include \
		-DCONFIG_PM

LOADLIBES=

TARGET = rng_test_module.ko
obj-m= rng_test_module.o

all: 
	echo $(CFLAGS)
	@make -C $(KERNEL_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -f *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** ALL : rng test module                    **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL :rng test module                 **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f rng_test_module.ko *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf ./.tmp_versions modules.order
	@echo "***********************************************"
	@echo "** CLEAN: rng test module                   **"
	@echo "***********************************************"
	@echo
