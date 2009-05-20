###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the g726_encoder_testapp test.
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
#D.Simakov / smkd001c         04/11/2004     TLSbo43522   Initial version
#D.Simakov / smkd001c         22/02/2005     TLSbo47179   Updated
#D.Simakov / smkd001c         05/04/2005     TLSbo47116   The configs are copied into the bin.
#B.Giabicani\r70160c	      18/04/2005     TLSbo49674   ARM11 codecs P3 tests
#D.Simakov / smkd001c         18/10/2005     TLSbo57009   Rework
#A.Pshenichnikov              13/01/2006     TLSbo61033   Makefiles / shared libraries
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(G726E_LIBDIR) -l$(G726E_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS       = $(wildcard *.c) ./stuff/fconf.c
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = g726_encoder_testapp
CONFIGS    = g726e_*cfg*

all: $(TARGET)
	@echo "*************************************************"
	@echo "** ALL : g726_encoder_testapp test application **"
	@echo "*************************************************"

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)
	cp -rf $(G726E_INPUT) $(LTP_BIN)
	cp -rf $(G726E_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(G726E_LIBDIR)/lib$(G726E_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "************************************"
	@echo "** INSTALL : g726_encoder_testapp **"
	@echo "************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : g726_encoder_test.h ./stuff/fconf.h



