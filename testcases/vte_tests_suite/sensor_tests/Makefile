#Copyright (C) 2005-2009,2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile ACC and Magnetic tests
#
################################################################################
SUBDIRS-y =
SUBDIRS-$(CONFIG_VTE_ACC)     += acc_tests
SUBDIRS-$(CONFIG_VTE_LSENSOR)     += lsensor_tests
SUBDIRS-$(CONFIG_VTE_MAGN)     += magn_tests

all:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i; done

install:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install; done

clean:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i clean; done
