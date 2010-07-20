#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the dvfs_dptc_testapp test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Blake

TARGET-y += dptc.sh 
TARGET-$(CONFIG_VTE_DRYICE) += dryice.sh

all:

install:
	-ln -f $(TARGET-y) ../../bin/
	@echo "***********************************************"
	@echo "** INSTALL : dptc.sh                    **"
	@echo "***********************************************"
	@echo

clean:
	@echo "***********************************************"
	@echo "** CLEAN : dptc.sh                      **"
	@echo "***********************************************"
	@echo
