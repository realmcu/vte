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
#    @brief  configuration file to compile the VTE MMC test applications.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Becker/rc023c         07/09/2004     TLSbo40423   Initial version 
#V.Becker/rc023c         15/10/2004     TLSbo40423   Add mmc_testapp_5 
#V.Becker/rc023c         25/10/2004     TLSbo43545   Update test for L26.1.6
#L.Delaspre/rc149c       08/12/2004     TLSbo40142   update with Freescale identity
#L.Delaspre/rc149c       21/01/2005     TLSbo45643   VTE 1.7 integration
#D.Kazachkov/e1403c      22/03/2005     TLSbo46507   Update test for L26.1.7   
#S.ZAVJALOV/ZVJS001C     01/04/2005     TLSbo45047   Testcase nominal functionality added
#E.Gromazina             27.10.2005     TLSbo57740   Erasing the obsolete tests
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################
ifeq ( $(PROJECT_MX37), 1)
SUBDIRS = \
eject_mmc_testcase \
rw_mmc_testcase \
perform_mmc_testcase \
stressRW_mmc_testcase
else
SUBDIRS = \
erw_mmc_testcase \
lock_mmc_testcase \
eject_mmc_testcase \
rw_mmc_testcase \
perform_mmc_testcase \
stressRW_mmc_testcase
endif

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	ln -f mmc_test_minix.sh ../../bin
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
