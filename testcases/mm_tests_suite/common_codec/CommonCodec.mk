##############################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#     
##############################################################################
#
#   @file   Makefile
#
#   @brief  configuration file template to compile the mm tests.
#
#   @par Portability:
#       ARM GCC
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
# D.Simakov/smkd001c   24/01/2006   TLSbo61035   Initial version
# D.Simakov            27/02/2006   TLSbo61035   kev_io.c and ycbcr.c were added
##############################################################################
# 
#
##############################################################################

COMMON_DIR = ../../common_codec

CFLAGS += -I$(COMMON_DIR) -I.

SRCS   += $(COMMON_DIR)/common_codec_main.c \
          $(COMMON_DIR)/common_codec_test.c \
          $(COMMON_DIR)/util/cfg_parser.c \
          $(COMMON_DIR)/util/llist.c \
          $(COMMON_DIR)/util/mem_stat.c \
          $(COMMON_DIR)/util/fb_draw_api.c \
          $(COMMON_DIR)/util/kev_io.c \
          $(COMMON_DIR)/util/ycbcr.c