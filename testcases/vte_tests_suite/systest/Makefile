####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile systest test applications.
#
####################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Simakov                    05/04/2007      ENGR37674  Initial version
#D.Simakov                    10/04/2007      ENGR37676  systest2 was added
####################################################################################################

ifeq ($(ARCH_PLATFORM), $(filter mxc27530evb mxc30030evb mxc30031ads imx27ads , $(ARCH_PLATFORM)))
SUBDIRS += systest1
endif

SUBDIRS = \
	systest2 systest4 systest5 systest6
	#systest2 systest3 systest4 systest5 systest6

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
