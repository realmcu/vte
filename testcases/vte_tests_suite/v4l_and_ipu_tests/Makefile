###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE MC13783 test applications.
#
###################################################################################################
#
#                              Motorola Confidential Proprietary
#                   (c) Copyright 2004, Motorola, Inc.  All rights reserved.
#
#Presence of a copyright notice is not an acknowledgement of publication.
#This software file listing contains information of Motorola, Inc. that is of a confidential and
#proprietary nature and any viewing or use of this file is prohibited without specific written
#permission from Motorola, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Geniatov/gntl002	10/04/2004     TLSbo41670   Initial version
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################
export CFLAGS=-Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
export LOADLIBES=-L../../../../lib -lltp -lpthread -lm

#SUBDIRS = v4l_capture_testapp v4l_output_testapp v4l2_basic_test

ifeq ($(ARCH_PLATFORM), $(filter imx31stack imx35stack, $(ARCH_PLATFORM)))
SUBDIRS = v4l_capture_testapp v4l_output_testapp v4l2_basic_test  ipu_pf_testapp
else
SUBDIRS = v4l_capture_testapp v4l_output_testapp v4l2_basic_test
endif


all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
	$(MAKE) -C pxp_test-1.01
install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	ln -f pxp_test-1.01/pxp_test ../../bin/
	ln -f pxp_test-1.01/testfiles/* ../../bin/

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	$(MAKE) -C pxp_test-1.01 distclean
config:
	@set -e; cd pxp_test-1.01 && ./myconfig && cd ..
