###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the NOR MTD test applications.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Becker/rc023c              25/06/2004     TLSbo39142   Change subdir names
#S.ZAVJALOV/ZVJS001C          27/07/2004     TLSbo40261   nand mtd
#L.DELASPRE/rc149c            02/08/2004     TLSbo40891   VTE 1.4 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#D.Khoroshev/b00313           03/13/2007     TLSbo92116   NAND test removed for mxc27520evb
#V.Khalabuda/b00306           12/09/2006     ENGR37140    Update the Carmel test suite environment
#
###################################################################################################
#Portability: NAND is not supported on Islay(mxc27520evb) and Carmel(mxc30020evb).
#
###################################################################################################
SUBDIRS = nor_mtd_testapp nor_mtd_testapp_2 nand_mtd_testapp

ifneq ($(ARCH_PLATFORM), $(filter mxc27520evb mxc30020evb, $(ARCH_PLATFORM)))
SUBDIRS += nand_mtd_testapp
endif

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
