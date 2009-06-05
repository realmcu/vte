##############################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#     
##############################################################################
#
#   @file   Makefile
#
#   @brief  configuration file template to compile the H.264 tests.
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
#D.Simakov/smkd001c    27/05/2005   TLSbo47117   Initial version
#D.Simakov             19/05/2006   TLSbo66280   Phase2
##############################################################################

SUBDIRS = wmv9_decoder_testapp

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; $(MAKE) -C $$i ; done
        
install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
