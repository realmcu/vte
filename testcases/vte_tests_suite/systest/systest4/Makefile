#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the systeat_testapp1 test.
#
########################### REVISION HISTORY #################################
#
#Author/Core ID          Date       CR Number    Description of Changes
#-------------------   ----------   -----------  -----------------------------
#A.Ozerov              29/05/2007    ENGR37685   Initial version
#
###################################################################################################

LTP_DIR = ../../../..
BIN_DIR = ../../../bin

CFLAGS += -Wall -DLINUX -I$(LTP_DIR)/include
LOADLIBES  += -L$(LTP_DIR)/lib -lltp -pthread

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = systest4_testapp

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : systest2_testapp                    **"
	@echo "***********************************************"
	@echo
	rm -f $(OBJS)

install:
	cp -f $(TARGET) $(BIN_DIR)
	@echo "***********************************************"
	@echo "** INSTALL : systest2_testapp                **"
	@echo "***********************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)
