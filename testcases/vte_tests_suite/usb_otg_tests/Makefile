####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sockets test.
#
####################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#A.Ozerov/B00320              04/07/2006     TLSbo58840  Initial version
#A.Ozerov/B00320              19/07/2007     ENGR43014   Two subdirs were added.
#
####################################################################################################



SUBDIRS = \
usb_ev_testapp \
usb_gserial_testapp \
usb_otg_cdc \
#ptp_module
#usb_ptp_testapp \

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
    
install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
