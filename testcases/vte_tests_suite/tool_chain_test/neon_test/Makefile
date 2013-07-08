TARGET_vfp = fir_vfp
TARGET_neon = fir_neon
TARGET_intrinsics = fir_neon_intrisics
TARGET_asm = fir_neon_asm

ALL_TARGET = $(TARGET_vfp) $(TARGET_neon) $(TARGET_intrinsics) $(TARGET_asm)

SOURCE = fir.cpp

ifneq ($(findstring -mfloat-abi=hard,$(CFLAGS)), -mfloat-abi=hard)
CFLAGS := -O3 -fsigned-char -mcpu=cortex-a9 -mfpu=vfp -mfloat-abi=softfp 
else
CFLAGS := -O3 -fsigned-char -mcpu=cortex-a9 -mfpu=vfp -mfloat-abi=hard
endif

CFLAGS_NEON = -O3 -fsigned-char -mcpu=cortex-a9 -mfpu=neon \
-ftree-vectorize -ftree-vectorizer-verbose=6
ifneq ($(findstring -mfloat-abi=hard,$(CFLAGS)), -mfloat-abi=hard)
CFLAGS_NEON += -mfloat-abi=softfp
else
CFLAGS_NEON += -mfloat-abi=hard
endif

LDFLAGS = -lm
#comment following line to switch flexibly between -none- and -poky-
#CROSS_COMPILE := arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)g++

all: vfp neon neon_intrinsics

vfp:
	-$(CC) $(CFLAGS) -Dvfp -o $(TARGET_vfp) $(SOURCE) ${LDFLAGS}

neon:
	-$(CC) $(CFLAGS_NEON) -Dvfp -o $(TARGET_neon) $(SOURCE) ${LDFLAGS}

neon_intrinsics:
	-$(CC) $(CFLAGS_NEON) -Dneon_intrinsics -o ${TARGET_intrinsics} $(SOURCE) ${LDFLAGS}

asm:
	-$(CC) $(CFLAGS_NEON) -Dneon_asm -o ${TARGET_asm} $(SOURCE) ${LDFLAGS}

install:
	-ln -f $(ALL_TARGET) ../../../bin/

clean:
	-rm -rf $(ALL_TARGET) 
