SUBDIRS = cansocket

CFLAGS    = -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	-DETH_P_CAN=0x000C \
	-DPF_CAN=29 \
	-DAF_CAN=PF_CAN

all: filter_test
	for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	-ln -f tst-filter ../../bin/
	-ln -f tst-rcv-own-msgs ../../bin/
	-ln -f run_ltp-can_tests.sh ../../bin/


clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
	-@rm -f tst-filter tst-rcv-own-msgs

filter_test:
	$(CC) -o tst-filter $(CFLAGS) ./filter-tests/tst-filter.c 
	$(CC) -o tst-rcv-own-msgs $(CFLAGS) ./filter-tests/tst-rcv-own-msgs.c 

