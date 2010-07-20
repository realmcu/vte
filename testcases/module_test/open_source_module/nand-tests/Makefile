ifndef KERNEL_PATH
	export KERNEL_PATH=$(KLINUX_BLTDIR)
endif

export ARCH=$(ARCH_CPU)
export CROSS_COMPILE=$(CROSS_COMPILER)

SUBDIRS = oobtest pagetest subpagetest torture scantest speedtest stresstest

all clean tests: $(SUBDIRS)

install:
	@set -e; for i in $(SUBDIRS); do cp $$i/$$i.ko ../../../bin/ ; done

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
