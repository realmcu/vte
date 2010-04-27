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

ifeq ($(ARCH_PLATFORM), $(filter imx25stack imx233, $(ARCH_PLATFORM)))
	CFLAGS += -O2 -fsigned-char -msoft-float
endif
ifeq ($(ARCH_PLATFORM), $(filter imx31stack imx35stack imx37stack imx51stack imx51, $(ARCH_PLATFORM)))
	CFLAGS += -O2 -fsigned-char -mfloat-abi=softfp -mfpu=vfp
endif

LOADLIBES+= -L$(LTP_DIR)/lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%*.o,$(SRCS))

TARGET=dry2 dry2nr
 
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

$(TARGET):
	$(CC) $(CFLAGS) -o dry1.o -c dry.c
	$(CC) $(CFLAGS) -DPASS2 dry.c dry1.o -o dry2
	$(CC) $(CFLAGS) -DREG -o dry2.o -c dry.c
	$(CC) $(CFLAGS) -DPASS2 -DREG dry.c dry2.o -o dry2nr

