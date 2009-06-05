/****************************************************************************
 *									    *
 *			  COPYRIGHT (c) 1990 - 2000			    *
 *			   This Software Provided			    *
 *				     By					    *
 *			  Robin's Nest Software Inc.			    *
 *			       2 Paradise Lane  			    *
 *			       Hudson, NH 03051				    *
 *			       (603) 883-2355				    *
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
 * Module:	dt.c
 * Author:	Robin T. Miller
 *
 * Description:
 *	Main line code for generic data test program 'dt'.
 */
#include "dt.h"
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#if !defined(_QNX_SOURCE)
#  if !defined(sun)
#    include <sys/ioctl.h>
#  endif /* !defined(sun) */
#  include <sys/file.h>
#  include <sys/param.h>
#  if defined(sun) || defined(_OSF_SOURCE)
#    include <sys/mman.h>
#  endif /* defined(sun) || defined(_OSF_SOURCE) */
#endif /* !defined(_QNX_SOURCE) */
#include <sys/wait.h>
#if defined(DEC)
#  include <sys/utsname.h>
#endif /* defined(DEC) */

/*
 * Modification History:
 *
 * May 31st, 2001 by Robin Miller.
 *	Don't allow different data patterns with multiple processes,
 * unless writing to a regular file (each process has own filename).
 *
 * April 13th, 2001 by Robin Miller.
 *	Added "capacity=max" for Tru64 Unix, this will force using the
 * driver returned media capacity.  Prevents lseek/read algorithm when
 * using random I/O too!  Note:  Will inhibit disk driver EOM testing.
 *
 * February 24th, 2001 by Robin Miller.
 *	Add conditionalization for QNX RTP (Neutrino).
 *
 * February 21st, 2001 by Robin Miller.
 *	Disable EOF/EOM exit status by default.  This causes trouble for
 * normal testers.  Use "enable=eof" to reenable previous behaviour.
 *
 * February 6th, 2001 by Robin Miller.
 *	If input file and multiple slices, allow cycling through data
 * patterns, since the file is expected to have been written with unique
 * patterns in each slice region.  Minor update to deleting files too.
 *
 * February 3rd, 2001 by Robin Miller.
 *	Lift restriction of bs= or min= sizes being smaller than
 * sizeof(u_int32) for IOT pattern or lbdata option.  Although this
 * isn't *really* correct, enforcing this breaks numerious scripts.
 * Although smaller works, it's only by luck and the fact we have
 * pad bytes at EOB, which prvented data corruption in the past!
 *
 * January 28th, 2001 by Robin Miller.
 *	Added "slices=value" and "enable/disable=unique" options.
 * The slices option carves up a disk with each process exercising a
 * differnent range of blocks.  The unique pattern option sets up a
 * unique pattern for each process started for slices & regular files.
 *
 * January 26th, 2001 by Robin Miller.
 *	Added report_record() to the report record information.
 *	Added "iodir={forward,reverse}" option to all reverse I/O
 * to rotating media.  Add validation checks both before and after
 * a device is open (mostly random/reverse I/O checks).
 *
 * January 24th, 2001 by Robin Miller.
 *	Added "dsize=value" to set the device block size.
 *	Added "incr=var" option for variable request sizes.
 *	Removed some dead code (#if 0 stuff), to cleanup a bit.
 *	Updates to allow the IOT pattern to use non-modulo 512 byte
 * sizes.  The logic is: 1) user defined "lbs=value", 2) device block
 * size (Tru64 Unix), or 3) default to 512 byte block (original default).
 * This change was initiated by Windows/NT IOT disc using 2KB sector size.
 *
 * January 18th, 2001 by Robin Miller.
 *	When requesting multiple volumes, ensure the tape device gets
 * closed prior to retries, if the rewind operation fails.  Otherwise,
 * the next tape open fails with EBUSY (exclusive open device).
 *
 * January 14th, 2001 by Robin Miller.
 *	Added support for multiple volumes option.
 *
 * December 30th, 2000 by Robin Miller.
 *	Make changes to build using MKS/NuTCracker product.
 *
 * November 19th, 2000 by Robin Miller.
 *	Add missing enable/disable=fsync goto label statements.
 *
 * November 10th, 2000 by Robin Miller.
 *	Added sanity check to warn user about unpredictable results
 * when writing to a disk with multiple procs and multiple passes.
 *
 * November 8th, 2000 by Robin Miller.
 *	Added "disable=cerrors" to disable device close errors.
 * [ Note: This is really a workaround for the Linux tape driver. ]
 *	In HandleMultiVolume(), if closing the device fails, then
 * return that failure to abort the test.  With tapes, this means
 * flushing the buffered data or writing filemarks has failed.
 *
 * October 2nd, 2000 by Robin Miller.
 *	Enhanced report_error() to display errno value (same as Scu).
 *
 * August 22nd, 2000 by Robin Miller.
 *	Added boolean flags to track user set min, max, & incr values.
 *
 * July 14th, 2000 by Robin Miller.
 *	Added fsync_flag to control sync'ing data to disk files.
 *
 * May 8th, 2000 by Robin Miller.
 *	Adding parsing of "version" option, which is used to only
 * display the version string, dtversion() in file dtusage.c
 *
 * May 5th, 2000 by Robin Miller.
 *	Set proper exit code in terminate(), when exiting due to
 * an alarm, i.e., runtime= option.  Previously, the end of file
 * exit status was being ignored and we exited with failure status.
 *
 * April 25th, 2000 by Robin Miller.
 *	Fix problem of not breaking out of test loop if the error
 * limit has been reached on previous pass.
 *
 * March 28th, 2000 by Robin Miller.
 *	Modify "position=value" to accept a large numeric value,
 * otherwise we were limited to 32 bit value on non-64bit systems.
 * Do the same thing for "step=value" for 64-bit file offsets.
 * When using the "runtime=" option, when the alarm expires and
 * we call terminate() to exit, look at the exit_status variable
 * instead of error_count for errors, so we don't miss errors!
 *
 * March 27th, 2000 by Robin Miller.
 *   o	Fixed parsing of a couple flags, including "flags=direct".
 *   o	Added "capacity=value" option to set the drive capacity.
 *
 * March 20th, 2000 by Robin Miller.
 *	Don't allow a pass limit of zero.  This also avoids a
 * core dump when preparing statistics, since the active device
 * never got setup.
 *
 * March 2nd, 2000 by Robin Miller.
 *	In HandleMultiVolume(), reset the exit status to SUCCESS,
 * since this got set to END_OF_FILE earlier.  Failure to do this
 * means we exit with an END_OF_FILE (254) status (no good :-).
 *
 * February 17th, 2000 by Robin Miller.
 *	Adding better support for multi-volume tape testing.
 *
 * January 17th, 2000 by Robin Miller.
 *	In copy mode, is input file is stdin, don't do verify.
 *	Enable random I/O, for ralign, rlimit, or rseed options.
 *
 * January 14th, 2000 by Robin Miller.
 *	Don't delete the output file unless we're in test mode.
 *
 * January 6th, 2000 by Robin Miller.
 *	Added support for multi-volume media (lot's of changes).
 *
 * January 1st, 2000 by Robin Miller.
 *	Added read after write support, "enable=raw" option.
 *
 * December 31st, 1999 by Robin Miller.
 *	Added "rseed=" option, so user can specify the random seed.
 *	Modify reopen_file logic to use O_RDWR if skip_count exists.
 *
 * November 11th, 1999 by Robin Miller.
 *	Added logging of diagnostic information to event logger.
 *
 * November 10th. 1999 by Robin Miller.
 *	Fixed parsing of "dispose=delete".
 *
 * November 9th, 1999 by Robin Miller.
 *	Modify logic associated with making stderr stream buffered.
 *
 * August 26th, 1999 by Robin Miller.
 *	If during the write pass no data was transferred, don't do
 * the read pass since obviously this will result in a compare error.
 *
 * August 7th, 1999 by Robin Miller.
 *	Allow enabling open w/O_LARGEFILE via "flags=large", incase
 * _FILE_OFFSET_BITS=64 does _not_ enable this by default.
 *
 * July 22nd, 1999 by Robin Miller.
 *	Added support for IOT (DJ's) test pattern.
 * 
 * July 19th, 1999 by Robin Miller.
 *	Add flag to control log file header normally displayed.
 *
 * June 28, 1999 by Robin Miller.
 *	For 32-bit systems, change count and limit variables from
 * u_long to double, since u_long limits us to 4GB (too small).
 *
 * May 27, 1999 by Robin Miller.
 *	Merge in Goerge Bittner's changes for micro-second delays.
 *	Conditionalized all common/output file open flags.
 *
 * April 8, 1999 by Robin Miller.
 *	Merge in Jeff Detjen's changes for table()/sysinfo timing.
 *
 * April 7, 1999 by Robin Miller.
 *	On DEC systems, obtain tick/second via sysconf(_SC_CLK_TCK).
 *
 * December 21, 1998 by Robin Miller.
 *	Add pasing of "enable=resets", which enables tape repositioning
 * logic when bus/device resets are detected by DUNIX EEI status codes.
 *
 * December 19, 1998 by Robin Miller.
 *	- Disable setbuf() for stderr... screws up log files.
 *	- Write cmd line and version to log file for reference.
 *
 * December 16, 1998 by Robin Miller.
 *	Merge in changes made by George Bittner:
 *	- set random_seed using times() instead of time(0).
 *	- remove DLM_VALB, per Dan Christians' recommendation.
 *
 * October 29, 1998 by Robin Miller.
 *	Implement a random I/O data limit, instead of using the normal
 * data limit variable (not good to dual purpose this value).
 *
 * October 22, 1998 by Robin Miller. (Happy Birthday Mom!)
 *	When specifying a runtime, ensure we break out of the
 * passes loop if we exceed the error limit.  Previously, we'd
 * loop (possibly with error) for the duration of runtime :-)
 *
 * June 16, 1998 by Robin Miller.
 *	Add common open "flags={excl,ndelay,nonblock,rsync}".
 *	Add output (write) open "oflags={defer,dsync,trunc}".
 *
 * April 29, 1998 by Robin Miller.
 *	Add support for an alternate device directory.
 *
 * April 7, 1998 by Robin Miller.
 *	Setup the pattern as a pattern string, so non-modulo
 *	sizeof(pattern) read counts will data compare properly.
 *
 * March 20, 1998 by Robin Miller.
 *	When terminating, call close function before reporting stats,
 *	since in the case of AIO the I/O rundown updates statistics
 *	with outstanding requests, when we're aborted by a signal.
 *
 * May 14, 1997 by Robin Miller.
 *	In report_error(), set exit value to FAILURE instead of errno
 *	since this makes detecting errors easier in scripts.
 *
 * May 13, 1997 by Robin Miller.
 *	If a "skip=" count is specified with an output file "of=",
 *	open the file for R/W access since skips are accompished via
 *	read()'s of record size.
 *
 * March 27, 1997 by Ali Eghlima.
 *      Added cluster support, so more than one process or one system
 *      can access a resource. dlm are being used to synchronize all
 *      access.
 * 
 * March 7, 1997 by Robin Miller.
 *	If a copy or verify operation is selected, and a data limit
 *	is specified, then double the data limit to account for double
 *	the I/O's.  Previously, only half of the desired data limit was
 *	copied and/or verified.
 *
 * February 3, 1997 by Robin Miller.
 *	Check status from closing file descriptor, and if it's not
 *	SUCCESS, use this for the exit status.  For tapes, deferred
 *	writes and failure to write file marks occur at close time.
 *
 * March 30, 1996 by Robin Miller.
 *	When writing, now default to an infinite record limit, so
 *	writes stop when EOF is reached (disks and tapes only).
 *
 * February 29, 1996 by Robin Miller.
 *	Added callouts to setup data limit for disk random I/O.
 *
 * February 28, 1996 by Robin Miller.
 *	Added support for copying and verifying device/files.
 *
 * February 17, 1996 by Robin Miller.
 *	Add "enable=spad" option to control checking of pad bytes
 *	when long reads of short records is performed, since some
 *	controllers corrupt data bytes following the short record.
 *	[ NOTE:  At DEC, this action was deemed acceptable, since
 *	  the requester stated the driver could trash a larger buf. ]
 *
 * December 6, 1995 by Robin Miller.
 *	Add "ttymin=value" option to give user control over tty VMIN.
 *
 * November 19, 1995 by Robin Miller.
 *	When the device type is a disk, default min_size & incr_count to
 *	BLOCK_SIZE (512) instead of 1 (which is non-modulo for disks).
 *
 * November 18, 1995 by Robin Miller.
 *	Removing unaligned data access (ade) test code (code cleanup).
 *
 * July 24, 1995 by Robin Miller.
 *	Changed O_NDELAY to O_NONBLOCK which is defined by POSIX.  They are
 *	supposed to do the same thing, but they _do_ have different values.
 *	[ NOTE: This is only used for serial line modem testing. ]
 *
 * July 22, 1995 by Robin Miller.
 *	Add additional checking on "pattern=string" to avoid ambiguity
 *	with 4-byte hex data pattern strings... which is misleading).
 *
 * July 15, 1995 by Robin Miller.
 *	Add "oncerr=action" option to control child process
 *	error processing.
 *
 * July 7, 1995 by Robin Miller.
 *	Add "dlimit=value" option to override default dump limit.
 *
 * July 6, 1995 by Robin Miller.
 *	Enable dumping of data bytes on compare errors by default,
 *	since this program is used mainly as a diagnostic tool, and
 *	we now limit the number of bytes dumped.
 *
 * November 5, 1994 by Robin Miller.
 *	Don't set SIGCHLD signal to SIG_IGN (set to SIG_DFL) or else
 *	waitpid() won't detect any child processes (OSF R1.3 and QNX).
 *
 * September 23, 1994 by Robin Miller.
 *      Make changes necessary to build on QNX 4.21 release.  This
 *      required changing O_DSYNCH to O_DSYNC, and O_FSYNCH to O_SYNC.
 *
 * November 11, 1993 by Robin Miller.
 *	Removed code which was inadvertantly disabling data compares
 *	when there was no output file and verify was disabled.  This
 *	caused data comparisions to be disabled for terminal devices,
 *	now that the code has been rewritten/restructured (shit!!!).
 *
 * October 31, 1993 by Robin Miller.
 *	Enhance device type setup and honor user specified device type.
 *
 * October 28, 1993 by Robin Miller.
 *	Correct problem during loopback testing, where the device info
 *	structure fd's were not marked closed due to two places where
 *	close() was called directly (oops, missed a couple changes).
 *	After forking, display which process is the reader and writer.
 *
 * October 11, 1993 by Robin Miller.
 *	Conditionalize SunOS code (4.1.2 used) to avoid using strerror()
 *	function for obtaining error messages (return to perror() method).
 *
 * October 7, 1993 by Robin Miller.
 *	Add appropriate casting to block size checking, use ssize_t to
 *	prevent negative block sizes which don't work with read & write
 *	system calls (even though the count arg is declared as size_t).
 *
 * September 16, 1993 by Robin Miller.
 *	Properly report write pass count when read verify is disabled.
 *	Only update data pattern when writing files or loopback enabled
 *	when multiple passes specified (pattern cannot change for reads).
 *	Support "enable=loop" to enable loopback operation, rather than
 *	requiring both the same input & output devices (need only one).
 *	Added "oflags={append,sync}" for DEC OSF/1 systems (write opts).
 *
 * September 15, 1993 by Robin Miller.
 *	Added additional check for FIFO's prior to opening the device
 *	since O_NONBLOCK flag must be set to keep open() from blocking.
 *
 * September 8, 1993 by Robin Miller.
 *	Moved generic test functions into file dtgen.c.
 *
 * September 7, 1993 by Robin Miller.
 *	Moved tty specific code into dttty.c.  This code is dispatched
 *	to automatically after being setup in the device info structure.
 *
 * September 4, 1993 by Robin Miller.
 *	Lots of restructuring & code cleanup.  Nearly all testing is
 *	now appropriately parameterized (helps speed & new tests).
 *
 * September 3, 1993 by Robin Miller.
 *	Dispatch to test functions via function lookup table in
 *	preparation for device specific tests.
 *
 * September 1, 1993 by Robin Miller.
 *	Added "min=value", "max=value", and "incr=value" options for
 *	testing variable length records (mainly for tape devices).
 *
 * August 31, 1993 by Robin Miller.
 *	Added "align=rotate" option to rotate the starting data buffer
 *	address through sizeof(ptr) to force unaligned buffer accesses.
 *	This feature tests special handling of unaligned DMA addresses.
 *	Added "pattern=incr" option to use incrementing data pattern.
 *
 * August 27, 1993 by Robin MIller.
 *	Added support for DEC OSF/1 POSIX Asynchronous I/O (AIO).
 *
 * August 20, 1993 by Robin Miller.
 *	Added handling of reading & writing multiple tape files.
 *
 * August 18, 1993 by Robin Miller.
 *	Added "step=" option to specify a step offset for seeks.
 *	This is the number of bytes stepped after an I/O request.
 *
 * August 17, 1993 by Robin Miller.
 *	Added reporting of start & end times, time of errors, and
 *	added "runtime=" option to specify how long to run.
 *
 * August 13, 1993 by Robin Miller.
 *	Added alternate data patterns to use with multiple passes.
 *
 * August 5, 1993 by Robin Miller.
 *	Added "files=value" option for processing tape files.
 *
 * August 3, 1993 by Robin Miller.
 *	Added "procs=value" option to create multiple processes.
 *
 * September 19, 1992 by Robin Miller.
 *	Initialize flush_flag to TRUE so flushing of tty I/O queues
 *	gets done by default to discard any left over junk.
 * 
 * September 18, 1992 by Robin Miller.
 *	Added calls to save & restore terminal characteristics.
 *
 * September 17, 1992 by Robin Miller.
 *	Added parsing for "pattern=string" to allow ASCII string.
 *
 * September 12, 1992 by Robin Miller.
 *	Ensure the stderr stream is fully buffered, so the total time
 *	is not affected by flushing previous statistics.
 *
 * September 11, 1992 by Robin Miller.
 *	Added "oflags={cache,dsync,fsync,temp}" options to allow testing
 *	the affect of these open flags on QNX systems.
 *
 * September 10, 1992 by Robin Miller.
 *	Added parsing of MARK & SPACE parity for QNX Operating System.
 *
 * September 9, 1992 by Robin Miller.
 *	Allow loopback to same tty port (avoid exclusive open).
 *
 * September 5, 1992 by Robin Miller.
 *	Initial port to QNX 4.1 Operating System.
 *
 * September 3, 1992 by Robin Miller.
 *	Make changes necessary to remove BSD dependencies.
 *
 * August 19, 1992 by Robin Miller.
 *	Added support for testing tty modem control.
 *
 * May 25, 1992 by Robin Miller.
 *	Don't set exclusive open mode for terminal device if debug is
 *	enabled, so "stty -a" can be done on terminal under test.
 *
 * May 22, 1992 by Robin Miller.
 *	Control / force kernel address data exception via flag.
 *	Added option "enable=ade" (address data exception).
 *
 * April 24, 1992 by Robin Miller.
 *	Inform user that either a record count or a data limit must be
 *	specified.  Added flags to determine if we are reading stdin or
 *	writing to stdout streams.
 *
 * March 11, 1992 by Robin Miller.
 *	Changes necessary for port to 64-bit Alpha architecture.
 *
 * October 16, 1990 by Robin Miller.
 *	Added "align=offset" option to align memory buffers at a specific
 *	offset.  The default is page aligned.
 *
 * October 9, 1990 by Robin Miller.
 *	Added "hz=ticks" option to allow the ticks per second value to
 *	be specified.  On VAX systems, this is consistent (100), but on
 *	MIPS systems it various and is setup at boot time in the machine
 *	dependent code.
 *
 * August 21, 1990 by Robin Miller.
 *	Changed exit status so scripts can detect and handle errors
 *	based on the exit code.  If not success, fatal error, or end
 *	of file/tape, the exit code is the error number (errno).
 *
 * August 21, 1990 by Robin Miller.
 *	Added "position=offset" option to position to specified offset
 *	before starting test.
 *
 * August 16, 1990 by Robin Miller.
 *	Added "enable=coredump" option so if exiting with error status,
 *	we'll generate a core dump for analysis purposes.
 *
 * August 8, 1990 by Robin Miller.
 *	Changed malloc() to valloc() to align buffer on page boundry.
 *	On some archetectures, this results on better performance to
 *	raw devices since the DMA is done directly to the users' buffer.
 *
 * August 2, 1990 by Robin Miller.
 *	Added "pf=file" to specify a pattern file to use.  In this
 *	case, the contents of the specified file are used for the
 *	data pattern.
 *
 * April 11, 1990 by Robin Miller.
 *	Added "enable=mmap" option to enable memory mapped I/O.
 *
 * March 22, 1990 by Robin Miller.
 *	Added "dispose={delete|keep}" option to control deleting the
 *	output file at the end of testing.  "delete" is the default
 *	action, to avoid leaving test files around.
 *
 * Novemeber 9, 1989 by Robin Miller.
 *	Add additional delay commands (cdelay=, rdelay=, wdelay=) to
 *	help with testing and debugging various devices.  These aren't
 *	normally enabled and don't appear in the help section.
 *
 * Novemeber 6, 1989 by Robin Miller.
 *	Allow start delay (sdelay=) and end delay (edelay=) to be used
 *	with any device, not just terminals.  The parallel output device
 *	needs an end delay to allow the parallel input device to reopen
 *	during multiple passes (can't disable cycle request).
 *
 * November 3, 1989 by Robin Miller.
 *	Change open mode for output files from read/write (O_RDWR) to
 *	write only (O_WRONLY) so write only devices can be tested.
 *
 * October 16, 1989 by Robin Miller.
 *	Made flushing of input queue before testing optional via
 *	the "enable=flush" command.  This caused a timing problem
 *	where occasionally the writer started before the reader.
 *
 * October 4, 1989 by Robin Miller.
 *	Added fsync() system call to force in-core buffers to disk
 *	before reporting statistics after writing the file.
 *
 * September 27, 1989 by Robin Miller.
 *	Display statistics at end of each read/write pass by default.
 *	Added enable/disable=pstats to control end of pass statistics.
 *	Added enable/disable=compare to control data comparisions.
 *
 * September 18, 1989 by Robin Miller.
 *	Allow both an input and an output device to be specified.
 *	This was done mainly for terminal devices so a child could
 *	be forked to allow proper synchronization.
 *
 * July 25, 1989 by Robin Miller.
 *	Added support for testing terminal ports.  New options are:
 *		speed=, timeout=, parity=, flow=, sdelay=, edelay=
 *
 * January 25, 1989 by Robin Miller.
 *	Added reporting of pass statistics if debugging is enabled
 *	so write/read elapsed times can be determined.
 *
 * December 8, 1988 by Robin Miller.
 *	Added catching of software terminate signal (SIGTERM) so the
 *	final statistics will be displayed before exiting.
 */

