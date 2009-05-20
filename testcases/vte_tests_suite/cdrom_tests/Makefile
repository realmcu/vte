###################################################################################################
#
#       @file   Makefile
#
#       @brief  Configuration file to compile the CDROM Driver test.
#
###################################################################################################
#
#       Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#       THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#       BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#       Freescale Semiconductor, Inc.
#
###################################################################################################

#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################

SUBDIRS = cdrom_testapp

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
