###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the jpeg_decoder_testapp test.
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
#F.GAFFIE/rb657c              03/05/2004     TLSbo39336  Initial version
#L.DELASPRE/rc149c            16/09/2004     TLSbo42022  VTE 1.5 integration
#D.Simakov/smkd001c           07/02/2004     TLSbo47179	 Bad dependancies in the mm tests
#D.Simakov/smkd001c           23/03/2004     TLSbo48795  application build process
#L.Delaspre/rc149c            29/03/2005     TLSbo49055  ARM11 codecs P2 tests
#D.Simakov/smkd001c           28/10/2005     TLSbo57009  Rework
#A.Pshenichnikov	          28/11/2005     TLSbo59532  mem_stat.c was added
#D.Simakov/smkd001c           01/02/2006     TLSbo61035  Centralization of common features
##################################################################################################
#Portability: ARM GCC
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(JPEGD_HDIR) -I$(COMMON_DIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(JPEGD_LIBDIR) -l$(JPEGD_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN -DMAX_STR_LEN=512 -DWORDS_IN_ENTRY=7
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt
#CFLAGS    += -DPOST_BIT_MATCHING
#CFLAGS    += -DDEBUG_TEST

SRCS       = $(wildcard *.c)
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = jpeg_decoder_testapp
CONFIGS    = configs/*

include ../../common_codec/CommonCodec.mk

all: $(TARGET)
	@echo "*************************************************"
	@echo "** ALL : jpeg_decoder_testapp test application **"
	@echo "*************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/configs
	cp -f $(CONFIGS) $(LTP_BIN)/configs
	cp -rf $(JPEGD_INPUT) $(LTP_BIN)
	cp -rf $(JPEGD_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/jpegd_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(JPEGD_LIBDIR)/lib$(JPEGD_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "*****************************************************"
	@echo "** INSTALL : jpeg_decoder_testapp test application **"
	@echo "*****************************************************"
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS)
	$(CC)  $(OBJS) $(LOADLIBES) -o  $@

$(OBJS) : $(SRCS)

