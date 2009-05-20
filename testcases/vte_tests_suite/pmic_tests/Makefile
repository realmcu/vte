####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile PMIC tests.
#
####################################################################################################
#
#        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
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
endif

ifeq ($(ARCH_PLATFORM),imx31ads)
CFLAGS+=    -DCONFIG_MXC_PMIC_MC13783
SUBDIRS +=\
        pmic_battery_testapp	\
	pmic_power_testapp	\
	pmic_protocol_testapp	\
	pmic_light_testapp      \
	pmic_audio_testapp	\
	pmic_adc_testapp        \
	pmic_convity_testapp	\
        pmic_rtc_testapp
endif

ifeq ($(ARCH_PLATFORM),imx31stack)
CFLAGS+=    -DCONFIG_MXC_PMIC_MC13783
SUBDIRS +=\
        pmic_battery_testapp	\
	pmic_protocol_testapp	\
	pmic_adc_testapp        \
        pmic_rtc_testapp

endif

ifeq ($(ARCH_PLATFORM),imx35stack)
ifeq ($(CONFIG_MXC_PMIC_MC9SDZ60),y)
CFLAGS+=    -DCONFIG_MXC_PMIC_MC9SDZ60
SUBDIRS +=\
	pmic_protocol_testapp	\
        pmic_rtc_testapp
endif


endif

ifeq ($(ARCH_PLATFORM),imx37stack)
#CFLAGS+=
#SUBDIRS +=\
	pmic_protocol_testapp	\
	pmic_light_testapp

endif

ifeq ($(ARCH_PLATFORM),imx51stack)
#CFLAGS+=    -DCONFIG_MXC_PMIC_MC13892
#SUBDIRS +=\
          pmic_13892   \
          pmic_adc_testapp   \
          pmic_rtc_testapp
endif

CFLAGS+=-Dbool=int -Dture=1 -Dfalse=0

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done
