###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the gif_encoder_testapp.
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
#     Elina Gromazina         25/02/2005      TLSbo47116   initial version
#     D.Simakov / smkd001c    06/04/2005      TLSbo47116   the configs are installed now
#     B.Giabicani\r70160c     18/04/2005      TLSbo49674   ARM11 codecs P3 tests
#     D.Simakov/smkd001c      24/10/2005      TLSbo57009   Rework
#     A.Pshenichnikov         07/12/2005      TLSbo59709   mem_stat.c was added
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(CODECS_DIR)/common
LOADLIBES  += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(GIFDEC_LIBDIR) -l$(GIFDEC_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs #-DDEBUG_TEST 
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS=$(wildcard *.c) sub/pars_conf.c sub/fb_draw_api.c sub/mem_stat.c
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=gif_decoder_testapp

CONFIGS = opts_cfg opts_cfg_er

all: $(TARGET)
	@echo "***************************************************"
	@echo "** ALL : gif_decoder_testapp test application    **"
	@echo "***************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -lf $(CONFIGS) $(LTP_BIN) 
	cp -rf $(GIFDEC_INPUT) $(LTP_BIN)
	cp -rf $(GIFDEC_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/gifd_output   
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -rf $(GIFDEC_LIBDIR)/lib$(GIFDEC_LIBRARY).so $(LTP_BIN)/sharedlibs                
	@echo "***************************************************"
	@echo "** INSTALL : gif_decoder_testapp                 **"
	@echo "***************************************************"
    
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : gif_decoder_test.h

