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
#S.ZAVJALOV/zvjs001c          10/08/2004     TLSbo40418   Initial version                          #
#S.ZAVJALOV/zvjs001c          01/10/2004     TLSbo40649   Version after inspection                 #
#Ludovic Delaspre (rc149c)    06/10/2004     TLSbo42821   VTE 1.5.1 integration                    #
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity           #
#A.URUSOV                     14/09/2005     TLSbo53754   CFLAGS and LOADLIBES edited              #
#A.URUSOV                     18/10/2005     TLSbo57061   CFLAGS edited                            #
####################################################################################################


EXTRA_CFLAGS += -Wall \
          -I$(KERNEL_SRCDIR)/include/ \
          -I$(KERNEL_SRCDIR)/include/asm/ \
          -I$(KERNEL_SRCDIR)/include/asm/arch/ \
          -I$(KERNEL_SRCDIR)/drivers/mxc/security \
	  -I$(KLINUX_BLTDIR)/include \
          -DCONFIG_PM

LOADLIBES =


TARGET = hacc_test_module.ko
obj-m= hacc_test_module.o
 
all: 
	@make -C $(KERNEL_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) modules
	@rm -f *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** ALL : HACC_TEST_MODULE                    **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f hacc_test_module.ko *.mod.ko *.mod.c .*.cmd *.o .*.o.d
	@rm -rf ./.tmp_versions
	@echo "***********************************************"
	@echo "** CLEAN: HACC_TEST_MODULE                   **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : HACC_TEST_MODULE                **"
	@echo "***********************************************"
	@echo
