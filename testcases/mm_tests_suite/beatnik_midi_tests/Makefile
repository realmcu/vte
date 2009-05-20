###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the beatnik_midi_testapp test.
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
#D.Simakov / smkd001c         11/09/2005     TLSbo53248   Initial version
#D.Simakov / smkd001c         09/12/2005     TLSbo57009   Rework
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN    = ../../bin

CFLAGS    += -Wall -I../../../include $(MBAE_HDIR) -DPAL_BUILD_FULL_MOBILEBAE -DPAL_ENABLE_ERROR_STRINGS -DNDEBUG -DPAL_ENABLE_ARM_OPTIMIZATIONS
LOADLIBES += -L../../../lib -lltp -lpthread -lm -Bdynamic -L$(MBAE_LIBDIR) -l$(MBAE_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN -DDEBUG_TEST
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS       = $(wildcard *.c) stuff/cfg_parser.c
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = beatnik_midi_testapp
CONFIGS    = *cfg*

all: $(TARGET)
	@echo "*************************************************"
	@echo "** ALL : beatnik_midi_testapp test application **"
	@echo "*************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)
	cp -ruf $(MBAE_INPUT) $(LTP_BIN)
	cp -f $(MBAE_LIBDIR)/Soundbanks/*.dls $(LTP_BIN)
	cp -f beatnik_midi_testapp $(LTP_BIN)/beatnik_midi_testapp.mid
	cp -f mbae_cfg $(LTP_BIN)/mbae_cfg.imy
	cp -f mbae_cfg_robustness $(LTP_BIN)/mbae_cfg_robustness.xmf
	mkdir -p $(LTP_BIN)/mbae_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(MBAE_LIBDIR)/lib$(MBAE_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "*************************************************"
	@echo "** INSTALL : beatnik_midi_testapp              **"
	@echo "*************************************************"
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY)
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS)



