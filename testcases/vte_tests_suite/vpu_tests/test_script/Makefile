###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file.
#
###################################################################################################
#
#        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
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
endif

ifeq ($(ARCH_PLATFORM), imx51stack)
TARGET +=  vpu_enc_test.sh
TARGET += vpu_multi_enc_test.sh
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

