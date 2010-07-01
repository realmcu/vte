#CC=$(CROSS_COMPILE)gcc
#AR=$(CROSS_COMPILE)ar

# list of platforms which want this test case

OBJ = pxp_lib.o
INCLUDE = -I$(KLINUX_SRCDIR)/include
#INCLUDE = -I/home/smb/nfs_server/linux-2.6-testbuild/include
LIBNAME = libpxp

all: $(LIBNAME).so $(LIBNAME).a

install:
	echo "not need to install"

%.o: %.c
	$(CC) $(INCLUDE) -Wall -O2 -fPIC -c $^ -o $@

$(LIBNAME).so: $(OBJ)
	$(CC) -shared -nostartfiles -Wl,-soname,$@ $^ -o $@

$(LIBNAME).a: $(OBJ)
	$(AR) -rc $@  $^

.PHONY: clean
clean:
	rm -f $(LIBNAME).* $(OBJ)
