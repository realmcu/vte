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
#   @brief  configuration file template to compile the mu_sdma_ipc_test test.
#
#   @par Portability:
#           Indicate if this module is portable to other compilers or platforms.
#           If not, indicate specific reasons why is it not portable.
#
########################### REVISION HISTORY #################################
#
#Author (core ID)          Date         CR Number    Description of Changes
#-----------------------   ----------   ----------   -------------------------
#I. Semenchukov/smng001c   02/03/2005   TLSbo47942   Initial version 
#L.Delaspre/rc149c         07/04/2005   TLSbo48760   VTE 1.9 integration 
#
##############################################################################

CFLAGS+=	-Wall -I../../../../include -I$(KLINUX_BLTDIR)/include/asm/arch
LOADLIBES+=	-L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=mu_sdma_ipc_test

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : mu_sdma_ipc_testapp built           **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : mu_sdma_ipc_testapp set         **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : mu_sdma_ipc_test.h
