#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#                                                                                                  #
#    @file   Makefile                                                                              #
#                                                                                                  #
#    @brief  configuration file to compile the sockets test.                                       #
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
#A.URUSOV                     01/11/2005     TLSbo57925   CFLAGS and LOADLIBES changed             #
###################################################################################################

LTP_DIR = ../../../../
BIN_DIR = ../../../
MODULE_DIR = ../../../module_test/vte_test_modules/rng_test_module 

ifeq ($(ARCH_PLATFORM), mxc91331evb)
CFLAGS += -DCONFIG_ARCH_MXC91331
endif

ifeq ($(ARCH_PLATFORM), mxc91221evb)
CFLAGS += -DCONFIG_ARCH_MXC91221
endif

ifeq ($(ARCH_PLATFORM), mxc91321evb)
CFLAGS += -DCONFIG_ARCH_MXC91321
endif

ifeq ($(ARCH_PLATFORM), mxc91231evb)
CFLAGS += -DCONFIG_ARCH_MXC91231
endif

ifeq ($(ARCH_PLATFORM), mxc91131evb)
CFLAGS += -DCONFIG_ARCH_MXC91131
endif

ifeq ($(ARCH_PLATFORM), imx31_3stack)
CFLAGS += -DCONFIG_ARCH_MX3
endif

ifeq ($(ARCH_PLATFORM), imx27evb)
CFLAGS += -DCONFIG_ARCH_MX27
endif

ifeq ($(ARCH_PLATFORM), imx21evb)
CFLAGS += -DCONFIG_ARCH_MX21
endif

ifeq ($(ARCH_PLATFORM), imx35_3stack)
CFLAGS += -DCONFIG_ARCH_MX35
endif

ifeq ($(ARCH_PLATFORM), imx37_3stack)
CFLAGS += -DCONFIG_ARCH_MX37
endif

ifeq ($(ARCH_PLATFORM), imx51_3stack)
CFLAGS += -DCONFIG_ARCH_MX51
endif

ifeq ($(ARCH_PLATFORM), imx51)
CFLAGS += -DCONFIG_ARCH_MX51
endif

CFLAGS += -Wall -I$(LTP_DIR)include/ \
          -I$(MODULE_DIR) \
          -I$(KLINUX_SRCDIR)/drivers/mxc/security/ \
          -I$(KLINUX_SRCDIR)/drivers/mxc/security/sahara2/include \
          -I$(KLINUX_SRCDIR)/drivers/mxc/security/rng/include \
	  -I$(KLINUX_SRCDIR)/include \
          -I$(KLINUX_BLTDIR)/include \
          -DCONFIG_PM

LOADLIBES = -L$(LTP_DIR)lib -lltp

SRCS=$(wildcard *.c) 
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=rng_test


all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : rng test application               **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : rng test application           **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN:  rng test application             **"
	@echo "***********************************************"
	@echo


$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : rng_test.h
