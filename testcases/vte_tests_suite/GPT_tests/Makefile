#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the NOR MTD test applications.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Becker/rc023c              25/06/2004     TLSbo39142   Change subdir names 
#S.ZAVJALOV/ZVJS001C          27/07/2004     TLSbo40261   nand mtd
#L.DELASPRE/rc149c            02/08/2004     TLSbo40891   VTE 1.4 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#D.Khoroshev/b00313           03/13/2007     TLSbo92116   NAND test removed for mxc27520evb
#
###################################################################################################
#Portability: NAND is not supported on Islay(mxc27520evb). 
#
###################################################################################################

SUBDIRS = GPT_testapp


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

