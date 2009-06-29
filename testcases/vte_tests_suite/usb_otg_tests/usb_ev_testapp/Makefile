#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#        @file   Makefile
#
#        @brief  Makefile to compile the USB-HID test.
#
#Portability: ARM GCC

##/include -I$(KLINUX_SRCDIR)/drivers/usb/gadget
#
####################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../../bin

CFLAGS += -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include -I$(KLINUX_SRCDIR)/drivers/usb/input  
LOADLIBES += -L$(LTP_DIR)/lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET = usb_ev_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "************************************************"
	@echo "** ALL : $(TARGET)                    **"
	@echo "************************************************"
	@echo

install:
	ln -f $(TARGET) $(BIN_DIR)
	@echo "************************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "************************************************"
	@echo "** CLEAN : $(TARGET)                  **"
	@echo "************************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : usb-evtest.h

