###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE SPI test applications.
#
###################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490  SPI test development 
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
SUBDIRS = 
#	spi_test_module \
#	spi_testapp \
#	spi_testapp_0
#	spi_testapp_1 \
#	spi_testapp_2 \
#	spi_testapp_3

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
