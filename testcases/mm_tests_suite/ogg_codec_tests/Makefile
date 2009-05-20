###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE ogg_decoder test application.
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
#D.Simakov                   23/04/2006      TLSbo65887   Initial version
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################

SUBDIRS = \
ogg_decoder_testapp


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

