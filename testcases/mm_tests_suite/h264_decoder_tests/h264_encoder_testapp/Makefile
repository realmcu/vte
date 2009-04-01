###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the h264_encoder_testapp test.
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
#I.Inkina/nknl001              1/07/2005     TLSbo52105   Initial version
#D.Simakov/smkd001c           22/08/2005     TLSbo53252   Re-written
#D.Simakov/smkd001c           14/10/2005     TLSbo57009   Rework
#A.Pshenichnikov	      13/01/2006     TLSbo61033   Makefiles / shared libraries	 	
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN     = ../../../bin

CFLAGS     += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(H264E_HDIR)
LOADLIBES  += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(H264E_LIBDIR) -l$(H264E_LIBRARY)

CFLAGS     += -mcpu=arm1136j-s -mabi=aapcs -mfpu=vfp -DFILEIO -DLITTLE_ENDIAN
LOADLIBES  += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS        = $(wildcard *.c) stuff/cfg_parser.c stuff/io.c stuff/ui.c stuff/kev.c
OBJS        = $(patsubst %.c,%.o,$(SRCS))

TARGET      = h264_encoder_testapp
CONFIGS     = h264e_*cfg* *.ctl

all: $(TARGET)
	@echo "*************************************************"
	@echo "** ALL : h264_encoder_testapp test application **"
	@echo "*************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)
	cp -rf $(H264E_INPUT) $(LTP_BIN)
	cp -rf $(H264E_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/h264e_output    
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -rf $(H264E_LIBDIR)/lib$(H264E_LIBRARY).so $(LTP_BIN)/sharedlibs                            
	@echo "*************************************************"
	@echo "** INSTALL : h264_encoder_testapp              **"
	@echo "*************************************************"        
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 



