###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the pmic_adc_testapp.
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
LTP_DIR = ../../../
BIN_DIR = $(LTP_DIR)testcases/

TARGET_SCRIPT = $(wildcard *.sh)

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

all: $(TARGET_BIN)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo

install:
	echo "camera install"
	ln -f $(TARGET_SCRIPT) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

$(TARGET_BIN): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : 

.phone:
clean:
	@echo "do nothing"

