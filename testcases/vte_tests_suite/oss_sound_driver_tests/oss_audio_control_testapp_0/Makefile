#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#        @file   Makefile
#
#        @brief  configuration file to compile the oss_audio_control_testapp_0 test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#L.Delaspre/rc149c            13/12/2004     TLSbo45068  .wav files issues
#A.Ozerov/b00320              07/11/2005     TLSbo56870  Compilation flags for SC55112 and MC13783
#                                                        platforms
#D.Khoroshev/b00313           03/03/2006     TLSbo62323  Code cleaning
#D.Simakov                    22/12/2005     TLSbo87096  Zeus compilation issue
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS+=	-Wall -I../../../../include -I$(KLINUX_SRCDIR)/include  -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/sound/oss -I$(KLINUX_SRCDIR)/sound/arm
LOADLIBES+=	-L../../../../lib -lltp

ifeq ($(ARCH_PLATFORM),mxc91131evb)
CFLAGS+= -DCONFIG_MXC_PMIC_SC55112
else
CFLAGS+= -DCONFIG_MXC_MC13783_PMIC
endif 

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=oss_audio_control_testapp_0

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET) built   **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	cp -uf ../alsa_bin/aplay ../../../bin
	cp -uf ../alsa_bin/arecord ../../../bin        
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)     **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN   : $(TARGET)     **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : oss_sound_driver_test.h
