###############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the spdif test applications.
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
# Z.Spring            21/05/2009        N/A         Initial version
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers 
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################

SUBDIRS=
TARGETS=spdif_basic.sh

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@for i in $(TARGETS); do \
		ln -f $$i ../../bin/ ;\
		echo "***********************************************" ; \
		echo "** INSTALL : $$i " ;\
		echo "***********************************************" ; \
	done
	@echo

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done


