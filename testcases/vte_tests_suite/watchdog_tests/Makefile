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
#    @brief  configuration file to compile the second watchdog test applications.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Becker/rc023c              23/07/2004     TLSbo40889  Initial version 
#L.Delaspre/rc149c            03/08/2004     TLSbo40891  VTE 1.4 integration 
#D.Kazachkov/e1403c           13/08/2004     TLSbo41612  WD module creation
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#V.Khalabuda/b00306           06/07/2006     TLSbo69495  Update for MX27 platform in accordance with changes in time.c file
#Tina Zhao                    06/07/2008                 modify for imx chips
#Hake Huang                   04/15/2009                 add imx233 wdt tets case
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
SUBDIRS = watchdog_testapp_2 \
watchdog_testapp_3 \
wdt_test

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
