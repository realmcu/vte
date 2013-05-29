SUBDIRS-$(CONFIG_VTE_SAHARA) += sahara_testapp
SUBDIRS-$(CONFIG_VTE_SCC)    += scc_testcases  
SUBDIRS-$(CONFIG_VTE_SCC2)   += scc2_testcases    
SUBDIRS-$(CONFIG_VTE_RNG)    += rng_testcase
SUBDIRS-$(CONFIG_VTE_RNGA)   += rnga_testcase  
SUBDIRS-$(CONFIG_VTE_CRYPTO) += crypto_scripts 
SUBDIRS-$(CONFIG_VTE_HWRNG)  += hwrng_test

all:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i clean ; done