#if defined(ultrix) || defined(sun)
extern int sys_nerr;
extern char *sys_errlist[];
#endif /* defined(sun) */

/*
 * Forward References:
 */
static u_long number(int base);		/* Numeric conversion function.	*/
static large_t large_number(int base);	/* Large numeric conversions.	*/
void parse_args(int argc, char **argv);
static time_t time_value(void);		/* Time conversion function.	*/

/*
 * Variable Declarations:
 */

#if defined(MUNSA)
bool	munsa_flag = FALSE;		/* if TRUE enable MUNSA features*/
dlm_lock_mode_t  munsa_lock_type = DLM_NLMODE; /* default munsa lock type*/
dlm_lock_mode_t  input_munsa_lock_type = DLM_PRMODE; /* lock for input file*/
dlm_lock_mode_t  output_munsa_lock_type = DLM_PWMODE; /* lock for output file*/
char	     *resnam;
int	     resnlen, i;
dlm_lkid_t   lkid;
dlm_status_t l_stat;
dlm_nsp_t    nsp;
#endif /* defined(MUNSA) */

u_int32 pattern = DEFAULT_PATTERN;	/* Default data pattern.	*/
bool	user_pattern = FALSE;		/* Flags user specified pattern	*/
bool	unique_pattern = TRUE;		/* Unique pattern per process.	*/
char	*pattern_string;		/* Pointer to pattern string.	*/

bool	aio_flag = FALSE;		/* Asynchronous I/O (AIO) flag.	*/
int	align_offset = 0;		/* Align buffer at this offset.	*/
speed_t	baud_rate;			/* The user selected baud rate.	*/
size_t	block_size = BLOCK_SIZE;	/* Default block size to use.	*/
size_t	data_size;			/* Data buffer size + pad bytes	*/
size_t	dump_limit = 64;		/* The dump buffer data limit.	*/
u_int32	device_size = 0;		/* Default device block size.	*/
pid_t	child_pid;			/* For the child process ID.	*/
bool	bypass_flag = FALSE;		/* Bypass (some) sanity checks.	*/
bool	cerrors_flag = TRUE;		/* Report device close errors.	*/
bool	compare_flag = TRUE;		/* Controls data comparisions.	*/
bool	core_dump = FALSE;		/* Generate core dump on errors	*/
bool	debug_flag = FALSE;		/* Enable debug output flag.	*/
bool	Debug_flag = FALSE;		/* Verbose debug output flag.	*/
bool	eDebugFlag = FALSE;		/* End of file debug flag.	*/
bool	rDebugFlag = FALSE;		/* Random (seek) debug flag.	*/
bool	dump_flag = TRUE;		/* Dump data buffer on errors.	*/
#if defined(EEI)
bool	eei_flag = TRUE;		/* Extended Error Information.	*/
bool	eei_resets = FALSE;		/* Handle device reset errors.	*/
#endif /* defined(EEI) */
v_bool	end_of_file = FALSE;		/* End of file detected.	*/
#if defined(_NT_SOURCE)
/*
 * This workaround is being added for our Tcl/Tk test scripts.
 * The Windows TclX wait{} procedure, isn't returning exit status!
 */
