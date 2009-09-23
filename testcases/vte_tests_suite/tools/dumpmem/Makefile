
CFLAGS += -O2
LDFLAGS +=

SRC = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRC))

TARGET = dumpmem

all: $(OBJS)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJS)

install:
	-ln -f $(TARGET) ../../../bin/

clean:
	-rm -f $(TARGET) *.o

$(OBJS): $(SRC)


