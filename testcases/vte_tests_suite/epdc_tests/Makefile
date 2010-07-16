#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

CC = $(CROSS_COMPILER)gcc
CFLAGS += -I../../../include/ -I$(KLINUX_SRCDIR)/include
LDFLAGS += -L../../../lib -lltp -lpthread

TARGET = epdc_test
TARGET_SCRIPT = epdc_full.sh
SRC = epdc_main.c epdc_test.c
HEADS = epdc_test.h
OBJ=$(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)
install:
	-ln -f $(TARGET) ../../bin/
	-ln -f $(TARGET_SCRIPT) ../../bin/

clean:
	rm -f *.o
	rm -f $(TARGET)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^
