####################################################################################################
#                                                                                                  #
#    @file   Makefile                                                                              #
#                                                                                                  #
#    @brief  configuration file to compile the sockets test.                                       #
#                                                                                                  #
####################################################################################################
#                                                                                                  #
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#                                                                                                  #
####################################################################################################
#Revision History:                                                                                 #
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes                    #
#-------------------------   ------------    ----------  ------------------------------------------#
#S.ZAVJALOV/-----             19/07/2004     TLSbo40259   Initial version                          #
#L.DELASPRE/rc149c            03/08/2004     TLSbo40891   VTE 1.4 integration                      #
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#                                                                                                  #
####################################################################################################

CFLAGS+=	-Wall -I../../../../include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=sdma_tty

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : TEST_${TARGET}                      **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : TEST_${TARGET}                  **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : sdma_tty_test.h
