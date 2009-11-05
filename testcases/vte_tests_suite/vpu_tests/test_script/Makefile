#Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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

ifeq ($(ARCH_PLATFORM), $(filter imx37stack imx51stack, $(ARCH_PLATFORM)))
TARGET += vpu_dec_test.sh
TARGET += vpu_multi_dec_test.sh
TARGET += vpu_pm_test.sh
TARGET += vpu_app_test.sh
TARGET += vpu_sequence_test.sh
TARGET += vpu_create_test_sequence.sh
TARGET += vpu_create_task.sh
endif

ifeq ($(ARCH_PLATFORM), imx51stack)
TARGET +=  vpu_enc_test.sh
TARGET += vpu_multi_enc_test.sh
TARGET +=  vpu_tvout_720P.sh
TARGET +=  vpu_tvout_NTSC.sh
TARGET +=  vpu_tvout_PAL.sh
endif



all:
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo

install:
	@set -e; for i in $(TARGET); do ln -f $$i $(BIN_DIR)bin ; done
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

clean:
	@echo "do nothing"

