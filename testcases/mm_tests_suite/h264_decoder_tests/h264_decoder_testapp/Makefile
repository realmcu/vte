###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the h264_decoder_testapp test.
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
#D.Simakov/smkd001c           31/03/2005     TLSbo47116	 Initial version	
#B.Giabicani\r70160c	      18/04/2005     TLSbo49674  ARM11 codecs P3 tests
#D.Simakov/smkd001c           14/10/2005     TLSbo57009  Rework
#D.Simakov                    27/02/2006     TLSbo61035  Centralization of common features 
###################################################################################################
#Portability: ARM_GCC
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(H264D_HDIR) -I$(COMMON_DIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -L$(H264D_LIBDIR) -l$(H264D_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -DFILEIO -DLITTLE_ENDIAN -DMAX_STR_LEN=512 -DWORDS_IN_ENTRY=3
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt 
#CFLAGS    += -DDEBUG_TEST
CFLAGS    += -DPOST_BIT_MATCHING
#CFLAGS    += -DMAX_THREADS=2

SRCS       = $(wildcard *.c) 
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = h264_decoder_testapp
CONFIGS    = configs/*

include ../../common_codec/CommonCodec.mk

all: $(TARGET)
	@echo "*************************************************"
	@echo "** ALL : h264_decoder_testapp test application **"
	@echo "*************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/configs
	cp -f $(CONFIGS) $(LTP_BIN)/configs
	cp -rf $(H264D_INPUT) $(LTP_BIN)
	cp -ruf $(H264D_REF) $(LTP_BIN)        
	mkdir -p $(LTP_BIN)/h264d_output    
	mkdir -p $(LTP_BIN)/sharedlibs       
	cp -rf $(H264D_LIBDIR)/lib$(H264D_LIBRARY).so $(LTP_BIN)/sharedlibs    
	@echo "********************************************"
	@echo "** INSTALL : h264_decoder_testapp         **"
	@echo "********************************************"    
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 






