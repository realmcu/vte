#
# vte/lcov
# 
# work in progress...
#

VERSION=$(LBP_VERSION)-$(LBP_PLATFORM)-$(LBP_GUI)

# COV_RESDIR is declared in vte.auomation.env
COV_INFODIR = COVERAGE/$(VERSION)/info
COV_DATADIR = COVERAGE/$(VERSION)/data
COV_HTMLDIR = COVERAGE/$(VERSION)/html

vte_lcov = vte-lcov.sh -vv
vte_html = vte-genhtml.sh

lcov_check_testcases_env = \
@if [ "x$(TESTCASES)" == "x" ]; \
then \
	echo "[ERROR] Usage: make $@ TESTCASES=<regexp>"; \
	exit 1; \
fi
lcov_check_title_env = \
@if [ "x$(TITLE)" == "x" ]; \
then \
	echo "[ERROR] Usage: make $@ TITLE=<TITLE>"; \
	exit 1; \
fi

lcov_info_all:
	make lcov_info TESTCASES='.*'

lcov_info: 
	$(lcov_check_testcases_env)
	list=`cd $(COV_RESDIR) && \ls -v *.gcov.tar | grep '$(TESTCASES)'`; \
	for testcase in $$list; \
	do \
		testcase=$${testcase%%.gcov.tar}; \
		mkdir -p $(COV_INFODIR); \
		mkdir -p $(COV_DATADIR); \
		sudo rm -Rf $(COV_DATADIR)/$$testcase; \
		$(vte_lcov) -S $(LNX_SRCDIR) \
		            -B $(LNX_BLDDIR) \
	        	    -o $(COV_INFODIR) \
		            -d $(COV_DATADIR) \
		            $(COV_RESDIR)/$$testcase.gcov.tar; \
	done

lcov_html: 
	$(lcov_check_testcases_env)
	$(lcov_check_title_env)
	list=`ls -v $(COV_INFODIR)/*.info | grep '$(TESTCASES)'`;\
	mkdir -p $(COV_HTMLDIR)/$(TITLE); \
	$(vte_html) -o $(COV_HTMLDIR)/$(TITLE) \
	            -t  $(VERSION)_$(TITLE) \
	            $$list




lcov_help:
	@echo "* GCOV/LCOV targets:"
	@echo "  =================="
	@echo "  - lcov_info_all: generate .info files for all the captured data found in coverage data repository."
	@echo "  - lcov_info TESTCASES=<regexp>: generate .info files for all testcases that match <regexp>"
	@echo "  - lcov_html TITLE=<title> TESTCASES=<regexp>: generate an HTML report named <title> with info files matching <regexp>"
	@echo "  - lcov_clean: clean away info dir, and data dir."
	@echo "  - lcov_mrproper: lcov_clean + clean away html dir, result.cov dir."
	@echo ""

lcov_clean:
	rm -Rf $(COV_INFODIR)/* $(COV_DATADIR)/*

lcov_mrproper: lcov_clean
	rm -Rf $(COV_HTMLDIR)/* $(COV_RESDIR)/*
