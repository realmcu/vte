###############################################################################
#
#    @file   Makefile
#
#    @brief  dt.
#
##############################################################################

SUBDIRS = i2c-tools-3.0.3
TARGETS = i2cdetect i2cdump i2cget i2cset

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i CC=$(CROSS_COMPILER)gcc; done

install:
	- for i in $(TARGETS);do ln -f i2c-tools-3.0.3/tools/$$i ../../bin/ ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
