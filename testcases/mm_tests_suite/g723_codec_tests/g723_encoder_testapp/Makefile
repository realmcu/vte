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
#   @brief  configuration file template to compile the g723_1_encoder_testapp.
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)        Date         CR Number    Description of Changes
#-------------------     ----------   ----------   ------------------------------
# I.Inkina /nknl001      27/12/2004   TLSbo47117   BRIEF desc. of changes
# D.Simakov / smkd001c   15/04/2005   TLSbo47117   The configs are copied now
# S. V-Guilhou / svan01c 19/05/2005   TLSbo50534   ARM11 codecs P4 tests
# D.Simakov/smkd001c     24/10/2005   TLSbo57009   Rework
# A.Pshenichnikov  13/01/2006   TLSbo63011   Makefiles / shared libraries
##############################################################################


LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(G723E_HDIR) -I$(CODECS_DIR)/common
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(G723E_LIBDIR) -l$(G723E_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS       = $(wildcard *.c)
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = g723_encoder_testapp
CONFIGS    = g723e_*cfg*

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : g723_encoder_testapp built          **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)
	cp -rf $(G723E_INPUT) $(LTP_BIN)
	cp -rf $(G723E_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/g723e_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(G723E_LIBDIR)/lib$(G723E_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "***********************************************"
	@echo "** INSTALL : g723_encoder_testapp set        **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : g723_1_encoder_test.h
