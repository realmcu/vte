TARGET=whetstone
SRC=whets.c
CC=$(CROSS_COMPILE)gcc

CFLAGS += -Wall -DUNIX

NEON_CFLAGS = -mcpu=cortex-a9 -mfpu=neon -Wall -DUNIX
ifneq ($(findstring -mfloat-abi=hard,$(CFLAGS)), -mfloat-abi=hard)
	NEON_CFLAGS += -mfloat-abi=softfp
else
	NEON_CFLAGS += -mfloat-abi=hard
endif

DP_CFLAGS = -DDP $(CFLAGS)

DP_NEON_CFLAGS = -DDP -mcpu=cortex-a9 -mfpu=neon -Wall -DUNIX
ifneq ($(findstring -mfloat-abi=hard,$(CFLAGS)), -mfloat-abi=hard)
	DP_NEON_CFLAGS += -mfloat-abi=softfp
else
	DP_NEON_CFLAGS += -mfloat-abi=hard
endif

all: neon
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) -lm
	$(CC) $(DP_CFLAGS) $(SRC) -o whetstone_dp -lm

neon:
	$(CC) $(NEON_CFLAGS) $(SRC) -o whets_neon -lm
	$(CC) $(DP_NEON_CFLAGS) $(SRC) -o whets_neon_dp -lm

install:
	-ln -f $(TARGET) ../../bin/
	-ln -f whets_neon ../../bin/
	-ln -f whets_neon_dp ../../bin/
	-ln -f whetstone_dp ../../bin/

clean:
	-rm -f ${TARGET} whets_neon whets_neon_dp whetstone_dp

