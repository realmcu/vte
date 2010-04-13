#ignore devnode check in cross compile
SUBDIRS = pec

all:
	@set -e; \
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done 

install:
	@set -e; \
	ln -f connector_test.sh ../../bin/connector_test.sh; \
	chmod ugo+x connector_test.sh; \
	for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i $@ ; done
