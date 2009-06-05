#CROSS_COMPILER = /opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-

CC=$(CROSS_COMPILER)gcc
LINK=$(CROSS_COMPILER)gcc


ifeq ($(ARCH_PLATFORM),$(filter imx37stack,$(ARCH_PLATFORM)))
PLATFORM =IMX37_3STACK
endif

OBJ = main.o \
      dec.o \
      enc.o \
      capture.o \
      display.o \
      fb.o \
      utils.o \
      loopback.o

VPULIBRARY = -L../vpu_lib
INCLUDE = -I../vpu_lib
OBJDIR = ../../../../bin/

# list of platforms which want this test case
INCLUDE_LIST:=IMX27ADS IMX31ADS IMX37_3STACK

ifeq ($(PLATFORM),$(findstring $(PLATFORM),$(INCLUDE_LIST)))
TARGET = vpu_testapp
else
TARGET =
endif

all: $(TARGET)

install:
	ln -f $(TARGET) $(OBJDIR)
	-cp -f ./configs/* $(OBJDIR)

$(TARGET): $(OBJ)
	$(LINK) -o $(TARGET) $(OBJ) $(VPULIBRARY) -lvpu -lpthread

%.o: %.c
	$(CC) -Wall -O2 $(INCLUDE) -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ) 
