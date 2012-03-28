TARGET=whetstone
SRC=whets.c
CC=$(CROSS_COMPILE)gcc

CFLAGS += -DUNIX -static

NEON_CFLAGS = -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=softfp -static -Wall -DUNIX

all: neon
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) -lm

neon:
	$(CC) $(NEON_CFLAGS) $(SRC) -o whets_neon -lm

install:
	-ln -f $(TARGET) ../../bin/
	-ln -f whets_neon ../../bin/

clean:
	-rm -f ${TARGET} whets_neon

