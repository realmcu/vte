CC=$(CROSS_COMPILER)gcc
AR=$(CROSS_COMPILER)ar

INCLUDE = -I$(KLINUX_SRCDIR)/include
INC = $(INCLUDE) -I../ipu

OBJS = ScreenLayer.o

LIBNAME=libIpuScreenLayer

all: $(LIBNAME).so

install:
	echo "Caution !!! need check with unit test library version"

$(LIBNAME).so: $(OBJS)
	$(CC) -shared -nostartfiles -Wl,-soname,$@ -o $@ $^ -L../ipu -lipu

%.o: %.c
	$(CC) $(INC) -Wall -O2 -c $^ -o $@

.PHONY: clean
clean:
	@rm -f $(OBJS) $(LIBNAME).*
