Target_memcpy = memcpy_test
Target_spread = spread
Target_membm = memcpybm

TARGET = $(Target_memcpy) $(Target_spread) $(Target_membm)

SRC_memcpy = memcpy_test.c
SRC_spread = spread.c
SRC_membm = memcpybm.c
SRC = $(SRC_memcpy) $(SRC_spread) $(SRC_membm) 
#OBJ =$(patsubst %.c,%.o,$(SRC))
CFLAGS = -Wall -O2 -lrt -std=gnu99

all: $(Target_memcpy) $(Target_spread) $(Target_membm) 

$(Target_memcpy): $(SRC_memcpy)
	$(CC) -o $@ $(CFLAGS) $<

$(Target_spread): $(SRC_spread)
	$(CC) -o $@ $(CFLAGS) $<

$(Target_membm):$(SRC_membm)
	$(CC) -o $@ $(CFLAGS) $<

install:
	ln -f $(TARGET) ../../bin/

clean:
	rm -rf $(TARGET)
