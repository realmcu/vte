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
# Irina Inkina                26/09/2004                   initial version creation
# Ludovic Delaspre/rc149c     27/10/2004      TLSbo44018   make GTK compilation available
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

GTK_SRCDIR = /usr/local/arm/gtk

CFLAGS= -Wall -I../../../../include -g `PKG_CONFIG_PATH=$(GTK_SRCDIR)/lib/pkgconfig pkg-config --cflags --libs gtk+-2.0`
LOADLIBES= -L../../../../lib -lltp -lfreetype -lfontconfig -lexpat

SUBDIRS = \
Dialog_test \
Drawing_test \
List_text \
Pixbuf_test \
Widget_test \
Tictactoe


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
