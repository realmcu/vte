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
#        @file   Makefile
#
#        @brief  configuration file to compile the VTE tests.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#S.ZAVJALOV/None              26/08/2004     TLSbo40418  Initial version                          
#S.ZAVJALOV/None              01/10/2004     TLSbo40649  Zeroize, HACC, KEM                          
#L.DELASPRE/RC149C            11/10/2004     TLSbo42821  VTE 1.5.1 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#A.Urusov/None                10/10/2005     TLSbo55835  Replace AIC KEM Zeroize with SCC
#D.Kazachkov/e1403c           26/10/2005     TLSbo55834  add Sahara under conditional compilation
#A.Ozerov/None                01/12/2005     TLSbo58662  Update for linux-2.6.10-rel-L26_1_14
#A.Ozerov/b00320              02/05/2006     TLSbo66687  Support MXC30030evb, MXC30030ads platforms
#                                                        for Sahara driver test suite was added.
#V.Khalabuda/b00306           12/09/2006     TLSbo76799  Update the iMX27 test suite environment
#V.Khalabuda/b00306           20/09/2006     TLSbo78166  VTE Data Base update
#Pradeep K /b01016            07/05/2007     ENGR38017   Update RTIC for Islay
#D.Khoroshev/b00313           07/22/2007     engr42278   Added imx32ads and mxc30031ads
#                                                        platforms
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
# Zeus (Zeus EVB)
MXC91131 = mxc91131evb
# SCMA-11 (SCMA-A11 EVB and Sonic)
MXC91231 = mxc27530evb mxc27530ads
# Argon LV (Argon LV EVB and MXC300-30 BB)
MXC91321 = mxc30030evb mxc30030ads
# Argon Plus (Argon Plus EVB and Odyssey)
MXC91331 = i30030evb i30030ads
# SCMA-9 (Islay) 
MXC91221 = mxc27520evb
# i.MX27 (i.MX27 ADS and i.MX27 EVB)
iMX27 = imx27evb imx27ads 
# i.MX31 (i.MX31 ADS)
iMX31 = imx31ads

# RNG is Supported by MXC27520evb
#ifeq ($(ARCH_PLATFORM),mxc27520evb)
#SUBDIRS += rnga_testcase \
#           rnga_test_module
#endif

# SAHARA is supported by MXC27530evb, MXC30030evb, iMX27ads, imx27evb
#ifeq ($(ARCH_PLATFORM),imx27evb)
#SUBDIRS += sahara_testapp \
#           sahara_module
#endif

#ifeq ($(ARCH_PLATFORM),mxc27530evb)
#SUBDIRS += sahara_testapp \
#           sahara_module
#endif

#ifeq ($(ARCH_PLATFORM),mxc30031ads)
#SUBDIRS += sahara_testapp \
#           sahara_module
#endif

#ifeq ($(ARCH_PLATFORM),mxc30030evb)
#SUBDIRS += sahara_testapp \
#           sahara_module
#endif

ifeq ($(ARCH_PLATFORM),$(filter imx27ads imx51stack, $(ARCH_PLATFORM)))
SUBDIRS += sahara_testapp \
           sahara_module
endif
        
ifeq ($(ARCH_PLATFORM), $(filter imx31stack imx35stack, $(ARCH_PLATFORM)))
SUBDIRS += scc_test_module  \
          scc_testcases  
endif
             
ifeq ($(ARCH_PLATFORM), $(filter imx37stack imx51stack, $(ARCH_PLATFORM)))
SUBDIRS += scc2_test_module  \
          scc2_testcases    
endif

ifeq ($(ARCH_PLATFORM),$(filter imx37stack imx35stack,$(ARCH_PLATFORM)))
SUBDIRS += rng_test_module \
           rng_testcase
endif

ifeq ($(ARCH_PLATFORM), $(filter imx31stack, $(ARCH_PLATFORM)))
SUBDIRS += rnga_test_module  \
          rnga_testcase  
endif
 
ifeq ($(ARCH_PLATFORM), $(filter imx233, $(ARCH_PLATFORM)))
SUBDIRS += crypto 
endif



all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
