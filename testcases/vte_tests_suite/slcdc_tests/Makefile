###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE MU test applications.
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
#                              Modification     Tracking
#Author (core ID)                  Date          Number    Description of Changes
#---------------------------   ------------    ----------  ------------------------------------------
#Igor Semenchukov/smng001c      29/09/2004     TLSbo41672   Initial version 
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#A.Smirnov                     1/07/2005     TLSbo51716   Testcases reorganization
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = lcd_test power_test

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

