###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the amr_encoder_nodtx test.
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
# F.GAFFIE/rb657c             21/07/2004     TLSbo40930  Initial version 
# L.DELASPRE/rc149c           18/10/2004     TLSbo43867  Update with new API 
#
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

PROJ_DIR = /vob/armcodecs/ARM9/src/speech/nb_amr

TEST_SRC_DIR = ../amr_encoder_testapp

COM_INC_DIR = $(PROJ_DIR)/library/include/common
ENC_INC_DIR = $(PROJ_DIR)/library/include/encoder
DEC_INC_DIR = $(PROJ_DIR)/library/include/decoder

LIB_DIR = /home/rc149c/CHUCK/library

INCLUDE_FLAGS= -I../. -I/vob/armcodecs/ARM9/API_include -I$(COM_INC_DIR) -I$(DEC_INC_DIR) -I$(ENC_INC_DIR)

CFLAGS=	-Wall  $(INCLUDE_FLAGS) -I$(KLINUX_BLTDIR)/include -DAMRD_USE_SCRATCH -I../../../../include 
LOADLIBES+= -L../../../../lib -lltp -lpthread -lm
LIBRARY = $(LIB_DIR)/encoderlib.a 

SRCS=$(wildcard $(TEST_SRC_DIR)/*.c)
OBJS=$(patsubst %.c,%.o,$(SRCS)) \

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=amr_encoder_nodtx

all: $(TARGET)
	rm -f $(patsubst %.c,%.o,$(SRCS))
	@echo "***********************************************"
	@echo "** ALL : amr_encoder_nodtx test application **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : amr_encoder_nodtx              **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(patsubst %.c,%.o,$(SRCS)) $(TARGET)

$(TARGET): $(OBJS) $(LIBRARY) 
	$(CC)  $(OBJS) $(LOADLIBES) $(LIBRARY) -o  $@

$(OBJS) : $(SRCS) 

$(SRCS) : $(TEST_SRC_DIR)/amr_encoder_test.h 



