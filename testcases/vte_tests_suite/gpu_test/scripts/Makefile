#Copyright (C) 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
TARGET = gpu_test.sh
BIN = ../../../bin


all:
	-rm -rf $(TARGET)
	if [ $$ARCH_PLATFORM == "imx51stack" ] || [ $$ARCH_PLATFORM == "imx53" ]; \
		then cp mx5x_gpu_test.sh $(TARGET); fi
	if [ $$ARCH_PLATFORM == "imx35stack" ]; then cp mx35_gpu_test.sh $(TARGET); fi

install:
	ln -f $(TARGET) $(BIN)

clean:
	-rm -rf $(TARGET)

