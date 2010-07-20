#Copyright 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the dvfs test.
#
###############################################################################

TARGETS=dvfs.sh cpu_lowfreq.sh dvfs_per.sh pm_suspend_stress.sh

SRC-$(CONFIG_VTE_IMX25) = dvfs_mx25.sh
SRC-$(CONFIG_VTE_IMX233) = dvfs_mx233.sh

all:
	

install:
	@for i in $(TARGETS); do ln -f $$i ../../bin ; done
	-ln -f dvfs_mx25.sh ../../bin/
	-ln -f dvfs_mx233.sh ../../bin/
	@echo "***********************************************"
	@echo "** INSTALL : dvfs.sh                    **"
	@echo "***********************************************"
	@echo

clean:
	@echo "***********************************************"
	@echo "** CLEAN : dvfs.sh                      **"
	@echo "***********************************************"
	@echo
