#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
####################################################################################################
#                                                                                                  #
#    @file   Makefile                                                                              #
#                                                                                                  #
#    @brief  configuration file to compile the sockets test.                                       #
#                                                                                                  #
####################################################################################################
#Revision History:                                                                                 #
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes                    #
#-------------------------   ------------    ----------  ------------------------------------------#
#S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version                          #
#L.DELASPRE/rc149c            25/11/2004     TLSbo44180   VTE 1.6 integration                      #
#L.Delaspre/rc149c            08/12/2004     TLSbo40142   update with Freescale identity           #
#S.ZAVJALOV/zvjs001c          26/05/2005     TLSbo50734   test.txt added                           #
#A.URUSOV                     13/09/2005     TLSbo55076   Fix compilation issue and warnings
#A.URUSOV                     01/11/2005     TLSbo57063   CFLAGS and LOADLIBES changed             #
####################################################################################################

LTP_DIR = ../../../../
BIN_DIR = ../../../

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

CFLAGS += -Wall \
          -I$(LTP_DIR)include \
          -I../rtic_test_module/

LOADLIBES = -L$(LTP_DIR)lib -lltp

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=	rtic_test

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : rtic test application               **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(BIN_DIR)bin
	@ln -f test.txt $(BIN_DIR)bin
	@echo "***********************************************"
	@echo "** INSTALL : rtic test application           **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(OBJS) $(TARGET)
	@echo "***********************************************"
	@echo "** CLEAN : rtic test application             **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : rtic_test.h

