#CROSS_COMPILE = arm-none-linux-gnueabi-
#KLINUX_SRCDIR = /home/vobs/linux-2.6-imx

#CC=$(CROSS_COMPILE)gcc
#AR=$(CROSS_COMPILE)ar

OBJS = mxc_ipu_hl_lib.o mxc_ipu_lib.o

LIBNAME=libipu

INCLUDE = -I$(KLINUX_SRCDIR)/include


all: $(LIBNAME).so

install:
	echo "Caution !!! need check with unit test library version"

$(LIBNAME).so: $(OBJS)
	-$(CC) -shared -nostartfiles -Wl,-soname,$@ -o $@ $^ -lpthread

%.o: %.c
	-$(CC)  $(INCLUDE) -Wall -O2 -c $^ -o $@

.PHONY: clean
clean:
	@rm -f $(OBJS) $(LIBNAME).*
