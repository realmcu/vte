#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#
#    @file   Makefile
#
#    @brief  Rules for compile the asrc test.
#
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
BIN_DIR = ../../bin

CFLAGS+= -Wall -I../../../include -I$(KLINUX_SRCDIR)/include \
         -I$(KLINUX_BLTDIR)/include \
	 -I$(KLINUX_SRCDIR)/drivers/net

LOADLIBES+=     -L../../../lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=asrc_testapp
SCRIPT=asrc_test1.sh asrc_grp_conv.sh asrc_clock_source_test1.sh asrc_clock_source_test2.sh asrc_clock_source_test3.sh

all: $(TARGET)
	rm -f $(OBJS)
	@echo "****************************************************"
	@echo "** ALL : $(TARGET) test application             **"
	@echo "****************************************************"
	@echo

install:
	cp -f $(TARGET) $(SCRIPT) $(BIN_DIR)
	@echo "****************************************************"
	@echo "** INSTALL : $(TARGET)                          **"
	@echo "****************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "****************************************************"
	@echo "** CLEAN : $(TARGET)                            **"
	@echo "****************************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)
