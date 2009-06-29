/****************************************************************************
 *									    *
 *			  COPYRIGHT (c) 1990 - 2004			    *
 *			   This Software Provided			    *
 *				     By					    *
 *			  Robin's Nest Software Inc.			    *
 *									    *
 * Permission to use, copy, modify, distribute and sell this software and   *
 * its documentation for any purpose and without fee is hereby granted,	    *
 * provided that the above copyright notice appear in all copies and that   *
 * both that copyright notice and this permission notice appear in the	    *
 * supporting documentation, and that the name of the author not be used    *
 * in advertising or publicity pertaining to distribution of the software   *
 * without specific, written prior permission.				    *
 *									    *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 	    *
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN	    *
 * NO EVENT SHALL HE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL   *
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR    *
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS  *
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF   *
 * THIS SOFTWARE.							    *
 *									    *
 ****************************************************************************/
/*
 * Module:	dtusage.c
 * Author:	Robin T. Miller
 *
 * Description:
 *	Display usage information for generic data test program.
 *
 * Modification History:
 *
 * December 21st, 2004 by Robin Miller.
 *      Add displaying of keepalive format control characters.
 *
 * December 5th, 2003 by Robin Miller.
 *      Conditionalize help text for tty options.
 *
 * March 15th, 2003 by Robin Miller.
 *	Add displaying of prefix string if specified.
 *
 * January 24th, 2001 by Robin Miller.
 *	Add help text for variable I/O requests sizes.
 *
 * December 30th, 2000 by Robin Miller.
 *	Make changes to build using MKS/NuTCracker product.
 *
 * May 8th, 2000 by Robin Miller.
 *	Add dtversion() to display just the version string.
 *
 * August 6th, 1999 by Robin Miller.
 *	Better parameterizing of "long long" printf formatting.
 *
 * July 29th, 1999 by Robin Miller.
 *	Merge in changes made to compile on FreeBSD.
 *
 * July 24th, 1999 by Robin Miller.
 *	Bumped major version number to 12.x due to pattern=iot support.
 *
 * December 21, 1998 by Robin Miller.
 *	Bump of major version number to 11.x due to tape reset support.
 *
 * March 27, 1997 by Ali Eghlima.
 *	Document "munsa={cr,cw,pr,pw,ex}" option.  MUti-Node Simultaneous
 *	Access (MUNSA) is used in a cluster system when accessing shared
 *	resources.  The Distributed Lock Manager (DLM) is used to control
 *	access to a resource and to synchronize all access.
 *
 * February 21, 1996 by Robin Miller.
 *	Document iotype={random,sequential} options.
 *
 * December 19, 1995 by Robin Miller
 *      Conditionalize for Linux Operating System.
 *
 * November 19, 1995 by Robin Miller.
 *	Describe logical block data options (lba=, lbs=, enable=lbdata).
 *
 * July 26, 1995 by Robin Miller.
 *	Added AIO bug fixes/enhancements, special pattern string mapping,
 *	corrected data dumping/add dump limit, reporting of lba on disk
 *	devices, child signal bug fix/error control, additional stats...
 *	So, it's finally time for a new version string -> V8.0 (enjoy).
 *
 * September 23, 1994 by Robin Miller.
 *      Change O_DSYNCH to O_DSYNC, and O_FSYNCH to O_SYNC.
 *
 * December 7, 1993 by Robin Miller.
 *	Ported to SunOS 5.1 (Solaris).
 *
 * September 1, 1993 by Robin Miller.
 *	Added description of "min=", "max=", & "incr=" parameters.
 *	Finally added version and date identifiers for tracking.
 *	V1.0=Sun/3 (disk & tape), V2.0=Sun/386i (tty support),
 *	V3.0=Sun/Sparc (mmap files), V4.0=Ultrix, V5.0=DEC OSF/1,
 *	V6.0=QNX (POSIX support),
 *	V7.0=(procs, files, step, runtime, AIO, variable records).
 *
 * August 31, 1993 by Robin Miller.
 *	Added description of "align=rotate" & "pattern=incr" options.
 *
 * August 30, 1993 by Robin Miller.
 *	Added description of AIO options: enable=aio & aios=value
 *
 * August 17, 1993 by Robin Miller.
 *	Added text to describe "runtime=" option & format.
 *
 * August 5, 1993 by Robin Miller.
 *	Add text to describe "files=value" option (for tapes).
 *
 * August 3, 1993 by Robin Miller.
 *	Add text to describe "procs=value" option.
 *
 * September 19, 1992 by Robin Miller.
 *	Add text to describe "enable/disable=flush" tty option.
 *
 * September 17, 1992 by Robin Miller.
 *	Add text to describe "pattern=string" option, and to describe
 *	how to override the default decimal radix numeric input.
 *
 * September 11, 1992 by Robin Miller.
 *	Display help text to stdout instead of stderr so output can be
 *	easily piped to a pager.
 */
