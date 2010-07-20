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
#Hake huang/b20222            07/20/2010                 restructure
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################

SUBDIRS-$(CONFIG_VTE_SAHARA) += sahara_testapp
SUBDIRS-$(CONFIG_VTE_SCC)    += scc_testcases  
SUBDIRS-$(CONFIG_VTE_SCC2)   += scc2_testcases    
SUBDIRS-$(CONFIG_VTE_RNG)    += rng_testcase
SUBDIRS-$(CONFIG_VTE_RNGA)   += rnga_testcase  
SUBDIRS-$(CONFIG_VTE_CRYPTO) += crypto_scripts 

all:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i clean ; done
