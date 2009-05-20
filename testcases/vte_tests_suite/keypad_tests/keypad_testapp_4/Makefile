###################################################################################################
#
#        @file   Makefile
#
#        @brief  Makefile to compile the Keypad test 4
#
###################################################################################################
#
#        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#        Freescale Semiconductor, Inc.
#
###################################################################################################
# Revision History:
#                            Modification     Tracking
# Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
# V.Becker/rc023c              01/04/2004     TLSbo38735  Initial version
# V.Becker/rc023c              17/05/2004     TLSbo38735  MXC Keypad configuration
# V.Becker/rc023c              25/05/2004     TLSbo38735  Change file name
# L.Delaspre/rc149c            11/06/2004     TLSbo40109  Replace hard include path by a variable
# L.Delaspre/rc149c            16/08/2004     TLSbo40891  VTE 1.4 integration
# L.Delaspre/rc149c            08/12/2004     TLSbo40142  Update with Freescale identity
# A.Ozerov/NONE                10/01/2006     TLSbo61037  Update in accordance with linux-2.6.10-rel-L26_1_15
# A.Ozerov/B00320              15/02/2006     TLSbo61037  Device was changed and testapp was reworked accordingly
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS = -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include
LOADLIBES = -L../../../../lib -lltp -lpthread

ifeq ($(ARCH_PLATFORM), mxc30030ads)
CFLAGS+=    -DCONFIG_MACH_MXC30030ADS
endif

ifeq ($(ARCH_PLATFORM), imx27ads)
CFLAGS+=    -DCONFIG_MACH_MX27ADS
else
CFLAGS+=    -DCONFIG_MACH_ALL
endif

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET=keypad_testapp_4

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET) test application   **"
	@echo "***********************************************"

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET) test application **"
	@echo "***********************************************"

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : keypad_test_4.h
