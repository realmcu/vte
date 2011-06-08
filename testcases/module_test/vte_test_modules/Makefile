

include ../../vte_tests_suite/vte_configs/$(ARCH_PLATFORM)_deconfigs 

SUBDIRS-$(CONFIG_VM_DRYICE)   += dryice_test_module
SUBDIRS-$(CONFIG_VM_SDMA)     += sdma_rw_module
SUBDIRS-$(CONFIG_VM_SAHARA)   += sahara_module
SUBDIRS-$(CONFIG_VM_SCC)      += scc_test_module  
SUBDIRS-$(CONFIG_VM_SCC2)     += scc2_test_module  
SUBDIRS-$(CONFIG_VM_RNG)      += rng_test_module
SUBDIRS-$(CONFIG_VM_RNGA)     += rnga_test_module
SUBDIRS-$(CONFIG_VM_SPI)      += spi_test_module
SUBDIRS-$(CONFIG_VM_HACC)     += hacc_test_module
SUBDIRS-$(CONFIG_VM_SDMA_API) += sdma_module
SUBDIRS-$(CONFIG_VM_MXC_WD)   += mxc_wd_module
SUBDIRS-$(CONFIG_VM_MXS_WD)   += mxs_wd_module
SUBDIRS-$(CONFIG_LIST_OP)     += list_entry_module
SUBDIRS-$(CONFIG_MEM_TEST)    += mxc_memory_test

all:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS-y) ; do $(MAKE) -C $$i clean ; done

