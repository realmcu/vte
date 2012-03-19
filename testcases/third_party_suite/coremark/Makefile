SUBDIR = coremark_v1.0

all: P4

P4: P4_build
	ln -f $(SUBDIR)/coremark.exe $(SUBDIR)/coremark_P4.exe

P4_build: F4
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=4 -DUSE_PTHREAD" CC=${CROSS_COMPILE}gcc REBUILD=1 link

F4: F4_build
	ln -f $(SUBDIR)/coremark.exe $(SUBDIR)/coremark_F4.exe

F4_build: P2
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=4 -DUSE_FORK=1" CC=${CROSS_COMPILE}gcc REBUILD=1 link

P2: P2_build
	ln -f $(SUBDIR)/coremark.exe $(SUBDIR)/coremark_P2.exe

P2_build: F2
	 $(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=2 -DUSE_PTHREAD" CC=${CROSS_COMPILE}gcc REBUILD=1 link

F2: F2_build
	ln -f $(SUBDIR)/coremark.exe $(SUBDIR)/coremark_F2.exe

F2_build: P1
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=2 -DUSE_FORK=1" CC=${CROSS_COMPILE}gcc REBUILD=1 link

P1: P1_build
	ln -f $(SUBDIR)/coremark.exe $(SUBDIR)/coremark_P1.exe

P1_build: F1
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=1 -DUSE_PTHREAD" CC=${CROSS_COMPILE}gcc REBUILD=1 link

F1: F1_build
	ln -f $(SUBDIR)/coremark.exe $(SUBDIR)/coremark_F1.exe

F1_build:
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=1 -DUSE_FORK=1" CC=${CROSS_COMPILE}gcc REBUILD=1 link

install:
	ln -f $(SUBDIR)/coremark_P4.exe ../../bin/
	ln -f $(SUBDIR)/coremark_F4.exe ../../bin/
	ln -f $(SUBDIR)/coremark_P2.exe ../../bin/
	ln -f $(SUBDIR)/coremark_F2.exe ../../bin/
	ln -f $(SUBDIR)/coremark_P1.exe ../../bin/
	ln -f $(SUBDIR)/coremark_F1.exe ../../bin/
clean:
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=4 -DUSE_FORK=1" clean
	rm -f $(SUBDIR)/coremark_P4.exe $(SUBDIR)/coremark_F4.exe  $(SUBDIR)/coremark_P2.exe $(SUBDIR)/coremark_F2.exe  $(SUBDIR)/coremark_P1.exe $(SUBDIR)/coremark_F1.exe
