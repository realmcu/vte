

#CC = $(CROSS_COMPILER)gcc
CC = $(CROSS_COMPILER)gcc
CFLAGS +=  

TARGET = iim_test
#SRC = $(wildcard *.c)
SRC=iim_test.c
OBJ=$(patsubst %.c,%.o,$(SRC))

all: $(OBJ)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJ)
	
install:
	-ln -f $(TARGET) ../../bin/

clean:
	rm -f *.o
	rm -f $(TARGET)
	
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $^
