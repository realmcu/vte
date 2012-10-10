TARGET = clinpack_rs clinpack_rd clinpack_urs clinpack_urd clinpack_rs_neon clinpack_rd_neon clinpack_urs_neon clinpack_urd_neon
SRC = clinpack.c

CC=$(CROSS_COMPILE)gcc
RS_CFLAGS = -O3 -mcpu=cortex-a9 -ffast-math -DSP -DROLL -mfpu=vfp -mfloat-abi=softfp
RD_CFLAGS = -O3 -mcpu=cortex-a9 -ffast-math -DDP -DROLL -mfpu=vfp -mfloat-abi=softfp
URS_CFLAGS = -O3 -mcpu=cortex-a9 -ffast-math -DSP -DUNROLL -mfpu=vfp -mfloat-abi=softfp
URD_CFLAGS = -O3 -mcpu=cortex-a9 -ffast-math -DDP -DUNROLL -mfpu=vfp -mfloat-abi=softfp
RS_CFLAGS_NEON = -O3 -mcpu=cortex-a9 -ffast-math -DSP -DROLL -mfpu=neon -mfloat-abi=softfp
RD_CFLAGS_NEON = -O3 -mcpu=cortex-a9 -ffast-math -DDP -DROLL -mfpu=neon -mfloat-abi=softfp
URS_CFLAGS_NEON = -O3 -mcpu=cortex-a9 -ffast-math -DSP -DUNROLL -mfpu=neon -mfloat-abi=softfp
URD_CFLAGS_NEON = -O3 -mcpu=cortex-a9 -ffast-math -DDP -DUNROLL -mfpu=neon -mfloat-abi=softfp


all: clinpack_rd clinpack_urs clinpack_urd clinpack_rs_neon clinpack_rd_neon clinpack_urs_neon clinpack_urd_neon
	$(CC) $(RS_CFLAGS) $(SRC) -o clinpack_rs -lm

clinpack_urs:
	$(CC) $(URS_CFLAGS) $(SRC) -o clinpack_urs -lm
clinpack_urd:
	$(CC) $(URD_CFLAGS) $(SRC) -o clinpack_urd -lm
clinpack_rd:
	$(CC) $(RD_CFLAGS) $(SRC) -o clinpack_rd -lm
clinpack_urs_neon:
	$(CC) $(URS_CFLAGS_NEON) $(SRC) -o clinpack_urs_neon -lm
clinpack_urd_neon:
	$(CC) $(URD_CFLAGS_NEON) $(SRC) -o clinpack_urd_neon -lm
clinpack_rd_neon:
	$(CC) $(RD_CFLAGS_NEON) $(SRC) -o clinpack_rd_neon -lm
clinpack_rs_neon:
	$(CC) $(RS_CFLAGS_NEON) $(SRC) -o clinpack_rs_neon -lm


.PHONY: clean
clean:
	rm -f $(TARGET)

.PHONY: install
install:
	-ln -f $(TARGET) ../../bin/

.PHONY: uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)
