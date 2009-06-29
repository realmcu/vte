#Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
#        @brief  configuration file to compile the sockets test.
#
####################################################################################################
#Revision History:                                                                                 
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes                    
#-------------------------   ------------    ----------  -------------------------------------------
#S.ZAVJALOV/-----             13/07/2004     TLSbo40259  Initial version                          
#L.DELASPRE/rc149c            13/08/2004     TLSbo40891  VTE 1.4 integration                      
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#A.Ozerov/B00320              10/02/2006     TLSbo61734  Code was cast to coding conventions  
#                                                                                                  
####################################################################################################
#Portability: ARM GCC
#
####################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../..

CFLAGS +=    -Wall -I$(LTP_DIR)/include 
LOADLIBES += -L$(LTP_DIR)/lib -lltp

SUBDIRS = module

SRCS = mxc_sdma_test.c
OBJS = mxc_sdma_test.o

TARGET = sdma_rw
SCRIPT = sdma_rw.sh

all: $(TARGET)
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                            **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(OBJS) $(TARGET)
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	@echo "***********************************************"
	@echo "** CLEAN: $(TARGET)                           **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) $(SCRIPT) $(BIN_DIR)/bin
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                        **"
	@echo "***********************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

#$(SRCS) : sdma_test.h
