###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE realvideo decoder test application.
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
#D.Simakov/smkd001c           16/06/2005     TLSbo51185	 Initial version	
#D.Simakov                    15/05/2006      TLSbo66278  Phase2
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = realvideo_decoder_testapp 


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

