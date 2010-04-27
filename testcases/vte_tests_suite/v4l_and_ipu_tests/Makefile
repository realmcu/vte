#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE MC13783 test applications.
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
SUBDIRS = v4l_capture_testapp v4l_output_testapp v4l2_basic_test ipu_function 
endif


all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
	$(MAKE) -C pxp_test-1.01
install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	ln -f pxp_test-1.01/pxp_test ../../bin/
	ln -f pxp_test-1.01/testfiles/*.rgba32 ../../bin/
	ln -f pxp_test-1.01/testfiles/*.s* ../../bin/

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-$(MAKE) -C pxp_test-1.01 distclean
config:
	@set -e; cd pxp_test-1.01 && ./myconfig && cd ..
