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
#   @brief  configuration file to compile imelody to midi converter test case
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
#S.ZAVJALOV/zvjs001c   19/10/2004   TLSbo47116   Initial version
#B.Giabicani\r70160c   18/04/2005   TLSbo49674   ARM11 codecs P3 tests
#D.Simakov/smkd001c    24/10/2005   TLSbo57009   Rework
#A.Pshenichnikov       13/01/2006   TLSbo61033   Makefiles / shared libraries
##############################################################################

LTP_BIN = ../../../bin

CFLAGS+=        -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(IMY_HDIR)
LOADLIBES+=     -L../../../../lib -lltp -lpthread -Bdynamic -L$(IMY_LIBDIR) -l$(IMY_LIBRARY)

CFLAGS+= -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES+= -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

CONFIGS = imelody.cfg imelody_robustness.cfg
TARGET  = imelody_conv_test

all: $(TARGET)
	@echo "**************************************************"
	@echo "** ALL : imy_converter_testapp test application **"
	@echo "**************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -lf $(CONFIGS) $(LTP_BIN)
	cp -rf $(IMY_INPUT) $(LTP_BIN)
	cp -rf $(IMY_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/imy_output
	mkdir -p $(LTP_BIN)/sharedlibs
	cp -rf $(IMY_LIBDIR)/lib$(IMY_LIBRARY).so $(LTP_BIN)/sharedlibs
	@echo "*************************************"
	@echo "** INSTALL : imy_converter_testapp **"
	@echo "*************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : imelody_conv_test.h
