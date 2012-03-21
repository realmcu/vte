TARGET = timer_resolution nanosleep_test timer_interrupt

CFLAGS  +=  -Wall -I../../../include
LDFLAGS +=  -L../../../lib -lltp
SRC = timer_resolution.c nanosleep_test.c

all: $(TARGET)

install:
	-ln -f $(TARGET) ../../bin/
clean:
	-rm -rf $(TARGET)

.PHONY:timer_resolution
timer_resolution: 
	$(CC)	$(CFLAGS) timer_resolution.c $(LDFLAGS) -o $@

.PHONY: nanosleep_test
nanosleep_test:
	$(CC)  $(CFLAGS) nanosleep_test.c $(LDFLAGS) -o $@

.PHONY: timer_interrupt
timer_interrupt:
	$(CC)  $(CFLAGS) timer_interrupt.c $(LDFLAGS) -o $@
	
