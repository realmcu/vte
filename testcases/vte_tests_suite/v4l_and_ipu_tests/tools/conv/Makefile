#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html


#CC = $(CROSS_COMPILER)gcc
CFLAGS += -O3 
LDFLAGS += -lpthread

TARGET = conv
objs=main.o conv_yuv.o
SRC=main.c conv_yuv.c 

all: $(objs)
	$(CC) -o $(TARGET) $(LDFLAGS) $(objs)
	
install:

clean:
	rm -f *.o
	rm -f $(TARGET)
	
$(objs): $(SRC)
	$(CC) $(CFLAGS) -c $^
