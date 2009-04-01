#
# %Z%%M% %I% %E% 1990 by Robin Miller
#
#/****************************************************************************
# *									     *
# *			  COPYRIGHT (c) 1990 - 1999			     *
# *			   This Software Provided			     *
# *				     By					     *
# *			  Robin's Nest Software Inc.			     *
# *			       2 Pardise Lane     			     *
# *			      Hudson, NH 03051				     *
# *			       (603) 883-2355				     *
# *									     *
# * Permission to use, copy, modify, distribute and sell this software and   *
# * its documentation for any purpose and without fee is hereby granted	     *
# * provided that the above copyright notice appear in all copies and that   *
# * both that copyright notice and this permission notice appear in the	     *
# * supporting documentation, and that the name of the author not be used    *
# * in advertising or publicity pertaining to distribution of the software   *
# * without specific, written prior permission.				     *
# *									     *
# * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 	     *
# * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN	     *
# * NO EVENT SHALL HE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL   *
# * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR    *
# * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS  *
# * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF   *
# * THIS SOFTWARE.							     *
# *									     *
# ****************************************************************************/
#
# Makefile -- makefile for program 'dt'
#	

### MKMF:DEFINITIONS ###


# System makefile definitions for program makefiles

.SUFFIXES:	.ln

.c.ln:
#		@lint -i $(LINTFLAGS) $<
		@lint -c $(LINTFLAGS) $<

#.c~.ln:
#		@echo $<
#		@$(GET) -G$*.c $(GFLAGS) $<
#		@lint -i $(LINTFLAGS) $*.c
#		@lint -c $(LINTFLAGS) $*.c
#		@rm -f $*.c

PRINTER=	lpr

PR_FLAGS=	-p -P28

TR_PRINTER=	$(PRINTER)

TR_FLAGS=	-t -P28

# Libraries the program links to which are considered volatile

LIBS=

# Libraries considered static

#
# If desired, AIO is possible to file systems, but you must use
# shared aio and pthread libraries and remove -non_shared below.
#
# NOTE:  My tests showed slower performance (at least with ufs).
#
EXTLIBS=

LINTLIBS=

# P or G flag ( override on command line by invoking make PORG=-g )

PORG=	-O

AWK=	awk
#
# Note: Assumption made that _FILE_OFFSET_BITS=64 enables O_LARGEFILE!
#
CFLAGS= $(PORG) -Kthread -DAIO -DMMAP -DFIFO -DRAND48 -DSCO -D_FILE_OFFSET_BITS=64
#CPP=	/lib/cpp
#
# 'gcc' is used for "make depend", since SCO's compiler has no '-M' option :-)
#
CPP=	/usr/local/bin/gcc
CPPOPTS= -M -DAIO -DMMAP -DFIFO -DRAND48 -DSCO -D_FILE_OFFSET_BITS=64
#
# Note:  Threads library requires 'dt' to be built shared (dynamic libraries)!
#
LDFLAGS= 
#LDFLAGS= -Bstatic
LINTFLAGS=

# end of system makefile definitions


HDRS=		dt.h

### MKMF:SOURCES ###


CFILES=		\
    		dt.c		\
		dtgen.c		\
		dtinfo.c	\
    		dtread.c	\
    		dtwrite.c	\
		dtstats.c	\
		dttty.c		\
    		dtutil.c	\
		dtusage.c	\
		dtprocs.c	\
		dttape.c	\
		dtaio.c		\
		dtmmap.c	\
		dtfifo.c


### MKMF:OBJECTS ###

OBJS=		${CFILES:.c=.o}


### MKMF:LINTOBJS ###

LINTOBJS=	${CFILES:.c=.ln}


### MKMF:TARGETS ###

PROGRAM=	dt


# system targets for program makefile


$(PROGRAM):	$(OBJS) $(XOBJS) $(LIBS)
#		@echo -n loading $(PROGRAM) ... 
		$(CC) -o $(PROGRAM) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) $(EXTLIBS)
		@echo done

