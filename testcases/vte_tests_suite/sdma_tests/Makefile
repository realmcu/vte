###############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE TEMPLATE test applications.
#
################################################################################
#
#    Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
###############################################################################
#Revision History:
#                      Modification     Tracking
#Author                    Date          Number    Description of Changes
#-------------------   ------------    ----------  ----------------------
#D.Kazachkov/e1403c     27/07/2004     TLSbo40261   initial version
#L.Delaspre/rc149c      03/08/2004     TLSbo40891   VTE 1.4 integration
#L.Delaspre/rc149c      08/12/2004     TLSbo40142   update with Freescale identity
#E.Gromazina            06/12/2005     TLSbo60060   remove sdma_tty from the compilation for MXC91131
#O.DAVARD               03/05/2006     TLSbo68696   imxc27ads platform added
#Z.Spring               19/11/2008     n/a          disable sdma_testapp_api for
#                                                   it is recommended to use
#                                                   unified SDMA interface.
###############################################################################
#Portability: ARM GCC
#
###############################################################################
#SUBDIRS = sdma_testapp_api sdma_testapp_rw
SUBDIRS = sdma_testapp_rw

#
# Not supported by i.MX31 and MXC91131 EVB
#
ifneq ($(ARCH_PLATFORM),imx31ads)
ifneq ($(ARCH_PLATFORM),mxc91131evb)
ifneq ($(ARCH_PLATFORM),imx27ads)
#SUBDIRS += sdma_testapp_tty
endif
endif
endif

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
