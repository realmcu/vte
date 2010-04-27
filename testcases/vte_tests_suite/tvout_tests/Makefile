#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number     Description of Changes
#-------------------------   ------------    ----------   -----------------------------------------
#Hake Huang                    07/11/2008       NA          Initial version

###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../
BIN_DIR = $(LTP_DIR)testcases/

TARGET_BIN = tvout_test_usercase.sh
TARGET_BIN2 = tvout_overlay_test.sh
SUBTARGET = $(TARGET_BIN)
SUBTARGET2 = $(TARGET_BIN2)
ifeq ($(ARCH_PLATFORM), imx31stack)
SUBTARGET = mx31_tvout_test_usercase.sh
endif

ifeq ($(ARCH_PLATFORM),$(filter imx37stack imx51stack imx51, $(ARCH_PLATFORM)))
SUBDIRS = app
endif

ifeq ($(ARCH_PLATFORM),$(filter imx37stack imx51stack imx51, $(ARCH_PLATFORM)))
SUBTARGET = mx37_tvout_test_usercase.sh
SUBTARGET2 = mx37_tvout_overlay_test.sh
TARGET = $(TARGET_BIN)
endif

all:
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done

install:
	-cp -f $(SUBTARGET) $(TARGET_BIN)
	-ln -f $(SUBTARGET2) $(BIN_DIR)bin/tvout_overlay_test.sh
	-ln -f $(TARGET_BIN) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done

