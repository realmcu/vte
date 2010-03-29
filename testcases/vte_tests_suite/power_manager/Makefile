ifeq ($(ARCH_PLATFORM), $(filter imx51stack imx53, $(ARCH_PLATFORM)))
SUBDIRS = usb_pm
endif

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS) ; do $(MAKE) -C $$i clean ; done

