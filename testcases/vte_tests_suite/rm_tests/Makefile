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
#Telma                        10/06/2004     TLSbo39741   initial version 
#Ludovic Delaspre / rc149c    22/06/2004     TLSbo39941   VTE 1.3 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = message_testapp_1 pipe_testapp_1 pipe_testapp_2 pipe_testapp_3 \
semaphore_testapp_1 semaphore_testapp_2 semaphore_testapp_3 \
shared_mem_testapp_1 shared_mem_testapp_2 shared_mem_testapp_3 \
shared_mem_testapp_4 shared_mem_testapp_5 shared_mem_testapp_6 shared_mem_testapp_7 \
signal_testapp_1 signal_testapp_2 signal_testapp_3 signal_testapp_4 \
signal_testapp_5 signal_testapp_6 signal_testapp_7

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

