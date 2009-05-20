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
# F.GAFFIE/rb657c             18/08/2004     TLSbo40930  Initial version
# F.DELASPRE/rc149c           16/09/2004     TLSbo42022  VTE 1.5 integration
# L.Delaspre\rc149c           16/03/2005     TLSbo48584  ARM11 codecs P1 tests
# L.Delaspre\rc149c           29/03/2005     TLSbo49055  ARM11 codecs P2 tests
# D.Simakov/smkd001c          06/02/2006     TLSbo61035  The phase2
###################################################################################################
#Portability:
#       ARM GCC
#
###################################################################################################

SUBDIRS = jpeg_encoder_testapp \
jpeg_decoder_testapp \


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

