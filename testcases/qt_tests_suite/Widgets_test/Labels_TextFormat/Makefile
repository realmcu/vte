##############################################################################
#
#    @file   Makefile
#
#    @brief  Makefile to build a Qt/e test application
#
##############################################################################
#
#   Copyright (C) 2005, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#
##############################################################################
# Revision History:
#                 Modification     Tracking
# Author             Date          Number     Description of Changes
# -------------   ------------    ----------  ----------------------
# cgag1c           05/04/03       TLSbo48037  New Makefile
# cgag1c           05/0X/07       TLSbo48237  Minor fix in install target
#
##############################################################################
# Portability: 
#
##############################################################################

##############################################################################
# VTE settings
##############################################################################
# VTE top directory
VTE_TOPDIR=../../../../
VTE_BINDIR=$(VTE_TOPDIR)/testcases/bin

##############################################################################
# Qt/e settings
##############################################################################
# Normally set in environment
# QTDIR=
# QTSPEC=

##############################################################################
# Application specific build settings
##############################################################################
EXTRA_INCDIR=
EXTRA_LIBDIR=
EXTRA_LIBS=
EXTRA_DEFS=


##############################################################################
# Internal stuff !!! __DO_NOT_EDIT__ !!!
##############################################################################
QMAKE_CFLAGS   = -I$(VTE_TOPDIR)/include $(EXTRA_INCDIR) $(EXTRA_DEFS)
QMAKE_CXXFLAGS = -I$(VTE_TOPDIR)/include $(EXTRA_INCDIR) $(EXTRA_DEFS)
QMAKE_LFLAGS   = -L$(VTE_TOPDIR)/lib $(EXTRA_LIBDIR) 
QMAKE_LIBS     = -lltp $(EXTRA_LIBS)
QMAKE          = $(QTDIR)/bin/qmake
QMAKE_GENMAKE_FLAGS = 	-after \
			QMAKE_CFLAGS+="$(QMAKE_CFLAGS)" \
			QMAKE_CXXFLAGS+="$(QMAKE_CXXFLAGS)" \
			QMAKE_LFLAGS+="$(QMAKE_LFLAGS)" \
			QMAKE_LIBS+="$(QMAKE_LIBS)"
QMAKE_GENMAKE  = $(QMAKE) -spec $(QTSPEC) $(QMAKE_GENMAKE_FLAGS)

ifeq (,$(QTSPEC))
$(error No QTSPEC env var, please fix.)
endif
ifeq (,$(QTDIR))
$(error No QTDIR env var, please fix.)
endif


TARGET=$(shell basename $$PWD)

default: Makefile.qt
	make -f Makefile.qt

init: Makefile.qt

Makefile.qt: $(TARGET).pro
	$(QMAKE_GENMAKE) -o $@ $<

clean:
	-make -f Makefile.qt clean
	rm -f Makefile.qt


install:
	ln -f $(TARGET) $(VTE_BINDIR)
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)              **"
	@echo "***********************************************"
	@echo


uninstall:
	rm -f $(VTE_BINDIR)/$(TARGET)
	@echo "***********************************************"
	@echo "** UNINSTALL : $(TARGET)              **"
	@echo "***********************************************"
	@echo

