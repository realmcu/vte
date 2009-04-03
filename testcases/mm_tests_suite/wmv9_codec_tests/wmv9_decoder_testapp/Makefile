###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the wmv_decoder_testapp test.
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
#D.Simakov/smkd001c           25/05/2005     TLSbo47117	 Initial version	
#S. V-Guilhou\svan01c	      02/06/2005     TLSbo50938  ARM11 codecs P4 tests
#S. V-Guilhou\svan01c	      22/06/2005     TLSbo51635  Add the new config file
#D.Simakov/smkd001c           16/10/2005     TLSbo57009  Rework 
#A.Pshenichnikov              13/01/2006     TLSbo61033  Makefiles / shared libraries#
#D.Simakov                    19/05/2006     TLSbo66280  Phase2
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(WMV9D_HDIR) -I$(COMMON_DIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -L$(WMV9D_LIBDIR) -l$(WMV9D_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -DFILEIO -DLITTLE_ENDIAN -DMAX_STR_LEN=512 -DWORDS_IN_ENTRY=4
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt 
#CFLAGS    += -DDEBUG_TEST
CFLAGS    += -DMAX_THREADS=2

SRCS       = $(wildcard *.c) 
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = wmv_decoder_testapp
CONFIGS    = configs/*

include ../../common_codec/CommonCodec.mk

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : wmv_decoder_testapp test application **"
	@echo "************************************************"
	@echo

install:
	cp -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/configs
	cp -f $(CONFIGS) $(LTP_BIN)/configs
	cp -ruf $(WMV9D_INPUT) $(LTP_BIN)
	cp -ruf $(WMV9D_REF) $(LTP_BIN)        
	mkdir -p $(LTP_BIN)/wmv9d_output    
	mkdir -p $(LTP_BIN)/sharedlibs       
	cp -rf $(WMV9D_LIBDIR)/lib$(WMV9D_LIBRARY).so $(LTP_BIN)/sharedlibs    
	@echo "*******************************************"
	@echo "** INSTALL : wmv_decoder_testapp         **"
	@echo "*******************************************"    
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 





