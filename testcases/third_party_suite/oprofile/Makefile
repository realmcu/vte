###############################################################################
#
#    @file   Makefile
#
#    @brief  dt.
#
##############################################################################

SUBDIRS = oprofile-0.9.6
LIBRARIES = bfd popt-1.7
LIBRARIES_DIR = ../../../openlibs


all: config
	$(MAKE) -C $(SUBDIRS)
install:
	ln -f $(SUBDIRS)/utils/opcontrol ../../bin/
clean:
	-$(MAKE) -C $(SUBDIRS) distclean
	-$(MAKE) -C $(LIBRARIES_DIR)/popt-1.7 distclean
	-$(MAKE) -C $(LIBRARIES_DIR)/binutils-2.19.51.0.12/bfd distclean
	$(MAKE) -C $(LIBRARIES_DIR)/binutils-2.19.51.0.12/libiberty distclean
	

config: libs
	cd $(SUBDIRS); ./myconfig

libs:
	cd $(LIBRARIES_DIR)/binutils-2.19.51.0.12/bfd ; ./myconfig
	$(MAKE) -C $(LIBRARIES_DIR)/binutils-2.19.51.0.12/bfd
	$(MAKE) -C $(LIBRARIES_DIR)/binutils-2.19.51.0.12/bfd install
	cd $(LIBRARIES_DIR)/popt-1.7 ; ./myconfig
	$(MAKE) -C $(LIBRARIES_DIR)/popt-1.7
	$(MAKE) -C $(LIBRARIES_DIR)/popt-1.7 install
	cd $(LIBRARIES_DIR)/binutils-2.19.51.0.12/libiberty ; ./myconfig
	$(MAKE) -C $(LIBRARIES_DIR)/binutils-2.19.51.0.12/libiberty
	$(MAKE) -C $(LIBRARIES_DIR)/binutils-2.19.51.0.12/libiberty install
