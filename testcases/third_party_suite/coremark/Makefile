SUBDIR = coremark_v1.0

all: P4

P4: P4_build
	ln -f $(SUBDIR)/coremark.exe $(SUBDIR)/coremark_P4.exe

P4_build: F4
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=4 -DUSE_PTHREAD" CC=${CROSS_COMPILER}gcc REBUILD=1 link

F4: F4_build
	ln -f $(SUBDIR)/coremark.exe $(SUBDIR)/coremark_F4.exe

F4_build :
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=4 -DUSE_FORK=1" CC=${CROSS_COMPILER}gcc REBUILD=1 link

install:
	ln -f $(SUBDIR)/coremark_P4.exe ../../bin/
	ln -f $(SUBDIR)/coremark_F4.exe ../../bin/

clean:
	$(MAKE) -C $(SUBDIR) PORT_DIR=linux XCFLAGS="-DMULTITHREAD=4 -DUSE_FORK=1" clean
	rm -f $(SUBDIR)/coremark_P4.exe $(SUBDIR)/coremark_F4.exe
