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
BIN_DIR = $(LTP_DIR)testcases/bin/

TARGET_UC = tvout_test_usercase.sh
TARGET_OT = tvout_overlay_test.sh

SRC_UC-$(CONFIG_VTE_IMX31) = mx31_tvout_test_usercase.sh
SRC_UC-$(CONFIG_VTE_IMX37) = mx37_tvout_test_usercase.sh
SRC_UC-$(CONFIG_VTE_IMX51) = mx37_tvout_test_usercase.sh

SRC_OT-$(CONFIG_VTE_IMX37) = mx37_tvout_overlay_test.sh
SRC_OT-$(CONFIG_VTE_IMX51) = mx37_tvout_overlay_test.sh

#for imx37 imx5x
SUBDIRS-$(CONFIG_VTE_TVOUT_APP) += app


all:
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i; done

install:
	-ln -f $(SRC_UC-y) $(BIN_DIR)/$(TARGET_UC)
	-ln -f $(SRC_OT-y) $(BIN_DIR)/$(TARGET_OT)
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install; done

clean:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i clean; done

