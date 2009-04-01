####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the sockets test.
#
####################################################################################################
#
#    Copyright (C) 2006 Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#S.ZAVJALOV/ZVJS001C          27/07/2004     TLSbo40261  Initial version
#L.DELASPRE/rc149c            02/08/2004     TLSbo40891  VTE 1.4 integration
#L.DELASPRE/rc149c            06/10/2004     TLSbo42821  VTE 1.5.1 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  Update with Freescale identity
#A.Ozerov/b00320              19/04/2006     TLSbo61865  Minor fixes.
#
####################################################################################################
CFLAGS    +=     -Wall -I../../../../include -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/include
LOADLIBES +=     -L../../../../lib -lltp

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = gpt_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                    **"
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
	@echo "** CLEAN : $(TARGET)                  **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : GPT_test.h
