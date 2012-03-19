#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the module fot the watchdog test.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#S.ZAVJALOV/ZVJS001C          13/08/2004     TLSbo41612  Initial version
#L.DELASPRE/rc149c            16/08/2004     TLSbo40891  VTE 1.4 integration
#L.DELASPRE/rc149c            14/09/2004     TLSbo42022  VTE 1.5 integration
#L.DELASPRE/rc149c            07/10/2004     TLSbo42821  VTE 1.5.1 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#S.V-Guilhou/svan01c          19/08/2005     TLSbo53364  Adapt test suite for MXC91131
#S.V-Guilhou/svan01c          20/10/2005     TLSbo56971  Adapt test suite for MX31
#V.Khalabuda/b00306           06/07/2006     TLSbo63552  Update for ArgonLV support
#
####################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS += -Wall 
LOADLIBES +=

# Platforms with one watchdog only
ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_1_WDG
endif

ifeq ($(ARCH_PLATFORM),imx31ads)
CFLAGS+= -DCONFIG_1_WDG
endif

# Platforms with two watchdogs
# CONFIG_ARCH_MXC91331 - ArgonPlus
ifeq ($(ARCH_PLATFORM), $(filter mxc91331 i30030evb , $(ARCH_PLATFORM)))
CFLAGS+= -DCONFIG_2_WDG
endif

# CONFIG_ARCH_MXC91321 - ArgonLV
ifeq ($(ARCH_PLATFORM),$(filter mxc91321 mxc30030evb , $(ARCH_PLATFORM)))
CFLAGS+= -DCONFIG_2_WDG
endif

# CONFIG_ARCH_MXC91231 - SCMA-A11
ifeq ($(ARCH_PLATFORM), $(filter mxc91231 mxc27530evb, $(ARCH_PLATFORM)))
CFLAGS+= -DCONFIG_2_WDG
endif

# Odyssey
ifeq ($(ARCH_PLATFORM),i30030ads)
CFLAGS+= -DCONFIG_2_WDG
endif

obj-m= watchdog_test.o
TARGET= watchdog_test.ko

all: 
	@make -C $(KERNEL_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules
	@rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                    **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(TARGET) *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf .tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                  **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo
