###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the cdrom_driver_testapp.
#
###################################################################################################
#
#        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
#
###################################################################################################
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR=../../../..
BIN_DIR=../../..

CFLAGS+= -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include
LOADLIBES+= -L$(LTP_DIR)/lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=cdrom_testapp

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : $(TARGET)                 **"
	@echo "************************************************"
	@echo
	rm -f $(OBJS)

install:
	@echo "************************************************"
	@echo "** INSTALL : $(TARGET)             **"
	@echo "************************************************"
	ln -f $(TARGET) $(BIN_DIR)/bin

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "************************************************"
	@echo "** CLEAN : $(TARGET)               **"
	@echo "************************************************"

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : cdrom_testapp.h
