###############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the WIFI test applications.
#
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
###############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
###############################################################################
#Revision History:
#                 Modification     Tracking
#Author               Date          Number    Description of Changes
#---------------  ------------    ----------  ----------------------
# Z.Spring         28/11/2008     ENGR100588   Initial ver, move from
#                                           testscripts to vte_test_suite
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers 
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################

SUBDIRS=iperf
TARGETS=
# AP test scripts
SCRIPTS=wifi_testap001.sh wifi_testap002.sh wifi_testap003.sh wifi_testap004.sh wifi_testap005.sh wifi_testap008.sh wifi_testap009.sh wifi_testap010.sh
# ad-hoc test scripts
SCRIPTS+= wifi_testadhoc005.sh wifi_testadhoc005c.sh wifi_testadhoc005cw.sh
SCRIPTS+= wifi_testadhoc006.sh wifi_testadhoc006c.sh wifi_testadhoc006cw.sh
SCRIPTS+= wifi_testadhoc007.sh wifi_testadhoc007c.sh wifi_testadhoc007cw.sh
# config file
SCRIPTS+= WPA-PSK.conf WPA-PSK-wrong.conf WPA-PSK2.conf

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@for i in $(SCRIPTS); do \
		ln -f $$i ../../bin/ ;\
		echo "***********************************************" ; \
		echo "** INSTALL : $$i " ;\
		echo "***********************************************" ; \
	done
	@echo

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done


