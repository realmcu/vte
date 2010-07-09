#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#    @brief  configuration file to compile the VTE RTC test applications.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Becker/rc023c           19/04/2004   TLSbo38652   Initial version
#V.Becker/rc023c           25/05/2004   TLSbo38652   Change directory names 
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#E.Gromazina			29/06/2005	   TLSbo49951   update for rtc_testapp_5
#
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################
SUBDIRS =rtc_testapp_1 \
         rtc_testapp_2 \
         rtc_testapp_3 \
         rtc_testapp_4 \
         rtc_testapp_5 \
         rtc_testapp_6 \
         rtc_testapp_7 \
         rtc_scripts

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
