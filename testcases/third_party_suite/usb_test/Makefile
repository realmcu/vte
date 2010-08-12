TARGET = testusb

all:
	$(CROSS_COMPILER)gcc -Wall -g -lpthread -o testusb testusb.c 
install:
	ln -f $(TARGET) ../../bin/
	-ln -f test.sh   ../../bin/test_usb.sh
clean:
	rm -rf *.o
