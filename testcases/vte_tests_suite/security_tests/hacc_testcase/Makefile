####################################################################################################
#                                                                                                  #
#    @file   Makefile                                                                              #
#                                                                                                  #
#    @brief  configuration file to compile the sockets test.                                       #
#                                                                                                  #
####################################################################################################
#                                                                                                  #
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved                          #
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT                                   #
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF                                       #
#   Freescale Semiconductor, Inc.                                                                  #
#                                                                                                  #
####################################################################################################
#Revision History:                                                                                 #
#                            Modification     Tracking                                             #
#Author                          Date          Number    Description of Changes                    #
#-------------------------   ------------    ----------  ------------------------------------------#
#S.ZAVJALOV/zvjs001c          10/08/2004     TLSbo40418   Initial version                          #
#S.ZAVJALOV/zvjs001c          01/10/2004     TLSbo40649   Version after inspection                 #
#L.DELASPRE/rc149c            06/10/2004     TLSbo42821   VTE 1.5.1 integration                    #
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity           #
#S.ZAVJALOV/zvjs001c          04/07/2005     TLSbo51629   Change hacc test strategy                #
#A.URUSOV                     14/09/2005     TLSbo53754   CFLAGS and LOADLIBES edited              #
#A.URUSOV                     18/10/2005     TLSbo57061   CFLAGS edited                            #
####################################################################################################

LTP_DIR = ../../../../
BIN_DIR = ../../../

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

CFLAGS += -I$(LTP_DIR)include \
          -I../hacc_test_module/ \
          -DCONFIG_PM

LOADLIBES = -L$(LTP_DIR)lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=hacc_test

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : TEST_$(TARGET)                     **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN: TEST_$(TARGET)                     **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : TEST_$(TARGET)                  **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : hacc_test.h
