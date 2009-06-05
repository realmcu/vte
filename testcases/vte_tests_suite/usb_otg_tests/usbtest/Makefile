####################################################################################################
#
#        @file   Makefile
#
#        @brief  Makefile to compile the USB-HID test.
#
#Portability: ARM GCC

##/include -I$(KLINUX_SRCDIR)/drivers/usb/gadget
#
####################################################################################################
LTP_DIR = ../../../..
BIN_DIR = ../../../bin

CFLAGS += -Wall -I$(LTP_DIR)/include -I$(KLINUX_SRCDIR)/include -I$(KLINUX_BLTDIR)/include  
LOADLIBES += -L$(LTP_DIR)/lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

TARGET = usbtest_testapp

all: $(TARGET)
	rm -f $(OBJS)
	@echo "************************************************"
	@echo "** ALL : $(TARGET)                    **"
	@echo "************************************************"
	@echo

install:
	ln -f $(TARGET) $(BIN_DIR)
	@echo "************************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "************************************************"
	@echo

clean:
	rm -f $(OBJS) $(TARGET)
	@echo "************************************************"
	@echo "** CLEAN : $(TARGET)                  **"
	@echo "************************************************"
	@echo

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : usbtest.h

