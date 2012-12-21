TARGET = memory_stress
SCRIPT = sanity.sh

SRCS = mmap001.c 
OBJS = $(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)
	@echo "*********************"
	@echo "memory stress on board DDR"
	@echo "*********************"

clean:
	rm -r $(OBJS) $(TARGET)

install:
	ln -f $(TARGET) ../../../bin/
	ln -f $(SCRIPT) ../../../bin/

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJS) : $(SRCS)




