###############################################################################
#
#    @file   Makefile
#
#    @brief  the third part test suite for storage.
#
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
###############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
###############################################################################
#Revision History:
#                 Modification     Tracking
#Author               Date          Number    Description of Changes
#---------------  ------------    ----------  ----------------------
# Victor Cui      01/12/2008                  Initial ver
# Hake Huang      07/19/2010                  restructure
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers 
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################
#CLEAN_DIRS=`ls */Makefile | sed "s/Makefile//g"`

include ./vte_configs/$(ARCH_PLATFORM)_tp_configs

SUBDIRS-y                        += c-ray
SUBDIRS-y                        += coremark
SUBDIRS-y                        += util-linux
SUBDIRS-$(CONFIG_TP_BONNIE)      += bonnie++ 
SUBDIRS-$(CONFIG_TP_BC)          += bc
SUBDIRS-$(CONFIG_TP_CANSOCKET)   += can
SUBDIRS-$(CONFIG_TP_CPUFREQ)     += cpufrequtils-008
SUBDIRS-$(CONFIG_TP_CPUPOWER)    += cpupowerutils
SUBDIRS-$(CONFIG_TP_CRASHME)     += crashme-2.4 
SUBDIRS-$(CONFIG_TP_CRYPTO_APP)  += crypto_app
SUBDIRS-$(CONFIG_TP_DRY)         += dry-2.2 
SUBDIRS-$(CONFIG_TP_DT)          += dt 
SUBDIRS-$(CONFIG_TP_FBSET)       += fbset
SUBDIRS-$(CONFIG_TP_FBV)         +=	fbv
SUBDIRS-$(CONFIG_TP_FMTOOL)      += fm_tool
SUBDIRS-$(CONFIG_TP_HOST_TOOLS)  += host_tools
SUBDIRS-$(CONFIG_TP_IOZONE)      += iozone 
SUBDIRS-$(CONFIG_TP_I2C)         += i2c-tools
SUBDIRS-$(CONFIG_TP_IPERF)       += iperf
SUBDIRS-$(CONFIG_TP_LMBENCH)     += lmbench
SUBDIRS-$(CONFIG_TP_LINARO_PM)   += linaro-pm-qa
SUBDIRS-$(CONFIG_TP_NETWORK)     += network 
SUBDIRS-$(CONFIG_TP_NUTTCP)      += nuttcp
SUBDIRS-$(CONFIG_TP_OPROFILE)    +=	oprofile
SUBDIRS-$(CONFIG_TP_PFM)         += pfm
SUBDIRS-$(CONFIG_TP_STREAMBENCH) += streambench 
SUBDIRS-$(CONFIG_TP_USB)         += usb_test
SUBDIRS-$(CONFIG_TP_WHETS)       += whets

all:
	for i in $(SUBDIRS-y); do \
	$(MAKE) -C $$i ; \
	done

install:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install ; done

clean:
	-@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i clean ; done
