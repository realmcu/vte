#Copyright 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
CFLAGS+= -Wall -I../../../../include
LDFLAGS+= -L../../../../lib -lltp

TARGETS=fs_perms testx.file test.txt
TARGETS+=fs_perms_list.sh testx
all: fs_perms testx.file testx
fs_perms: fs_perms.o
	$(CC) fs_perms.o -o fs_perms $(CFLAGS) $(LDFLAGS)
testx.file: testx.o
	$(CC) testx.o -o testx.file $(CFLAGS) $(LDFLAGS)
testx: testx_r0.c
	$(CC) testx_r0.c -o testx $(CFLAGS) $(LDFLAGS)
text.txt:
	;
install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i ../../../bin/$$i ; done
fs_perms.o: fs_perms.c
	$(CC) -c fs_perms.c $(CFLAGS) 
testx.o: testx.c
	$(CC) -c testx.c $(CFLAGS) 
clean:
	rm -f *.o fs_perms testx.file
