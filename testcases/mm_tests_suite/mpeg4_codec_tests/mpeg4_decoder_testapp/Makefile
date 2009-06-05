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
#   @brief  configuration file template to compile the TEMPLATE_testapp_X test.
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
#Filinova Natalya      15/02/2005   TLSbo47117   BRIEF desc. of changes 
#Filinova Natalya      28/02/2005   TLSbo47117   BRIEF desc. of changes 
#Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale
#Delaspre/rc149c       14/03/2005   TLSbo47726   VTE 1.8 integration
#Delaspre/rc149c       16/03/2005   TLSbo48584   ARM11 codecs P1 tests
#L.Delaspre/rc149c     01/04/2005   TLSbo49055   ARM11 codecs P2 tests
#D.Simakov/smkd001c    20/10/2005   TLSbo57009   Update
#D.Simakov/smkd001c    16/12/2005   TLSbo59667   Advanced decode w skip fixed 
##############################################################################

LTP_BIN    = ../../../bin
INCLUDE    =../../../../include
LIB        =../../../../lib

CFLAGS    += -Wall -I$(INCLUDE) -I$(MPEG4D_HDIR)  -I$(MMAPI_HDIR) -I$(CODECS_DIR)/common
LOADLIBES += -L$(LIB) -lltp -lpthread -lm -Bdynamic -L$(MPEG4D_LIBDIR) -l$(MPEG4D_LIBRARY)

CFLAGS    += -g -mcpu=arm1136j-s -mabi=aapcs -DFILEIO -DLITTLE_ENDIAN -DNDEBUG #-DDEBUG_TEST
LOADLIBES += -Wl,--no-warn-mismatch -mcpu=arm1136j-s -lm -lc -lrt

SRCS       = $(wildcard *.c)
OBJS       = $(patsubst %.c,%.o,$(SRCS))

TARGET     = mpeg4_decoder_testapp
CONFIGS    = list/error.*

all: $(TARGET)
	@echo "***************************************************"
	@echo "** ALL : Mpeg4_decoder_testapp test application  **"
	@echo "***************************************************"
	@echo

install:
	ln -f $(TARGET) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/list
	cp -Lrf $(CONFIGS) $(LTP_BIN)/list
	cp -Lrf $(MPEG4D_INPUT) $(LTP_BIN)
	cp -Lrf $(MPEG4D_REF) $(LTP_BIN)
	mkdir -p $(LTP_BIN)/mpeg4d_output
	mkdir -p $(LTP_BIN)/sharedlibs   
	cp -Lrf $(MPEG4D_LIBDIR)/lib$(MPEG4D_LIBRARY).so $(LTP_BIN)/sharedlibs    
	@echo "***************************************************"
	@echo "** INSTALL : Mpeg4_decoder_testapp               **"
	@echo "***************************************************"
	@echo


clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : mpeg4_decoder_test.h



