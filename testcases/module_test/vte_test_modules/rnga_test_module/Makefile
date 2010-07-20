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
####################################################################################################
include $(KLINUX_BLTDIR)/.config

ifeq "$(CONFIG_ARCH_MXC91331)" "y"
EXTRA_CFLAGS += -DCONFIG_ARCH_MXC91331
endif

ifeq "$(CONFIG_ARCH_MXC91221)" "y"
EXTRA_CFLAGS += -DCONFIG_ARCH_MXC91221
endif

ifeq "$(CONFIG_ARCH_MXC91321)" "y"
EXTRA_CFLAGS += -DCONFIG_ARCH_MXC91321
endif

ifeq "$(CONFIG_ARCH_MXC91231)" "y"
EXTRA_CFLAGS += -DCONFIG_ARCH_MXC91231
endif

ifeq "$(CONFIG_ARCH_MXC91131)" "y"
EXTRA_CFLAGS += -DCONFIG_ARCH_MXC91131
endif

ifeq "$(CONFIG_ARCH_MX3)" "y"
EXTRA_CFLAGS += -DCONFIG_ARCH_MX3
endif

ifeq "$(CONFIG_ARCH_MX27)" "y"
EXTRA_CFLAGS += -DCONFIG_ARCH_MX27
endif

ifeq "$(CONFIG_ARCH_MX21)" "y"
EXTRA_CFLAGS += -DCONFIG_ARCH_MX21
endif


BIN_DIR = ../../../
EXTRA_CFLAGS += -DLINUX_KERNEL -I$(KLINUX_SRCDIR)/drivers/mxc \
                -I$(KLINUX_SRCDIR)/drivers/mxc/security/rng/include \
		-I$(KLINUX_SRCDIR)/drivers/mxc/security/sahara2/include \
		-I$(KLINUX_SRCDIR)/include \
		-I$(KLINUX_BLTDIR)/include \
		-DCONFIG_PM

LOADLIBES=

TARGET = rnga_test_module.ko
obj-m= rnga_test_module.o

all: 
	echo $(CFLAGS)
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -f *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** ALL : rnga test module                    **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL :rnga test module                 **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f rnga_test_module.ko *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN: rnga test module                   **"
	@echo "***********************************************"
	@echo
