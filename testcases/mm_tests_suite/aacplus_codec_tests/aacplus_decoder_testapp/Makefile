###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the aacplus_decoder_testapp test.
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
#D.Simakov / smkd001c        06/09/2005      TLSbo53247  Initial version
#D.Simakov / smkd001c        07/12/2005      TLSbo57009  Rework
#A.Pshenichnikov             13/01/2006      TLSbo61033  Makefiles / shared libraries
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

LTP_BIN    = ../../../bin

CFLAGS    += -Wall -I../../../../include -I$(MMAPI_HDIR) -I$(AACPD_HDIR) -I$(AACPD_APIDIR) 
LOADLIBES += -L../../../../lib -lltp -lpthread -lm -Bdynamic -L$(AACPD_LIBDIR) -l$(AACPD_LIBRARY) 

DEFS       = -DMPEG4 -DLC -DARM_OPT_MACROS -DMOT_AAC -DHIGH_QUALITY_SBR -DDUMP_CODE_REF -DPARAMETRICSTEREO
CFLAGS    += $(DEFS)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN #-DDEBUG_TEST
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt 

SRCS       = $(wildcard *.c) stuff/cfg_parser.c stuff/aacplus_aux.c stuff/aacplus_portio.c stuff/mem_stat.c
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = aacplus_decoder_testapp
CONFIGS    = aacplusd_*cfg* makeref_cfg install.sh refdecoder adif2mp4

all: $(TARGET)
	@echo "****************************************************"
	@echo "** ALL : aacplus_decoder_testapp test application **"
	@echo "****************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	cp -f $(CONFIGS) $(LTP_BIN)
	cp -ruf $(AACPD_INPUT) $(LTP_BIN)       
	@echo "Converting mp4 and generating the reference vector. Please wait..."    
	@`cd $(LTP_BIN) ; . ./install.sh > /dev/null`        
	mkdir -p $(LTP_BIN)/sharedlibs       
	cp -rf $(AACPD_LIBDIR)/lib$(AACPD_LIBRARY).so $(LTP_BIN)/sharedlibs                
	@echo "**********************************************"
	@echo "** INSTALL : aacplus_decoder_testapp        **"
	@echo "**********************************************"    
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 



