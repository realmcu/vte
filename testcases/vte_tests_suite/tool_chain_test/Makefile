TARGET = memcpy_test

SRC = memcpy_test.c
OBJ =$(patsubst %.c,%.o,$(SRC))
CFLAGS = -Wall -O2

all: $(OBJ)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJ)

$(OBJ):$(SRC)
	$(CC) $(CFLAGS) -c $^

install:
	ln -f $(TARGET) ../../bin/

clean:
	rm -rf $(OBJ) $(TARGET)