print:;
		@$(PRINTER) $(PRINTFLAGS) $(CFILES)

prettyprint:;
		@vgrind $(CFILES) | $(TROFFPRINTER) $(TROFFPRINTERFLAGS)

lint:		$(LINTOBJS)
		lint $(LINTFLAGS) $(LINTOBJS) $(LINTLIBS)
		touch lint

clean:;
		@rm -f $(OBJS) $(PROGRAM)

tags:	$(CFILES) $(HDRS)
	ctags -wt $(CFILES) $(HDRS)

# end of system targets for program makefile


depend: makedep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ex - Makefile < eddep
	rm eddep makedep makedep1 makedeperrs

# Kinda Micky Mouse, but it's better than nothing (IMHO) :-)
makedep: ${CFILES}
	@cat /dev/null >makedep
	-(for i in ${CFILES} ; do \
		${CPP} ${CPPOPTS} $$i >> makedep; done) \
		2>makedeperrs
	sed \
		-e 's,\/local\/lib\/gcc-lib/\(.*\)\/include,\/include,' \
		-e 's,^\(.*\)\.o:,\1.o \1.ln:,' makedep > makedep1
	${AWK} ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$3) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$3 } } \
	      END { print rec } ' makedep1 > makedep
	@cat makedeperrs
	@(if [ -s makedeperrs ]; then false; fi)


# DO NOT DELETE THIS LINE

dt.o dt.ln: dt.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h \
 /usr/include/ctype.h \
 /usr/include/signal.h \
 /usr/include/sys/signal.h \
 /usr/include/sys/procset.h /usr/include/sys/ioctl.h \
 /usr/include/sys/file.h /usr/include/sys/wait.h
dtgen.o dtgen.ln: dtgen.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h /usr/include/sys/file.h
dtinfo.o dtinfo.ln: dtinfo.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h /usr/include/sys/ioctl.h
dtread.o dtread.ln: dtread.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h /usr/include/sys/file.h
dtwrite.o dtwrite.ln: dtwrite.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h /usr/include/sys/file.h
dtstats.o dtstats.ln: dtstats.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h
dttty.o dttty.ln: dttty.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h /usr/include/sys/ioctl.h \
 /usr/include/sys/file.h
dtutil.o dtutil.ln: dtutil.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h \
 /usr/include/ctype.h \
 /usr/include/stdarg.h
dtusage.o dtusage.ln: dtusage.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h
dtprocs.o dtprocs.ln: dtprocs.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h \
 /usr/include/signal.h \
 /usr/include/sys/signal.h \
 /usr/include/sys/procset.h /usr/include/sys/wait.h
dttape.o dttape.ln: dttape.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h /usr/include/sys/ioctl.h \
 /usr/include/sys/tape.h
dtaio.o dtaio.ln: dtaio.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h \
 /usr/include/limits.h
dtmmap.o dtmmap.ln: dtmmap.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h \
 /usr/include/limits.h \
 /usr/include/sys/mman.h
dtfifo.o dtfifo.ln: dtfifo.c dt.h \
 /usr/include/stdio.h \
 /usr/include/sys/types.h \
 /usr/include/sys/select.h \
 /usr/include/stdlib.h \
 /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/fcntl.h \
 /usr/include/termios.h /usr/include/sys/termios.h \
 /usr/include/time.h \
 /usr/include/unistd.h /usr/include/sys/unistd.h \
 /usr/include/math.h \
 /usr/include/errno.h /usr/include/sys/errno.h \
 /usr/include/sys/times.h \
 /usr/include/sys/stat.h \
 /usr/include/sys/time.h \
 /usr/include/sys/param.h \
 /usr/include/sys/param_p.h /usr/include/aio.h /usr/include/siginfo.h \
 /usr/include/sys/siginfo.h
