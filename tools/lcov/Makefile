##########################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to support gcov/lcov in VTE.
#
##########################################################################################
#
#                         Freescale Semiconductor Confidential Proprietary
#                 (c) Copyright 2004, Freescale Semiconductor.  All rights reserved.
#
# Presence of a copyright notice is not an acknowledgement of publication.
# This software file listing contains information of Freescale
# Semiconductor. that is of a confidential and proprietary nature and
# any viewing or use of this file is prohibited without specific
# written permission from Freescale Semiconductor.
#
##########################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  ----------------------------
#C.GAGNERAUD cgag1c          2004/11/04      Tlsbo42836  Creation
##########################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
##########################################################################################



TARGET_FILES = lcov-capture.sh \
               lcovize-runtest.sed \
               lcov-reset.sh

HOST_FILES   = gendesc \
               genhtml \
               geninfo \
               genpng \
               lcov \
               vte-genhtml.sh \
               vte-lcov.sh

PATCHES      = linux-L26.1.5-gcov-2.6.5.patch \
               linux-L26.1.5-gcov-vpath.patch \
               linux-L26.1.5-gcov-read_proc.patch \
               linux-L26.1.5-gcov-arm.patch \
               linux-L26.1.5-gcov-mxc.patch \
               linux-L26.1.5-gcov-freescale.patch

EXTRADIST    = README \
               README.lcov \
               COPYING.lcov

all:

install:
	cp -f $(TARGET_FILES) ../../testcases/bin
	cd ../../testcases/bin && chmod a+x $(TARGET_FILES)

clean:


host-install:
	if [ "x$(BINDIR)" == "x" ]; then \
		echo "***"; \
		echo "*** [ERROR] Usage: make $@ BINDIR=<...>"; \
		echo "***"; \
		exit 1; \
	fi
	cp -f $(HOST_FILES) $(BINDIR)
	cd $(BINDIR) && chmod a+x $(HOST_FILES)

distclean: clean
	find . -name '*~' -o -name '#*#' -exec rm {} \;

patch-kernel:
	if [ "x$(KERNEL_PREFIX)" == "x" ]; then \
		echo "***"; \
		echo "*** [ERROR] Usage: make $@ KERNEL_PREFIX=<path_to_linux_tree>"; \
		echo "***"; \
		exit 1; \
	fi
	here=`pwd`; \
	cd $(KERNEL_PREFIX); \
	for f in $(PATCHES); do \
		echo "*** Applying $$f..."; \
		patch -p1 < $$here/$$f; \
	done
