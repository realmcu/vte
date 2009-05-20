##############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the udev and sysfs test case
#            -
#
##############################################################################
#
#   Copyright (C) 2008, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
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

SUBDIRS=scull
TARGETS=udev_sysfs_test1.sh

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGETS)                      **"
	@echo "***********************************************"
	ln -f $(TARGETS) ../../../bin/$(TARGETS)
	@echo

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done


