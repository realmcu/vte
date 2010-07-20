#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE SPI test applications.
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

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
