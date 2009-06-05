###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the keypad_test_script.
#
###################################################################################################
#
#        Copyright (C) 2006-2008, Freescale Semiconductor, Inc. All Rights Reserved
#        The code contained herein is licensed under the GNU Lesser General
#        Public License.  You may obtain a copy of the GNU Lesser General
#        Public License Version 2.1 or later at the following locations:
#
#        http://www.opensource.org/licenses/lgpl-license.html
#        http://www.gnu.org/copyleft/lgpl.html
#     
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number     Description of Changes
#-------------------------   ------------    ----------   -----------------------------------------
#Hake Huang                    24/11/2008       NA          Initial version

###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../../
BIN_DIR = $(LTP_DIR)testcases/

ifeq ($(ARCH_PLATFORM), $(filter imx233 imx25stack, $(ARCH_PLATFORM)))
TARGET = $(wildcard *.sh)
DATA = keymap_mx233.txt
endif


all:
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo

install:
	set -e; for i in $(TARGET); do ln -f $$i $(BIN_DIR)bin; done
	touch $(BIN_DIR)bin/keymap.txt
	ln -f $(DATA) $(BIN_DIR)bin/keymap.txt
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

clean:
	@echo "do nothing"

