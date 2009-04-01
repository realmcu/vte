###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the multicast test.
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
#S.ZAVJALOV/-----             10-06-2004     TLSbo39738   Initial version
#Ludovic Delaspre / rc149c    22/06/2004     TLSbo39941   VTE 1.3 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#
###################################################################################################

CFLAGS+=	-Wall -I../../../../include
LOADLIBES+=	-L../../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=multicast

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : multicast test application          **"
	@echo "***********************************************"
	@echo

install:
	@echo "***********************************************"
	@echo "** INSTALL : multicast test                  **"
	@echo "***********************************************"
	ln -f $(TARGET) ../../../bin
	ln -f message_examp.txt ../../../bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : multicast_test.h
