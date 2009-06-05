###############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the ALSA test applications.
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
# Z.Spring            07/24/2008        N/A         Initial version
# Z.Spring            11/24/2008        N/A       Add platform identify script
#
###############################################################################
#Portability: Indicate if this module is portable to other compilers 
#             or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###############################################################################

SUBDIRS=
TARGETS=dac_test1.sh adc_test1.sh dac_vol_adj.sh alsa_ctl.sh dac_channel_swap.sh
TARGETS+=speaker_test1.sh speaker_switch_headset.sh
TARGETS+=audio48k16S.wav
#Platform identify script
TARGETS+=platfm_identify.sh

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


