LTP_DIR = ../../../../
BIN_DIR = $(LTP_DIR)testcases/bin/

TARGET = v4l_tvin

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)  test application  **"
	@echo "***********************************************"
	@echo

install:
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo
	ln -f ${TARGET} ${BIN_DIR}

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJS) : $(SRCS)

$(SRCS) : 

.phone:
clean:
	-rm -rf ${OBJS} ${TARGET}
