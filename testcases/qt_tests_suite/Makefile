###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE TEMPLATE tests.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
# Revision History:
#                            Modification     Tracking
# Author                          Date          Number    Description of Changes
# -------------------------   ------------    ----------  -------------------------------------------
# Roman Holodov                18/05/24                   initial version creation
# Sergey Zavjalov              22/05/04                   adding "install" and "gennerate" sections
# Dmitriy Kazachkov            26/05/04                   cddnig cross-platform version
#L.Delaspre/rc149c             08/12/2004    TLSbo40142   update with Freescale identity
#L.Delaspre/rc149c             14/04/2005    TLSbo48760   VTE 1.9 integration
#
###################################################################################################
# Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
# This test suile can be compiled using regular g++ and arm-linux-g++
#
# To switch, uncomment the corresponing section and comment out the another.
#
###################################################################################################

QTDIR=/local/linux_baseport/REF/qt/qte-free-3.3.4/usr/
QTSPEC=$(QTDIR)/qte/data/mkspecs/qws/linux-zasfsl-g++

export QTDIR QTSPEC

SUBDIRS = \
Widgets_test \
Canvas_test \
Graphics_test \
Date_time_test \
Test_applications \
Test_games \
Dialogs_test \


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

uninstall:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i uninstall ; done

generate:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i generate ; done

