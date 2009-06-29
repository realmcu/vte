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
#Hake Huang                    02/13/2008       NA          Initial version

###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../../
BIN_DIR = $(LTP_DIR)testcases/
#CROSS_COMPILER = /opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILER)gcc
LINK=$(CROSS_COMPILER)gcc

TARGET_BIN = tvout_cd

OBJ = tv_cd_poll.o

all: $(TARGET_BIN)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo

install:
	-ln -f $(TARGET_BIN) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

$(TARGET_BIN): $(OBJ)
	$(LINK) -o $(TARGET_BIN) $(OBJ)

%.o: %.c
	$(CC) -Wall -O2 $(INCLUDE) -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(TARGET_BIN) $(OBJ) 

