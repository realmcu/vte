# To cross compile, override one or more of CC, AR, CROSS_CFLAGS,
# LOADLIBES, LDFLAGS, & LIB_DIR and be sure to always build from the top level.
#
# To override them from the make commandline, do something like this:
# $ CROSS_COMPILER=/opt/cegl-1.4/hardhat/devkit/ppc/405/bin/powerpc-linux-
# $ make \
#     CROSS_CFLAGS="-mcpu=403 -D__PPC405__" \
#     CC=${CROSS_COMPILER}gcc \
#     AR=${CROSS_COMPILER}ar \
#     LDFLAGS=-static \
#     LOADLIBES="-lpthread -lc -lresolv -lnss_dns -lnss_files -lm -lc"
#
# Alternately, to override them by editing this file, uncomment the 
# following lines:
#   CROSS_COMPILER=/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-
   CROSS_CFLAGS=-I$(KLINUX_BLTDIR)/include
   CC=$(CROSS_COMPILER)gcc
   AR=$(CROSS_COMPILER)ar 
   LDFLAGS=-static
   LOADLIBES=
   LIB_DIR=
   export CC AR LDFLAGS LOADLIBES LIB_DIR
#   export CROSS_COMPILER CC AR LDFLAGS LOADLIBES LIB_DIR
   export CC AR LDFLAGS LOADLIBES LIB_DIR
#
# Note: If you override a variable from the commandline all
# assignments to it in the Makefiles will be ignored. To set it both 
# in the commandline and in the Makefiles use a dummy variable like in
# CFLAGS

#tina add
#	ifeq ($(LPDK), 1)
#		CFLAGS+= -DPROJECT_LPDK=1
#	endif
#	ifeq ($(MARLEY), 1)
#		CFLAGS+= -DPROJECT_MX37=1	
#	endif
#	ifeq ($(RINGO), 1)
#		CFLAGS+= -DPROJECT_MX35=1
#	endif
#	ifeq ($(mx51), 1)
#		CFLAGS+= -DPROJECT_MX51=1	
#	endif
#end
export CFLAGS+=-Wall $(CROSS_CFLAGS)

#by Spring, 2009.2.6, config.mk config.h is added by ltp_081231
-include config.mk

VPATH += include m4

all: config.h config.mk libltp.a 
	@$(MAKE) -C pan $@
	@$(MAKE) -C testcases $@
	@$(MAKE) -C tools $@
	@echo
	@echo "***********************************************"
	@echo "** You now need to do a make install as root **"
	@echo "***********************************************"

install: all
	@$(MAKE) -C testcases install
	@$(MAKE) -C tools install
	@./IDcheck.sh

libltp.a: config.h
	@echo
	@echo "***********************************************"
	@echo "** MAKE - Library LTP : libltp.a             **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C lib $@

pandir:
	@echo
	@echo "***********************************************"
	@echo "** MAKE - LTP test environment : pan         **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C pan all
	-mkdir testcases/bin
	-ln -f pan/pan testcases/bin/

tools: libltp.a
	@echo
	@echo "***********************************************"
	@echo "** MAKE - LTP test environment : tools       **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C tools all
	@mkdir -p testcases/bin
	@$(MAKE) -C tools install

menuconfig:
	@./ltpmenu

clean:
	@$(MAKE) -C lib $@
	@$(MAKE) -C pan $@
	@$(MAKE) -C tools $@
	@$(MAKE) -C openlibs $@
	@$(MAKE) -C testcases $@

binclean:
	-rm -rf ./testcases/bin

config.h: config.h.default
	cp include/config.h.default include/config.h
config.mk:
	touch $@

