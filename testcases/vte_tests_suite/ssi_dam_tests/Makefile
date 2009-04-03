###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE TEMPLATE test applications.
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
#Tony THOMASSIN/RB595C        29/07/2004     TLSbo41151   SSI test development 
#L.Delaspre/rc149c            09/12/2004     TLSbo40142   update with Freescale identity
#D.Khoroshev/NONE             02/12/2005     TLSbo56844   update for PMIC API
#D.Simakov                    05/06/2006     TLSbo67103   No sound on SSI1 and SSI2 
###################################################################################################
#Portability: ARM GCC
###################################################################################################
SUBDIRS = ssi_dam_testapp \
	pmic_module

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
