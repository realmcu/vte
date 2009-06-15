#
# mandatory target:
# vte_help vte_ref2estr vte_install_ref vte_install vte_clean vte_mrproper
#

ifneq ($(VTE_VERSION),1.10)
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
	@echo "  - vte_clean:"
	@echo "  - vte_mrproper:"
	@echo ""

vte_ref2estr: vte_mrproper
	@echo "$@: Not yet implemented"

vte_clean:
	$(VMAKE) clean vte_clean gtk_clean mm_clean

vte_mrproper: vte_clean
	@echo "$@: Not yet implemented"

ifndef KEEP
ifndef DEL
list_bin=ls
else
list_bin=ls | grep -v '$(DEL)'
endif
else
list_bin=ls | grep '$(KEEP)'
endif

vte_install: vte_build_vte vte_build_qt vte_build_ltp vte_real_install

vte_install_vte: vte_build_vte vte_real_install

vte_install_qt: vte_build_qt vte_real_install

vte_install_gtk: vte_build_gtk vte_real_install

vte_install_mm: vte_build_mm vte_real_install

vte_install_ltp: vte_build_ltp vte_real_install

vte_real_install: vte_core_install
#	#sudo rm -Rf $(VTE_TESTCASE_DESTDIR)/*
#	#sudo: unable to exec /bin/rm: Argument list too long	
	#sudo rm -Rf $(VTE_TESTCASE_DESTDIR)
	#sudo mkdir $(VTE_TESTCASE_DESTDIR)
	#cd $(VTE_BLDDIR)/testcases/bin && list=`$(list_bin)` && \
	#sudo cp -pdR $$list $(VTE_TESTCASE_DESTDIR)
#	# Fix some perms
#	# FIXME: This break some tests!
	#sudo find $(VTE_DESTDIR) -type d -exec chmod a+rx {} \;
	#sudo find $(VTE_DESTDIR) -type f -exec chmod a+r  {} \;
	#sudo find $(VTE_TESTCASE_DESTDIR) -type d -exec chmod a+rx {} \;
	#sudo find $(VTE_TESTCASE_DESTDIR) -type f -exec chmod a+r  {} \;

# install needed stuff to the ROOTFS
vte_core_install:
	#sudo mkdir -p $(VTE_DESTDIR)
	#cd $(VTE_BLDDIR) && \
	#sudo cp -Rf launch.sh runalltests.sh IDcheck.sh ver_linux runltp runtest/ \
	#	$(VTE_DESTDIR)
ifdef VTE_COVERAGE
	cd $(VTE_BLDDIR)/tools/lcov && \
	sudo cp lcov-capture.sh lcovize-runtest.sed lcov-reset.sh \
		$(VTE_DESTDIR)
endif
	#sudo mkdir -p $(VTE_DESTDIR)/pan
	#cd $(VTE_BLDDIR)/pan && sudo cp -f bump pan $(VTE_DESTDIR)/pan
#	# For test scripts
	#make -C $(VTE_BLDDIR)/tools/apicmds $(VTE_MAKEFLAGS)
	#make -C $(VTE_BLDDIR)/tools/apicmds install $(VTE_MAKEFLAGS)
	#cd $(VTE_BLDDIR)/testcases/bin && sudo cp -f tst_* $(VTE_DESTDIR)/testcases/bin/


vte_build_vte: $(LNX_BLDDIR)/include/linux/autoconf.h
	$(VMAKE) vte

vte_build_qt:
	$(VMAKE) qt_tests

vte_build_gtk:
	$(VMAKE) gtk_tests

vte_build_mm:
	$(VMAKE) mm_tests

vte_build_ltp: $(LNX_BLDDIR)/include/linux/autoconf.h
	#sudo $(VMAKE) ltp
	$(VMAKE) ltp

vte_build_all: $(LNX_BLDDIR)/include/linux/autoconf.h
	$(VMAKE) all
	#sudo $(VMAKE) install
	$(VMAKE) install


.PHONY: vte_help vte_ref2estr vte_clean vte_mrproper 
.PHONY: vte_install_ref vte_install vte_build
