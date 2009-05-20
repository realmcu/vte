###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the aaclc_decoder_testapp test.
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
#Igor Semenchukov/smng001c    26/10/2004     TLSbo43521   Initial version
#Igor Semenchukov/smng001c    04/02/2005     TLSbo47179   Added variables that point to libraries
#                                                         and header files location.
#Igor Semenchukov/smng001c    25/03/2005     TLSbo48795   Updated due to changes in the codec's code
#L.Delaspre/rc149c            29/03/2005     TLSbo49055   ARM11 codecs P2 tests
#D.Simakov/smkd001c           22/10/2005     TLSbo57009   Rework
#D.Simakov/smkd001c           15/12/2005     TLSbo59668   Improved
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN     = ../../../bin

CFLAGS     += -Wall -I../../../../include -I$(AACD_HDIR) -I$(MMAPI_HDIR) -DMPEG4 -DARM_OPT_MACROS -DLC #-DOLD_FORMAT_ADTS_HEADER
LOADLIBES  += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(AACD_LIBDIR) -l$(AACD_LIBRARY)

CFLAGS     += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES  += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt


SRCS        = $(wildcard *.c)
OBJS        = $(patsubst %.c,%.o,$(SRCS))

TARGET      = aaclc_decoder_testapp
CONFIGS     = list/*

all: $(TARGET)
	@echo "***************************************************"
	@echo "** ALL : aaclc_decoder_testapp test application  **"
	@echo "***************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/list
	cp -Lrf $(CONFIGS) $(LTP_BIN)/list
	cp -ruf $(AACD_INPUT) $(LTP_BIN)
	cp -ruf $(AACD_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/aacd_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(AACD_LIBDIR)/lib$(AACD_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "***************************************************"
	@echo "** INSTALL : aaclc_decoder_testapp               **"
	@echo "***************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : aaclc_decoder_test.h
