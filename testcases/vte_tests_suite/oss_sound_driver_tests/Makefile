###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE MC13783 test applications.
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
#RB657C /gsch1c              20/07/2004      TLSbo40898   OSS sound driver test development
#RB657C /gsch1c              28/09/2004      TLSbo43102   Added oss_audio_rec_testapp_1
#Ludovic Delaspre/rc149c     09/11/2004	     TLSbo44472   complete the CR TLSbo43778
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = \
oss_audio_control_testapp_0 \
oss_audio_control_testapp_1 \
#tina changed oss_audio_mngt_testapp_1 \
oss_audio_mngt_testapp_2 \
oss_audio_play_testapp_0 \
oss_audio_play_testapp_1 \
oss_audio_play_testapp_2 \
oss_audio_rec_testapp_0 \
oss_audio_rec_testapp_1

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
