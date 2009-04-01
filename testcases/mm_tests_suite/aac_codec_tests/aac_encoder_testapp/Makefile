###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the aac_encoder_testapp test.
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
#D.Simakov / smkd001c        09/09/2005      TLSbo53249  Initial version  
#D.Simakov / smkd001c        20/12/2005      TLSbo57009  Improved 
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(AACE_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(AACE_LIBDIR) -l$(AACE_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN -DADIF_HEADER_ENABLE
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS       = $(wildcard *.c) stuff/cfg_parser.c 
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = aac_encoder_testapp
CONFIGS    = aace_*cfg*

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : aac_encoder_testapp test application **"
	@echo "************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)
	cp -rf $(AACE_INPUT) $(LTP_BIN)
	cp -rf $(AACE_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/aace_output    
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -rf $(AACE_LIBDIR)/lib$(AACE_LIBRARY).so $(LTP_BIN)/sharedlibs                        
	@echo "************************************************"
	@echo "** INSTALL : aac_encoder_testapp              **"
	@echo "************************************************"
    
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 



