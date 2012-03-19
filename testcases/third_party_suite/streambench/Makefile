#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#        @brief  configuration file to compile the cdrom_driver_testapp.
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
LTP_DIR=../../../..
BIN_DIR=../../

LOADLIBES+= -L$(LTP_DIR)/lib

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%*.o,$(SRCS))

TARGET=stream_benchmark

CFLAGS = -Wall -O3 -fsigned-char -march=armv7-a  -mfpu=neon -mfloat-abi=softfp
#CFLAGS = -Wall -O3 -fsigned-char -march=armv7-a

all: $(TARGET)
	@echo "************************************************"
	@echo "** ALL : $(TARGET)                 **"
	@echo "************************************************"
	@echo

install:
	@echo "************************************************"
	@echo "** INSTALL : $(TARGET)             **"
	@echo "************************************************"
	ln -f $(TARGET) $(BIN_DIR)/bin

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "************************************************"
	@echo "** CLEAN : $(TARGET)               **"
	@echo "************************************************"

$(TARGET): Makefile
	$(CROSS_COMPILE)gcc $(CFLAGS) stream.c -o stream_benchmark

