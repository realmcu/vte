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
#   @brief  configuration file template to compile the v4l_capture_testapp test.
#
#   @par Portability:
#
#
#
########################### REVISION HISTORY #################################
#
#Author (core ID)      Date         CR Number    Description of Changes
#-------------------   ----------   ----------   ------------------------------
#Kardakov Dmitriy/ID   09/11/06     TLSbo76802   Initial version
##############################################################################
CFLAGS+= -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include/
LOADLIBES+=     -L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=v4l_capture_testapp

all: $(TARGET)
	@echo "****************************************************"
	@echo "** ALL : v4l_capture_testapp test application     **"
	@echo "****************************************************"
	@echo

install:
	@echo "****************************************************"
	@echo "** INSTALL : v4l_capture_testapp                  **"
	@echo "****************************************************"
	ln -f $(TARGET) ../../../bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : v4l_capture_test.h
