#Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#        @brief  configuration file to compile the VTE UDMA test applications.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#A.Ozerov/b00320              03/10/2006     TLSbo78550  Initial version.
#A.Ozerov/b00320              01/11/2006     TLSbo81158  UDMA module was fixed for working with all platforms.
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../..
MOD_DIR = ../udma_test_module

ifeq ($(ARCH_PLATFORM), imx27ads)
CFLAGS += -Wall -DCONFIG_IMX27
else
CFLAGS += -Wall -DCONFIG_OTHER_PLATFORM
endif

CFLAGS    +=    -I$(LTP_DIR)/include -I$(MOD_DIR)
LOADLIBES +=    -L$(LTP_DIR)/lib -lltp -lpthread

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = udma_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL:      $(TARGET)                     **"
	@echo "***********************************************"
	@echo

install:
	cp -rf $(TARGET) $(BIN_DIR)/bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                     **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN :   $(TARGET)                     **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS): $(SRCS)
$(SRCS): udma_test.h
