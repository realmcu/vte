####################################################################################################
#                                                                                                  #
#    @file   Makefile                                                                              #
#                                                                                                  #
#    @brief  configuration file to compile the sockets test.                                       #
#                                                                                                  #
####################################################################################################
#                                                                                                  #
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved                          #
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT                                   #
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF                                       #
#   Freescale Semiconductor, Inc.                                                                  #
#                                                                                                  #
####################################################################################################
#Revision History:                                                                                 #
#                            Modification     Tracking                                             #
#Author                          Date          Number    Description of Changes                    #
#-------------------------   ------------    ----------  ------------------------------------------#
#S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version                          #
#L.DELASPRE/rc149c            25/11/2004     TLSbo44180   VTE 1.6 integration                      #
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity           #
#A.URUSOV                     06/09/2005     TLSbo53754   CFLAGS and LOADLIBES changed             #
#A.URUSOV                     01/11/2005     TLSbo57925   CFLAGS and LOADLIBES changed             ####################################################################################################
#include $(KLINUX_BLTDIR)/include/linux/autoconf.h

include $(KLINUX_BLTDIR)/.config

LTP_DIR = ../../../../
BIN_DIR = ../../../
MODULE_DIR = ../rnga_test_module

ifeq "$(CONFIG_ARCH_MXC91331)" "y"
CFLAGS += -DCONFIG_ARCH_MXC91331
endif

ifeq "$(CONFIG_ARCH_MXC91221)" "y"
CFLAGS += -DCONFIG_ARCH_MXC91221
endif

ifeq "$(CONFIG_ARCH_MXC91321)" "y"
CFLAGS += -DCONFIG_ARCH_MXC91321
endif

ifeq "$(CONFIG_ARCH_MXC91231)" "y"
CFLAGS += -DCONFIG_ARCH_MXC91231
endif

ifeq "$(CONFIG_ARCH_MXC91131)" "y"
CFLAGS += -DCONFIG_ARCH_MXC91131
endif

ifeq "$(CONFIG_ARCH_MX3)" "y"
CFLAGS += -DCONFIG_ARCH_MX3
endif

ifeq "$(CONFIG_ARCH_MX27)" "y"
CFLAGS += -DCONFIG_ARCH_MX27
endif

ifeq "$(CONFIG_ARCH_MX21)" "y"
CFLAGS += -DCONFIG_ARCH_MX21
endif

CFLAGS += -Wall -I$(LTP_DIR)include/ \
          -I$(MODULE_DIR) \
          -I$(KLINUX_SRCDIR)/drivers/mxc/security/ \
          -I$(KLINUX_SRCDIR)/drivers/mxc/security/sahara2/include \
          -I$(KLINUX_SRCDIR)/drivers/mxc/security/rng/include \
          -DCONFIG_PM

#          -I$(KLINUX_SRCDIR)/drivers/mxc/security/sahara2/include \
          -I$(KLINUX_BLTDIR)/include \
#
LOADLIBES = -L$(LTP_DIR)lib -lltp

SRCS=$(wildcard *.c) 
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=rnga_test


all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : rnga test application               **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : rnga test application           **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN:  rnga test application             **"
	@echo "***********************************************"
	@echo


$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : rnga_test.h
