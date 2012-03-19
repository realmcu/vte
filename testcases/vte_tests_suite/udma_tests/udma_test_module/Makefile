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
#        @file   Makefile
#
#        @brief  configuration file to compile the UDMA test application.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Ozerov/b00320              02/10/2006     TLSbo78550  Initial version.
#A.Ozerov/b00320              01/11/2006     TLSbo81158  UDMA module was fixed for working with all platforms.
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
ifeq ($(ARCH_PLATFORM), imx27ads)
CFLAGS += -Wall -DCONFIG_IMX27
else
CFLAGS += -Wall -DCONFIG_OTHER_PLATFORM
endif

LOADLIBES +=

obj-m  = udma_test_module.o
TARGET = udma_test_module.ko

all: 
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules
	rm -rf *.mod.ko *.mod.c .*.cmd *.o .*.o.d .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : UDMA_TEST_MODULE                    **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(TARGET) *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	rm -rf .tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN : UDMA_TEST_MODULE                  **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : UDMA_TEST_MODULE                **"
	@echo "***********************************************"
	@echo
