###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the second watchdog test applications.
#
###################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V.Becker/rc023c              23/07/2004     TLSbo40889  Initial version
#L.Delaspre/rc149c            03/08/2004     TLSbo40891  VTE 1.4 integration
#D.Kazachkov/e1403c           13/08/2004     TLSbo41612  WD module creation
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#V.Khalabuda/b00306           06/07/2006     TLSbo69495  Update for MX27 platform in accordance with changes in time.c file
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
#ifeq ($(ARCH_PLATFORM), $(filter imx31stack imx35stack imx37stack imx51stack, $(ARCH_PLATFORM)))
SUBDIRS = watchdog_testapp_2 \
watchdog_testapp_3
#endif

#       Kernel_driver \
#	watchdog_testapp_1 \
#	watchdog_testapp_2  \
#      watchdog_testapp_3

#ifeq ($(ARCH_PLATFORM),imx27ads)
#SUBDIRS += watchdog_testapp_2
#else
#SUBDIRS += \
#tina changed
#     Kernel_driver \
#watchdog_testapp_1
#endif

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
