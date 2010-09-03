TARGET=whetstone
SRC=whets.c
CC=$(CROSS_COMPILER)gcc

CFLAGS += -DUNIX -static

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) -lm

install:
	-ln -f $(TARGET) ../../bin/

clean:
	-rm -f ${TARGET}

