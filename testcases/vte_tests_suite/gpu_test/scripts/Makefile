#Copyright (C) 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
TARGET = gpu_test.sh
TARGETS-$(CONFIG_VTE_IMX5) = gpu_3dmark_cpuload.sh gles.sh vg.sh
TARGETS-$(CONFIG_VTE_IMX6) = gles_viv.sh vg_gc2000.sh vg_gc35x.sh cl_gc2000.sh dfb_gc320.sh
BIN = ../../../bin/

SRC-$(CONFIG_VTE_IMX5)  =  mx5x_gpu_test.sh
SRC-$(CONFIG_VTE_IMX35) =  mx35_gpu_test.sh

all:

install:
	-cp -f $(SRC-y) $(BIN)/$(TARGET)
	-ln -f $(TARGETS-y) $(BIN)/

clean:

