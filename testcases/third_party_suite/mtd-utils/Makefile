SUBDIRS = mtd-utils-1.4.4

#sudo apt-get install uuid-dev
#sudo apt-get install uuid-dev libacl1-dev liblzo2-dev

all:
	for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILE)gcc; done

install:
	-ln -f $(SUBDIRS)/nandwrite ../../../testcases/bin/
	-ln -f $(SUBDIRS)/nanddump ../../../testcases/bin/

clean:
	-@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
