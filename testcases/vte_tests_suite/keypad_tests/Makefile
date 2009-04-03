##################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the keypad test applications.
#
###################################################################################################
#
#       Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#       THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#       BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#       Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Vincent BECKER/rc023c        07/05/2004     TLSbo38735  Initial version 
#Vincent BECKER/rc023c        25/05/2004     TLSbo38735  Change directory names 
#D.Simakov/smkd001c           10/06/2004     TLSbo39737  add keypad_testapp_3
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#V.Halabuda/b00306            15/02/2006     TLSbo61037  up for new keypad driver (/dev/input/event/0)
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################

ifeq ($(ARCH_PLATFORM), $(filter imx31stack imx37stack imx51stack, $(ARCH_PLATFORM)))
SUBDIRS = keypad_testapp_1
endif
SUBDIRS += keypad_test_script

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
