ifndef KERNEL_PATH
	export KERNEL_PATH=/home/vobs/linux-2.6-imx/
endif

export ARCH=$(ARCH_CPU)

SUBDIRS = oobtest pagetest subpagetest torture scantest speedtest stresstest

all clean tests: $(SUBDIRS)

install:
	@set -e; for i in $(SUBDIRS); do cp $$i/$$i.ko ../../bin/ ; done

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
