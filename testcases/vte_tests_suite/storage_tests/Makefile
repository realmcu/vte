#Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###############################################################################
#
#    @file   Makefile
#
#    @brief  storage scripts and usb windows drivers.
#
###############################################################################
#Revision History:
#                 Modification     Tracking
#Author               Date          Number    Description of Changes
#---------------  ------------    ----------  ----------------------
# Victor Cui      01/12/2008                  Initial ver
#
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################

# prepare envirnoment
TARGET = auto_prepare.sh

# storage test basic scripts
TARGET += storage_test.sh storage_test_concurrent.sh storage_random_test.sh usbh_test.sh usb_test.sh

# usb
TARGET += usbd_testall.pl usbd_test.pl

# drivers
#TARGET += gserial.inf linux.inf usbser.sys

#performance
TARGET += performance performance_command_line performance_prepare.sh performance_test.sh

#ubi 
TARGET += ubi_test.sh

#usb ptp
TARGET += usb_ptp_test.sh Agl.sh

#TOOL_BIN = dd.exe activeperl.rar InstallUSBCV132.msi ptpinstall.tar.gz Agl.sh

#all modules insert and remove test 
TARGET += modules_verify.sh

#all block storage test
TARGET += storage_all.sh

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : do nothing only copy                **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../bin
#	-ln -f $(TOOL_BIN) ../../bin/
	@echo "***********************************************"
	@echo "** INSTALL : ln all storage scripts to bin   **"
	@echo "***********************************************"
	@echo

clean:
	@echo "***********************************************"
	@echo "** CLEAN : do nothing                        **"
	@echo "***********************************************"
	@echo

