###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE AMR CODEC test application.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
# F.GAFFIE/rb657c    21/07/2004     TLSbo40930  Initial version
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = amr_decoder_vad1 amr_decoder_vad1_mms\
		amr_decoder_vad2 amr_decoder_vad2_mms\
		amr_decoder_nodtx amr_decoder_nodtx_mms\
		amr_encoder_vad1 amr_encoder_vad1_mms\
		amr_encoder_vad2 amr_encoder_vad2_mms\
		amr_encoder_nodtx\

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

