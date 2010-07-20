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

SUBDIRS-$(CONFIG_VTE_V4l_IPU_PF)   += ipu_pf_testapp
SUBDIRS-$(CONFIG_VTE_V4l_CAPTURE)  += v4l_capture_testapp
SUBDIRS-$(CONFIG_VTE_V4l_OUTPUT)   += v4l_output_testapp
SUBDIRS-$(CONFIG_VTE_V4l_BASIC)    += v4l2_basic_testapp
SUBDIRS-$(CONFIG_VTE_V4l_IPU_FUNC) += ipu_function

all: 
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