#include "dt.h"
#include <fcntl.h>

/*
 * usage()	Display valid options for dt program.
 *
 *	Really should be in a man page since it's grown so much.
 */
#define P	printf
#define D	Fprint

char *version_str = "Date: December 21st, 2004, Version: 15.21, Author: Robin T. Miller";

void
dtusage(void)
{
    D ("Usage: %s options...\n", cmdname);
    D (" Type '%s help' for a list of valid options.\n", cmdname);
    exit (FATAL_ERROR);
}

void
dtversion(void)
{
    P ("    --> %s <--\n", version_str);
    exit (SUCCESS);
}

void
dthelp(void)
{
    static char *enabled_str = "enabled";
    static char *disabled_str = "disabled";

    P ("Usage: %s options...\n", cmdname);
    P ("\n    Where options are:\n");
    P ("\tif=filename      The input file to read.\n");
    P ("\tof=filename      The output file to write.\n");
    P ("\tpf=filename      The data pattern file to use.\n");
    P ("\tbs=value         The block size to read/write.\n");
    P ("\tlog=filename     The log file name to write.\n");
#if defined(MUNSA)
    P ("\tmunsa=string     Set munsa to: cr, cw, pr, pw, ex.\n");
#endif /* defined(MUNSA) */
#if defined(AIO)
    P ("\taios=value       Set number of AIO's to queue.\n");
#endif /* defined(AIO) */
#if !defined(_QNX_SOURCE)
    P ("\talign=offset     Set offset within page aligned buffer.\n");
    P ("    or\talign=rotate     Rotate data address through sizeof(ptr).\n");
#endif /* !defined(_QNX_SOURCE) */
    P ("\talarm=time       The keepalive alarm time.\n");
    P ("\tkeepalive=string The keepalive message string.\n");
    P ("\tcapacity=value   Set the device capacity in bytes.\n");
#if defined(DEC)
    P ("    or\tcapacity=max     Set maximum capacity from disk driver.\n");
#endif
    P ("\tdispose=mode     Set file dispose to: {delete or keep}.\n");
    P ("\tdlimit=value     Set the dump data buffer limit.\n");
    P ("\tdtype=string     Set the device type being tested.\n");
    P ("\tidtype=string    Set input device type being tested.\n");
    P ("\todtype=string    Set output device type being tested.\n");
    P ("\tdsize=value      Set the device block (sector) size.\n");
    P ("\terrors=value     The number of errors to tolerate.\n");
    P ("\tfiles=value      Set number of tape files to process.\n");
    P ("\tflow=type        Set flow to: none, cts_rts, or xon_xoff.\n");
/*  P ("\thz=value         Set number of clock ticks per second.\n");	*/
    P ("\tincr=value       Set number of record bytes to increment.\n");
    P ("    or\tincr=variable    Enables variable I/O request sizes.\n");
    P ("\tiodir=direction  Set I/O direction to: {forward or reverse}.\n");
    P ("\tiomode=mode      Set I/O mode to: {copy, test, or verify}.\n");
    P ("\tiotype=type      Set I/O type to: {random or sequential}.\n");
    P ("\tmin=value        Set the minumum record size to transfer.\n");
    P ("\tmax=value        Set the maximum record size to transfer.\n");
    P ("\tlba=value        Set starting block used w/lbdata option.\n");
    P ("\tlbs=value        Set logical block size for lbdata option.\n");
    P ("\tlimit=value      The number of bytes to transfer.\n");
    P ("\tflags=flags      Set open flags:   {excl,sync,...}\n");
    P ("\toflags=flags     Set output flags: {append,trunc,...}\n");
    P ("\toncerr=action    Set child error action: {abort or continue}.\n");
#if defined(_QNX_SOURCE)
    P ("\tparity=string    Set parity to: {even, odd, mark, space, or none}.\n");
#else /* !defined(_QNX_SOURCE) */
    P ("\tparity=string    Set parity to: {even, odd, or none}.\n");
#endif /* defined(_QNX_SOURCE) */
    P ("\tpasses=value     The number of passes to perform.\n");
    P ("\tpattern=value    The 32 bit hex data pattern to use.\n");
    P ("    or\tpattern=iot      Use DJ's IOT test pattern.\n");
    P ("    or\tpattern=incr     Use an incrementing data pattern.\n");
    P ("    or\tpattern=string   The string to use for the data pattern.\n");
    P ("\tposition=offset  Position to offset before testing.\n");
    P ("\tprefix=string    The data pattern prefix string.\n");
    P ("\tprocs=value      The number of processes to create.\n");
#if defined(HP_UX)
    P ("\tqdepth=value     Set the queue depth to specified value.\n");
#endif /* defined(HP_UX) */
    P ("\tralign=value     The random I/O offset alignment.\n");
    P ("\trlimit=value     The random I/O data byte limit.\n");
    P ("\trseed=value      The random number generator seed.\n");
    P ("\trecords=value    The number of records to process.\n");
    P ("\truntime=time     The number of seconds to execute.\n");
    P ("\tslice=value      The specific disk slice to test.\n");
    P ("\tslices=value     The number of disk slices to test.\n");
    P ("\tskip=value       The number of records to skip past.\n");
    P ("\tseek=value       The number of records to seek past.\n");
    P ("\tstep=value       The number of bytes seeked after I/O.\n");
#if defined(TTY)
    P ("\tspeed=value      The tty speed (baud rate) to use.\n");
    P ("\ttimeout=value    The tty read timeout in .10 seconds.\n");
    P ("\tttymin=value     The tty read minimum count (sets vmin).\n");
#endif /* defined(TTY) */
    P ("\ttrigger=type     The trigger to execute during errors.\n");
    P ("\tvolumes=value    The number of volumes to process.\n");
    P ("\tvrecords=value   The record limit for the last volume.\n");
    P ("\tenable=flag      Enable one or more of the flags below.\n");
    P ("\tdisable=flag     Disable one or more of the flags below.\n");
    P ("\n    Flags to enable/disable:\n");
#if defined(AIO)
    P ("\taio              POSIX Asynchronous I/O.(Default: %s)\n",
				(aio_flag) ? enabled_str : disabled_str);
#endif /* defined(AIO) */
    P ("\tcerrors          Report close errors.   (Default: %s)\n",
				(cerrors_flag) ? enabled_str : disabled_str);
    P ("\tcompare          Data comparison.       (Default: %s)\n",
				(compare_flag) ? enabled_str : disabled_str);
    P ("\tcoredump         Core dump on errors.   (Default: %s)\n",
				(core_dump) ? enabled_str : disabled_str);
    P ("\tdebug            Debug output.          (Default: %s)\n",
				(debug_flag) ? enabled_str : disabled_str);
    P ("\tDebug            Verbose debug output.  (Default: %s)\n",
				(Debug_flag) ? enabled_str : disabled_str);
    P ("\trdebug           Random debug output.   (Default: %s)\n",
				(rDebugFlag) ? enabled_str : disabled_str);
#if defined(LOG_DIAG_INFO)
    /* Only has meaning on Tru64 Unix (right now)... may change later! */
    P ("\tdiag             Log diagnostic msgs.   (Default: %s)\n",
				(logdiag_flag) ? enabled_str : disabled_str);
#endif /* defined(LOG_DIAG_INFO) */
    P ("\tdump             Dump data buffer.      (Default: %s)\n",
				(dump_flag) ? enabled_str : disabled_str);
    P ("\teof              EOF/EOM exit status.   (Default: %s)\n",
				(eof_status) ? enabled_str : disabled_str);
    P ("\tfsalign          File system align.     (Default: %s)\n",
				(fsalign_flag) ? enabled_str : disabled_str);
#if defined(EEI)
    P ("\teei              Tape EEI reporting.    (Default: %s)\n",
				(eei_flag) ? enabled_str : disabled_str);
    P ("\tresets           Tape reset handling.   (Default: %s)\n",
				(eei_resets) ? enabled_str : disabled_str);
#endif /* defined(EEI) */
#if defined(TTY)
    P ("\tflush            Flush tty I/O queues.  (Default: %s)\n",
				(flush_flag) ? enabled_str : disabled_str);
#endif /* defined(TTY) */
    P ("\tfsync            Controls file sync'ing.(Default: %s)\n",
				(fsync_flag == UNINITIALIZED) ? "runtime"
				: (fsync_flag) ? enabled_str : disabled_str);
    P ("\theader           Log file header.       (Default: %s)\n",
				(header_flag) ? enabled_str : disabled_str);
    P ("\tlbdata           Logical block data.    (Default: %s)\n",
				(lbdata_flag) ? enabled_str : disabled_str);
#if defined(TTY)
    P ("\tloopback         Loopback mode.         (Default: %s)\n",
				(loopback) ? enabled_str : disabled_str);
#endif /* defined(TTY */
    P ("\tmicrodelay       Microsecond delays.    (Default: %s)\n",
				(micro_flag) ? enabled_str : disabled_str);
#if defined(MMAP)
    P ("\tmmap             Memory mapped I/O.     (Default: %s)\n",
				(mmap_flag) ? enabled_str : disabled_str);
#endif /* defined(MMAP) */
#if defined(TTY)
    P ("\tmodem            Test modem tty lines.  (Default: %s)\n",
				(modem_flag) ? enabled_str : disabled_str);
#endif /* defined(TTY) */
    P ("\tmulti            Multiple volumes.      (Default: %s)\n",
				(multi_flag) ? enabled_str : disabled_str);
    P ("\tpstats           Per pass statistics.   (Default: %s)\n",
				(pstats_flag) ? enabled_str : disabled_str);
    P ("\traw              Read after write.      (Default: %s)\n",
				(raw_flag) ? enabled_str : disabled_str);
#if defined(sun) && defined(TTY)
    P ("\tsoftcar          tty software carrier.  (Default: none)\n");
#endif /* defined(sun) && defined(TTY) */
    P ("\tstats            Display statistics.    (Default: %s)\n",
				(stats_flag) ? enabled_str : disabled_str);
#if defined(DEC)
    P ("\ttable            Table(sysinfo) timing. (Default: %s)\n",
				(table_flag) ? enabled_str : disabled_str);
#endif /* defined(DEC) */
#if defined(TTY)
    P ("\tttyport          Flag device as a tty.  (Default: %s)\n",
				(ttyport_flag) ? enabled_str : disabled_str);
#endif /* defined(TTY) */
    P ("\tunique           Unique pattern.        (Default: %s)\n",
				(unique_pattern) ? enabled_str : disabled_str);
    P ("\tverbose          Verbose output.        (Default: %s)\n",
				(verbose_flag) ? enabled_str : disabled_str);
    P ("\tverify           Verify data written.   (Default: %s)\n",
				(verify_flag) ? enabled_str : disabled_str);
    P ("\n");
    P ("      Example: enable=debug disable=compare,pstats\n");
#if defined(MUNSA)
    P ("\n    MUNSA Lock Options:\n");
    P ("\tcr = Concurrent Read (permits read access, cr/pr/cw by others)\n");
    P ("\tpr = Protected Read (permits cr/pr read access to all, no write)\n");
    P ("\tcw = Concurrent Write (permits write and cr access to resource by all)\n");
    P ("\tpw = Protected Write (permits write access, cr by others)\n");
    P ("\tex = Exclusive Mode (permits read/write access, no access to others)\n");
    P ("\n");
    P ("\t    For more details, please refer to the dlm(4) reference page.\n");
#endif /* defined(MUNSA) */
    P ("\n    Common Open Flags:\n");
#if defined(O_EXCL)
    P ("\texcl (O_EXCL)         Exclusive open. (don't share)\n");
#endif /* defined(O_EXCL) */
#if defined(O_NDELAY)
    P ("\tndelay (O_NDELAY)     Non-delay open. (don't block)\n");
#endif /* defined(O_NDELAY) */
#if defined(O_NONBLOCK)
    P ("\tnonblock (O_NONBLOCK) Non-blocking open/read/write.\n");
#endif /* defined(O_NONBLOCK) */
#if defined(O_CACHE)
    P ("\tcache (O_CACHE)       Attempt to keep data in file system cache.\n");
#endif /* defined(O_CACHE) */
#if defined(O_DIRECT)
    P ("\tdirect (O_DIRECT)     Direct disk access. (don't cache data).\n");
#endif /* defined(O_DIRECT) */
#if defined(O_DIRECTIO)
    P ("\tdirect (O_DIRECTIO)   Direct disk access. (don't cache data).\n");
#endif /* defined(O_DIRECTIO) */
#if defined(O_FSYNC)
    P ("\tfsync (O_FSYNC)       Sync both read/write data with disk file.\n");
#endif /* defined(O_FSYNC) */
#if defined(O_RSYNC)
    P ("\trsync (O_RSYNC)       Synchronize read operations.\n");
#endif /* defined(O_RSYNC) */
#if defined(O_SYNC)
    P ("\tsync (O_SYNC)         Sync updates for data/file attributes.\n");
#endif /* defined(O_SYNC) */
#if defined(O_LARGEFILE)
    P ("\tlarge (O_LARGEFILE)   Enable large (64-bit) file system support.\n");
#endif /* defined(O_LARGEFILE) */
    P ("\n    Output Open Flags:\n");
#if defined(O_APPEND)
    P ("\tappend (O_APPEND)     Append data to end of existing file.\n");
#endif /* defined(O_APPEND) */
#if defined(O_DEFER)
    P ("\tdefer (O_DEFER)       Defer updates to file during writes.\n");
#endif /* defined(O_DEFER) */
#if defined(O_DSYNC)
    P ("\tdsync (O_DSYNC)       Sync data to disk during write operations.\n");
#endif /* defined(O_DSYNC) */
#if defined(O_TRUNC)
    P ("\ttrunc (O_TRUNC)       Truncate an exisiting file before writing.\n");
#endif /* defined(O_TRUNC) */
#if defined(O_TEMP)
    P ("\ttemp (O_TEMP)         Temporary file, try to keep data in cache.\n");
#endif /* defined(O_TEMP) */
    P ("\n    Delays (Values are seconds, unless microdelay enabled):\n");
    P ("\tcdelay=value     Delay before closing the file.    (Def: %d)\n",
							cdelay_count);
    P ("\tedelay=value     Delay between multiple passes.    (Def: %d)\n",
							edelay_count);
    P ("\trdelay=value     Delay before reading each record. (Def: %d)\n",
							rdelay_count);
    P ("\tsdelay=value     Delay before starting the test.   (Def: %d)\n",
							sdelay_count);
    P ("\ttdelay=value     Delay before child terminates.    (Def: %d)\n",
							tdelay_count);
    P ("\twdelay=value     Delay before writing each record. (Def: %d)\n",
							wdelay_count);
    P ("\n    Numeric Input:\n");
    P ("\tFor options accepting numeric input, the string may contain any\n");
    P ("\tcombination of the following characters:\n");
    P ("\n\tSpecial Characters:\n");
    P ("\t    w = words (%d bytes)", sizeof(int));
    P ("            q = quadwords (%d bytes)\n", sizeof(large_t));
    P ("\t    b = blocks (512 bytes)         k = kilobytes (1024 bytes)\n");
    P ("\t    m = megabytes (1048576 bytes)  p = page size (%d bytes)\n", page_size);
    P ("\t    g = gigabytes (%ld bytes)\n", GBYTE_SIZE);
    P ("\t    t = terabytes (" LDF " bytes)\n", TBYTE_SIZE);
    P ("\t    inf or INF = infinity (" LUF " bytes)\n", INFINITY);
    P ("\n\tArithmetic Characters:\n");
    P ("\t    + = addition                   - = subtraction\n");
    P ("\t    * or x = multiplcation         / = division\n");
    P ("\t    %% = remainder\n");
    P ("\n\tBitwise Characters:\n");
    P ("\t    ~ = complement of value       >> = shift bits right\n");
    P ("\t   << = shift bits left            & = bitwise 'and' operation\n");
    P ("\t    | = bitwise 'or' operation     ^ = bitwise exclusive 'or'\n\n");
    P ("\tThe default base for numeric input is decimal, but you can override\n");
    P ("\tthis default by specifying 0x or 0X for hexadecimal conversions, or\n");
    P ("\ta leading zero '0' for octal conversions.  NOTE: Evaluation is from\n");
    P ("\tright to left without precedence, and parenthesis are not permitted.\n");

    P ("\n    Keepalive Format Control:\n");
    P ("\t    %%b = The bytes read or written. %%B = Total bytes read and written.\n");
    P ("\t    %%d = The device name.           %%D = The real device name.\n");
    P ("\t    %%e = The number of errors.      %%E = The error limit.\n");
    P ("\t    %%f = The files read or written. %%F = Total files read and written.\n");
    P ("\t    %%h = The host name.             %%H = The full host name.\n");
    P ("\t    %%k = The kilobytes this pass.   %%K = Total kilobytes for this test.\n");
    P ("\t    %%l = Blocks read or written.    %%L = Total blocks read and written.\n");
    P ("\t    %%m = The megabytes this pass.   %%M = Total megabytes for this test.\n");
    P ("\t    %%p = The pass count.            %%P = The pass limit.\n");
    P ("\t    %%r = Records read or written.   %%R = Total records read and written.\n");
    P ("\t    %%t = The pass elapsed time.     %%T = The total elapsed time.\n");
    P ("\t    %%i = The I/O mode (read/write)  %%u = The user (login) name.\n");
    P ("\n");
    P ("      Default: %%d Stats: mode %%i, bytes %%b, blocks %%l, pass %%p/%%P, elapsed %%T\n");

    P ("\n    Pattern String Input:\n");
    P ("\t    \\\\ = Backslash   \\a = Alert (bell)   \\b = Backspace\n");
    P ("\t    \\f = Formfeed    \\n = Newline        \\r = Carriage Return\n");
    P ("\t    \\t = Tab         \\v = Vertical Tab   \\e or \\E = Escape\n");
    P ("\t    \\ddd = Octal Value    \\xdd or \\Xdd = Hexadecimal Value\n");
  
    P ("\n    Prefix Format Control:\n");
    P ("\t    %%d = The device name.           %%D = The real device name.\n");
    P ("\t    %%h = The host name.             %%H = The full host name.\n");
    P ("\t    %%p = The process ID.            %%P = The parent PID.\n");
    P ("\t    %%u = The user name.\n");
    P ("\n");
    P ("      Example: prefix=\"%%u@%%h (pid %%p)\"\n");

    P ("\n    Time Input:\n");
    P ("\t    d = days (%d seconds),      h = hours (%d seconds)\n",
						SECS_PER_DAY, SECS_PER_HOUR);
    P ("\t    m = minutes (%d seconds),      s = seconds (the default)\n\n",
								SECS_PER_MIN);
    P ("\tArithmetic characters are permitted, and implicit addition is\n");
    P ("\tperformed on strings of the form '1d5h10m30s'.\n");

    P ("\n    Trigger Types:\n");
    P ("\t    br = Execute a bus reset.\n");
    P ("\t    bdr = Execute a bus device reset.\n");
    P ("\t    seek = Issue a seek to the failing lba.\n");
    P ("\t    cmd:string = Execute command with these args:\n");
    P ("\t      string dname op dsize offset position lba errno\n");
    P ("\n");
    P ("\t    The first three options require Scu in your PATH.\n");

    /*
     * Display the program defaults.
     */
    P ("\n    Defaults:\n");
    P ("\terrors=%ld", error_limit);
    P (", files=%ld", file_limit);
    P (", passes=%ld", pass_limit);
    P (", records=%ld", (u_long) 0);
    P (", bs=%d", block_size);
    P (", log=stderr\n");

    P ("\tpattern=%#x", pattern);
#if defined(TTY)
    P (", flow=%s", flow_str);
    P (", parity=%s", parity_str);
    P (", speed=%s\n", speed_str);

    P ("\ttimeout=%d seconds", (tty_timeout / 10));
#endif /* defined(TTY) */
    P (", dispose=delete");
    P (", align=%d (page aligned)\n", align_offset);

#if defined(AIO)
    P ("\taios=%d", aio_bufs);
    P (", dlimit=%d", dump_limit);
#else /* !defined(AIO) */
    P ("\tdlimit=%d", dump_limit);
#endif /* defined(AIO) */
    P (", oncerr=%s", (oncerr_action == ABORT) ? "abort" : "continue");
    P (", volumes=%d, vrecords=%lu\n", volume_limit, volume_records);
    P ("\tiodir=%s", (io_dir == FORWARD) ? "forward" : "reverse");
    P (", iomode=%s", (io_mode == TEST_MODE) ? "test" :
			(io_mode == COPY_MODE) ? "copy" : "verify");
    P (", iotype=%s\n", (io_type == RANDOM_IO) ? "random" : "sequential");
    P ("\n    --> %s <--\n", version_str);

    exit (SUCCESS);
    /*NOTREACHED*/
}
