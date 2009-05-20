###############################################################################
#
#    @file   Makefile
#
#    @brief  storage scripts and usb windows drivers.
#
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
###############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
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
TARGET += dd.exe usbd_test.pl

# drivers
TARGET += gserial.inf linux.inf usbser.sys

#performance
TARGET += performance performance_command_line performance_prepare.sh performance_test.sh

#ubi
TARGET += ubi_test.sh

#usb ptp
TARGET += usb_ptp_test.sh ptpinstall.tar.gz

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : do nothing only copy                **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../bin
	@echo "***********************************************"
	@echo "** INSTALL : ln all storage scripts to bin   **"
	@echo "***********************************************"
	@echo

clean:
	@echo "***********************************************"
	@echo "** CLEAN : do nothing                        **"
	@echo "***********************************************"
	@echo

