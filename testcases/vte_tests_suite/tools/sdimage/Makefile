
TARGET=sdimage
SRC=sdimage.c
OBJ=$(patsubst %.c,%.o, $(SRC))

all: $(OBJ)
	$(CC) -o $(TARGET) $(OBJ)

$(OBJ): $(SRC)

clean:
	rm -rf *.o

distclean:
	rm -rf *.o $(TARGET)
