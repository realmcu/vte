###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the uart_testapp_3 test.
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
#E.Gromazina                07/04/2005     TLSbo48747   Initial version
#L.Delaspre/rc149c          14/04/2005     TLSbo48760   VTE 1.9 integration
#E.Gromazina       14/06/2005	   TLSbo50931	add script
#
##################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
#
###################################################################################################

CFLAGS+= -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include/
LOADLIBES+=	-L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))


#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=uart_testapp_3

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : uart_testapp_3      test application **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	ln -f uart3.sh ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : uart_testapp_3                  **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : uart_test_3.h
