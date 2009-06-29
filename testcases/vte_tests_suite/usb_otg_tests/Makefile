#Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sockets test.
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
usb_otg_cdc
#ptp_module
#usb_ptp_testapp \

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
    
install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
