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
#    @brief  configuration file to compile the ipu_pf_testapp.
#
####################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#A.Smirnov                    26/06/2005     TLSbo49894   Initial varsion
#D.Simakov/smkd001c           23/09/2005     TLSbo55077   Re-worked
#A.Pshenichnikov              23/11/2005     TLSbo58740   ring.yuv and blocks.yuv are copied 
#E.Gromazina/NONE             10/01/2006     TLSbo61481   Clean the LCD after testing
#
###################################################################################################
#Portability: ARM GCC
#
###################################################################################################
CFLAGS+=        -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include -DDEBUG_TEST
LOADLIBES+=     -L../../../../lib -lltp

SRCS = $(wildcard *.c) stuff/ycbcr.c stuff/fb_draw_api.c
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = ipu_pf_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** $(TARGET)                            **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin/
	#cp -f ring.yuv blocks.yuv ../../../bin/
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                  **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** DELETE *.o $(TARGET)         "
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : ipu_pf_test.h
