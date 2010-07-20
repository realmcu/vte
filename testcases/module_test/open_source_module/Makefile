

include ../../vte_tests_suite/vte_configs/$(ARCH_PLATFORM)_deconfigs 

#SUBDIRS = `ls */Makefile | sed "s/Makefile//g"` 

SUBDIRS-$(CONFIG_OM_NAND)    += nand-tests  
SUBDIRS-$(CONFIG_OM_RAMDISK) += ramdisk  
SUBDIRS-$(CONFIG_OM_SCULL)   += scull 

all:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS-y) ; do $(MAKE) -C $$i clean ; done

