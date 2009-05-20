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
#   @brief  configuration file template to compile the jpeg_encoder_testapp test.
#
#   @par Portability:
#       ARM GCC
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
# F.GAFFIE/rb657c             3/05/2004      TLSbo39336  Initial version
# D.Simakov/smkd001c          7/02/2005      TLSbo47179	 Bad dependancies in the mm tests
#                                                        application build process
# L.Delaspre\rc149c           16/03/2005     TLSbo48584  ARM11 codecs P1 tests
# D.Simakov/smkd001c          18/10/2005     TLSbo57009  Rework
# D.Simakov/smkd001c          28/10/2005     TLSbo59191  Improved
##############################################################################
#
#
##############################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(JPEGE_HDIR) -I$(COMMON_DIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(JPEGE_LIBDIR) -l$(JPEGE_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN -DMAX_STR_LEN=512 -DWORDS_IN_ENTRY=19
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt
CFLAGS    += #-DPOST_BIT_MATCHING #-DDEBUG_TEST

SRCS       = $(wildcard *.c)
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = jpeg_encoder_testapp
CONFIGS    = configs/*

include ../../common_codec/CommonCodec.mk

all: $(TARGET)
	@echo "*************************************************"
	@echo "** ALL : jpeg_encoder_testapp test application **"
	@echo "*************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/configs
	cp -f $(CONFIGS) $(LTP_BIN)/configs
	cp -rf $(JPEGE_INPUT) $(LTP_BIN)
	cp -rf $(JPEGE_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/jpege_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(JPEGE_LIBDIR)/lib$(JPEGE_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "*****************************************************"
	@echo "** INSTALL : jpeg_encoder_testapp test application **"
	@echo "*****************************************************"
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS)
	$(CC)  $(OBJS) $(LOADLIBES) -o  $@

$(OBJS) : $(SRCS)




