#
# %Z%%M% %I% %E% 1990 by Robin Miller
#
#/****************************************************************************
# *									     *
# *			  COPYRIGHT (c) 1990 - 1999			     *
# *			   This Software Provided			     *
# *				     By					     *
# *			  Robin's Nest Software Inc.			     *
# *			       2 Paradise Lane  	     *
# *			       Hudson, NH 03051				     *
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
# * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,      *
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

# System makefile definitions for program makefiles

.SUFFIXES:	.ln

.c.ln:
		@lint -c $(LINTFLAGS) $<

.c~.ln:
		@echo $<
		@$(GET) -G$*.c $(GFLAGS) $<
		@lint -c $(LINTFLAGS) $*.c
		@rm -f $*.c

PRINTER=	lpr

PR_FLAGS=	-p -P28

TR_PRINTER=	$(PRINTER)

TR_FLAGS=	-t -P28

# Libraries the program links to which are considered volatile

LIBS=

# Libraries considered static

EXTLIBS=
#
# This library is necessary for AIO functions, but it is being
# omitted (for now), since AIO isn't working quite right (yet)!
# [ Note: I'm pretty sure the AIO failures are not 'dt's fault. ]
# BTW:  The failure is showing up as a data compare error, when
#	32-bit file offsets are used, or we get a premature EOF
#	when _FILE_OFFSET_BITS=64 is defined :-) damn!
#
#EXTLIBS= -lrt

LINTLIBS=

# P or G flag ( override on command line by invoking make PORG=-g )

#PORG=	-Otail -mc
#PORG=	-Otail -mf -w3
#PORG=	-Otail -mf
PORG      = -ggdb -Wall\
                          -Waggregate-return\
                          -Wcast-align\
                          -Wimplicit\
                          -Wmissing-prototypes\
                          -Wnested-externs\
                          -Wno-parentheses

# Include these (perhaps?) after code gets cleaned up...
#                         -Wshadow\
#			  -Wstrict-prototypes \
#                         -Wwrite-strings

AWK=	awk
CFLAGS= $(PORG) -DFIFO -DMMAP -D__linux__ -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64
#
# Linux AIO isn't working with _FILE_OFFSET_BITS=64 defined.
# NOTE: To enable AIO, define EXTLIBS above, and add dtaio.c below.
#
#CFLAGS= $(PORG) -DAIO -DFIFO -DMMAP -D__linux__ -D_GNU_SOURCE
CPP=	/lib/cpp
CPPOPTS= -D__GNUC__ -DFIFO -DMMAP -D__linux__ -D_GNU_SOURCE
#CPPOPTS= -P
LDFLAGS= -static

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
		dtmmap.c	\
		dtfifo.c	\
		dttape.c


### MKMF:OBJECTS ###

OBJS=		${CFILES:.c=.o}


### MKMF:LINTOBJS ###

LINTOBJS=	${CFILES:.c=.ln}


PROGRAM=	dt

# system targets for program makefile

$(PROGRAM):	$(OBJS) $(XOBJS) $(LIBS)
		@echo -n loading $(PROGRAM) ...
		@$(CC) -o $(PROGRAM) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) $(EXTLIBS)
		@echo done

print:;
		@$(PRINTER) $(PRINTFLAGS) $(CFILES)

prettyprint:;
		@vgrind $(CFILES) | $(TROFFPRINTER) $(TROFFPRINTERFLAGS)

lint:		$(LINTOBJS)
		lint $(LINTFLAGS) $(LINTOBJS) $(LINTLIBS)
		touch lint
all:
	make depend
	make


BIN_DIR=../../..
install:
	ln -f dt $(BIN_DIR)/bin


clean:;
		@rm -f $(OBJS) $(PROGRAM)

tags:	$(CFILES) $(HDRS)
	ctags $(CFILES) $(HDRS)
	etags $(CFILES) $(HDRS)

# end of system targets for program makefile

depend: makedep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep makedep1 makedeperrs

makedep: ${CFILES}
	@cat /dev/null >makedep
	-(for i in ${CFILES} ; do \
		${CPP} -M ${CPPOPTS} $$i >> makedep; done) \
		2>makedeperrs
	sed \
		-e 's,^.*genassym\.o:,assym.s assym.L:,' \
		-e 's,^\(.*\)\.o:,\1.o \1.ln:,' makedep > makedep1
	${AWK} ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$3) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$3 } } \
	      END { print rec } ' makedep1 > makedep
	@cat makedeperrs
	@(if [ -s makedeperrs ]; then false; fi)


