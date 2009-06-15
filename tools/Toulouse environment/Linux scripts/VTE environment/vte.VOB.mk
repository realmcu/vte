#
# mandatory target:
# vte_help vte_ref2estr vte_install_ref vte_install vte_clean vte_mrproper
#

ifneq ($(VTE_VERSION),VOB)
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

vte_clean_vte:
	$(VMAKE) vte_clean

vte_qt_clean:
	$(VMAKE) qt_clean

vte_gtk_clean:
	$(VMAKE) gtk_clean

vte_mm_clean:
	$(VMAKE) mm_clean

vte_clean_ltp:
	$(VMAKE) clean

vte_clean: vte_clean_vte vte_qt_clean vte_gtk_clean vte_mm_clean

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



###############################################################################
#
# CGAG1C (28 juin 2005): 
#   - No more INSTALL dir, the build directory is directly 
#     exported by nfs
#   => all of the above are so commented out
###############################################################################
# ifndef KEEP
# ifndef DEL
# list_bin=ls
# else
# list_bin=ls | grep -v '$(DEL)'
# endif
# else
# list_bin=ls | grep '$(KEEP)'
# endif

# vte_install: vte_build_vte vte_build_qt vte_build_ltp vte_real_install

# vte_install_vte: vte_build_vte vte_real_install

# vte_install_qt: vte_build_qt vte_real_install

# vte_install_gtk: vte_build_gtk vte_real_install

# vte_install_mm: vte_build_mm vte_real_install

# vte_install_ltp: vte_build_ltp vte_real_install

# vte_real_install: vte_core_install $(VTE_BLDDIR)/launch.sh
# #
# #	Install testcases in TESTCASE_DESTDIR
# #
# 	sudo rm -Rf $(VTE_TESTCASE_DESTDIR)
# #	sudo mkdir -p $(VTE_TESTCASE_DESTDIR)
# #	cd $(VTE_BLDDIR)/testcases/bin && list=`$(list_bin)`	\
# #	for f in $$list; do 					\
# #		ln -s $$f $(VTE_TESTCASE_DESTDIR)/$$f; 		\
# #	done
# 	sudo cp -rlp $(VTE_BLDDIR)/testcases/bin $(VTE_TESTCASE_DESTDIR)

# #	Fix some perms 
# #	!!! FIXME: this caus testcases to fail! (test on perms) !!!!
# 	sudo find $(VTE_DESTDIR) -type d -exec chmod o+rx {} \;
# 	sudo find $(VTE_DESTDIR) -type f -exec chmod o+r  {} \;
# 	sudo find $(VTE_TESTCASE_DESTDIR) -type d -exec chmod o+rx {} \;
# 	sudo find $(VTE_TESTCASE_DESTDIR) -type f -exec chmod o+r  {} \;

# # install needed stuff to the ROOTFS
# vte_core_install:
# 	sudo mkdir -p $(VTE_DESTDIR)
# 	cd $(VTE_BLDDIR) && \
# 	sudo cp -rf launch.sh runalltests.sh IDcheck.sh ver_linux runltp runtest/ \
# 		$(VTE_DESTDIR)
# 	sudo mkdir -p $(VTE_DESTDIR)/testcases/bin
# 	make -C $(VTE_BLDDIR)/tools/apicmds $(VTE_MAKEFLAGS)
# 	make -C $(VTE_BLDDIR)/tools/apicmds install $(VTE_MAKEFLAGS)
# 	cd $(VTE_BLDDIR)/testcases/bin && sudo cp -f tst_* $(VTE_DESTDIR)/testcases/bin/
# 	sudo mkdir -p $(VTE_DESTDIR)/pan
# 	cd $(VTE_BLDDIR)/pan && sudo cp -f bump pan $(VTE_DESTDIR)/pan
# ifdef VTE_COVERAGE
# 	cd $(PREFIX)/extravte/ && \
# 	sudo cp lcov-capture.sh lcovize-runtest.sed lcov-reset.sh \
# 		$(VTE_DESTDIR)
# endif


# $(VTE_BLDDIR)/launch.sh:
# 	cp ../extravte/launch.sh $(VTE_BLDDIR)
# 	perl -pi~ -e 's,^FTPDIR=.*$$,FTPDIR=$(LBP_VERSION)-$(LBP_PLATFORM)-$(LBP_GUI),g' $(VTE_BLDDIR)/launch.sh

# vte_build_vte: $(LNX_BLDDIR)/include/linux/autoconf.h
# 	$(VMAKE) vte

# vte_build_qt:
# 	$(VMAKE) qt_tests

# vte_build_gtk:
# 	$(VMAKE) gtk_tests

# vte_build_mm:
# 	$(VMAKE) mm_tests

# vte_build_ltp: $(LNX_BLDDIR)/include/linux/autoconf.h
# 	sudo $(VMAKE) ltp

# vte_build_all: $(LNX_BLDDIR)/include/linux/autoconf.h
# 	$(VMAKE) all
# 	sudo $(VMAKE) install


.PHONY: vte_help vte_ref2estr vte_clean vte_mrproper 
.PHONY: vte_install_ref vte_install vte_build
