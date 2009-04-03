###################################################################################################
#
#         file   Makefile
#
#         brief  Makefile to compile the Keypad test 3
#
###################################################################################################
#
#        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        E USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        reescale Semiconductor, Inc.
#     
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/Core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#D.Simakov                    04/06/2004     TLSbo39737   Initial version
#L.Delaspre/rc149c            22/06/2004     TLSbo39941   VTE 1.3 integration
#L.Delaspre/rc149c            22/06/2004     TLSbo40109   Replace hard include path by a variable
#L.Delaspre/rc149c            16/08/2004     TLSbo40891   VTE 1.4 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity
#A.Ozerov/NONE                10/01/2006     TLSbo61037   Update in accordance with linux-2.6.10-rel-L26_1_15
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS = -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include
LOADLIBES = -L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=keypad_testapp_3

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET) test application   **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET) test application **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : keypad_test_3.h
