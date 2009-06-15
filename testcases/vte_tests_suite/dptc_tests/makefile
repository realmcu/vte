###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the dvfs_dptc_testapp test.
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
#Blake

all:
	if [ ${ARCH_PLATFORM} == "imx25stack" ]; then $(MAKE) -C modules; fi

install:
	ln -f dptc.sh ../../bin
	if [ ${ARCH_PLATFORM} == "imx25stack" ]; then $(MAKE) -C modules install; fi
	if [ ${ARCH_PLATFORM} == "imx25stack" ]; then ln -f dryice.sh ../../bin/ ; fi
	@echo "***********************************************"
	@echo "** INSTALL : dptc.sh                    **"
	@echo "***********************************************"
	@echo

clean:
	@echo "***********************************************"
	@echo "** CLEAN : dptc.sh                      **"
	@echo "***********************************************"
	@echo
	if [ ${ARCH_PLATFORM} == "imx25stack" ]; then $(MAKE) -C modules clean; fi
