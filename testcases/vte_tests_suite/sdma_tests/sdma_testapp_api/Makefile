####################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the sockets test.
#
####################################################################################################
#
#       Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#       THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#       BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#       Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:                                                                                 
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes                    
#-------------------------   ------------    ----------  -------------------------------------------
#S.ZAVJALOV/-----             13/07/2004     TLSbo40259  Initial version                          
#L.DELASPRE/rc149c            13/08/2004     TLSbo40891  VTE 1.4 integration                      
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#A.Ozerov/B00320              10/02/2006     TLSbo61734  Code was cast to coding conventions  
#                                                                                                  
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../..

CFLAGS +=    -Wall -I$(LTP_DIR)/include 
LOADLIBES += -L$(LTP_DIR)/lib -lltp

SUBDIRS = module

SRCS = sdma_main.c sdma_test.c
OBJS = sdma_main.o sdma_test.o

TARGET = sdma_api

all: $(TARGET)
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                            **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(OBJS) $(TARGET)
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	@echo "***********************************************"
	@echo "** CLEAN: $(TARGET)                           **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(BIN_DIR)/bin
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                        **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : sdma_test.h
