SUBDIR = libpfm-4.2.0

all:
	${MAKE} -C ${SUBDIR} ARCH=armv7 CC=${CROSS_COMPILE}gcc

install:
	ln -f $(SUBDIR)/tests/validate  ../../bin/
	ln -f $(SUBDIR)/examples/showevtinfo  ../../bin/
	ln -f $(SUBDIR)/examples/check_events  ../../bin/

clean:
	$(MAKE) -C $(SUBDIR) distclean
