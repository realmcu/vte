#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the udev and sysfs test case
#
##############################################################################
#Revision History:
#                            Modification     Tracking
# Author                         Date          Number    Description of Changes
#-------------------------   ------------    ----------  ---------------------
# Spring Zhang                07/02/2008         n/a     Initial version
#
###############################################################################
CFLAGS+= -Wall -I ../../../../include
LOADLIBES += -L ../../../lib -lltp

TARGETS=udev_sysfs_test1.sh

all:

install:
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGETS)                      **"
	@echo "***********************************************"
	ln -f $(TARGETS) ../../../bin/$(TARGETS)
	@echo

clean:


