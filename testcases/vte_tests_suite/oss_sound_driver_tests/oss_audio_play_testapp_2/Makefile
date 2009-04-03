####################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the oss_sound_driver_testapp test.
#
####################################################################################################
#
#    Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
#    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#    Freescale Semiconductor, Inc.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
# RB657C/gsch1c               20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
#L.Delaspre/rc149c            09/12/2004     TLSbo40142  update with Freescale identity
#L.Delaspre/rc149c            13/12/2004     TLSbo45068  .wav files issues
#A.Ozerov/B00320              17/02/2006     TLSbo62323  Index_ssi argument was deleted. Master mode
#                                                        was removed. Checking if the audio file given
#                                                        in argument is stereo or mono was added.
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
CFLAGS+=	-Wall -I../../../../include -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_SRCDIR)/sound/oss
LOADLIBES+=	-L../../../../lib -lltp
ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+= -DCONFIG_MXC_MC13783_PMIC
endif
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

AUDIO_DIR = ../audio_test_files

TARGET=oss_audio_play_testapp_2

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET) built      **"
	@echo "***********************************************"
	@echo 

install:
	cp -f $(TARGET) ../../../bin
	cp -uf $(AUDIO_DIR)/* ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET) set    **"
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
