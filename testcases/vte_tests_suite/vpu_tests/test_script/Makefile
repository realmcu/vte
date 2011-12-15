#Copyright (C) 2008-2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
LTP_DIR = ../../../../
BIN_DIR = $(LTP_DIR)testcases/

#TARGET = $(wildcard *.sh)

TARGET-y += vpu_dec_test.sh
TARGET-y += vpu_multi_dec_test.sh
TARGET-y += vpu_pm_test.sh
TARGET-y += vpu_app_test.sh
TARGET-y += vpu_sequence_test.sh
TARGET-y += vpu_create_test_sequence.sh
TARGET-y += vpu_create_task.sh

TARGET-$(CONFIG_VTE_IMX5) += vpu_enc_test.sh
TARGET-$(CONFIG_VTE_IMX5) += vpu_multi_enc_test.sh
TARGET-$(CONFIG_VTE_IMX5) += vpu_tvout_720P.sh
TARGET-$(CONFIG_VTE_IMX5) += vpu_tvout_NTSC.sh
TARGET-$(CONFIG_VTE_IMX5) += vpu_tvout_PAL.sh
TARGET-$(CONFIG_VTE_IMX5) += vpu_firmware_setup.sh
TARGET-$(CONFIG_VTE_IMX6) += vpu_performance.sh
TARGET-$(CONFIG_VTE_IMX6) += vpu_enc_test.sh
TARGET-$(CONFIG_VTE_IMX6) += vpu_multi_enc_test.sh
all:
	@echo "***********************************************"
	@echo "** ALL : $(TARGET-y)  test application  **"
	@echo "***********************************************"
	@echo

install:
	@set -e; for i in $(TARGET-y); do ln -f $$i $(BIN_DIR)bin ; done
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET-y)                **"
	@echo "***********************************************"
	@echo

clean:
	@echo "do nothing"

