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
# RB657C /gsch1c              20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
#L.Delaspre/rc149c            09/12/2004     TLSbo40142  update with Freescale identity
#L.Delaspre/rc149c            13/12/2004     TLSbo45068  .wav files issues
#D.Khoroshev/b00313           04/14/2006     TLSbo67022  VTE 2.0 Integration
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS+=	-Wall -I../../../../include  -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_SRCDIR)/sound/oss
LOADLIBES+=	-L../../../../lib -lltp -lpthread

ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+= -DCONFIG_MXC_MC13783_PMIC
endif
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

AUDIO_DIR = ../audio_test_files

TARGET=oss_audio_play_testapp_1

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)            **"
	@echo "***********************************************"
	@echo

install:
	cp -f $(TARGET) ../../../bin
	cp -uf $(AUDIO_DIR)/* ../../../bin
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

$(SRCS) : 
