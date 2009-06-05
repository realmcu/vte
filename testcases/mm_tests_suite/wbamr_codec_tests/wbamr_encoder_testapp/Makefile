###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the wbamr_encoder_testapp test.
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
#Igor Semenchukov/smng001c    01/12/2004     TLSbo43523   Initial version
#Igor Semenchukov/smng001c    07/02/2005     TLSbo47179   Added variables that point to libraries
#                                                         and header files location.
#S. V-Guilhou\svan01c	      19/05/2005     TLSbo50534   ARM11 codecs P4 tests
#D.Simakov/smkd001c           03/11/2005     TLSbo57009   Rework
#A.Pshenichnikov              13/01/2006     TLSbo61033   Makefiles / shared libraries
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(WBAMRE_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(WBAMRE_LIBDIR) -l$(WBAMRE_LIBRARY)

CFLAGS+= -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES+= -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt 

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET  = wbamr_encoder_testapp
CONFIGS = list/wbamre_full_list list/wbamre_def_test_files

all: $(TARGET)
	@echo "***************************************************"
	@echo "** ALL : wbamr_encoder_testapp test application  **"
	@echo "***************************************************"
	@echo

# we only copy WBAMRD_INPUT because WBAMRD_REF is the same
install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN) 
	cp -rf $(WBAMRE_INPUT) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/wbamre_output   
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -rf $(WBAMRE_LIBDIR)/lib$(WBAMRE_LIBRARY).so $(LTP_BIN)/sharedlibs                        
	@echo "***************************************************"
	@echo "** INSTALL : wbamr_encoder_testapp               **"
	@echo "***************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : wbamr_encoder_test.h
