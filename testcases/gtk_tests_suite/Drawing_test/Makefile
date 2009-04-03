##################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the TEMPLATE_testapp_X test.
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
#   Inkina Irina               10/09/2004     ??????      Initial version
# Ludovic Delaspre/rc149c     27/10/2004     TLSbo44018   make GTK compilation available
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = \
Balls \
Drawingarea \
Drawrectangle \
Linestyle \
Visual \
Rulers


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

uninstall:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i uninstall ; done

generate:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i uninstall ; done
