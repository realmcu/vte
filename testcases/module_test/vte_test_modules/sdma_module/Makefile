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
#        @file   Makefile
#
#        @brief  configuration file to compile the sockets test.
#
####################################################################################################
#Revision History:                                                                                 
#                       Modification	 Tracking
#Author/core ID             Date          Number        Description of Changes                    
#------------------	------------	----------	------------------------------------------
#S.ZAVJALOV/-----        13/07/2004     TLSbo40259      Initial version                          
#L.DELASPRE/rc149c       13/08/2004     TLSbo40891      VTE 1.4 integration                     
#L.DELASPRE/rc149c       14/09/2004     TLSbo42022      VTE 1.5 integration                    
#L.DELASPRE/rc149c       07/10/2004     TLSbo42821      VTE 1.5.1 integration                    
#L.Delaspre/rc149c       08/12/2004     TLSbo40142      update with Freescale identity
#V.HALABUDA/HLBV001      05/07/2005     TLSbo52340      Update version for linux-2.6.11-CVS and fix
#A.Ozerov/B00320         10/02/2006     TLSbo61734      Code was cast to coding conventions
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
#CFLAGS += -Wall 
LOADLIBES +=

obj-m = sdma_test_module.o
TARGET = sdma_test_module.ko

all: 
	@make -C $(KLINUX_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@echo "***********************************************"
	@echo "** ALL : TEST_SDMA_MODULE                    **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(TARGET) *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf .tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN: TEST_SDMA_MODULE                   **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin/
	@echo "***********************************************"
	@echo "** INSTALL : TEST_SDMA_MODULE                **"
	@echo "***********************************************"
	@echo
