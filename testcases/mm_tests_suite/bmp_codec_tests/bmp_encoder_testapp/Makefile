###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the bmp_encoder_testapp test.
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
# D.Simakov / smkd001c        26/07/2004     TLSbo40263   Initial version 
# L.Delaspre / rc149c         17/09/2004     TLSbo42022   VTE 1.5 integration 
# D.Simakov / smkd001c        02/03/2005     TLSbo47117   Update 
# D.Simakov / smkd001c        25/04/2005     TLSbo47117   The configs are installed now     
#S. V-Guilhou / svan01c	      19/05/2005     TLSbo50534   ARM11 codecs P4 tests
# D. Simakov / smkd001c       28/05/2005     TLSbo52108   bmpe_*.rgb are copied
# D.Simakov/smkd001c          08/11/2005     TLSbo57009   Rework
# A.Pshenichnikov             13/01/2006     TLSbo61033   Makefiles / shared libraries 
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(BMPE_HDIR)
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(BMPE_LIBDIR) -l$(BMPE_LIBRARY) 

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS=$(wildcard *.c) stuff/fconf.c
OBJS=$(patsubst %.c,%.o,$(SRCS)) 

TARGET  = bmp_encoder_testapp
CONFIGS = bmpe_cfg*

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : bmp_encoder_testapp test application **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)    
	cp -rf $(BMPE_INPUT) $(LTP_BIN)
	cp -rf $(BMPE_REF) $(LTP_BIN)
	cp -rf bmpe_*.rgb bmpe_*.bmp $(LTP_BIN)
	mkdir -p $(LTP_BIN)/bmpe_output   
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -rf $(BMPE_LIBDIR)/lib$(BMPE_LIBRARY).so $(LTP_BIN)/sharedlibs                    
	@echo "***********************************************"
	@echo "** INSTALL : bmp_encoder_testapp             **"
	@echo "***********************************************"
	@echo   

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 

$(SRCS) : bmp_encoder_test.h 