# DO NOT DELETE THIS LINE

#dt.o dt.ln: dt.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/ctype.h /usr/include/signal.h \
#  /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
#  /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
#  /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
#  /usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
#  /usr/include/sys/ioctl.h /usr/include/bits/ioctls.h \
#  /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h \
#  /usr/include/bits/ioctl-types.h /usr/include/sys/file.h \
#  /usr/include/sys/param.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/limits.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/syslimits.h \
#  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
#  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
#  /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h \
#  /usr/include/linux/param.h /usr/include/asm/param.h \
#  /usr/include/sys/wait.h /usr/include/sys/resource.h \
#  /usr/include/bits/resource.h
#dtgen.o dtgen.ln: dtgen.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/sys/file.h
#dtinfo.o dtinfo.ln: dtinfo.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/sys/ioctl.h \
#  /usr/include/bits/ioctls.h /usr/include/asm/ioctls.h \
#  /usr/include/asm/ioctl.h /usr/include/bits/ioctl-types.h
#dtread.o dtread.ln: dtread.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/sys/file.h
#dtwrite.o dtwrite.ln: dtwrite.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/sys/file.h
#dtstats.o dtstats.ln: dtstats.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h
#dttty.o dttty.ln: dttty.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/sys/ioctl.h \
#  /usr/include/bits/ioctls.h /usr/include/asm/ioctls.h \
#  /usr/include/asm/ioctl.h /usr/include/bits/ioctl-types.h \
#  /usr/include/sys/file.h /usr/include/sys/param.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/limits.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/syslimits.h \
#  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
#  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
#  /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h \
#  /usr/include/linux/param.h /usr/include/asm/param.h
#dtutil.o dtutil.ln: dtutil.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/ctype.h
#dtusage.o dtusage.ln: dtusage.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h
#dtprocs.o dtprocs.ln: dtprocs.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/signal.h \
#  /usr/include/bits/signum.h /usr/include/bits/siginfo.h \
#  /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h \
#  /usr/include/asm/sigcontext.h /usr/include/bits/sigstack.h \
#  /usr/include/sys/ucontext.h /usr/include/bits/sigthread.h \
#  /usr/include/sys/wait.h /usr/include/sys/resource.h \
#  /usr/include/bits/resource.h
#dtmmap.o dtmmap.ln: dtmmap.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/limits.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/syslimits.h \
#  /usr/include/limits.h /usr/include/bits/posix1_lim.h \
#  /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
#  /usr/include/bits/posix2_lim.h /usr/include/bits/xopen_lim.h \
#  /usr/include/sys/mman.h /usr/include/bits/mman.h
#dtfifo.o dtfifo.ln: dtfifo.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h
#dttape.o dttape.ln: dttape.c dt.h /usr/include/stdio.h /usr/include/features.h \
#  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
#  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stddef.h \
#  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
#  /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h \
#  /usr/include/bits/wchar.h /usr/include/gconv.h \
#  /usr/lib/gcc/i386-redhat-linux/4.1.0/include/stdarg.h \
#  /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h \
#  /usr/include/stdlib.h /usr/include/bits/waitflags.h \
#  /usr/include/bits/waitstatus.h /usr/include/endian.h \
#  /usr/include/bits/endian.h /usr/include/xlocale.h \
#  /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
#  /usr/include/bits/select.h /usr/include/bits/sigset.h \
#  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
#  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
#  /usr/include/string.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
#  /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/termios.h \
#  /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h \
#  /usr/include/unistd.h /usr/include/bits/posix_opt.h \
#  /usr/include/bits/environments.h /usr/include/bits/confname.h \
#  /usr/include/getopt.h /usr/include/math.h /usr/include/bits/huge_val.h \
#  /usr/include/bits/huge_valf.h /usr/include/bits/huge_vall.h \
#  /usr/include/bits/inf.h /usr/include/bits/nan.h \
#  /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
#  /usr/include/errno.h /usr/include/bits/errno.h \
#  /usr/include/linux/errno.h /usr/include/asm/errno.h \
#  /usr/include/sys/times.h /usr/include/sys/ioctl.h \
#  /usr/include/bits/ioctls.h /usr/include/asm/ioctls.h \
#  /usr/include/asm/ioctl.h /usr/include/bits/ioctl-types.h \
#  /usr/include/sys/mtio.h
