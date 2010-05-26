#Copyright 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################

TRAGET = mxs_ptp_test
SRC = $(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJ)

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

$(OBJ) : $(SRC)
