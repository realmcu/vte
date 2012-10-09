TARGET_vfp = fir_vfp
TARGET_neon = fir_neon
TARGET_intrinsics = fir_neon_intrisics
TARGET_asm = fir_neon_asm

ALL_TARGET = $(TARGET_vfp) $(TARGET_neon) $(TARGET_intrinsics) $(TARGET_asm)

SOURCE = fir.cpp

CFLAGS := -O3 -fsigned-char -mcpu=cortex-a9 -mfpu=vfp -mfloat-abi=softfp 
CFLAGS_NEON = -O3 -fsigned-char -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon \
-ftree-vectorize -ftree-vectorizer-verbose=6
LDFLAGS = -lm
CROSS_COMPILE := arm-linux-gnueabi-
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
