###################################################################################################$
#$
#    @file   Makefile$
#$
#    @brief  configuration file to compile the uart_testapp_5 test.$
#$
#$
CFLAGS+= -Wall -I../../../../include -I$(KLINUX_SRCDIR)/include/$
LOADLIBES+=     -L../../../../lib -lltp -lpthread$
$
SRCS=$(wildcard *.c)$
OBJS=$(patsubst %.c,%.o,$(SRCS))$
$
#TO BE UPDATED WITH THE TEST APPLICATION NAME$
TARGET=uart_testapp_10$
$
all: $(TARGET)$
	@echo "***********************************************"$
	@echo "** ALL : uart_testapp_10     test application **"$
	@echo "***********************************************"$
	@echo$
$
install:$
	ln -f $(TARGET) ../../../bin$
	@echo "***********************************************"$
	@echo "** INSTALL : uart_testapp_10                  **"$
	@echo "***********************************************"$
	@echo$
$
clean:$
	rm -f $(OBJS) $(TARGET)$
$
$(TARGET): $(OBJS)$
	$(CC) $(OBJS) $(LOADLIBES) -o $@$
$
$(OBJS) : $(SRCS)$
$
$(SRCS) :$
