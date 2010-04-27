#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile PMIC tests.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#V.HALABUDA/HLBV001           26/07/2005     TLSbo52694  Initial version
#V.HALABUDA/HLBV001           09/12/2005     TLSbo59968  Update for common API version
#C.Gagneraud/cgag1c           17/02/2006     TLSbo62202  VTE-2.0 integration: fix multi-platform 
#                                                        build issue
#A.Ozerov/b00320              31/05/2006     TLSbo64237  Names of connectivity testapp and module were#                                                        changed.
#A.Ozerov/b00320              01/06/2006     TLSbo64237  Support MC13783 for PMIC light testapp was added.
#A.Ozerov/b00320              10/08/2006     TLSbo73745  sc55112_audio_module and sc55112_audio_testapp names were changed to pmic_*.
#A.Ozerov/b00320              11/08/2006     TLSbo74269  pmic_convity_testapp is worked for MC13783.
#Rakesh S Joshi/R65956        01/03/2007     TLSbo88188  pmic_power_module added.
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
export CFLAGS+=-Wall -I../../../../include
export LOADLIBES+=-L../../../../lib -lltp -lpthread

# - PMIC for MXC91131 have headers not synchronized with other platforms
# - PMIC for MXC91131 have not yet RTC support
# - PMIC work only with MC13783 2.X

# Only supported on MXC91131
ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+=    -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+=    -DCONFIG_MXC_PMIC_MC13783
endif

CFLAGS+=    -Dbool=int

#add by Spring
ifeq ($(ARCH_PLATFORM), $(filter imx31stack, $(ARCH_PLATFORM)))
SUBDIRS += ts_testapp1 
endif

SUBDIRS += ts_testapp2 
SUBDIRS += ts_testapp3 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done
