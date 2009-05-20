###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the ogg_decoder_testapp test.
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
#D.Simakov                   23/04/2006      TLSbo65887   Initial version
###################################################################################################
#Portability:
#       ARM GCC
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(OGGD_HDIR) -I$(COMMON_DIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -L$(OGGD_LIBDIR) -l$(OGGD_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -DFILEIO -DLITTLE_ENDIAN -DMAX_STR_LEN=512 -DWORDS_IN_ENTRY=3
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt
#CFLAGS    += -DDEBUG_TEST
#CFLAGS    += -DMAX_THREADS=2

SRCS       = $(wildcard *.c)
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = ogg_decoder_testapp
CONFIGS    = configs/*

include ../../common_codec/CommonCodec.mk

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : ogg_decoder_testapp test application **"
	@echo "************************************************"
	@echo

install:
	cp -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/configs
	cp -f $(CONFIGS) $(LTP_BIN)/configs
	cp -rf $(OGGD_INPUT) $(LTP_BIN)
	cp -rf $(OGGD_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/oggd_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(OGGD_LIBDIR)/lib$(OGGD_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "*******************************************"
	@echo "** INSTALL : ogg_decoder_testapp         **"
	@echo "*******************************************"
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY)
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS)



