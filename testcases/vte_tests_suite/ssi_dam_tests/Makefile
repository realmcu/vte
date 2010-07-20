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
#    @brief  configuration file to compile the VTE TEMPLATE test applications.
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
SUBDIRS = ssi_dam_testapp

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
