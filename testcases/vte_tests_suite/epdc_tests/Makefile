#
# Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

CC = $(CROSS_COMPILER)gcc
CFLAGS += -I../../../include/ -I$(KLINUX_SRCDIR)/include -trigraphs
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
