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
#    @brief  configuration file to compile the VTE MU test applications.
#
###################################################################################################
#Revision History:
#                              Modification     Tracking
#Author (core ID)                  Date          Number    Description of Changes
#---------------------------   ------------    ----------  ------------------------------------------
#Igor Semenchukov (smng001c)    26/08/2004     TLSbo40411   Initial version 
#Ludovic Delaspre (rc149c)      13/09/2004     TLSbo42022   VTE 1.5 integration
#L.Delaspre/rc149c              08/12/2004     TLSbo40142   update with Freescale identity
#Igor Semenchukov (smng001c)    21/12/2004     TLSbo43804   MU tests update
#Igor Semenchukov (smng001c)    09/03/2005     TLSbo47942   Ported IPC's team test application
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = mu_api_err_test \
mu_api_ioctl_test \
mu_api_open_test \
mu_api_rw_test \
mu_api_rwstress_test \
mu_sdma_ipc_test 


ifeq ($(ARCH_PLATFORM),imx31ads)
SUBDIRS = 
endif

ifeq ($(ARCH_PLATFORM),imx27ads)
SUBDIRS = 
endif


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

