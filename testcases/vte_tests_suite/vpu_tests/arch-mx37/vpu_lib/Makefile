CC=$(CROSS_COMPILER)gcc
AR=$(CROSS_COMPILER)ar

# list of platforms which want this test case
# for test program we do not need to install the library
#INCLUDE_LIST:= IMX27ADS IMX31ADS IMX32ADS IMX37_3STACK IMX51_3STACK

OBJ = vpu_io.o vpu_util.o vpu_lib.o

LIBNAME = libvpu

ifeq ($(ARCH_PLATFORM), $(filter imx37stack, $(ARCH_PLATFORM)))
PLATFORM=IMX37_3STACK
endif


ifeq ($(ARCH_PLATFORM), $(filter imx51stack, $(ARCH_PLATFORM)))
PLATFORM=IMX51_3STACK
endif

all: $(LIBNAME).so $(LIBNAME).a

ifeq ($(PLATFORM), $(findstring $(PLATFORM), $(INCLUDE_LIST)))

install: install_headers
	@mkdir -p $(DEST_DIR)/usr/lib
	cp $(LIBNAME).* $(DEST_DIR)/usr/lib

install_headers:
	@mkdir -p $(DEST_DIR)/usr/include
	cp vpu_lib.h $(DEST_DIR)/usr/include
	cp vpu_io.h $(DEST_DIR)/usr/include
else
install :
endif

%.o: %.c
	$(CC) -D$(PLATFORM) -Wall -O2 -c $^ -o $@

$(LIBNAME).so: $(OBJ)
	$(CC) -shared -nostartfiles -Wl,-soname,$@ $^ -o $@

$(LIBNAME).a: $(OBJ)
	$(AR) -rc $@  $^

.PHONY: clean
clean:
	rm -f $(LIBNAME).* $(OBJ)
