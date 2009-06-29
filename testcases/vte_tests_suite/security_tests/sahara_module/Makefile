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
#    @file   Makefile
#
#    @brief  Configuration file to compile the module.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/Core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Ozerov/NONE                01/12/2005     TLSbo58662  Initial version for linux-2.6.10-rel-L26_1_14
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
EXTRA_CFLAGS += -Wall -DDEBUG -I$(KLINUX_SRCDIR)/drivers/mxc/security/sahara2/include

obj-m = sahara_module.o
TARGET = sahara_module.ko

all:
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : SAHARA_MODULE                       **"
	@echo "***********************************************"

clean:
	rm -rf *.ko *.mod.ko *.mod.c .*.cmd *.o .tmp_versions .*.o.d
	@echo "***********************************************"
	@echo "** CLEAN: SAHARA_MODULE                      **"
	@echo "***********************************************"

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : SAHARA_MODULE                   **"
	@echo "***********************************************"
