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
#   @brief  configuration file to compile the nbamr_decoder_testapp test.
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)         Date         CR Number    Description of Changes
#-----------------------  ----------   ----------   --------------------------
# I.Semenchukov/smng001c  18/01/2005   TLSbo46857   Initial version
# I.Semenchukov/smng001c  07/02/2005   TLSbo47179   Added variables that point
#                                                   to libraries and header files location.
# I.Semenchukov/smng00c   23/03/2005   TLSbo48795   Made changes to reflect with new version
# L.Delaspre/rc149c       29/03/2005   TLSbo49055   ARM11 codecs P2 tests
# D.Simakov/smkd001c      01/11/2005   TLSbo57009   Rework
# A.Pshenichnikov         13/01/2006   TLSbo61033   Makefiles / shared libraries
#
##############################################################################

LTP_BIN = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -Bdynamic -lm -L$(NBAMRD_LIBDIR) -l$(NBAMRD_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt


SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET  = nbamr_decoder_testapp
CONFIGS = list/nbamrd_full_list list/nbamrd_rob_test_files list/nbamrd_test_files

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : nbamr_decoder_testapp built         **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/list
	cp -Lrf $(CONFIGS) $(LTP_BIN)/list
	cp -ruf $(NBAMRD_INPUT) $(LTP_BIN)
	cp -ruf $(NBAMRD_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/nbamrd_output
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -rf $(NBAMRD_LIBDIR)/lib$(NBAMRD_LIBRARY).so $(LTP_BIN)/sharedlibs                
	@echo "***********************************************"
	@echo "** INSTALL : nbamr_decoder_testapp set       **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : nbamr_decoder_test.h