bool	eof_status = FALSE;		/* Controls EOF exit status.	*/
#else /* !defined(_NT_SOURCE) */
bool	eof_status = TRUE;		/* Controls EOF exit status.	*/
#endif /* defined(_NT_SOURCE) */
vu_long	error_count;			/* Number of errors detected.	*/
u_long	error_limit = 1;		/* Number of errors tolerated.	*/
int	exit_status = SUCCESS;		/* Normal success exit status.	*/
u_long	file_limit;			/* # of tape files to process.	*/
bool	forked_flag = FALSE;		/* Forked child process flag.	*/
bool	fsync_flag = UNINITIALIZED;	/* fsync() after writes flag.	*/
off_t	file_position;			/* File position to lseek to.	*/
off_t	last_position;			/* Last position lseeked to.	*/
off_t	step_offset;			/* Step offset for disk seeks.	*/
bool	flush_flag = TRUE;		/* Flush tty input/output queue	*/
bool	keep_existing = TRUE;		/* Don't delete existing files.	*/
bool	header_flag = TRUE;		/* The log file header flag.	*/
bool	user_incr = FALSE;		/* User specified incr count.	*/
bool	user_min = FALSE;		/* User specified min size.	*/
bool	user_max = FALSE;		/* User specified max size.	*/
size_t	incr_count;			/* Record increment byte count.	*/
size_t	min_size;			/* The minimum record size.	*/
size_t	max_size;			/* The maximum record size.	*/
bool	lbdata_flag = FALSE;		/* Logical block data flag.	*/
u_int32	lbdata_addr = 0;		/* Starting logical block addr.	*/
size_t	lbdata_size = 0;		/* Logical block data size.	*/
bool	user_lbdata = FALSE;		/* User specified starting lba.	*/
bool	user_lbsize = FALSE;		/* User specified lbdata size.	*/
bool	user_position = FALSE;		/* User specified file position.*/
bool	iot_pattern = FALSE;		/* IOT test pattern selected.	*/
bool	logdiag_flag = FALSE;		/* Log diagnostic messages.	*/
bool	loopback = FALSE;		/* Loopback to the same device.	*/
bool	micro_flag = FALSE;		/* Controls micro-second delay.	*/
bool	mmap_flag = FALSE;		/* Do memory mapped file I/O.	*/
bool	modem_flag = FALSE;		/* Testing tty modem control.	*/
bool	media_changed = FALSE;		/* Shows when media changed.	*/
bool	multi_flag = FALSE;		/* Multi-volume media flag.	*/
v_int	multi_volume = 1;		/* Multi-volume media count.	*/
int	open_flags = 0;			/* Common file open flags.	*/
int	wopen_flags = 0;		/* Additional write open flags.	*/
int	ropen_mode = O_RDONLY;		/* The read open mode to use.	*/
int	wopen_mode = O_WRONLY;		/* The write open mode to use.	*/
bool	pad_check = TRUE;		/* Check data buffer pad bytes.	*/
bool	spad_check = FALSE;		/* Check short record pad bytes.*/
u_long	pass_count;			/* Number of passes completed.	*/
u_long	pass_limit = 1UL;		/* Default number of passes.	*/
u_long	skip_count;			/* # of input record to skip.	*/
u_long	seek_count;			/* # of output records to seek.	*/
large_t	record_limit;			/* Max # of records to process.	*/
vu_long records_processed;		/* # of full records processed.	*/
vu_long	partial_records;		/* # of partial records proc'ed	*/
large_t	data_limit;			/* Total data limit per pass.	*/
u_long	random_align = 0UL;		/* Random I/O offset alignment.	*/
large_t	rdata_limit = 0;		/* The random I/O data limit.	*/
large_t	total_bytes;			/* Total bytes transferred.	*/
large_t total_bytes_read;		/* Total bytes read.		*/
large_t total_bytes_written;		/* Total bytes written.		*/
vu_long	total_errors;			/* Total errors (all passes).	*/
large_t	total_files;			/* Total files (all passes).	*/
large_t total_files_read;		/* Total files read.		*/
large_t total_files_written;		/* Total files written.		*/
large_t	total_records;			/* Total records (all passes).	*/
u_long	total_partial;			/* Total partial records.	*/
u_long	warning_errors;			/* Total non-fatal error count.	*/
bool	pstats_flag = TRUE;		/* Display per pass statistics.	*/
bool	raw_flag = FALSE;		/* The read after write flag.	*/
bool	rotate_flag = FALSE;		/* Force data buffer rotating.	*/
int	rotate_offset = 0;		/* Current rotate buffer offset	*/
bool	stats_flag = TRUE;		/* Display total statistics.	*/
bool	stdin_flag = FALSE;		/* Presume not reading stdin.	*/
bool	stdout_flag = FALSE;		/* Presume not writing stdout.	*/
bool	terminating_flag = FALSE;	/* Program terminating flag.	*/
bool	ttyport_flag = FALSE;		/* Input/output is a terminal.	*/
bool	verbose_flag = TRUE;		/* Verbose messages output.	*/
bool	verify_flag = TRUE;		/* Verify the read/write data.	*/
bool	verify_only = FALSE;		/* Verify of copied data flag.	*/
char	*cmd_line;			/* Copy of our command line.	*/
char	*log_file;			/* Pointer to log file name.	*/
char	*log_buffer;			/* Pointer to log file buffer.	*/
char	*log_bufptr;			/* Pointer into log buffer.	*/
char	*msg_buffer;			/* Diagnostic message buffer.	*/
char	*input_file;			/* Pointer to input file name.	*/
char	*output_file;			/* Pointer to output file name.	*/
char	*pattern_file;			/* Pointer to pattern file name	*/
u_char	*pattern_buffer;		/* Pointer to pattern buffer.	*/
u_char	*pattern_bufptr;		/* Pointer into pattern buffer.	*/
u_char	*pattern_bufend;		/* Pointer to end of pat buffer	*/
u_char	*base_buffer;			/* Base address of data buffer.	*/
u_char	*data_buffer;			/* Pointer to data buffer.	*/
u_char	*mmap_buffer;			/* Pointer to mmapped buffer.	*/
u_char	*mmap_bufptr;			/* Pointer into mmapped buffer.	*/
u_char	*verify_buffer;			/* The data verification buffer.*/
size_t	patbuf_size;			/* The pattern buffer size.	*/
int	pattern_size;			/* User specified pattern size.	*/
int	page_size = 0;			/* Define number of bytes/page.	*/
u_int	cdelay_count = 0;		/* Delay before closing file.	*/
u_int	edelay_count = 0;		/* Delay between multiple passes*/
u_int	rdelay_count = 0;		/* Delay before reading record.	*/
u_int	sdelay_count = 0;		/* Delay before starting test.	*/
u_int	tdelay_count = 1;		/* Child terminate delay count.	*/
u_int	wdelay_count = 0;		/* Delay before writing record.	*/
u_int	random_seed = 0;		/* Seed for random # generator.	*/
bool	user_rseed = FALSE;		/* Flags user specified rseed.	*/
bool	max_capacity = FALSE;		/* Use max capacity from IOCTL.	*/
large_t	user_capacity;			/* The user set drive capacity.	*/
bool	variable_flag = FALSE;		/* Variable block size flag.	*/
bool	volumes_flag = FALSE;		/* Flags the volumes option.	*/
int	volume_limit = 0;		/* Number of volumes to process.*/
vu_long	volume_records = 1;		/* The last volume record limit.*/

enum opt softcar_opt = OPT_NONE;	/* Leave tty soft carrier alone.*/
enum flow flow_type = XON_XOFF;		/* Terminal flow type to use.	*/
enum iodir  io_dir  = FORWARD;		/* Default is forward I/O.	*/
enum iomode io_mode = TEST_MODE;	/* Default to testing mode.	*/
enum iotype io_type = SEQUENTIAL_IO;	/* Default to sequential I/O.	*/
enum dispose dispose_mode = DELETE_FILE; /* Output file dispose mode.	*/
enum onerrors oncerr_action = CONTINUE;	/* The child error action.	*/
					/* Error limit controls tests.	*/
#if defined(SOLARIS) || defined(OSFMK) || defined(__QNXNTO__)
clock_t hz;
#else
clock_t	hz = HZ;			/* Default clock ticks / second	*/
#endif

/*
 * Values for Terminal (serial) Line Testing:
 */
u_short	tty_timeout = 3*10;		/* Default tty timeout (3 sec).	*/
					/* VTIME = 0.10 second interval	*/
bool	tty_minflag = FALSE;		/* User specified VMIN value.	*/
u_short	tty_minimum = 0;		/* The tty minimum (VMIN) value	*/

int	pfd = NoFd;			/* Pattern file descriptor.	*/
char	*cmdname;			/* Pointer to our program name.	*/
char	*string;			/* Pointer to argument string.	*/

/*
 * Pointers to various device information.
 */
struct dinfo *active_dinfo;		/* Active device information.	*/
struct dinfo *input_dinfo;		/* Input device information.	*/
struct dinfo *output_dinfo;		/* Output device information.	*/
struct dtype *input_dtype;		/* The input device type info.	*/
struct dtype *output_dtype;		/* The output device type info.	*/

/*
 * System time information.
 */
clock_t start_time, end_time, pass_time; /* Per pass elapsed time.	*/
struct tms stimes, ptimes, etimes;	/* For user / system times.	*/
#if defined(DEC)
bool table_flag = FALSE;		/* Table control flag.		*/
struct tbl_sysinfo s_table, p_table, e_table;	/* Table information.	*/
#endif /* defined(DEC) */
#if defined(_BSD)
union wait child_status;		/* For child exit status.	*/
#else /* !defined(_BSD) */
int	child_status;			/* For child exit status.	*/
#endif /* defined(_BSD) */

/*
 * Program run time information.
 */
time_t	runtime;			/* The program run time.	*/
time_t	elapsed_time;			/* Amount of time program ran.	*/
time_t	program_start, program_end;	/* Program start & end times,	*/
time_t	error_time;			/* Time last error occurred.	*/
bool	TimerActive;			/* Set after timer activated.	*/
bool	TimerExpired;			/* Set after timer has expired.	*/
char	*user_runtime;			/* User specific runtime string	*/

/*
 * Data patterns used for multiple passes.
 */
u_int32 data_patterns[] = {
	DEFAULT_PATTERN,
	0x00ff00ffU,
	0x0f0f0f0fU,
	0xc6dec6deU,
	0x6db6db6dU,
	0x55555555U,
	0xaaaaaaaaU,	/* Complement of previous data pattern.		 */
	0x33333333U,	/* Continuous worst case pattern (media defects) */
	0x26673333U,	/* Frequency burst worst case pattern #1.	 */
	0x66673326U,	/* Frequency burst worst case pattern #2.	 */
	0x71c7c71cU,	/* Dibit worst case data pattern.		 */
	0x00000000U,
	0xffffffffU,
};
int npatterns = sizeof(data_patterns) / sizeof(u_int32);

/*
 * main() - Start of data transfer program.
 */
