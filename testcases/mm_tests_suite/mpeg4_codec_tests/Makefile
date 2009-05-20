###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE JPEG_ENCODER test application.
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
# I.Inkina                   26/11/2004     TLSbo40263   Initial version
#L.Delaspre\rc149c           16/03/2005     TLSbo48584   ARM11 codecs P1 tests
#L.Delaspre\rc149c           29/03/2005     TLSbo49055   ARM11 codecs P2 tests
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

SUBDIRS = \
mpeg4_encoder_testapp \
mpeg4_decoder_testapp \


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

