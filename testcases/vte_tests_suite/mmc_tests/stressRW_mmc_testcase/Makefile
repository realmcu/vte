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
#        @brief  configuration file to compile the MMC driver test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  ------------------------------------------
#S.ZAVJALOV/zvjs001c          22/03/2005     TLSbo46706  Initial version
#L.Delaspre/rc149c            12/04/2005     TLSbo48760  VTE 1.9 integration
#A.Ozerov/b00320              20/02/2006     TLSbo61899  Testapp was cast to coding standarts
#
###################################################################################################
#Portability: ARM GCC 
#
###################################################################################################
CFLAGS+=          -Wall -I../../../../include
LOADLIBES+=       -L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET=stressRW_mmc_test

all: $(TARGET)
	rm -f $(OBJS)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                         **"
	@echo "***********************************************"
	@echo

install:
	ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                     **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                       **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : stressRW_mmc_test.h
