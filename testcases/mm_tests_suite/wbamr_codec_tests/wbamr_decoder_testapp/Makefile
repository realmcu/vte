###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the wbamr_decoder_testapp test.
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
#Igor Semenchukov/smng001c    30/11/2004     TLSbo43523   Initial version
#Igor Semenchukov/smng001c    07/02/2005     TLSbo47179   Added variables that point to libraries
#                                                         and header files location.
#S. V-Guilhou\svan01c	      19/05/2005     TLSbo50534   ARM11 codecs P4 tests
#D.Simakov/smkd001c           03/11/2005     TLSbo57009   Rework
#D.Simakov/smkd001c           16/12/2005     TLSbo60691   Improved
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(WBAMRD_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(WBAMRD_LIBDIR) -l$(WBAMRD_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET  = wbamr_decoder_testapp
CONFIGS = list/*

all: $(TARGET)
	@echo "***************************************************"
	@echo "** ALL : wbamr_decoder_testapp test application  **"
	@echo "***************************************************"
	@echo

# we only copy WBAMRD_INPUT because WBAMRD_REF is the same
install:
	ln -f $(TARGET) $(LTP_BIN)  
	cp -rf $(WBAMRD_INPUT) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/wbamrd_output   
	mkdir -p $(LTP_BIN)/list
	cp -f $(CONFIGS) $(LTP_BIN)/list/
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -rf $(WBAMRD_LIBDIR)/lib$(WBAMRD_LIBRARY).so $(LTP_BIN)/sharedlibs                    
	@echo "***************************************************"
	@echo "** INSTALL : wbamr_decoder_testapp               **"
	@echo "***************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : wbamr_decoder_test.h
