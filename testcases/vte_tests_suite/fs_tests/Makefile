###############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the FS test applications.
#
###############################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###############################################################################
#Revision History:
#                    Modification     Tracking
#Author                  Date          Number    Description of Changes
#------------------  ------------    ----------  ----------------------
#S.Zavjalov/-----c    10/06/2004     TLSbo39741  Initial version
#L.Delaspre/rc149c    08/12/2004     TLSbo40142  update with Freescale identity
# Spring Zhang        07/02/2008        n/a      Add jffs2,nfs,sysfs,etc. tests
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers 
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################

# Previous fs test cases
SUBDIRS+=fs_cr_rm fs_fsck fs_inod
# Test cases reused from LTP kernel/fs
SUBDIRS+=fs_perms doio
# New fs test cases
SUBDIRS+=udev
TARGETS=jffs2_test1.sh cramfs_test1.sh procfs_test1.sh sample.cramfs 
TARGETS+=jffs2_test2.sh jffs2_test3.sh

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


