#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the dvfs test.
#
###################################################################################################
all:
	

install:
	@ln -f dvfs.sh  ../../bin
	@if [ $(ARCH_PLATFORM) == "imx25stack" ]; then ln -f dvfs_mx25.sh ../../bin ; fi
	@if [ $(ARCH_PLATFORM) == "imx233" ]; then ln -f dvfs_mx233.sh ../../bin ; fi
	@echo "***********************************************"
	@echo "** INSTALL : dvfs.sh                    **"
	@echo "***********************************************"
	@echo

clean:
	@echo "***********************************************"
	@echo "** CLEAN : dvfs.sh                      **"
	@echo "***********************************************"
	@echo
