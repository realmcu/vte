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
LTP_DIR =../../../../
BIN_DIR = $(LTP_DIR)/testcases/

TARGET = egl_test power_test sw_camera_ctrl glinfo ogles_camera_ext services_test  OGLESLighting


all:
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

clean:
	@echo "not to clean anything from third party"

