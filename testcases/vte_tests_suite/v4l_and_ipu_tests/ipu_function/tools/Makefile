CC = $(CROSS_COMPILER)gcc
CFLAGS += -O3 -I$(KLINUX_SRCDIR)/include -I../../../../../include
LDFLAGS +=  -L../../../../../lib -lltp 

TARGET = imx_fb 
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))


all:$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)
	 
install:
	-ln -f $(TARGET) ../../../../bin

clean:
	-rm -f $(TARGET) *.o

$(OBJS) : $(SRCS)
