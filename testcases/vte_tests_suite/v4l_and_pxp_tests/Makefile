
TARGET=pxp_test

SRC=pxp_test.c

OBJ=$(patsubst %.c,%.o,$(SRC))
CFLAGS+= -Wall -I$(KLINUX_SRCDIR)/include/


.phony: all

all: $(TARGET)


clean:
	rm -f $(OBJ) $(TARGET)

install:
	-ln -f $(TARGET) ../../bin/

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

$(OBJ): $(SRC)