int
main (int argc, char **argv)
{
	struct dinfo *dip;
	struct dtfuncs *dtf;
	char *tmp;
	int status;

#if defined(_BSD)
	tmp = rindex (argv[0], '/');
#else /* !defined(_BSD) */
	tmp = strrchr (argv[0], '/');
#endif /* defined(_BSD) */
	cmdname = tmp ? &(tmp[1]) : argv[0];
#if defined(_QNX_SOURCE) && !defined(_QNX_32BIT)
	page_size = 0;		/* Presume page size doesn't matter. */
#elif defined(_QNX_SOURCE) && defined(_QNX_32BIT)
	page_size = 4096;	/* Not sure how to query for this... */
#elif defined(DEC) || defined(SOLARIS) || defined(__linux__) || defined(SCO) || defined(__NUTC__)
	hz = sysconf(_SC_CLK_TCK);
	page_size = sysconf(_SC_PAGESIZE);
#else /* !defined(_QNX_SOURCE) && !defined(_QNX_32BIT) */
	page_size = getpagesize();
#endif /* defined(_QNX_SOURCE) && !defined(_QNX_32BIT) */
#if defined(OSFMK) || defined(__QNXNTO__)
	hz = CLK_TCK;			/* Actually a libc function. */
#endif /* defined(OSFMK) || defined(__QNX_NTO__) */

#if defined(LOG_DIAG_INFO)
	/*
	 * Allow DT_LOG_DIAG to enable logging diagnostic information.
	 */
	if (tmp = getenv ("DT_LOG_DIAG")) {
	    logdiag_flag = TRUE;
	}
#endif /* defined(LOG_DIAG_INFO) */

	data_limit = INFINITY;		/* Set to maximum limit. */

	parse_args (argc, argv);

	/*
	 * Options parsed, validate options, do initialization, and open
	 * input & output files to be tested.
	 */
	program_start = time((time_t) 0);	/* Record our start time. */

	/*
	 * If a log file was specified, redirect stderr to that file.
	 */
	if (log_file) {
	    if (freopen (log_file, "a", stderr) == NULL) {
		report_error (log_file, TRUE);
		exit (exit_status);
	    }
	}

	/*
	 * Make stderr buffered, so timing is not affected by output.
	 */
	if ((log_buffer = (char *) malloc (LOG_BUFSIZE)) == NULL) {
	    Fprintf ("Unable to allocate log file buffer of %d bytes, exiting...\n",
								LOG_BUFSIZE);
	    exit (ENOMEM);
	}

	/*
	 * The concept here is simple, set stderr buffered so multiple processes
	 * don't have their output intermixed.  This piece of code has been very
	 * problematic, so it you have problems with garbled output, remove it.
	 */
	log_bufptr = log_buffer;
	/*
	 * Since stderr is normally unbuffered, we make it buffered here.
	 */
	if ( isatty(fileno(stderr)) ) {
	    char *stderr_buffer = (char *)malloc(LOG_BUFSIZE);
	    if (stderr_buffer == NULL) {
		Fprintf ("Unable to allocate stderr buffer of %d bytes, exiting...\n",
									LOG_BUFSIZE);
		exit (ENOMEM);
	    }
	    /*
	     * Can't use log buffer, or we get undesirable results :-)
	     */
	    if (setvbuf(stderr, stderr_buffer, _IOFBF, LOG_BUFSIZE) < 0) {
		report_error ("setvbuf", TRUE);
		exit (exit_status);
	    }
	}

	/*
	 * Write the command line to the log file, if one exists.
	 */
	if (log_file && header_flag) {
	    int arg;
	    /*
	     * Write the command line for future reference.
	     */
	    Lprintf("Command Line:\n\n    %c ", getuid() ? '%' : '#');
	    for (arg = 0; arg < argc; arg++) {
		Lprintf("%s ", argv[arg]);
	    }
	    Lprintf("\n\n\t--> %s <--\n", version_str);
	    Lflush();
	}

	if (!input_file && !output_file) {
	    Fprintf ("You must specify an input file, an output file, or both.\n");
	    exit (FATAL_ERROR);
	}

	/*
	 * Disallow both seek type options, to simplify test loops.
	 */
	if ( (io_dir == REVERSE) && (io_type == RANDOM_IO) ) {
	    Fprintf ("Please specify one of iodir=reverse or iotype=random, not both!\n");
	    exit (FATAL_ERROR);
	}

#if defined(MUNSA)
	if (munsa_flag) {

	    if (input_file && !output_file) {
	        input_munsa_lock_type =  munsa_lock_type;
		if (debug_flag) {
		    Fprintf ("input_munsa_lock_type = %d\n",
					input_munsa_lock_type);
		}
	    }

	    if (output_file && !input_file) {
		if ((munsa_lock_type == DLM_PWMODE) ||
		    (munsa_lock_type == DLM_EXMODE)) {
		    output_munsa_lock_type =  munsa_lock_type;
		    if (debug_flag) {
			Fprintf ("output_munsa_lock_type = %d\n",
					output_munsa_lock_type);
		    }
		} else {
		    Fprintf ("invalid write lock type it should be pw,ex\n");
		    exit(FATAL_ERROR);
		}
	    }

	    if (input_file && output_file) {
		input_munsa_lock_type =  DLM_PRMODE;
		output_munsa_lock_type =  DLM_PWMODE;
		if (debug_flag) {
		    Fprintf ("input_munsa_lock_type = %d\n",
					input_munsa_lock_type);
		    Fprintf ("output_munsa_lock_type = %d\n",
					output_munsa_lock_type);
		}
	    }
	}  /*   end if(munsa_flag) ....    */
#endif /* defined(MUNSA) */

	if ( (!input_file || !output_file) &&
	      ((io_mode == COPY_MODE) || (io_mode == VERIFY_MODE)) ) {
	    Fprintf ("Copy/verify modes require both input and output devices.\n");
	    exit (FATAL_ERROR);
	}

	/*
	 * When reading multiple tape files, don't require data or record
	 * limits (these will vary).  But when writing multiple tape files,
	 * we need to know how many records or bytes to be written.
	 */
	if (input_file && !output_file && file_limit && !record_limit) {
	    record_limit = INFINITY;
	}

	/*
	 * Check the variable record size parameters.
	 */
	if (min_size && !max_size) max_size = block_size;
	if (block_size < max_size) block_size = max_size;
	/* NOTE: Other checks are done below now... */

	/*
	 * Calculate the data limit if it wasn't specified by the user.
	 */
	if ( (data_limit == INFINITY) &&
	     ( (record_limit != 0L) && (record_limit != INFINITY) ) ) {
	    data_limit = (block_size * record_limit);
	}

	/*
	 * Process the pattern file (if one was specified).
	 */
	if (pattern_file) {
	    process_pfile (&pfd, pattern_file, O_RDONLY);
	}

	if (min_size && (max_size <= min_size)) {
	    Fprintf ("Please specify max count > min count for record sizes.\n");
	    exit (FATAL_ERROR);
	}

	/*
	 * Verify counts are large enough, to avoid false compare errors.
	 */
#if 0
	if ( !bypass_flag && (iot_pattern || lbdata_flag) &&
	     ((block_size < sizeof(u_int32)) ||
	      (min_size && (min_size <= sizeof(u_int32)))) ) {
	    Fprintf("Please specify block sizes > %d for IOT or Lbdata options!\n",
								sizeof(u_int32));
	    exit (FATAL_ERROR);
	}
#endif
	if ( (iot_pattern || lbdata_flag) && (block_size < lbdata_size) ) {
	    Fprintf(
	"Please specify a block size >= %u (lbdata size) for IOT or Lbdata options!\n",
								lbdata_size);
	    exit (FATAL_ERROR);
	}

	if ( ((io_mode == COPY_MODE) || (io_mode == VERIFY_MODE)) &&
	     (iot_pattern || lbdata_flag) ) {
	    Fprintf ("IOT and Lbdata options disallowed with Copy/Verify options!\n");
	    exit (FATAL_ERROR);
	}

	if (iot_pattern) {
	    size_t size = block_size;
	    u_char *buffer = (u_char *) myalloc(size, 0);
	    setup_pattern (buffer, size);
	    pattern_string = "IOT Pattern";
	    /* IOT takes precedence! */
	    lbdata_flag = FALSE;
	    user_lbdata = FALSE;
	}
#if 1
	/*
	 * Setup the pattern as a pattern string, so non-modulo
	 * sizeof(u_int) read counts will data compare properly.
	 */
	if (!pattern_buffer) {
	    size_t size = sizeof(u_int32);
	    u_char *buffer = (u_char *) Malloc (size);
	    copy_pattern (pattern, buffer);
	    setup_pattern (buffer, size);
	}
#endif

	/*
	 * The following check was added for tty loopback to same port.
	 * [ A future version may extend this support to other devices. ]
	 */
	if (input_file && output_file) {
	    if (strcmp (input_file, output_file) == 0) {
		loopback = TRUE;	/* Loopback to the same file. */
	    }
	} else if (loopback) {
	    if (!input_file && output_file) input_file = output_file;
	    if (input_file && !output_file) output_file = input_file;
	}

	/*
	 * Setup the initial device information & validate options.
	 */
	if (input_file) {
	    dip = setup_device_info (input_file, input_dtype);
	    active_dinfo = input_dinfo = dip;
	    dip->di_mode = READ_MODE;
	    dip->di_ftype = INPUT_FILE;
	    status = (*dip->di_funcs->tf_validate_opts)(dip);
	    if (status == FAILURE) exit (FATAL_ERROR);
	}
	if (output_file) {
	    dip = setup_device_info (output_file, output_dtype);
	    active_dinfo = output_dinfo = dip;
	    dip->di_mode = WRITE_MODE;
	    dip->di_ftype = OUTPUT_FILE;
	    status = (*dip->di_funcs->tf_validate_opts)(dip);
	    if (status == FAILURE) exit (FATAL_ERROR);
	    /*
	     * The following problem was resolved by not switching patterns
	     * when multiple processes are selected.  So, this will go...
	     */
#if 0
	    /*
	     * Sanity Check: Major source of problem reports due to folks
	     * using multiple processes with multiple passes, and subsequent
	     * passes overwriting the previous data pattern!
	     */
	    if ( (pass_limit > 1) && (num_procs > 1) &&
		 !user_pattern && dip->di_random_access ) {
		Fprintf (
	    "Warning: Multiple passes with multiple processes can cause unpredictable\n");
	 	Fprintf (
	    "results due to process scheduling, since each pass uses a different pattern!\n");
	    }
#endif
	}

#if defined(sun)
	/*
	 * Soft carrier existed on the Sun/386i (Roadrunner) system.
	 * Setting O_NDELAY was necessary to open the terminal line.
	 *
	 * Normally, O_NDELAY must NOT be set, otherwise the terminal
	 * driver returns EWOULDBLOCK on reads if no data is available.
	 *
	 * Note:  Without O_NDELAY, the open will hang on modem lines
	 *        (-CLOCAL) if carrier is not asserted.
	 */
	if (ttyport_flag) {
	    open_flags = O_NDELAY;	/* Incase no soft carrier.	*/
	}
#endif /* defined(sun) */

    /*
     * Do multiple slices processing.
     */
    if (num_slices) {
	if (input_file && output_file) {
	    Fprintf ("Please specify only an input or output file, not both!\n");
	    exit (FATAL_ERROR);
	}

	/*
	 * Create multiple slices (if requested).
	 */
	active_dinfo = dip = (input_file) ? input_dinfo : output_dinfo;
	if (!dip->di_random_access) {
	    Fprintf ("Multiple slices is only supported on random access devices!\n");
	    exit (FATAL_ERROR);
	}
	if ((status = FindCapacity (dip)) == FAILURE) {
	    exit (FATAL_ERROR);
	}
	/*
	 * The remaining work is done when starting the processes.
	 */
	if ( start_slices() ) {
	    await_procs();
	    exit (exit_status);
	}
    }

	/*
	 * Create multiple processes (if requested).
	 */
	if (num_procs && !loopback && !ttyport_flag) {
	    if ( start_procs() ) {
		await_procs();
		exit (exit_status);
	    }
	}

	/*
	 * Open device / Setup system / device specific test information.
	 */
	if (input_file) {
	    int open_mode = (ropen_mode | open_flags);
	    dip = active_dinfo = input_dinfo;
	    if ((*dip->di_funcs->tf_open)(dip, open_mode) == FAILURE) {
		exit (exit_status);
	    }
	    system_device_info (dip);
	    input_dtype = dip->di_dtype;

	    status = (*dip->di_funcs->tf_validate_opts)(dip);
	    if (status == FAILURE) exit (FATAL_ERROR);

#if defined(MUNSA)
	    if (munsa_flag) {
		/* first we need to join a namespace */

		l_stat = dlm_nsjoin( getuid(), &nsp, DLM_USER);
		if ((l_stat != DLM_SUCCESS) && (l_stat != DLM_ATTACHED)) {
		    Fprintf ("Can't join namespace\n");
		    dlm_error(0, l_stat);
		}

		resnam = input_file;
		/* now let DLM know what signal to use for blocking routines */
		dlm_set_signal(SIGIO, &i);
		resnlen = strlen(resnam);
		if (debug_flag) {
		    Fprintf ("dlm_set_signal: i %d\n", i);
		    Fprintf ("resnam %s\n", resnam);
		    Fprintf ("grab a NL mode lock\n");
		}
		l_stat = dlm_lock(nsp, 
				  (uchar_t *)resnam,
				  resnlen,
				  0,
				  &lkid,
				  DLM_NLMODE,
				  NULL,
				  DLM_SYNCSTS,
				  0, 0, 0, 0);
		/* NL mode better be granted SYNC status */
		if (l_stat !=  DLM_SYNCH) {
		    if (debug_flag) {
			Fprintf ("dlm_lock failed\n");
		    }
		    dlm_error(&lkid, l_stat);
		}
	    }  /*  end if(munsa_flag)... */
#endif /* defined(MUNSA) */

	    /*
	     * If disk device and random I/O selected, attempt to get
	     * device / partition capacity to limit random I/O seeks.
	     */
	    if ( user_capacity ||
		( ((io_dir == REVERSE) || (io_type == RANDOM_IO)) &&
		  (dip->di_random_access && !num_slices) ) ) {
		if ((status = FindCapacity (dip)) == FAILURE) {
		    exit (exit_status);
		}
	    }
	    if (!record_limit) {
		record_limit = INFINITY;	/* Read until EOF on reads. */
	    }
	}

	/*
	 * Process the output device/file.
	 */
	if (output_file) {
	    int open_mode;
	    dip = active_dinfo = output_dinfo;
	    /*
	     * If a skip count was specified, open output file for R/W,
	     * since skips are accomplished via read()'s. (for pelle)
	     */
	    if (skip_count || raw_flag) {
		open_mode = (O_RDWR  | wopen_flags | open_flags);
	    } else {
		open_mode = (wopen_mode | wopen_flags | open_flags);
	    }
	    /*
	     * Don't create files in the /dev directory (presume the file
	     * should exist instead of creating file & misleading user).
	     */
	    if ( (NEL (output_file, DEV_PREFIX, DEV_LEN)) &&
		 (NEL (output_file, ADEV_PREFIX, ADEV_LEN)) ) {
		open_mode |= O_CREAT;	/* For creating test files.	*/
	    }
	    /*
	     * If verify mode, the output device is open for reads.
	     */
	    if (io_mode == VERIFY_MODE) {
		open_mode = (ropen_mode | open_flags);
		dip->di_mode = READ_MODE;
	    }
	    if ((*dip->di_funcs->tf_open)(dip, open_mode) == FAILURE) {
		exit (exit_status);
	    }

#if defined(MUNSA)
	    if (munsa_flag) {
		/* first we need to join a namespace */
		l_stat = dlm_nsjoin( getuid(), &nsp, DLM_USER);
		if ((l_stat != DLM_SUCCESS) && (l_stat != DLM_ATTACHED)) {
		    Fprintf ("Can't join namespace\n");
		    dlm_error(0, l_stat);
		}

		resnam = output_file;
		/* now let DLM know what signal to use for blocking routines */
		dlm_set_signal(SIGIO, &i);  /* do we need this ????? */

		resnlen = strlen(resnam);
		if (debug_flag) {
		    Fprintf ("dlm_set_signal: i %d\n", i);
		    Fprintf ("resnam %s\n", resnam);
		    Fprintf ("grab a NL mode lock\n");
		}
		l_stat = dlm_lock(nsp, 
				  (uchar_t *)resnam, 
				  resnlen,
				  0,
				  &lkid,
				  DLM_NLMODE,
				  NULL,
				  DLM_SYNCSTS,
				  0, 0, 0, 0);
		/* NL mode better be granted SYNC status */
		if (l_stat !=  DLM_SYNCH) {
		    Fprintf ("dlm_lock failed\n");
		    dlm_error(&lkid, l_stat);
		}
	    }  /* end if (munsa_flag)... */
#endif /* defined(MUNSA) */

	    system_device_info (dip);
	    output_dtype = dip->di_dtype;
	    open_flags &= ~O_CREAT;	/* Only create on first open. */

	    status = (*dip->di_funcs->tf_validate_opts)(dip);
	    if (status == FAILURE) exit (FATAL_ERROR);

	    /*
	     * If disk device and random I/O selected, attempt to get
	     * device / partition capacity to limit random I/O seeks.
	     */
	    if ( user_capacity ||
		( ((io_dir == REVERSE) || (io_type == RANDOM_IO)) &&
		  (dip->di_random_access && !num_slices) ) ) {
		if ((status = FindCapacity (dip)) == FAILURE) {
		    exit (exit_status);
		}
	    }
	    /*
	     * For disks and tapes, default writing until EOF is reached.
	     */
	    if ( !record_limit &&
		 ((dip->di_dtype->dt_dtype == DT_DISK)  ||
		  (dip->di_dtype->dt_dtype == DT_BLOCK) ||
		  (dip->di_dtype->dt_dtype == DT_TAPE)) ) {
		record_limit = INFINITY;
	    }
	}

	/*
	 * Set the default lbdata size, if not setup by the system
	 * dependent functions above.  Delaying this check to this
	 * point allows the device sector size to be setup, instead
	 * of forcing it to 512 byte blocks.  At least this is true
	 * on Tru64 Unix, where this disk information is available.
	 */
	if (!lbdata_size) lbdata_size = BLOCK_SIZE;

	/*
	 * This is to catch me (dah!) as much as anyone else :-)
	 */
	if ( (rdata_limit || random_align) &&
	     ((io_dir != REVERSE) && (io_type != RANDOM_IO)) ) {
	    Fprintf ("Warning, random options have no effect without iotype=random!\n");
	}

	/*
	 * If random I/O was selected, and a data limit isn't available,
	 * inform the user we need one, and don't allow testing.
	 */
	if (rdata_limit == 0UL) rdata_limit = data_limit;
	if ( (rdata_limit == 0) && (io_type == RANDOM_IO) ) {
	    Fprintf ("Please specify a record or data limit for random I/O.\n");
	    exit (FATAL_ERROR);
	}

	/*
	 * Sanity check the random I/O data limits.
	 */
	if ( (io_type == RANDOM_IO) &&
	     ((file_position + block_size + random_align) > rdata_limit)) {
	    Fprintf ("The max block size is too large for random data limits!\n");
	    exit (FATAL_ERROR);
	}

	/*
	 * Ensure either a data limit and/or a record count was specified.
	 */
	if (!record_limit) {
	    Fprintf ("You must specify a data limit, a record count, or both.\n");
	    exit (FATAL_ERROR);
	}

#if defined(LOG_DIAG_INFO)
	/*
	 * If logging of diagnostic info is enabled, save copy of cmd line.
	 */
	if (logdiag_flag) {
	    int arg;
	    char *bp;
	    bp = cmd_line = (char *)Malloc(LOG_BUFSIZE);
	    msg_buffer = (char *)Malloc(LOG_BUFSIZE);
	    for (arg = 0; arg < argc; arg++) {
		(void)sprintf(bp, "%s ", argv[arg]);
		bp += strlen(bp);
	    }
	    sprintf(bp, "\n");
	}
#endif /* defined(LOG_DIAG_INFO) */

	/*
	 * Catch a couple signals to do elegant cleanup.
	 */
	(void) signal (SIGHUP, terminate);
	(void) signal (SIGINT, terminate);
	(void) signal (SIGTERM, terminate);
	(void) signal (SIGPIPE, terminate);

	/*
	 * If both an input and an output files were specified, then
	 * fork and make child process the reader, parent the writer.
	 */
	if ( (io_mode == TEST_MODE) && input_file && output_file) {
	    if ( (child_pid = fork()) == (pid_t) -1) {
		report_error ("fork", TRUE);
		exit (exit_status);
	    }
	    forked_flag = TRUE;
	    if (child_pid) {			/* Parent = writer. */
		struct dinfo *dip = input_dinfo;
		(void) close_file (dip);
		input_file = NULL;
		if (debug_flag) {
		    Fprintf ("Parent PID (Writer) = %d, Child PID (Reader) = %d\n",
						getpid(), child_pid);
		}
#if !defined(__MSDOS__) || defined(__NUTC__)
		signal (SIGCHLD, terminate);
#endif
	    } else {				/* Child = reader. */
		struct dinfo *dip = output_dinfo;
		(void) close_file (dip);
		output_file = NULL;
	    }
	}

	/*
	 * Some drivers require the input device to open before we start
	 * writing.  For example, terminal devices must have speed, parity,
	 * and flow control setup before we start writing.  The parallel
	 * input device must open before we send the "readya" interrupt.
	 */
	if (output_file && (sdelay_count != 0)) {
	    mySleep (sdelay_count);		/* Allow reader to start. */
	}

	/*
	 * Calculate size necessary for the data buffer & the pad bytes.
	 */
	data_size = (block_size + PADBUFR_SIZE);
	if (rotate_flag) data_size += ROTATE_SIZE;

	/*
	 * For read-after-write (raw) option, we need a verification buffer.
	 * Note:  Other tests can benefit from this too, switch to later.
	 */
	if (raw_flag) {
	    verify_buffer = myalloc(data_size, align_offset);
	}

	/*
	 * Do the device / test specific initialization.
	 *
	 * This function is responsible for allocating the necessary
	 * data buffers and performing special device setup/checking.
	 */
	if (input_file) {
	    dip = input_dinfo;
	    status = (*dip->di_funcs->tf_initialize)(dip);
	    if (status == FAILURE) exit (FATAL_ERROR);
	}
	if (output_file) {
	    dip = output_dinfo;
	    status = (*dip->di_funcs->tf_initialize)(dip);
	    if (status == FAILURE) exit (FATAL_ERROR);
	}

	/*
	 * Re-adjust size of data buffer to avoid init'ing too much.
	 */
	if (rotate_flag) data_size -= ROTATE_SIZE;

	/*
	 * Start an alarm timer if the run time was specified.
	 */
	if (runtime) {
	    (void) signal (SIGALRM, terminate);
	    (void) alarm (runtime);
	    TimerActive = TRUE;
	}

	/*
	 * Start of main test loop.
	 */
	start_time = times (&stimes);
#if defined(LOG_DIAG_INFO)
	if (logdiag_flag) {
	    sprintf(msg_buffer, "Starting: %s", cmd_line);
	    LogDiagMsg(msg_buffer);
	}
#endif /* defined(LOG_DIAG_INFO) */
#if defined(DEC)
	if (table_flag) {
	    status = table(TBL_SYSINFO,0,(char *)&s_table,1,sizeof(struct tbl_sysinfo));
	    if (status == FAILURE) report_error ("table", FALSE);
	}
#endif /* defined(DEC) */
    /*
     * We support multiple modes of operation: copy, test, & verify
     */
    if ( (io_mode == COPY_MODE) || (io_mode == VERIFY_MODE) ) {
	/*
	 * This is ugly, but I'm using pattern buffer as verify buffer.
	 */
	if ( (io_mode == VERIFY_MODE) || verify_flag) {
	    pattern_buffer = myalloc (data_size, align_offset);
	    setup_pattern (pattern_buffer, data_size);
	}
	pass_time = times (&ptimes);	/* Start the pass timer	*/
#if defined(DEC)
	if (table_flag) {
	    status = table(TBL_SYSINFO,0,(char *)&p_table,1,sizeof(struct tbl_sysinfo));
	    if (status == FAILURE) report_error ("table", FALSE);
	}
#endif /* defined(DEC) */
	dip = active_dinfo = input_dinfo;
	dtf = dip->di_funcs;
	dip->di_mode = READ_MODE;
	(void) (*dtf->tf_start_test)(dip);
	(void) (*dtf->tf_read_file)(dip);
	(void) (*dtf->tf_end_test)(dip);
	gather_stats(output_dinfo);

	/*
	 * Now verify the data copied (if requested).
	 */
	if ( (io_mode == COPY_MODE) && verify_flag &&
	     !stdin_flag && (error_count < error_limit) ) {
	    struct dinfo *odip = output_dinfo;
	    int open_mode = (ropen_mode | open_flags);
	    report_pass (dip, COPY_STATS);	/* Report copy statistics. */
	    /*
	     * Verify Pass.
	     */
	    status = (*dtf->tf_reopen_file)(dip, open_mode);
	    if (status != SUCCESS) terminate(status);
	    odip->di_mode = READ_MODE;	/* Switch to read mode. */
	    status = (*odip->di_funcs->tf_reopen_file)(odip, open_mode);
	    if (status != SUCCESS) terminate(status);
	    pass_time = times (&ptimes); /* Time the verify. */
#if defined(DEC)
	    if (table_flag) {
		status = table(TBL_SYSINFO,0,(char *)&p_table,1,sizeof(struct tbl_sysinfo));
		if (status == FAILURE) report_error ("table", FALSE);
	    }
#endif /* defined(DEC) */
	    io_mode = VERIFY_MODE;
	    (void) (*dtf->tf_start_test)(dip);
	    (void) (*dtf->tf_read_file)(dip);
	    (void) (*dtf->tf_end_test)(dip);
	    pass_count++;			/* End of copy/verify pass. */
	    gather_stats(output_dinfo);		/* Gather device statistics. */
	    report_pass (dip, VERIFY_STATS);	/* Report the verify stats. */
	} else {
	    pass_count++;			/* End of copy pass. */
	}
    } else { /* not Copy or Verify modes, Test Mode! */
	while ( (total_errors < error_limit) &&
		((pass_count < pass_limit) || runtime) ) {
	    if (pattern_buffer) {
		pattern_bufptr = pattern_buffer;
	    }
	    /*
	     * Use a different data pattern for each pass.
	     */
	    if ( !user_pattern && !ttyport_flag &&
		 (loopback || output_file || stdin_flag ||
		  (input_file && num_slices)) ) {
		/*
		 * Logic:
		 * - If multiple slices, choose a different pattern
		 *   for each pass, factoring in the process number.
		 * - Else, each pass gets a different data pattern
		 *   unless multiple processes were selected.
		 */
		if ( unique_pattern &&
		     ( (!num_procs && num_slices) ||
		       (num_procs && (dip->di_dtype->dt_dtype == DT_REGULAR)) ) ) {
		    int pindex = ((cur_proc - 1) + pass_count);
		    pattern = data_patterns[pindex % npatterns];
		} else if (!num_procs) {
		    pattern = data_patterns[pass_count % npatterns];
		}
		if (pattern_buffer) copy_pattern (pattern, pattern_buffer);
		if (debug_flag) {
		    Fprintf ("Using data pattern 0x%08x for pass %u\n",
						pattern, (pass_count + 1));
		}
	    }
	    /*
	     * Use time for random # generator seed so different areas
	     * of disk get affected during multiple passes.  Seed set
	     * here, since it must be the same seed during read pass.
	     * NOTE: Pid is added so seed different for each process.
	     */
	    if ( (io_type == RANDOM_IO) || variable_flag) {
		if ( !user_rseed ) {
		    random_seed = (u_int) times(&ptimes) + getpid();
		}
		set_rseed (random_seed);
	    }
	    pass_time = times (&ptimes);	/* Start the pass timer	*/
#if defined(DEC)
	    if (table_flag) {
		status = table(TBL_SYSINFO,0,(char *)&p_table,1,sizeof(struct tbl_sysinfo));
		if (status == FAILURE) report_error ("table", FALSE);
	    }
#endif /* defined(DEC) */
	    if (output_file) {			/* Write/read the file.	*/
		bool do_read_pass;
		dip = active_dinfo = output_dinfo;
		dtf = dip->di_funcs;
		dip->di_mode = WRITE_MODE;
		(void) (*dtf->tf_start_test)(dip);
		(void) (*dtf->tf_write_file)(dip);
		(void) (*dtf->tf_flush_data)(dip);
		(void) (*dtf->tf_end_test)(dip);
		if (error_count >= error_limit) break;
		do_read_pass = (dip->di_dbytes_written != (large_t) 0);
		/*
		 * Now verify (read and data compare) the data just written.
		 */
		if (verify_flag && do_read_pass && !raw_flag) { /* Verify data written. */
		    int open_mode = (ropen_mode | open_flags);
		    report_pass (dip, WRITE_STATS);	/* Report write stats.	*/
		    if (multi_flag && media_changed) {
			status = RequestFirstVolume(dip, open_flags);
		    } else {
			status = (*dtf->tf_reopen_file)(dip, open_mode);
		    }
		    if (status != SUCCESS) break;
		    if ( (io_type == RANDOM_IO) || variable_flag) {
			set_rseed (random_seed);
		    }
		    pass_time = times (&ptimes);	/* Time just the read.	*/
#if defined(DEC)
		    if (table_flag) {
			status = table(TBL_SYSINFO,0,(char *)&p_table,1,sizeof(struct tbl_sysinfo));
			if (status == FAILURE) report_error ("table", FALSE);
		    }
#endif /* defined(DEC) */
		    /*rotate_offset = 0;*/
		    if (pattern_buffer) {
			pattern_bufptr = pattern_buffer;
		    }
		    dip->di_mode = READ_MODE;
		    (void) (*dtf->tf_start_test)(dip);
		    (void) (*dtf->tf_read_file)(dip);
		    (void) (*dtf->tf_end_test)(dip);
		    pass_count++;			/* End read/write pass. */
		    report_pass (dip, READ_STATS);	/* Report read stats.	*/
		} else {
		    pass_count++;		/* End of write pass.	*/
		    if ( (pass_limit > 1) || runtime) {
			/* Report write stats. */
			if (raw_flag) {
			    report_pass (dip, RAW_STATS);
			} else {
			    report_pass (dip, WRITE_STATS);
			}
		    }
		}
		if ( (pass_count < pass_limit) || runtime) {
		    int open_mode;
		    if (skip_count || raw_flag) {
			open_mode = (O_RDWR | wopen_flags | open_flags);
		    } else {
			open_mode = (wopen_mode | wopen_flags | open_flags);
		    }
		    status = (*dtf->tf_reopen_file)(dip, open_mode);
		    if (status != SUCCESS) break;
		}
	    } else { /* Reading only. */
		dip = active_dinfo = input_dinfo;
		dtf = dip->di_funcs;
		dip->di_mode = READ_MODE;
		(void) (*dtf->tf_start_test)(dip);
		(void) (*dtf->tf_read_file)(dip);
		(void) (*dtf->tf_end_test)(dip);
		pass_count++;			/* End of read pass.	*/
		/*
		 * Prevent pass unless looping, since terminate reports
		 * the total statistics when called (prevents dup stats).
		 */
		if ( (pass_limit > 1) || runtime) {
		    report_pass (dip, READ_STATS);	/* Report read stats.	*/
		}
		if ( (total_errors < error_limit) &&
		     ((pass_count < pass_limit) || runtime) ) {
		    int open_mode = (ropen_mode | open_flags);
		    status = (*dtf->tf_reopen_file)(dip, open_mode);
		    if (status != SUCCESS) break;
		}
	    }
#if defined(MUNSA)
	    if (munsa_flag) {
		if (debug_flag) {
		    Fprintf ("converting to dlm NL-> %d \n", DLM_NLMODE);
		}
		l_stat = dlm_cvt(&lkid, DLM_NLMODE, NULL, 0, 0, 0, NULL, 0);
		if (l_stat !=  DLM_SUCCESS) {
		    Fprintf ("dlm_cvt failed\n");
		    dlm_error(&lkid, l_stat);	/* exit with FATAL ERROR */ 
		}
	    } /* end if(munsa_flag)... */
#endif /* defined(MUNSA) */
	}
    } /* end 'if ( (io_mode == COPY_MODE) || (io_mode == VERIFY_MODE) ) ' */

#if defined(MUNSA)
    if (munsa_flag) {
	if ((l_stat = dlm_unlock(&lkid, NULL, 0)) != DLM_SUCCESS)
	    dlm_error(&lkid, l_stat);  /* exit with FATAL ERROR  */

	if (debug_flag) {
	    Fprintf("\n   %s:  unlocked...\n", resnam);
	}
    }  /*  end if(munsa_flag)...  */
#endif /* defined(MUNSA) */

    terminate (exit_status);		/* Terminate with exit status.	*/
    /*NOTREACHED*/
    return (exit_status);		/* Quiet certain compilers!	*/
}

