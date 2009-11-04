SUBDIRS += libpcap-0.9.6 libpng-1.2.20

all: config
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc; done
	$(MAKE) -C libpcap-0.9.6  CC=$(CROSS_COMPILER)gcc shared
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done
	$(MAKE) -C libpcap-0.9.6 install-shared
install:
	echo "usually the rootfs will have test libs"
clean:
	-@set -e; for i in $(SUBDIRS) ; do $(MAKE) -C $$i clean ; done
	-@set -e; for i in $(SUBDIRS) ; do $(MAKE) -C $$i distclean ; done
	rm -rf bin lib share man include
config:
	cd zlib-1.2.3; ./myconfig ; cd ..
	make -C zlib-1.2.3 CC=$(CROSS_COMPILER)gcc
	make -C zlib-1.2.3 install
	@set -e; for i in $(SUBDIRS) ; do cd $$i ; ./myconfig ; cd ..; done

