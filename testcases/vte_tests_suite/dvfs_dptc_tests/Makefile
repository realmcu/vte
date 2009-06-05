###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE DPTC test applications.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Simakov/smkd001c           10/09/2004     TLSbo41777  Initial version 
#D.Simakov/smkd001c           27/09/2004     TLSbo41777  Add dtpc_module
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#L.Delaspre/rc149c            14/12/2004     TLSbo45361  mxc_kpp_interrupt build issue
#V.Khalabuda/b00306           14/12/2006     TLSbo80349  Version for DVFS/DTPC tests
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
SUBDIRS = dvfs_dptc_testapp

#
# DPTC only supported on MC13783
#

#dptc_module \
# MXC300-30 EVB
# ifeq ($(ARCH_PLATFORM),mxc30030evb)
# SUBDIRS = dvfs_testapp
# endif

# ifeq ($(ARCH_PLATFORM),imx31ads)
# SUBDIRS = dvfs_testapp
# endif

# ifeq ($(ARCH_PLATFORM),imx27ads)
# SUBDIRS = dvfs_testapp
# endif

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
