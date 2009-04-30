# list of platforms which want this test case$
$
#CROSS_COMPILE = arm-none-linux-gnueabi-$
#KLINUX_SRCDIR = /home/vobs/linux-2.6-imx$
CC=$(CROSS_COMPILER)gcc$
AR=$(CROSS_COMPILER)ar$
$
$
BINDIR = ../../../bin/$
SRCS = mxc_ipudev_test.c utils.c test_pattern.c test_utils.c$
#OBJS = $(patsubst %.c,%.o,$(SRCS))$
LIBS += -L../ipu/ -lipu -lm -L../screenlayer/ -lIpuScreenLayer$
INC = -I$(KLINUX_SRCDIR)/include -I../ipu -I../screenlayer$
CFLAGS += -O3$
$
TARGET = ipu_dev_test$
$
all: $(TARGET)$
$
install:$
	-ln -f $(TARGET) $(BINDIR)$
$
$(TARGET): $(SRCS)$
	-$(CC) $(LIBS) $(INC) $(CFLAGS) $(SRCS) -o $(TARGET)$
$
$
.PHONY: clean$
clean:$
	rm -f $(TARGET)$
$
