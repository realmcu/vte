SUBDIRS =  zlib-1.2.3 libpcap-0.9.6

all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc; done
	-$(MAKE) -C libpcap-0.9.6  CC=$(CROSS_COMPILER)gcc shared
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	-$(MAKE) -C libpcap-0.9.6 install-shared
install:
	echo "usually the rootfs will have test libs"
clean:
	rm -rf bin
	@set -e; for i in $(SUBDIRS) ; do $(MAKE) -C $$i clean ; done
config:
	-cd zlib-1.2.3 && ./configure --prefix=$$(pwd)/../ && cd .. 
	-cd libpcap-0.9.6  && ./myconfig && cd ..

