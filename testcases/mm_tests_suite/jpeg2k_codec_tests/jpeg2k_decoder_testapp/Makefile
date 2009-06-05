###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the jpeg2k_decoder_testapp test.
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
#D.Simakov / smkd001c         30/08/2005     TLSbo53250   Initial version 
#D.Simakov / smkd001c         06/09/2005     TLSbo53250   LCD support was added 
#D.Simakov / smkd001c         18/10/2005     TLSbo57009   Rework
#A.Pshenichnikov              13/01/2006     TLSbo61033  Makefiles / shared libraries
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(J2KE_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(J2KD_LIBDIR) -l$(J2KD_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN 
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt 

SRCS       = $(wildcard *.c) stuff/cfg_parser.c stuff/fb_draw_api.c
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = jpeg2k_decoder_testapp
CONFIGS    = j2kd_*cfg*

all: $(TARGET)
	@echo "***************************************************"
	@echo "** ALL : jpeg2k_decoder_testapp test application **"
	@echo "***************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)
	cp -rf $(J2KD_INPUT) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/j2kd_output    
	mkdir -p $(LTP_BIN)/sharedlibs       
	cp -rf $(J2KD_LIBDIR)/lib$(J2KD_LIBRARY).so $(LTP_BIN)/sharedlibs    
	@echo "**********************************************"
	@echo "** INSTALL : jpeg2k_decoder_testapp         **"
	@echo "**********************************************"    
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 



