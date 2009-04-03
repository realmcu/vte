###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the QT Labels tests.
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
#
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#L.Delaspre/rc149c            08/03/2004     TLSbo47726   VTE 1.8 integration
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = Buttons \
Checklist \
Labels_Alignment \
Labels_Buddy \
Labels_Fonts \
Labels_Indent \
Labels_Pixmap \
Labels_TextFlags_1 \
Labels_TextFlags_2 \
Labels_TextFormat \
LineEdits \
ListBox \
Menu \
Splitter \
Table \


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

uninstall:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i uninstall ; done

generate:
	@set -e; for i in $(SUBDIRS); do cd $$i; qmake; cd ..; done
