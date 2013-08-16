##############################################################################
#
#   @file   Makefile
#
#   @brief  configuration file template to compile the v4l_capture_testapp test.
#
#   @par Portability:
#           
#           
#
##############################################################################

LIBPATH = ../../epxp_test
CFLAGS += -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include
LOADLIBES += $(LIBPATH)/lib/libpxp.a  -L../../../../lib -lltp -lpthread

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

#TO BE UPDATED WITH THE TEST APPLICATION NAME
TARGET=v4l2_capture_emma_uptr

all: $(TARGET)
	@echo "****************************************************"
	@echo "** ALL : v4l_capture_testapp test application     **"
	@echo "****************************************************"
	@echo

install:
	@echo "****************************************************"
	@echo "** INSTALL : v4l_capture_testapp                  **"
	@echo "****************************************************"
	ln -f $(TARGET) ../../../bin
	@echo

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS) LIB
	$(CC) $(OBJS) $(LOADLIBES) -o $@

$(OBJS) : $(SRCS)

LIB:
	$(MAKE) -C $(LIBPATH)/lib

$(SRCS) : v4l_capture_test.h
