SUBDIR = kexec-tools-2.0.2


all: config
	for i in $(SUBDIR); do $(MAKE) -C $$i ; done

install:
	-ln -f kexec-tools-2.0.2/build/sbin/kexec ../../bin
	-ln -f kexec-tools-2.0.2/build/sbin/kdump ../../bin

config:
	cd kexec-tools-2.0.2; ./configure --host=arm-none-linux-gnueabi	

clean:
	for i in $(SUBDIR); do $(MAKE) -C $$i clean; done

