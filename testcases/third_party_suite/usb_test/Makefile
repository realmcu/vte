TARGET = testusb

all:
	$(CROSS_COMPILER)cc -Wall -g -lpthread -o testusb testusb.c 
install:
	ln -f $(TARGET) ../../bin/
	-ln -f test.sh   ../../bin/usb_test.sh
clean:
	rm -rf *.o
