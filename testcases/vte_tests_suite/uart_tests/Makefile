###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE UART test applications.
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
#V.Becker/rc023c        25/05/2004     TLSbo39126   Change directory names 
#L.Delaspre/rc149c      09/12/2004     TLSbo40142   update with Freescale identity
#I. Inkina / nknl001    07/04/2005     TLSbo48749   testapp_4 testapp_5
#E.Gromazina            08/04/2005     TLSbo48747   testapp_3 testapp_6
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = \
uart_testapp_3 \
uart_testapp_4 \
uart_testapp_5 \
uart_testapp_6 \
uart_testapp_7 \
uart_testapp_8 \
uart_testapp_9 \
uart_scripts


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
