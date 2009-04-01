###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the mp3_decoder_testapp test.
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
# D.Simakov / smkd001c        12/10/2004     TLSbo43519   Initial version 
# D.Simakov / smkd001c        19/02/2005     TLSbo47113   Update
# L.Delaspre\rc149c           16/03/2005     TLSbo48584   ARM11 codecs P1 tests
# D.Simakov/smkd001c          18/10/2005     TLSbo57009   Rework
# A.Pshenichnikov             13/01/2006     TLSbo61033   Makefiles / shared libraries
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN = ../../../bin

CFLAGS+=	-I../../../../include -I$(MMAPI_HDIR) -I$(MP3D_HDIR) -I$(CODECS_DIR)/common
LOADLIBES+=	-L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(MP3D_LIBDIR) -l$(MP3D_LIBRARY)

CFLAGS+= -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES+= -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS=$(wildcard *.c) stuff/fconf.c stuff/fileio.c stuff/dif32.c
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET  = mp3_decoder_testapp
CONFIGS = mp3_default_config mp3_robustness

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : mp3_decoder_testapp test application **"
	@echo "************************************************"

install:
	ln -f $(TARGET) $(LTP_BIN)
	ln -f $(CONFIGS) $(LTP_BIN)
	cp -rf ./robustness_input/ $(LTP_BIN)/inputs
	cp -rf $(MP3D_INPUT) $(LTP_BIN)
	cp -rf $(MP3D_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/mp3d_output
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -rf $(MP3D_LIBDIR)/lib$(MP3D_LIBRARY).so $(LTP_BIN)/sharedlibs        
	@echo "***********************************"
	@echo "** INSTALL : mp3_decoder_testapp **"
	@echo "***********************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : mp3_decoder_test.h stuff/fileio.h stuff/fconf.h



