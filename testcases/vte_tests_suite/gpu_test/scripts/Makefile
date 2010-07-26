#Copyright (C) 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
TARGET = gpu_test.sh
TARGETS = gpu_3dmark_cpuload.sh gpu_test.sh
BIN = ../../../bin/

SRC-$(CONFIG_VTE_IMX5)  =  mx5x_gpu_test.sh
SRC-$(CONFIG_VTE_IMX35) =  mx35_gpu_test.sh

all:
	-rm -rf $(TARGET)
	-ln -f $(SRC-y) $(TARGET)

install:
	ln -f $(TARGETS) $(BIN)

clean:
	-rm -rf $(TARGET)

