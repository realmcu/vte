###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the wbmp_decoder_testapp test.
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
#D.Simakov / smkd001c        12/10/2004     TLSbo40263   Initial version
#D.Simakov / smkd001c        01/03/2005     TLSbo47117   Update
#S. V-Guilhou\svan01c        19/05/2005     TLSbo50534   ARM11 codecs P4 tests
#D.Simakov                   10/11/2005     TLSbo57009   Rework
#A.Pshenichnikov             13/01/2006     TLSbo61033   Makefiles / shared libraries
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(WBMPD_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm  -Bdynamic -L$(WBMPD_LIBDIR) -l$(WBMPD_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS       = $(wildcard *.c) stuff/fconf.c stuff/fb_draw_api.c
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = wbmp_decoder_testapp
CONFIGS    = wbmpd_cfg*

all: $(TARGET)
	@echo "*************************************************"
	@echo "** ALL : wbmp_decoder_testapp test application **"
	@echo "*************************************************"

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)
	cp -rf $(WBMPD_INPUT) $(LTP_BIN)
	cp -rf $(WBMPD_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/wbmpd_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(WBMPD_LIBDIR)/lib$(WBMPD_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "************************************"
	@echo "** INSTALL : wbmp_decoder_testapp **"
	@echo "************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : wbmp_decoder_test.h stuff/fconf.h stuff/fb_draw_api.h



