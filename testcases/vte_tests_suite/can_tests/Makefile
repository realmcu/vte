###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE CAN test applications.
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
#hake huang                   07/28/2008     N/A   Initial version
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
#  GNU GCC
###################################################################################################

SUBDIRS = cansocket
 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

