###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the mpeg4_encoder_testapp test.
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
#-------------------------   ------------    ----------  -------------------------------------------#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
#I.Inkina\nknl001      20/11/2004   TLSbo45065  initial version
#L.Delaspre\rc149c     14/03/2005   TLSbo47726  VTE 1.8 integration
#L.Delaspre\rc149c     16/03/2005   TLSbo48584  ARM11 codecs P1 tests
#D.Simakov/smkd001c    18/10/2005   TLSbo57009  Rework
#A.Pshenichnikov       13/01/2006   TLSbo61033  Makefiles / shared libraries
#D.Simakov/smkd001c    29/03/2005   TLSbo66281   Centralization of common features
###################################################################################################
#Portability: ARM_GCC
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(MPEG4E_HDIR) -I$(COMMON_DIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -L$(MPEG4E_LIBDIR) -l$(MPEG4E_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -DFILEIO -DLITTLE_ENDIAN -DMAX_STR_LEN=512 -DWORDS_IN_ENTRY=3 -DMANY_REF_OUT_FILES
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt 
CFLAGS    += -DDEBUG_TEST
#CFLAGS    += -DPOST_BIT_MATCHING
#CFLAGS    += -DMAX_THREADS=2

SRCS       = $(wildcard *.c) 
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = mpeg4_encoder_testapp
CONFIGS    = configs/*

include ../../common_codec/CommonCodec.mk

all: $(TARGET)
	@echo "**************************************************"
	@echo "** ALL : mpeg4_encoder_testapp test application **"
	@echo "**************************************************"
	@echo

install:
	cp -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/configs
	cp -f $(CONFIGS) $(LTP_BIN)/configs
	cp -ruf $(MPEG4E_INPUT) $(LTP_BIN)
	cp -ruf $(MPEG4E_REF) $(LTP_BIN)        
	mkdir -p $(LTP_BIN)/mpeg4e_output    
	mkdir -p $(LTP_BIN)/sharedlibs       
	cp -rf $(MPEG4E_LIBDIR)/lib$(MPEG4E_LIBRARY).so $(LTP_BIN)/sharedlibs    
	@echo "*********************************************"
	@echo "** INSTALL : mpeg4_encoder_testapp         **"
	@echo "*********************************************"    
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 

