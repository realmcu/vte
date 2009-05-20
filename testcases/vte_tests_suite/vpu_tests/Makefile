###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file.
#
###################################################################################################
#
#   Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Kardakov                  02/26/2007      TLSbo79826  Initial version
###################################################################################################
#Portability: ARM GCC
###################################################################################################
BIN_DIR = ../..
SUBDIRS = vpu_lib \
	vpu_testapp

#ifeq ($(ARCH_PLATFORM), imx37stack)
#SOURCEDIR = ./arch-mx37
#endif

#ifeq ($(ARCH_PLATFORM), imx51stack)
SOURCEDIR = ./arch-mx51
#endif



all:
	@rm -rf $(SUBDIRS)
	@set -e; for i in $(SOURCEDIR);do ln -s $$i/* . ; done
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	-ln -f ./configs/* $(BIN_DIR)/bin
	@$(MAKE) -C test_script install

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@rm -rf $(SUBDIRS)
