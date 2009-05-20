###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the mu_api_rw_test test that checks Messaging Unit
#            driver read() and write() system calls in both blocking and non-blocking modes.
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
#                              Modification     Tracking
#Author (Core ID)                  Date          Number    Description of Changes
#---------------------------   ------------    ----------  ----------------------------------------
#Igor Semenchukov (smng001c)    23/08/2004     TLSbo40411   Initial version
#Igor Semenchukov (smng001c)    08/12/2004     TLSbo43804   Rework after heavy MU drv modification;
#                                                           merge two test cases in the one
##Sergey Yakubenko		04/10/2007     ENGR42513    Support of the SKYE Plus was added
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms.
#             If not, indicate specific reasons why is it not portable.
###################################################################################################


CFLAGS+=        -Wall -I../../../../include
LOADLIBES+=     -L../../../../lib -lltp

ifeq ($(ARCH_PLATFORM), mxc30031ads)
CFLAGS+= -DCONFIG_MACH_MXC30031ADS
endif

ifeq ($(ARCH_PLATFORM), mxc27520evb)
CFLAGS+=	-DCONFIG_MACH_MXC27520EVB
endif

ifeq ($(ARCH_PLATFORM), mxc30020evb)
CFLAGS+=	-DCONFIG_MACH_MXC30020EVB
endif

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=mu_api_rw_test

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL :   mu_api_rw_test test application   **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL :   mu_api_rw_test                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : mu_api_rw_test.h



