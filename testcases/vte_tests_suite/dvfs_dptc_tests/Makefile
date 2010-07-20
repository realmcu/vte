#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the VTE DPTC test applications.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Simakov/smkd001c           10/09/2004     TLSbo41777  Initial version 
#D.Simakov/smkd001c           27/09/2004     TLSbo41777  Add dtpc_module
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#L.Delaspre/rc149c            14/12/2004     TLSbo45361  mxc_kpp_interrupt build issue
#V.Khalabuda/b00306           14/12/2006     TLSbo80349  Version for DVFS/DTPC tests
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
SUBDIRS = dvfs_dptc_testapp

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
