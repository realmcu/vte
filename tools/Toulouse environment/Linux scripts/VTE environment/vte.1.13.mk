#
# mandatory target:
# vte_help vte_ref2estr vte_install_ref vte_install vte_clean vte_mrproper
#

ifneq ($(VTE_VERSION),1.14)
$(error VTE version mismatch)
endif

VMAKE=make -C $(VTE_BLDDIR) $(VTE_MAKEFLAGS)


vte_help:
	@echo "* VTE targets:"
	@echo "  ============"
	@echo "  - vte_ref2estr:"
	@echo "  - vte_install,"
	@echo "    vte_install_vte,"
	@echo "    vte_install_qt,"
	@echo "    vte_install_gtk,"
	@echo "    vte_install_ltp:"
	@echo "      Install all testsuites, vte testsuite, qt_testsuite or ltp testsuite(s)"
	@echo "      You can also pass to make: KEEP='<REG_EXP>' DEL='<REG_EXP>'"
	@echo "      If KEEP is passed then install only the files from"
	@echo "      testcase/bin that match <REG_EXP>,"
	@echo "      if DEL is passed then don't install the files from"
	@echo "      testcase/bin that match <REG_EXP>"
	@echo "  - vte_clean_all:"
	@echo "  - vte_mrproper:"
	@echo ""

vte_ref2estr:
	rm -Rf $(VTE_BLDDIR)
	cp -rL $(VTE_REFDIR) $(VTE_BLDDIR)
	perl -pi~ -e \"s,^export ARCH_CPU=.*$,export ARCH_CPU=$(LNX_ARCH),\" $(VTE_BLDDIR)/Makefile
	perl -pi~ -e \"s,^export KLINUX_SRCDIR=.*$,export KLINUX_SRCDIR=$(LNX_SRCDIR),\" $(VTE_BLDDIR)/Makefile
	perl -pi~ -e \"s,^export KLINUX_BLTDIR=.*$,export KLINUX_BLTDIR=$(LNX_BLDDIR),\" $(VTE_BLDDIR)/Makefile


vte_clean_all:
	$(VMAKE) clean vte_clean qt_clean gtk_clean mm_clean

vte_mrproper: vte_clean
	@echo "$@: Not yet implemented"


vte_build:
	$(VMAKE) all
vte_install:  vte_install_core
	$(VMAKE) install

vte_build_ltp:
	$(VMAKE) ltp

vte_install_ltp: vte_build_ltp vte_install_core

vte_build_vte: $(LNX_BLDDIR)/include/linux/autoconf.h
	$(VMAKE) vte

vte_install_vte: vte_build_vte vte_install_core

vte_build_qt: $(LNX_BLDDIR)/include/linux/autoconf.h
	$(VMAKE) qt_tests

vte_install_qt: vte_build_qt vte_install_core

vte_build_gtk: $(LNX_BLDDIR)/include/linux/autoconf.h
	$(VMAKE) gtk_tests

vte_install_gtk: vte_build_gtk vte_install_core

vte_build_mm: $(LNX_BLDDIR)/include/linux/autoconf.h
	$(VMAKE) mm_tests

vte_install_mm: vte_build_mm

vte_install_core: 
	make -C $(VTE_BLDDIR)/tools/apicmds $(VTE_MAKEFLAGS)
	make -C $(VTE_BLDDIR)/tools/apicmds install $(VTE_MAKEFLAGS)
ifdef VTE_COVERAGE
	cd $(PREFIX)/extravte/ && \
	sudo cp lcov-capture.sh lcovize-runtest.sed lcov-reset.sh \
		$(VTE_DESTDIR)
endif


.PHONY: vte_help vte_ref2estr vte_clean vte_mrproper 
.PHONY: vte_install_ref vte_install vte_build