#if defined(MUNSA)
/************************************************************************
 *									*
 * dlm_error() - Print all MUNSA dlm related error msg.			*
 *									*
 * Inputs:	dlm_lkid_t       *lk                            	*
 *		dlm_status_t      stat			                *
 *									*
 * 	If errors are detected, we simply exit with a fatal error.	*
 *									*
 ************************************************************************/
void
dlm_error(dlm_lkid_t *lk, dlm_status_t err_stat)
{
	dlm_rsbinfo_t rsb;		/* used by dlm_sperrno() */

	Fprintf ("lock error %s on lkid 0x%lx\n",
			dlm_sperrno(err_stat), *lk);
	exit (FATAL_ERROR);
}
#endif /* defined(MUNSA) */

/************************************************************************
 *									*
 * parse_args() - Parse 'dt' Program Arguments.				*
 *									*
 * Inputs:	argc = The number of arguments.				*
 *		argv = Array pointer to arguments.			*
 *									*
 * 	If errors are detected, we simply exit with a fatal error.	*
 *									*
 ************************************************************************/
void
parse_args (int argc, char **argv)
{
	int i;

	if (argc == 1) dtusage();
	for (i = 1; i < argc; i++) {
	    string = argv[i];
#if defined(AIO)
	    if (match ("aios=")) {
		aio_bufs = (int)number(ANY_RADIX);
		if (aio_bufs) aio_flag = TRUE;
		continue;
	    }
#endif /* defined(AIO) */
	    if (match ("align=")) {
		if (match ("rotate")) {
		    rotate_flag = TRUE;
		    continue;
		}
		align_offset = (u_int) number(ANY_RADIX);
		continue;
	    }
	    if (match ("bs=")) {
		block_size = number(ANY_RADIX);
		if ((ssize_t)block_size <= (ssize_t) 0) {
		    Fprintf ("block size must be positive and non-zero.\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("capacity=")) {
		if (match ("max")) {
		    max_capacity = TRUE;
		} else {
		    user_capacity = large_number(ANY_RADIX);
		}
		continue;
	    }
	    if (match ("dsize=")) {
		device_size = number(ANY_RADIX);
		continue;
	    }
	    if (match ("lba=")) {
		lbdata_flag = TRUE;
		lbdata_addr = number(ANY_RADIX);
		user_lbdata = TRUE;
		continue;
	    }
	    if (match ("lbs=")) {
		lbdata_flag = TRUE;
		lbdata_size = number(ANY_RADIX);
		user_lbsize = TRUE;
		if ((ssize_t)lbdata_size <= (ssize_t) 0) {
		    Fprintf ("lbdata size must be positive and non-zero.\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if ( (match ("count=")) || (match ("records=")) ) {
		record_limit = number(ANY_RADIX);
		continue;
	    }
	    if (match ("cdelay=")) {
		cdelay_count = (u_int) number(ANY_RADIX);
		continue;
	    }
	    if (match ("edelay=")) {
		edelay_count = (u_int) number(ANY_RADIX);
		continue;
	    }
	    if (match ("rdelay=")) {
		rdelay_count = (u_int) number(ANY_RADIX);
		continue;
	    }
	    if (match ("sdelay=")) {
		sdelay_count = (u_int) number(ANY_RADIX);
		continue;
	    }
	    if (match ("tdelay=")) {
		tdelay_count = (u_int) number(ANY_RADIX);
		continue;
	    }
	    if (match ("wdelay=")) {
		wdelay_count = (u_int) number(ANY_RADIX);
		continue;
	    }
	    if (match ("errors=")) {
		error_limit = number(ANY_RADIX);
		continue;
	    }
	    if (match ("files=")) {
		file_limit = number(ANY_RADIX);
		continue;
	    }
	    if (match ("hz=")) {
		hz = (u_int) number(ANY_RADIX);
		continue;
	    }
	    if (match ("incr=")) {
		user_incr = TRUE;
		if (match ("var")) {
		    variable_flag = TRUE;
		} else {
		    incr_count = number(ANY_RADIX);
		}
		continue;
	    }
	    if (match ("dlimit=")) {
		dump_limit = number(ANY_RADIX);
		continue;
	    }
	    if (match ("limit=")) {
		data_limit = large_number(ANY_RADIX);
		if (!record_limit) {
		    record_limit = INFINITY;
		}
		continue;
	    }
	    if (match ("ralign=")) {
		io_type = RANDOM_IO;
		random_align = number(ANY_RADIX);
		continue;
	    }
	    if (match ("rlimit=")) {
		io_type = RANDOM_IO;
		rdata_limit = large_number(ANY_RADIX);
		continue;
	    }
	    if (match ("max=")) {
		user_max = TRUE;
		max_size = (size_t) number(ANY_RADIX);
		continue;
	    }
	    if (match ("min=")) {
		user_min = TRUE;
		min_size = (size_t) number(ANY_RADIX);
		continue;
	    }
	    if (match ("enable=")) {
	    eloop:
		if (match(","))
		    goto eloop;
		if (*string == '\0')
		    continue;
#if defined(AIO)
		if (match("aio")) {
		    aio_flag = TRUE;
		    goto eloop;
		}
#endif /* defined(AIO) */
		if (match("bypass")) {
		    bypass_flag = TRUE;
		    goto eloop;
		}
		if (match("cerrors")) {
		    cerrors_flag = TRUE;
		    goto eloop;
		}
		if (match("compare")) {
		    compare_flag = TRUE;
		    goto eloop;
		}
		if (match("coredump")) {
		    core_dump = TRUE;
		    goto eloop;
		}
		if (match("debug")) {
		    debug_flag = TRUE;
		    goto eloop;
		}
		if (match("Debug")) {
		    Debug_flag = debug_flag = TRUE;
		    goto eloop;
		}
		if (match("edebug")) {
		    eDebugFlag = TRUE;
		    goto eloop;
		}
		if (match("rdebug")) {
		    rDebugFlag = TRUE;
		    goto eloop;
		}
		if (match("diag")) {
		    logdiag_flag = TRUE;
		    goto eloop;
		}
		if (match("dump")) {
		    dump_flag = TRUE;
		    goto eloop;
		}
		if (match("eof")) {
		    eof_status = TRUE;
		    goto eloop;
		}
#if defined(EEI)
		if (match("eei")) {
		    eei_flag = TRUE;
		    goto eloop;
		}
		if (match("resets")) {
		    eei_resets = TRUE;
		    goto eloop;
		}
#endif /* defined(EEI) */
		if (match("flush")) {
		    flush_flag = TRUE;
		    goto eloop;
		}
		if (match("fsync")) {
		    fsync_flag = TRUE;
		    goto eloop;
		}
		if (match("header")) {
		    header_flag = TRUE;
		    goto eloop;
		}
		if (match("lbdata")) {
		    lbdata_flag = TRUE;
		    goto eloop;
		}
		if (match("loopback")) {
		    loopback = TRUE;
		    verify_flag = FALSE;
		    goto eloop;
		}
		if (match("microdelay")) {
		    micro_flag = TRUE;
		    goto eloop;
		}
#if defined(MMAP)
		if (match("mmap")) {
		    mmap_flag = TRUE;
		    wopen_mode = O_RDWR;	/* MUST open read/write. */
		    goto eloop;
		}
#endif /* defined(MMAP) */
		if (match("modem")) {
		    open_flags = O_NONBLOCK;
		    modem_flag = TRUE;
		    goto eloop;
		}
		if (match("multi")) {
		    multi_flag = TRUE;
		    goto eloop;
		}
		if (match("pstats")) {
		    pstats_flag = TRUE;
		    goto eloop;
		}
		if (match("raw")) {
		    raw_flag = TRUE;
		    goto eloop;
		}
		if (match("spad")) {
		    spad_check = TRUE;
		    goto eloop;
		}
#if defined(sun)
		if (match("softcar")) {
		    open_flags = O_NDELAY;
		    softcar_opt = ON;
		    goto eloop;
		}
#endif /* defined(sun) */
#if defined(DEC)
		if (match("table")) {
		    table_flag = TRUE;
		    goto eloop;
		}
#endif /* defined(DEC) */
		if (match("ttyport")) {
		    ttyport_flag = TRUE;
		    goto eloop;
		}
		if (match("unique")) {
		    unique_pattern = TRUE;
		    goto eloop;
		}
		if (match("verbose")) {
		    verbose_flag = TRUE;
		    goto eloop;
		}
		if (match("verify")) {
		    verify_flag = TRUE;
		    goto eloop;
		}
		Fprintf ("Invalid enable keyword: %s\n", string);
		exit (FATAL_ERROR);
	    }
	    if (match ("disable=")) {
	    dloop:
		if (match(","))
		    goto dloop;
		if (*string == '\0')
		    continue;
#if defined(AIO)
		if (match("aio")) {
		    aio_flag = FALSE;
		    goto dloop;
		}
#endif /* defined(AIO) */
		if (match("bypass")) {
		    bypass_flag = FALSE;
		    goto dloop;
		}
		if (match("cerrors")) {
		    cerrors_flag = FALSE;
		    goto dloop;
		}
		if (match("compare")) {
		    compare_flag = FALSE;
		    goto dloop;
		}
		if (match("diag")) {
		    logdiag_flag = FALSE;
		    goto dloop;
		}
		if (match("dump")) {
		    dump_flag = TRUE;
		    goto dloop;
		}
		if (match("eof")) {
		    eof_status = FALSE;
		    goto dloop;
		}
#if defined(EEI)
		if (match("eei")) {
		    eei_flag = FALSE;
		    goto dloop;
		}
		if (match("resets")) {
		    eei_resets = FALSE;
		    goto dloop;
		}
#endif /* defined(EEI) */
		if (match("flush")) {
		    flush_flag = FALSE;
		    goto dloop;
		}
		if (match("fsync")) {
		    fsync_flag = FALSE;
		    goto dloop;
		}
		if (match("header")) {
		    header_flag = FALSE;
		    goto dloop;
		}
		if (match("lbdata")) {
		    lbdata_flag = FALSE;
		    user_lbdata = FALSE;
		    goto dloop;
		}
		if (match("loopback")) {
		    loopback = FALSE;
		    goto dloop;
		}
		if (match("microdelay")) {
		    micro_flag = FALSE;
		    goto dloop;
		}
#if defined(MMAP)
		if (match("mmap")) {
		    mmap_flag = FALSE;
		    goto dloop;
		}
#endif /* defined(MMAP) */
		if (match("modem")) {
		    open_flags &= ~(O_NONBLOCK);
		    modem_flag = FALSE;
		    goto dloop;
		}
		if (match("pad")) {
		    pad_check = FALSE;
		    goto dloop;
		}
		if (match("pstats")) {
		    pstats_flag = FALSE;
		    goto dloop;
		}
		if (match("raw")) {
		    raw_flag = FALSE;
		    goto dloop;
		}
		if (match("spad")) {
		    spad_check = FALSE;
		    goto dloop;
		}
		if (match("softcar")) {
		    softcar_opt = OFF;
		    goto dloop;
		}
		if (match("stats")) {
		    stats_flag = FALSE;
		    goto dloop;
		}
#if defined(DEC)
		if (match("table")) {
		    table_flag = FALSE;
		    goto dloop;
		}
#endif /* defined(DEC) */
		if (match("unique")) {
		    unique_pattern = FALSE;
		    goto dloop;
		}
		if (match("verbose")) {
		    verbose_flag = FALSE;
		    goto dloop;
		}
		if (match("verify")) {
		    verify_flag = FALSE;
		    goto dloop;
		}
		Fprintf ("Invalid disable keyword: %s\n", string);
		exit (FATAL_ERROR);
	    }
	    if (match ("dispose=")) {
		if (match("delete")) {
		    keep_existing = FALSE;
		    dispose_mode = DELETE_FILE;
		} else if (match("keep")) {
		    keep_existing = TRUE;
		    dispose_mode = KEEP_FILE;
		} else {
		    Fprintf ("Dispose modes are 'delete' or 'keep'.\n", string);
		    exit (FATAL_ERROR);
		}
		continue;
	    }
#if defined(MUNSA)
	    if (match ("munsa=")) {
		munsa_flag = TRUE;
		flow_str = string;
		if (match("cr")) {
		    munsa_lock_type = DLM_CRMODE;  /* Concurrent Read */
		} else if (match("pr")) {
		    munsa_lock_type = DLM_PRMODE;  /* Protected Read */
		} else if (match("cw")) {
		    munsa_lock_type = DLM_CWMODE;  /* Concurrent Write */
		} else if (match("pw")) {
		    munsa_lock_type = DLM_PWMODE;  /* Protected Write */
		} else if (match("ex")) {
		    munsa_lock_type = DLM_EXMODE;  /* EXclusive mode */
		} else {
		    Fprintf ("Invalid munsa lock type it must be 'cr', 'pr','cw', 'pw', or 'ex'.\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
#endif /* defined(MUNSA) */
	    if (match ("flow=")) {
		flow_str = string;
		if (match("none")) {
		    flow_type = FLOW_NONE;	/* No flow control. */
		} else if (match("cts_rts")) {
		    flow_type = CTS_RTS;	/* CTS/RTS flow control. */
		} else if (match("xon_xoff")) {
		    flow_type = XON_XOFF;	/* XON/XOFF flow control. */
		} else {
		    Fprintf ("Flow types are 'none', 'cts_rts', or 'xon_xoff'.\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("if=")) {
		input_file = string;
		continue;
	    }
	    if (match ("of=")) {
		output_file = string;
		continue;
	    }
	    if (match ("pf=")) {
		pattern_file = string;
		user_pattern = TRUE;
		continue;
	    }
	    if (match ("log=")) {
		log_file = string;
		continue;
	    }
	    if (match ("iodir=")) {
		if (match ("for")) {
		    io_dir = FORWARD;
		} else if (match ("rev")) {
		    io_dir = REVERSE;
		} else {
		    Fprintf ("Valid I/O directions are: 'forward' or 'reverse'.\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("iomode=")) {
		if (match ("copy")) {
		    dispose_mode = KEEP_FILE;
		    io_mode = COPY_MODE;
		} else if (match ("test")) {
		    io_mode = TEST_MODE;
		} else if (match ("verify")) {
		    io_mode = VERIFY_MODE;
		    verify_only = TRUE;
		} else {
		    Fprintf ("Valie I/O modes are: 'copy', 'test', or verify.\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("iotype=")) {
		if (match ("random")) {
		    io_type = RANDOM_IO;
		} else if (match ("sequential")) {
		    io_type = SEQUENTIAL_IO;
		} else {
		    Fprintf ("Valid I/O types are: 'random' or 'sequential'.\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    /*
	     * Flags which apply to read and write of a file.
	     *
	     * NOTE: I'm not sure all of flags applying to write only!
	     */
	    if (match ("flags=")) {
	    floop:
		if (match(","))
		    goto floop;
		if (*string == '\0')
		    continue;
#if defined(O_EXCL)
		if (match("excl")) {
		    open_flags |= O_EXCL;	/* Exclusive open. */
		    goto floop;
		}
#endif /* defined(O_EXCL) */
#if defined(O_NDELAY)
		if (match("ndelay")) {
		    open_flags |= O_NDELAY;	/* Non-delay open. */
		    goto floop;
		}
#endif /* defined(O_NDELAY) */
#if defined(O_NONBLOCK)
		if (match("nonblock")) {
		    open_flags |= O_NONBLOCK;	/* Non-blocking open. */
		    goto floop;
		}
#endif /* defined(O_NONBLOCK) */
#if defined(O_CACHE)
		if (match("cache")) {		/* QNX specific. */
		    open_flags |= O_CACHE;	/* Keep data in cache. */
		    goto floop;
		}
#endif /* defined(O_CACHE) */
#if defined(O_DIRECT)
		if (match("direct")) {		/* LINUX specific. */
		    open_flags |= O_DIRECT;	/* Direct disk access. */
		    goto floop;
		}
#endif /* defined(O_DIRECT) */
#if defined(O_DIRECTIO)
		if (match("direct")) {		/* Tru64 Unix (zulu). */
		    open_flags |= O_DIRECTIO;	/* Directio disk access. */
		    goto floop;
		}
#endif /* defined(O_DIRECTIO) */
#if defined(O_FSYNC)
		if (match("fsync")) {		/* File integrity. */
		    open_flags |= O_FSYNC;	/* Syncronize file I/O. */
		    goto floop;
		}
#endif /* defined(O_FSYNC) */
#if defined(O_RSYNC)
		if (match("rsync")) {
		    open_flags |= O_RSYNC;	/* Read I/O integrity. */
		    goto floop;			/* Use with O_DSYNC or O_SYNC. */
		}
#endif /* defined(O_RSYNC) */
#if defined(O_SYNC)
		if (match("sync")) {
		    open_flags |= O_SYNC;	/* Synchronous all data access. */
		    goto floop;			/* Sync data & file attributes. */
		}
#endif /* defined(O_SYNC) */
#if defined(O_LARGEFILE)
		if (match("large")) {
		    open_flags = O_LARGEFILE;	/* Enable large file support. */
		    goto floop;			/* Same as _FILE_OFFSET_BITS=64 */
		}
#endif /* defined(O_LARGEFILE) */
	    } /* End if "flags=" option. */
	    /*
	     * Flags which apply to opening a file for writes.
	     */
	    if (match ("oflags=")) {
	    oloop:
		if (match(","))
		    goto oloop;
		if (*string == '\0')
		    continue;
#if defined(O_APPEND)
		if (match("append")) {
		    wopen_flags |= O_APPEND;	/* Append to file. */
		    goto oloop;
		}
#endif /* defined(O_APPEND) */
#if defined(O_DEFER)
		if (match("defer")) {
		    wopen_flags |= O_DEFER;	/* Defer updates. */
		    goto oloop;
		}
#endif /* defined(O_DEFER) */
#if defined(O_DSYNC)
		if (match("dsync")) {		/* Write data integrity. */
		    wopen_flags |= O_DSYNC;	/* Synchronize data written. */
		    goto oloop;
		}
#endif /* defined(O_DSYNC) */
#if defined(O_SYNC)
		if (match("sync")) {
		    wopen_flags |= O_SYNC;	/* Synchronous all data access. */
		    goto oloop;			/* Sync data & file attributes. */
		}
#endif /* defined(O_SYNC) */
#if defined(O_TRUNC)
		if (match("trunc")) {
		    wopen_flags |= O_TRUNC;	/* Truncate output file. */
		    goto floop;
		}
#endif /* defined(O_TRUNC) */
#if defined(O_TEMP)
		if (match("temp")) {
		    wopen_flags |= O_TEMP;	/* Temporary file. */
		    goto oloop;
		}
#endif /* defined(O_TEMP) */
	    } /* End of "oflags=" option. */
	    if (match ("parity=")) {
		parity_str = string;
		if (match("even")) {
		    parity_code = PARENB;
		    data_bits_code = CS7;
		} else if (match("odd")) {
		    parity_code = (PARENB | PARODD);
		    data_bits_code = CS7;
		} else if (match("none")) {
		    parity_code = 0;
		    data_bits_code = CS8;
#if defined(_QNX_SOURCE)
		} else if (match("mark")) {
		    parity_code = (PARENB | PARODD | PARSTK);
		    data_bits_code = CS7;
		} else if (match("space")) {
		    parity_code = (PARENB | PARSTK);
		    data_bits_code = CS7;
#endif /* defined(_QNX_SOURCE) */
		} else {
#if defined(_QNX_SOURCE)
		   Fprintf ("Valid parity settings are: even, odd, mark, space, or none.\n");
#else /* !defined(_QNX_SOURCE) */
		   Fprintf ("Valid parity settings are: even, odd, or none.\n");
#endif /* defined(_QNX_SOURCE) */
		   exit (FATAL_ERROR);
		}
		if (data_bits_code == CS7) {
		    if (pattern == (u_int32)DEFAULT_PATTERN) {
			pattern = (u_int32)ASCII_PATTERN;
		    } else {
			pattern &= (u_int32)0x77777777;	/* Strip off parity bits. */
		    }
		}
		continue;
	    }
	    if (match ("oncerr=")) {
		if (match("abort")) {
		    oncerr_action = ABORT;
		} else if (match("continue")) {
		    oncerr_action = CONTINUE;
		} else {
		    Fprintf ("On error actions are 'abort' or 'continue'.\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("passes=")) {
		pass_limit = number(ANY_RADIX);
		if (pass_limit == 0) {
		    Fprintf("Please specify a pass limit greater than zero!\n");
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("pattern=")) {	/* TODO: Needs to be cleaned up. */
		int size = strlen(string);
		if (size == 0) {
		    Fprintf(
		"Please specify pattern of: { hex-pattern | incr | string }\n");
		    exit (FATAL_ERROR);
		}
		user_pattern = TRUE;
		if (match ("incr")) {	/* Incrementing pattern. */
		    int v, size = 256;
		    u_char *buffer = (u_char *) myalloc (size, 0);
		    u_char *bp = buffer;
		    for (v = 0; v < size; v++) {
			*bp++ = v;
		    }
		    setup_pattern (buffer, size);
		} else if ( (size == 3) && match("iot") ) {
		    iot_pattern = TRUE;
		    /* Allocate pattern buffer after parsing. */
		} else if ( IS_HexString(string) && (size <= 10) ) {
		    /* valid strings: XXXXXXXX or 0xXXXXXXXX */
		    pattern = (u_int32)number(HEX_RADIX);
		} else { /* Presume ASCII string for data pattern. */
		    u_char *buffer = (u_char *) myalloc (size, 0);
		    size = StrCopy (buffer, string, size);
		    pattern_size = size;
		    pattern_string = string;
		    setup_pattern (buffer, size);
		}
		continue;
	    }
	    if (match ("position=")) {
		file_position = (off_t)large_number(ANY_RADIX);
		user_position = TRUE;
		continue;
	    }
	    if (match ("procs=")) {
		num_procs = (u_short)number(ANY_RADIX);
		if (num_procs > MAX_PROCS) {
		    Fprintf("Please limit procs to <= %d!\n", MAX_PROCS);
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("rseed=")) {
		random_seed = (u_int)number(ANY_RADIX);
		user_rseed = TRUE;
		continue;
	    }
	    if (match ("runtime=")) {
		user_runtime = string;
		runtime = time_value();
		if (!record_limit) {
		    record_limit = INFINITY;
		}
		continue;
	    }
	    if (match ("seek=")) {
		seek_count = number(ANY_RADIX);
		continue;
	    }
	    if (match ("skip=")) {
		skip_count = number(ANY_RADIX);
		continue;
	    }
	    if (match ("slices=")) {
		num_slices = (u_short)number(ANY_RADIX);
		if (num_slices > MAX_SLICES) {
		    Fprintf("Please limit slices to <= %d!\n", MAX_SLICES);
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("step=")) {
		step_offset = (off_t)large_number(ANY_RADIX);
		continue;
	    }
	    if (match ("speed=")) {
		speed_str = string;
		baud_rate = (u_int32) number(ANY_RADIX);
		if (setup_baud_rate (baud_rate) == FAILURE) {
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("timeout=")) {
		tty_timeout = (u_short)number(ANY_RADIX);
		continue;
	    }
	    if (match ("ttymin=")) {
		tty_minflag = TRUE;
		tty_minimum = (u_short)number(ANY_RADIX);
		continue;
	    }
	    if (match ("dtype=")) {
		struct dtype *dtp;
		if ((dtp = setup_device_type (string)) == NULL) {
		    exit (FATAL_ERROR);
		}
		input_dtype = output_dtype = dtp;
		continue;
	    }
	    if (match ("idtype=")) {
		if ((input_dtype = setup_device_type (string)) == NULL) {
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("odtype=")) {
		if ((output_dtype = setup_device_type (string)) == NULL) {
		    exit (FATAL_ERROR);
		}
		continue;
	    }
	    if (match ("vrecords=")) {
	        volume_records = number(ANY_RADIX);
		continue;
	    }
	    if (match ("volumes=")) {
	        multi_flag = TRUE;
		volumes_flag = TRUE;
	        volume_limit = number(ANY_RADIX);
		continue;
	    }
	    if (match ("help")) {
		dthelp();
	    }
	    if (match ("version")) {
		dtversion();
	    }
	    Fprintf ("Invalid option '%s' specified, use 'help' for valid options.\n",
									string);
	    exit (FATAL_ERROR);
	}
}

/************************************************************************
 *									*
 * match()	Match string against next input argument.		*
 *									*
 * Inputs:	s = Pointer to string to match against.			*
 *									*
 * Outputs:	Returns TRUE/FALSE = Match/No Match			*
 *		string = Updated input argument pointer.		*
 *									*
 ************************************************************************/
int
match (char *s)
{
	char *cs;

	cs = string;
	while (*cs++ == *s) {
	    if (*s++ == '\0') {
		goto true;		/* I didn't write this junk. */
	    }
	}
	if (*s != '\0') {
		return (FALSE);
	}
true:
	cs--;
	string = cs;
	return (TRUE);
}

/************************************************************************
 *									*
 * number()	Converts ASCII string into numeric value.		*
 *									*
 * Inputs:	base = The base for numeric conversions.		*
 *									*
 * Outputs:	Returns converted number, exits on invalid numbers.	*
 *									*
 ************************************************************************/
static u_long
number (int base)
{
	char *eptr, *str = string;
	u_long value;

	value = CvtStrtoValue (str, &eptr, base);

	if (*eptr != '\0') {
	    Fprintf ("Invalid character detected in number: '%c'\n", *eptr);
	    exit (FATAL_ERROR);
	}
	return (value);
}

static large_t
large_number(int base)
{
	char *eptr, *str = string;
	large_t value;

	value = CvtStrtoLarge (str, &eptr, base);

	if (*eptr != '\0') {
	    Fprintf ("Invalid character detected in number: '%c'\n", *eptr);
	    exit (FATAL_ERROR);
	}
	return (value);
}

static time_t
time_value(void)
{
	char *eptr, *str = string;
	time_t value;

	value = CvtTimetoValue (str, &eptr);

	if (*eptr != '\0') {
	    Fprintf ("Invalid character detected in time string: '%c'\n", *eptr);
	    exit (FATAL_ERROR);
	}
	return (value);
}

/************************************************************************
 *									*
 * report_error() - Report system error information.			*
 *									*
 * Inputs:	error_info = Additional error info for perror.		*
 *		record_flag = Controls reporting error/time info.	*
 *									*
 ************************************************************************/
void
report_error(
	char		*error_info,
	int		record_flag)
{
    struct dinfo *dip = active_dinfo;
    int saved_errno = errno;

    if (dip) dip->di_errno = errno;

#if defined(LOG_DIAG_INFO)
    if (logdiag_flag) {
	(void)sprintf(msg_buffer, "%s: '%s', errno = %d - %s\n",
			cmdname, error_info, errno, strerror(errno));
	LogDiagMsg(msg_buffer);
    }
#endif /* defined(LOG_DIAG_INFO) */

    Fprintf ("'%s', errno = %d - %s\n", error_info, errno, strerror(errno));
    exit_status = FAILURE;
    if (record_flag) {
	error_time = time((time_t *) 0);
	/* ctime() adds newline '\n' to time stamp. */
	Fprintf ("Error number %lu occurred on %s",
			++error_count, ctime(&error_time));
    }
    errno = saved_errno;
}

/*
 * General purpose display record information function.
 */
void
report_record(
	struct dinfo	*dip,
	u_long		files,
	u_long		records,
	u_int32		lba,
	enum test_mode	mode,
	void		*buffer,
	size_t		bytes )
{
    char msg[STRING_BUFFER_SIZE];
    char *bp = msg;

    if (dip->di_dtype->dt_dtype == DT_TAPE) {
	bp += sprintf(bp, "File #%lu, ", files);
    }
    bp += sprintf(bp, "Record #%lu", records);
#if 0
    if (lbdata_flag || iot_pattern || aio_flag) {
#else
    if (lba != NO_LBA) {
#endif
	bp += sprintf(bp, " (lba %u), ", lba);
    } else {
	bp += sprintf(bp, ", ");
    }
    bp += sprintf(bp, "%s %ld bytes %s buffer %#lx...\n",
		 (mode == READ_MODE) ? "Reading" : "Writing",
		 (long)bytes,
		 (mode == READ_MODE) ? "into" : "from",
		 (u_long)buffer);
    Fprintf (msg);
}


/*
 * terminate() - Terminate program with specified exit code.
 *
 * Inputs:
 *	code = The exit code or signal number if kill done.
 */
void
terminate (int code)
{
    struct dinfo *dip = active_dinfo;
    int status;

    /*
     * If we enter here more than once, just exit to avoid
     * possible recursion.  kernel should do I/O rundown :-)
     */
    if (terminating_flag) {
	if (core_dump && (code != SUCCESS)) {
	    abort();		/* Generate a core dump. */
	} else {
	    exit (code);	/* get outta here now... */
	}
    } else {
	terminating_flag++;	/* Show we're terminating. */
    }

    /*
     * If an alarm timer is active, cancel it and calculate
     * the elapsed run time.
     */
    if (TimerActive) {
	elapsed_time = runtime - alarm(0);
	TimerActive = FALSE;
	code = exit_status;	/* Set the exit status! */
    }

    /*
     * We only come here for signals when executing multiple
     * processes, so abort active procs and continue waiting.
     */
    if ((num_procs || num_slices) && child_pid) {
	abort_procs();		/* Abort any active procs. */
	return;
    }
#if !defined(__MSDOS__) || defined(__NUTC__)
    if (debug_flag && (code == SIGCHLD)) {
	Fprintf ("Child process exited prematurely, parent exiting...\n");
    } else 
#endif

    /*
     * Report stats before waiting for the child process.
     */
#if !defined(__MSDOS__) || defined(__NUTC__)
    (void) signal (SIGCHLD, SIG_DFL); /* Don't deliver SIGCHLD now. */
#endif
    /*
     * Close file, which for AIO waits for outstanding I/O's,
     * before reporting statistics so they'll be correct.
     */
    if (dip) {
	dip->di_proc_eei = FALSE;
	status = (*dip->di_funcs->tf_close)(dip);
	if (status != SUCCESS) code = status;
    }
    gather_stats(dip);		/* Gather the device statistics. */
    gather_totals();		/* Update the total statistics.	*/
    report_stats(dip, TOTAL_STATS);

    if (child_pid) {		/* Always wait for child status. */
	pid_t wpid = (pid_t) 0;
	/*
	 * If exiting with error, ensure the child gets killed.
	 */
	if (code != SUCCESS) {
	    (void) kill (child_pid, SIGTERM);
	}
	if (debug_flag) {
	    Fprintf ("Waiting for child PID %d to exit...\n", child_pid);
	}
#if defined(_BSD)
	do {
	    if ((wpid = wait (&child_status)) == FAILURE) {
		report_error ("wait", TRUE);
		break;
	    }
	} while (wpid != child_pid);
	if ( (code == SUCCESS) && (wpid != FAILURE) ) {
	    code = WEXITSTATUS (child_status);
	}
#else /* !defined(_BSD) */
	if ((wpid = waitpid (child_pid, &child_status, 0)) == FAILURE) {
	    report_error ("waitpid", TRUE);
	} else if (code == SUCCESS) {
	    code = WEXITSTATUS (child_status);
	}
#endif /* defined(_BSD) */
	if (debug_flag && (wpid != FAILURE)) {
	    Fprintf("Child PID %d, exited with status = %d\n",
					wpid, WEXITSTATUS(child_status));
	}
    }

    /*
     * Delete the output file, if requested to do so.
     */
    if (output_file && (io_mode == TEST_MODE) &&
	(dispose_mode == DELETE_FILE)         &&
	(dip && dip->di_dtype && (dip->di_dtype->dt_dtype == DT_REGULAR)) ) {
	(void) delete_file (dip);
    }

#if defined(LOG_DIAG_INFO)
    if (logdiag_flag) {
	sprintf(msg_buffer, "Finished: %s", cmd_line);
	LogDiagMsg(msg_buffer);
    }
#endif /* defined(LOG_DIAG_INFO) */

    if (!eof_status && (code == END_OF_FILE)) {
	code = SUCCESS;		/* Map end-of-file status to Success! */
    }
    if (debug_flag && (code != SUCCESS)) {
	Fprintf ("Exiting with status code %d...\n", code);
    }
    if (core_dump && (code != SUCCESS) && (code != END_OF_FILE)) {
	abort();			/* Generate a core dump. */
    }
    if (forked_flag && tdelay_count && (child_pid == (pid_t) 0)) {
	(void)sleep (tdelay_count);	/* Allow parent to wait for us.	*/
    }
    exit (code);
}

int
nofunc (struct dinfo *dip)
{
	return (SUCCESS);
}

static char *multi_prompt = 
    "\nPlease insert volume #%d in drive %s, press ENTER when ready to proceed: \007";
static char *multi_nready =
    "The drive is NOT ready or encountered an error, Retry operation (Yes): \007";

int
HandleMultiVolume (struct dinfo *dip)
{
    int status;

    status = RequestMultiVolume (dip, FALSE, dip->di_oflags);
    if (status == FAILURE) return (status);

    if (dip->di_mode == READ_MODE) {
	dip->di_volume_bytes = (large_t)(dip->di_dbytes_read + total_bytes_read);
	if (verbose_flag) {
	  if (dip->di_dtype->dt_dtype == DT_TAPE) {
	    Fprint ("    [ Continuing in file #%lu, record #%lu, bytes read so far " LUF "... ]\n",
		(dip->di_files_read + 1), (dip->di_records_read + 1), dip->di_volume_bytes);
	  } else {
	    Fprint ("    [ Continuing at record #%lu, bytes read so far " LUF "... ]\n",
			(dip->di_records_read + 1), dip->di_volume_bytes);
	  }
	}
	dip->di_vbytes_read = (v_large) 0;
    } else {
	dip->di_volume_bytes = (large_t)(dip->di_dbytes_written + total_bytes_written);
	if (verbose_flag) {
	  if (dip->di_dtype->dt_dtype == DT_TAPE) {
	    Fprint ("    [ Continuing in file #%lu, record #%lu, bytes written so far " LUF "... ]\n",
		(dip->di_files_written + 1), (dip->di_records_written + 1), dip->di_volume_bytes);
	  } else {
	    Fprint ("    [ Continuing at record #%lu, bytes written so far " LUF "... ]\n",
			(dip->di_records_written + 1), dip->di_volume_bytes);
	  }
	}
	dip->di_vbytes_written = (v_large) 0;
    }
    (void)fflush(stderr);
    media_changed = TRUE;
    dip->di_volume_records = 0;
    if (exit_status == END_OF_FILE) {
	exit_status = SUCCESS;		/* Ensure END_OF_FILE status is reset! */
    }
    return (status);
}

int
RequestFirstVolume (struct dinfo *dip, int oflags)
{
    int status;

    multi_volume = 0;

    status = RequestMultiVolume (dip, TRUE, oflags);

    dip->di_volume_bytes = (large_t) 0;
    dip->di_volume_records = 0;

    return (status);
}

int
RequestMultiVolume (struct dinfo *dip, bool reopen, int oflags)
{
    struct dtfuncs *dtf = dip->di_funcs;
    char buffer[256];
    char *bp = buffer;
    FILE *fp;
    int saved_exit_status;
    u_long saved_error_count;
    int status;

    if (terminating_flag) return (FAILURE);

    if ( (status = (*dtf->tf_close)(dip)) == FAILURE) {
	return (status);
    }

    if ( (fp = fopen("/dev/tty", "r+")) == NULL) {
	report_error("open of /dev/tty failed", FALSE);
	return (FAILURE);
    }
    multi_volume++;

    (void)sprintf(bp, multi_prompt, multi_volume, dip->di_dname);

    (void) fputs (bp, fp); fflush(fp);
    if (fgets (bp, sizeof(buffer), fp) == NULL) {
	Fprint ("\n");
	status = FAILURE;	/* eof or an error */
	return (status);
    }

    saved_error_count = error_count;
    saved_exit_status = exit_status;

    /*
     * This is an important step, so allow the user to retry on errors.
     */
    do {
	if (!reopen) {
	    status = (*dtf->tf_open)(dip, oflags);
	} else {
	    status = (*dtf->tf_reopen_file)(dip, oflags);
	}
	if (status == SUCCESS) {
#if !defined(__NUTC__) && !defined(__QNXNTO__)
	    if (dip->di_dtype->dt_dtype == DT_TAPE) {
		status = DoRewindTape (dip);
		if (status == FAILURE) {
		    (void)(*dtf->tf_close)(dip);
		}
	    }
#endif /* !defined(__NUTC__) && !defined(__QNX_NTO__) */
	}
	if (status == FAILURE) {
	    (void) fputs (multi_nready, fp); fflush(fp);
	    if (fgets (bp, sizeof(buffer), fp) == NULL) {
		Fprint ("\n");
		break;
	    }
	    if ( (bp[0] == 'N') || (bp[0] == 'n') ) {
		break;
	    }
	    error_count = saved_error_count;
	    exit_status = saved_exit_status;
	} else {
	    break;		/* device is ready! */
	}
    } while (status == FAILURE);

    (void)fclose(fp);
    return (status);
}
