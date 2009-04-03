###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the oss_sound_driver_testapp test.
#
###################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#     
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
#L.Delaspre/rc149c            09/12/2004     TLSbo40142  update with Freescale identity
#D.Khoroshev/b00313           01/31/2006     TLSbo61785  Reworked version
#
###################################################################################################
#Portability: ARM GCC
###################################################################################################
CFLAGS+=	-Wall -I../../../../include  -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_SRCDIR)/sound/oss
LOADLIBES+=	-L../../../../lib -lltp

ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+= -DCONFIG_MXC_MC13783_PMIC
endif
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=oss_audio_mngt_testapp_2

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)            **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)        **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)          **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : oss_sound_driver_test.h
