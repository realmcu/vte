###############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the FM test applications.
#
###############################################################################
#
#   Copyright (C) 2008, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###############################################################################
#Revision History:
#                    Modification     Tracking
#Author                  Date          Number    Description of Changes
#------------------  ------------    ----------  ----------------------
# Z.Spring            11/12/2008        N/A         Initial version
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers 
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################

SUBDIRS = fmtools-1.0.2 
TARGETS = fm_basic.sh 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc; done

install:
	@for i in $(TARGETS); do \
		ln -f $$i ../../bin/ ;\
		echo "***********************************************" ; \
		echo "** INSTALL : $$i " ;\
		echo "***********************************************" ; \
	done
	@echo
	-ln -f fmtools-1.0.2/fm ../../bin/
	-ln -f fmtools-1.0.2/fmscan ../../bin/

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done


