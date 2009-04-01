###################################################################################################
#
#       @file   Makefile
#
#       @brief  Configuration file to compile the ATA Disk Driver test.
#
###################################################################################################
#
#       Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#       THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#       BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#       Freescale Semiconductor, Inc.
#     
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Ozerov/b00320              10/09/2006     TLSbo76800  Initial version.
#
###################################################################################################
#Portability: ARM GCC 
#
###################################################################################################

SUBDIRS = ata_testapp

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