ltp: libltp.a pandir tools config.h config.mk
	#@touch include/config.h.default
	#@$(MAKE) config.h
	@echo
	@echo "***********************************************"
	@echo "** MAKE ALL - Kernel LTP tests suite         **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/kernel all
	@echo
	@echo "***********************************************"
	@echo "** MAKE INSTALL - Kernel LTP tests suite     **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/kernel install
	@echo
	@echo "***********************************************"
	@echo "** LTP Kernel tests suite is available       **"
	@echo "***********************************************"
	@echo
	@echo "***********************************************"
	@echo "** MAKE ALL - Network LTP tests suite        **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/network all
	@echo
	@echo "***********************************************"
	@echo "** MAKE INSTALL - Kernel LTP tests suite     **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/network install
	@echo
	@echo "***********************************************"
	@echo "** LTP Network tests suite is available      **"
	@echo "***********************************************"

	@echo "** MAKE ALL - third party suite               **"

vte: libltp.a pandir tools
	@echo "***********************************************"
	@echo "** MAKE ALL - VTE tests suite                **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/vte_tests_suite all
	@$(MAKE) -C testcases/module_test all
	@echo
	@echo "***********************************************"
	@echo "** MAKE INSTALL - VTE tests suite            **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/vte_tests_suite install
	@$(MAKE) -C testcases/module_test install
	@echo
	@echo "***********************************************"
	@echo "** VTE tests suite is available              **"
	@echo "***********************************************"
	@echo
	@echo "***********************************************"
	@echo "**        build Finished                     **"
	@echo "***********************************************"
apps:
	@$(MAKE) -C testcases/third_party_suite all
	@$(MAKE) -C testcases/third_party_suite install
	@echo
	@echo "***********************************************"
	@echo "**        apps build Finished                **"
	@echo "***********************************************"

unit: libltp.a pandir tools
	@echo
	@echo "***********************************************"
	@echo "** MAKE ALL - unit tests suite               **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/unit_tests_suite all
	@echo
	@echo "***********************************************"
	@echo "** MAKE INSTALL - unit tests suite           **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/unit_tests_suite install
	@echo
	@echo "***********************************************"
	@echo "** unit tests suite is available             **"
	@echo "***********************************************"

qt_tests: libltp.a pandir
	@echo
	@echo "***********************************************"
	@echo "** MAKE ALL - QT tests suite                 **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/qt_tests_suite all
	@echo
	@echo "***********************************************"
	@echo "** MAKE INSTALL - QT tests suite             **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/qt_tests_suite install
	@echo
	@echo "***********************************************"
	@echo "** QT tests suite is available               **"
	@echo "***********************************************"

gtk_tests: libltp.a pandir
	@echo
	@echo "***********************************************"
	@echo "** MAKE ALL - GTK tests suite                **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/gtk_tests_suite all
	@echo
	@echo "***********************************************"
	@echo "** MAKE INSTALL - GTK tests suite            **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/gtk_tests_suite install
	@echo
	@echo "***********************************************"
	@echo "** GTK tests suite is available              **"
	@echo "***********************************************"

mm_tests: libltp.a pandir
	@echo
	@echo "***********************************************"
	@echo "** MAKE ALL - MM tests suite                 **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/mm_tests_suite all
	@echo
	@echo "***********************************************"
	@echo "** MAKE INSTALL - MM tests suite             **"
	@echo "***********************************************"
	@echo
	@$(MAKE) -C testcases/mm_tests_suite install
	@echo
	@echo "***********************************************"
	@echo "** MM tests suite is available               **"
	@echo "***********************************************"

unit_clean:
	@$(MAKE) -C testcases/unit_tests_suite clean

vte_clean:
	@$(MAKE) -C testcases/vte_tests_suite clean

qt_clean:
	@$(MAKE) -C testcases/qt_tests_suite clean

gtk_clean:
	@$(MAKE) -C testcases/gtk_tests_suite clean

mm_clean:
	$(MAKE) -C testcases/mm_tests_suite clean

oplib_clean:
	-$(MAKE) -C openlibs clean

vte_clean_all: vte_clean qt_clean gtk_clean mm_clean unit_clean oplib_clean

