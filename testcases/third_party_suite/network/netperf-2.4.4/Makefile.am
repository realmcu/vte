
AUTOMAKE_OPTIONS = dist-bzip2 dist-zip
SUBDIRS = src doc
EXTRA_DIST = README.* Release_Notes inet_ntop.c

install:
	ln -f src/netperf ../../../bin/netperf
	ln -f doc/examples/* ../../../bin/
