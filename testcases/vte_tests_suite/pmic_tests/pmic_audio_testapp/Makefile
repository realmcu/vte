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
#        @file Makefile
#
#        @brief configuration file to compile the PMIC audio test.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#S.Bezrukov/SBAZR1C           17/08/2005     TLSbo52697  Initial version
#A.Ozerov/b00320              08/08/2006     TLSbo73745  Review version(in accordance to L26_1_19 release).
#
###################################################################################################

LTP_DIR = ../../../../lib
BIN_DIR = ../../../bin
MC13783_LEGACY_DIR = $(KLINUX_SRCDIR)/drivers/mxc/mc13783_legacy

ifeq ($(ARCH_PLATFORM), mxc91131evb)
CFLAGS += -DCONFIG_MXC_PMIC_SC55112 -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
else
CFLAGS += -DCONFIG_MXC_PMIC_MC13783 -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
endif

LOADLIBES += -L$(LTP_DIR) -lltp

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

TARGET = pmic_testapp_audio

all: $(TARGET)
	rm -f $(OBJS)
	@echo "************************************************"
	@echo "** ALL : pmic_testapp_audio                   **"
	@echo "************************************************"
	@echo

install:
	@echo "************************************************"
	@echo "** INSTALL : pmic_testapp_audio               **"
	@echo "************************************************"
	ln -f $(TARGET) $(BIN_DIR)
	@echo

clean:
	@echo "************************************************"
	@echo "** CLEAN : pmic_testapp_audio                 **"
	@echo "************************************************"
	@echo
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : pmic_audio_test.h

