# list of platforms which want this test case

CC = $(CROSS_COMPILE)gcc
#CFLAGS +=

SRC = $(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SRC))
LIBPATH = .
CFLAGS += -I$(KLINUX_SRCDIR)/include -I$(KERNEL_SRCDIR)/include -I$(KERNEL_SRCDIR)/include/uapi  -I./lib

LIBS += -L$(LIBPATH)/lib lib/libpxp.a -lpthread

TARGET = epxp_test

all: $(OBJ) LIB
	$(VERBOSE) $(CC) $(OBJ) $(LIBS) -o $(TARGET)

LIB:
	$(MAKE) -C lib

install:
	-ln $(TARGET) ../../bin/

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET)
	$(MAKE) -C lib clean

$(OBJ): $(SRC)
