TARGET = memory_stress

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

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJS) : $(SRCS)




