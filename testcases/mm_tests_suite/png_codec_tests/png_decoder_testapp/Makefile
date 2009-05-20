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
#   @brief  configuration file to compile the png_decoder_testapp test.
#
#   @par Portability:
#           SCM-A11, Argon+
#           arm-linux-gcc 3.4
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   -----------------------------
#D.Simakov/smkd001c    12/10/2004   TLSbo41678   Initial version
#D.Simakov/smkd001c    06/04/2005   TLSbo47116   The configs are copied now.
#B.Giabicani\r70160c   18/04/2005   TLSbo49674   ARM11 codecs P3 tests
#D.Simakov/smkd001c    24/10/2005   TLSbo57009   Rework
#A.Pshenichnikov       13/01/2006   TLSbo61033   Makefiles / shared libraries
#
##############################################################################

LTP_BIN = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm  -Bdynamic -L$(PNGD_LIBDIR) -l$(PNGD_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS=$(wildcard *.c) stuff/fconf.c stuff/fb_draw_api.c
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET = png_decoder_testapp

CONFIGS = pngd_cfg pngd_cfg_corrupt

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : png_decoder_testapp test application **"
	@echo "************************************************"

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -lf $(CONFIGS) $(LTP_BIN)
	cp -rf $(PNGD_INPUT) $(LTP_BIN)
	cp -rf $(PNGD_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/pngd_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(PNGD_LIBDIR)/lib$(PNGD_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "***********************************"
	@echo "** INSTALL : png_decoder_testapp **"
	@echo "***********************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : png_decoder_test.h ./stuff/fconf.h
