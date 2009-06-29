static char *whatHeader = "@(#) dt.d/dtutil.c /main/9 Jan_24_03:56";
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
 * Module:	dtutil.c
 * Author:	Robin T. Miller
 *
 * Description:
 *	Utility routines for generic data test program.
 */
#include "dt.h"
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

#if !defined(WIN32)
#  include <sys/param.h>
#  include <netdb.h>		/* for MAXHOSTNAMELEN */
#  include <sys/wait.h>
#endif /* !defined(WIN32) */

#if defined(WIN32)
#  ifndef MAXHOSTNAMELEN
#    define MAXHOSTNAMELEN 255
#  endif
#  if !defined(INVALID_SET_FILE_POINTER)
#    define INVALID_SET_FILE_POINTER -1
#  endif
#define strncasecmp _strnicmp
#endif /* !defined(WIN32) */

#if defined(sun)
#  define strtoul	strtol
#endif /* defined(sun) */

#if defined(ultrix)
extern void *valloc(size_t size);
#endif /* defined(ultrix) */

/*
 * Modification History:
 *
 * January 6th, 2007 by Robin T. Miller.
 *	When random I/O is used, scale the random number by a multiplier
 * and the device block size, to obtain larger offset for >1TB capacities.
 *
 * November 1st, 2006 by Robin T. Miller.
 *      In ReportDeviceInfo(), do not save the updated file offset
 * unless we're using AIO, since our normal read/write test functions
 * maintain this offset themselves.
 *
 * October 16th, 2006 by Robin T. Miller.
 *      Updated stoh() and htos() from Scu source.
 *      Handle new timestamp option:
 *      - skip timestamps during verification.
 *      - report time block was written during corruptions.
 *
 * October 4th, 2006 by Robin T. Miller.
 *      Added warning in delete_file() if file cannot be unlink'ed, but
 * this should never occur now that terminate() only calls if it's open.
 *
 * June 24th, 2006 by Robin T. Miller.
 *	Fix two places in ExecuteTrigger() where a status value was not
 * returned!  (overlooked when changing void to int return value).
 *
 * January 19th, 2005 by Nagendra Vadlakunta.
 * 	To add IA64 support for Windows, excluded MACHINE_64BIT
 * part of the code in functions stoh() and htos().
 * 
 * January 12th, 2005 by Robin T. Miller.
 *      Added keepalive format control string parsing (see help).
 *      For Hazard, do *not* prepend our program name to messages.
 *
 *      In Hazard mode, remove space following "RPCLOGn: ", so we are
 * consistent with how Hazard logs all its' output.
 *
 * December 16th, 2004 by Robin Miller.
 *      Updated printf control strings in ReportDeviceInfo() to properly
 * display lba following by offsets (the latter was incorrect, due to
 * wrong format used for lba).  Found on 32 bit systems (of course)
 * Side Note:  I've started coding lba's for 64 bits instead of 32 bits
 * which limits the size (of course), but there's much work to do yet!
 *
 * October 21st, 2004 by Robin Miller.
 *      Report the current file position during error report.
 * Although the relative block number is formulated from the file
 * offset, some folks would also like to see the actual offset.
 * For random I/O, always align offsets to the device size (dsize).
 * Failure to do this causes false data corruptions to regular files.
 * For big-endian machines, the IOT pattern must be byte swapped, since
 * IOT is designed to be little-endian only.
 *
 * June 24th, 2004 by Robin Miller.
 *      Major cleanup to init_iotdata() function to properly handle
 * non-modulo prefix and transfer counts.  The odd bytes were not
 * initialized to something (now ~0) which lead to false failures.
 *
 * June 22nd, 2004 by Robin Miller.
 *      Added support for triggers on corruption.
 *      Properly report failing lba when lbdata is used.
 *      Don't align random offsets to the device size when testing
 * regular files through a file system.  In general, random I/O is
 * not usually going to work to a file, since part of file usually
 * gets overwitten (sorry, we don't track blocks already written).
 *
 * March 30th, 2004 by Robin Miller.
 *      Improve lseek error messages (should they should fail).
 *
 * March 24th, 2004 by Robin Miller.
 *      Update code in do_random() where the random data limit
 * (rdata_limit) was being truncated to an unsigned long, which
 * is 32-bits many systems.  This causes large capacity disks,
 * such as 36GB (0x900000000), to be truncated to zero which
 * causes a divide by zero ("Arithmetic exception" on HP-UX).
 * Also increase the size of the random number (randum) to
 * 64-bits (on 32-bit systems), so larger seeks are possible.
 *
 * February 23, 2004 by Robin Miller.
 *      Reverse the buffer and prefix pattern bytes being dumped
 * in verify_prefix(), so they properly reflect the expected versus
 * found bytes.  Otherwise, the information is misleading.
 *
 * November 25th, 2003 by Robin Miller.
 *      When formatting the prefix string, if we're doing random
 * I/O, round the prefix string up to sizeof(u_int32), so partial
 * patterns (non-modulo our 4 byte pattern) do not get used, which
 * causes false data compare failures.
 * Note: Failures still occur if random I/O is used with pattern
 * files containing non-repeating data pattern bytes!
 *
 * November 20th, 2003 by Robin Miller.
 *	Broken verify data function up for better performance.
 *	Update prefix string logic to write the string in every
 * logical block (lbdata_size).  This had to be done or else random
 * I/O with prefix strings failed!  It also give better coverage.
 *
 * November 17th, 2003 by Robin Miller.
 *	Breakup output to stdout or stderr, rather than writing
 * all output to stderr.  If output file is stdout ('-') or a log
 * file is specified, then all output reverts to stderr.
 *
 * October 8th, 2003 by Robin Miller.
 *	On AIX, accept ENXIO for I/O's pass EOF.
 *
 * September 27th, 2003 by Robin Miller.
 *      Added support for AIX.
 *
 * March 20th, 2003 by Robin Miller.
 *	Added FmtPrefix() to format the prefix string.
 *
 * March 18th, 2003 by Robin Miller.
 *	Optimize code and loops using USE_PATTERN_BUFFER define.
 *
 * March 15th, 2003 by Robin Miller.
 *	Added support for data pattern prefix strings.
 *
 * March 4th, 2003 by Robin Miller.
 *	Add support for broken EOF SunOS release.  This means writes
 * at EOF return a count of zero, but there is no errno value to key
 * off like POSIX specifies.
 *
 * November 21st, 2002 by Robin Miller.
 *	On HP-UX, accept ENXIO for I/O's pass EOF.
 *
 * November 14th, 2002 by Robin Miller.
 *	Add support for 32-bit HP-UX compilation.
 *
 * October 10th, 2002 by Robin Miller.
 *	Display correct erroring byte during data compare errors,
 * when using a 32-bit hex data pattern.
 *
 * February 1st, 2002 by Robin Miller.
 *	Make porting changes necessary for Solaris 8.
 *
 * January 29th, by Robin Miller.
 *	Minor tweak to clarify correct versus incorrect data dumped.
 *
 * June 25th, 2001 by Robin Miller.
 *	Report mt status for all errors, not just EIO errors.  Also,
 * report mt status on unexpected EOF/EOM when no data transferred.
 * Note: Reporting mt and EEI status is only done for Tru64 Unix.
 *
 * February 24th, 2001 by Robin Miller.
 *	Add conditionalization for QNX RTP (Neutrino).
 *
 * January 24th, 2001 by Robin Miller.
 *	Add support for variable I/O requests sizes.
 *	Conditionalize some functions to allow INLINE macros.
 *	Modifications to allow IOT test pattern to honor the lba
 * size variable, rather than hardcoding IOT to 512 byte sectors.
 *
 * January 15th, 2001 by Robin Miller.
 *	Don't terminate() on failures, return error to caller.
 *	Note: seek functions still call terminate()... more work!
 *
 * January 2nd, 2001 by Robin Miller.
 *      Make changes to build using MKS/NuTCracker product.
 *
 * October 4th, 2000 by Robin Miller.
 *	Update is_Eof() to accept ENXIO for AIO reads @ EOM on
 * SCO UnixWare systems.  All other systems return ENOSPC or zero.
 *
 * April 25th, 2000 by Robin Miller.
 *	Added an expect flag to dump_buffer to help with formatting.
 *
 * April 18th, 2000 by Robin Miller.
 *	Modified calls to report_error() to ensure error count bumped.
 *
 * March 28th, 2000 by Robin Miller.
 *	Modified file position functions to accept device information
 * parameter, so necessary debug could be added to these funcitons.
 * Also, only scale the random position upwards by the device size
 * every other record, so low blocks gets utilized more often.
 * Note:  These changes degrade random I/O performance slightly.
 * [ All debug needs ripped out to obtain better performance, ]
 *
 * February 17th, 2000 by Robin Miller.
 *	Adding better support for multi-volume tape testing.  Mainly,
 * make it work with writing multiple tape files, rather than one file.
 *
 * February 16th, 2000 by Robin Miller.
 *	Set exit_status to FAILURE in RecordError() function.  This
 * is necessary to catch any error being logged, since test functions
 * no longer call terminate() with FAILURE status.
 *
 * January 22nd, 2000 by Robin Miller.
 *	Add check for Cygwin device names for Windows/NT.
 *
 * January 4th, 2000 by Robin Miller.
 *	Major cleanup in verify_data().  Now reports IOT block number.
 *	Added verify_buffers() and verify_lbdata() functions for raw.
 *
 * December 31st, 1999 by Robin Miller.
 *	Modify do_random() to ensure rlimit is not exceeded when the lower
 * file position is factored in, to stay within desired lba range.
 *
 * December 30th, 1999 by Robin Miller.
 *	Modify do_random() to take into consideration the transfer size,
 * to ensure the requested random I/O data limit is not exceeded.
 *	Added get_lba() function to return the current logical block address.
 *
 * November 11th, 1999 by Robin Miller.
 *	Add LogDiagMsg() to log diagnostic information to event logger.
 *
 * November 10th, 1999 by Robin Miller.
 *	Update make_lbdata() & winit_lbdata() so if the IOT test pattern is
 * selected, and the device block size is NOT 512 bytes, we force 1 block.
 * Note: This only affects Tru64 Unix, since we obtain the real sector size.
 *
 * November 9th, 1999 by Robin Miller.
 *	Remove use of '%p' in dump_buffer(), since it's use is inconsistent.
 * With Cygnus Solutions, '%p' displays a leading '0x', other OS's don't!
 *
 * August 26th, 1999 by Robin Miller.
 *	Report an error for ENOSPC, if no data has been transferred.
 *	Previously, this error was only reported for tape devices.
 *
 * August 6th, 1999 by Robin Miller.
 *      Better parameterizing of "long long" printf formatting.
 *
 * July 29, 1999 by Robin Miller.
 *	Merge in changes made to compile on FreeBSD.
 *
 * July 22nd, 1999 by Robin Miller.
 *	Added support for IOT (DJ's) test pattern.
 * 
 * July 7th, 1999 by Robin Miller.
 *	Modify CvtStrtoValue() to check for recursive calls which return
 * zero (0) for '/' and '%', which cause a divide by zero fault/core dump.
 *
 * June 28, 1999 by Robin Miller.
 *	For 32-bit systems, added CvtStrtoLarge() function to
 * permit double or long long values, since u_long is too small.
 *
 * May 27, 1999 by Robin Miller.
 *	Added support for micro-second delays.
 *
 * April 8, 1999 by Robin Miller.
 *	Added format_ltime() to format table() sysinfo times.
 *
 * January 7, 1999 by Robin Miller.
 *	Removed fflush() in Fprintf() function which caused intermixed
 * output when multiple processes were running (i.e., serial line testing).
 *
 * December 21, 1998 by Robin Miller.
 *	- update Malloc() to clear allocated memory.
 *	- for DUNIX, updates to handle resets for tapes.
 *
 * December 16, 1998 by Robin Miller.
 *	Merge in changes made by George Bittner:
 *	- modify do_random(), use random value as a block number
 *	  instead of a byte offset, for testing larger disks/files.
 *	- ensure the random offset is within starting file position
 *	  and the end of the disk/partition.
 *
 * November 19, 1998, by Robin Miller.
 *	Fix problem where wrong lbdata lba was returned for offset 0.
 * This required updates to make_lbdata() and winit_lbdata() functions.
 *	[ sorry folks, major brain burp!!! ]
 *
 * November 16, 1998 by Robin Miller.
 *	For AIO, report the relative block from saved AIO control block.
 *
 * October 29, 1998 by Robin Miller.
 *	Implement a random I/O data limit, instead of using the normal
 * data limit variable (not good to dual purpose this value).
 *
 * October 26, 1998 by Robin Miller.
 *	Add functions make_lbdata() and winit_lbdata() which handle
 * using both random I/O and lbdata options.  The file offset seeked
 * to is used as the starting lbdata address.
 *
 * April 28, 1998 by Robin Miller.
 *	For WIN32/NT, or in O_BINARY into open flags to force binary
 *	mode (the default is text mode).
 *
 * May 16, 1997 by Robin Miller.
 *	Modified macro used in ReportDeviceInfo() which rounded block
 *	up, causing the erroring block to be off by one, when data
 *	compare errors were not at the beginning of a block.  Also,
 *	report the block offset (byte within block) when non-modulo.
 *
 * May 14, 1997 by Robin Miller.
 *	If we encounter a ENOSPC error and no data has been transferred,
 *	flag this as an error.  This normally indicates a zero length
 *	partition or the user may have seek'ed past eom or eop.
 *
 * March 27, 1997 by Ali Eghlima.
 *      Fix call to report_error() to reflect "unlink" failed.
 *
 * March 7, 1997 by Robin Miller.
 *	In ReportDeviceInfo(), when we're doing a copy/verify
 *	operation, allow output device to be a different offset.
 *
 * February 28, 1996 by Robin Miller.
 *	Modified ReportDeviceInfo() so we seek past disk errors.
 *	This action allows testing to continue with "errors=n".
 *
 * February 21, 1996 by Robin Miller.
 *	Added do_random() function for random I/O to disks.
 *
 * December 19, 1995 by Robin Miller.
 *	Conditionalize for Linux Operating System.
 *
 * Novemeber 18, 1995 by Robin Miller.
 *	Removing unaligned data access (ade) test code (code cleanup).
 *
 * November 11, 1995 by Robin Miller.
 *	Fix bug with verifying pad bytes.  Basically, the previous logic,
 *	while valid for short reads, was incorrect when the pad bytes did
 *	*not* start on a modulo sizeof(int) boundary. This caused variable
 *	length reads with small increment values to report an (invalid)
 *	pad byte data compare error.  NOTE: Only occurred w/pattern file.
 *
 * July 22, 1995 by Robin Miller.
 *	Finally, correctly dump data buffers with context (yes, I made
 *	sure I tested min/max limits via fault insertion this time).
 *
 * July 15, 1995 by Robin Miller.
 *	Add is_Eof() to handle end of media (ENOSPC), and cleanup code.
 *
 * July 7, 1995 by Robin Miller.
 *	Correctly dump the pattern buffer on data compare errors.
 *	When reporting a compare error, also display the byte count.
 *	This latter information is useful for variable length records.
 *
 * July 6, 1995 by Robin Miller.
 *	Changed a number of fprintf's to Fprintf so the child PID gets
 *	reported during error processing (how did I miss this stuff?).
 *
 * September 23, 1994 by Robin Miller.
 *      Make changes necessary to build on QNX 4.21 release.
 *
 * January 20, 1994 by Robin Miller.
 *	When checking the pad bytes, don't do the entire buffer since very
 *	large buffers (e.g. 100m) may have been specified using min, max,
 *	and incr options which cause excessive paging and poor performance.
 *
 * October 28, 1993 by Robin Miller.
 *	For multiple processes, display the PID to differentiate output.
 *
 * September 15, 1993 by Robin Miller.
 *	Limit pattern buffer dumping to size of pattern buffer.
 *
 * September 17, 1993 by Robin Miller.
 *	Added RecordWarning() function to reporting record number and
 *	time stamp on warning errors (useful for debugging purposes).
 *	More limiting of data & pattern buffer dumping (less bytes).
 *
 * September 3, 1993 by Robin Miller.
 *	Allow "inf" or "INF" keywords to set maximum counts.
 *
 * September 2, 1993 by Robin Miller.
 *	Added device specific information.
 *
 * September 1, 1993 by Robin Miller.
 *	Report proper record number during errors (add partial records).
 *	Limit data dumped when data verify errors occur (short records).
 *
 * August 27, 1993 by Robin MIller.
 *	Added support for DEC OSF/1 POSIX Asynchronous I/O (AIO).
 *
 * August 17, 1993 by Robin Miller.
 *	Added function RecordError() to record when an error occurs,
 *	and added function Ctime() to append date/time string to the
 *	log buffer.
 *
 * August 10, 1993 by Robin Miller.
 *	Added verify_padbuf() function to check pad buffer bytes after
 *	read operations to ensure they weren't overwritten.
 *
 * August 4, 1993 by Robin Miller.
 *	Added various printing functions to simplify error reporting.
 *
 * September 11, 1992 by Robin Miller.
 *	Added additional debug information when reopening a device.
 *
 * September 5, 1992 by Robin Miller.
 *	Initial port to QNX 4.1 Operating System.
 *
 * May 22, 1992 by Robin Miller.
 *	Control / force kernel address data exception via flag.
 *
 * March 11, 1992 by Robin Miller.
 *	Changes necessary for port to 64-bit Alpha architecture.
 *
 * October 16, 1990 by Robin Miller.
 *	Added myalloc() memory allocation function to allocate and
 *	align the buffer address using the alignment value.
 *
 * October 9, 1990 by Robin Miller.
 *	Use variable hz instead of HZ define for clock ticks per second
 *	so user can specify different value via "hz=ticks" option.
 *
 * August 21, 1990 by Robin Miller.
 *	Changed exit status so scripts can detect and handle errors
 *	based on the exit code.  If not success, fatal error, or end
 *	of file/tape, the exit code is the error number (errno).
 *
 * August 21, 1990 by Robin Miller.
 *	Added function dump_buffer() to dump buffer in hex bytes.
 *
 * August 8, 1990 by Robin Miller.
 *	Added functions seek_file() and skip_records().  Modified seek
 *	file function to avoid lseek() system call overhead (see code).
 *
 *	Changed malloc() to valloc() to align buffer on page boundry.
 *	On some archetectures, this results on better performance to
 *	raw devices since the DMA is done directly to the users' buffer.
 *
 * April 11, 1990 by Robin Miller.
 *	Added function mmap_file() to memory map a file.  Added munmap()
 *	syscall to reopen_file() which gets called for multiple passes.
 *	This allows us to time the mmap() as well as the munmap() code.
 *
 * March 21, 1990 by Robin Miller.
 *	Added function delete_file() which checks for a regular file
 *	and then unlinks it.
 *
 */

/*
 * Forward References:
 */
size_t copy_prefix(u_char *buffer, size_t bcount);
int verify_prefix(struct dinfo *dip, u_char *buffer, size_t bcount, size_t *pcount);

static int verify_data_normal(	struct dinfo	*dip,
				u_char		*buffer,
				size_t		bcount,
				u_int32		pattern);
static int verify_data_prefix(	struct dinfo	*dip,
				u_char		*buffer,
				size_t		bcount,
				u_int32		pattern );
static int verify_data_with_lba(struct dinfo	*dip,
				u_char		*buffer,
				size_t		bcount,
				u_int32		pattern,
				u_int32		*lba );

static size_t CalculateDumpSize(size_t size);
static int dopad_verify (
			struct dinfo	*dip,
			u_char		*buffer,
			size_t		offset,
			u_int32		pattern,
			size_t		pbytes,
			size_t		pindex,
			bool		inverted );
int vSprintf(char *bufptr, const char *msg, va_list ap);

#if 0
static char *pad_str =		"Pad";
#endif
static char *lba_str =          "Lba";
static char *data_str =		"Data";
static char *pattern_str =	"Pattern";
static char *prefix_str =	"Prefix";
static char *verify_str =	"Verify";

static char *compare_error_str =	"Data compare error at byte";
static char *bad_conversion_str =
		 "Warning: unexpected conversion size of %d bytes.\n";

/************************************************************************
 *									*
 * delete_file() - Delete the specified file.				*
 *									*
 *		This function ensures the file specified is a regular	*
 *		file before doing an unlink() to delete the file.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = File Delete/Not Deleted.	*
 *									*
 ************************************************************************/
int
delete_file (struct dinfo *dip)
{
    int status;

    if (debug_flag) {
	Printf("Deleting file '%s'...\n", dip->di_dname);
    }
    if ( (status = unlink(dip->di_dname)) == FAILURE) {
#if defined(WIN32)
   	LogMsg (efp, logLevelError, 0, "unlink failed!\n");
#else /* !defined(WIN32) */
        if (errno == ENOENT) {
            LogMsg (efp, logLevelWarn, 0,
                    "Warning: File '%s' does NOT exist during unlink, created?\n",
                    dip->di_dname);
        } else {
	    report_error ("unlink", TRUE);
        }
#endif /* defined(WIN32) */
    }
    return (status);
}

void
mySleep(unsigned int sleep_time)
{
    if (micro_flag) {
	(void) usleep(sleep_time);
    } else {
	(void) sleep(sleep_time);
    }
    return;
}

/************************************************************************
 *									*
 * CalculateDumpSize() - Calculate the number of data bytes to dump.	*
 *									*
 * Description:								*
 *	For non-memory mapped files, we'll dump the pad bytes.  These	*
 * pad bytes do not exist for memory mapped files which are directly	*
 * mapped to memory addresses.						*
 *									*
 * Inputs:	size = The size of the buffer to dump.			*
 *									*
 * Outputs:	Size of buffer to dump.					*
 *									*
 ************************************************************************/
static size_t
CalculateDumpSize (size_t size)
{
	size_t dump_size = size;

	if (!mmap_flag) {
	    dump_size += PADBUFR_SIZE;
	}
	if (dump_size > data_size) dump_size = data_size;
	return (dump_size);
}

/************************************************************************
 *									*
 * dump_buffer() Dump data buffer in hex bytes.				*
 *									*
 * Inputs:	name = The buffer name being dumped.			*
 *		base = Base pointer of buffer to dump.			*
 *		ptr  = Pointer into buffer being dumped.		*
 *		dump_size = The size of the buffer to dump.		*
 *		bufr_size = The maximum size of this buffer.		*
 *		expected = Boolean flag (True = Expected).		*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
dump_buffer (	char		*name,
		u_char		*base,
		u_char		*ptr,
		size_t		dump_size,
		size_t		bufr_size,
		bool		expected )
{
    size_t i, limit, offset;
    u_int field_width = 16;
    u_char *bend = (base + bufr_size);
    u_char *bptr;

    /*
     * Since many requests do large transfers, limit data dumped.
     */
    limit = (dump_size < dump_limit) ? dump_size : dump_limit;

    /*
     * Now to provide context, attempt to dump data on both sides of
     * the corrupted data, ensuring buffer limits are not exceeded.
     */
    bptr = (ptr - (limit >> 1));
    if (bptr < base) bptr = base;
    if ( (bptr + limit) > bend) {
	limit = (bend - bptr);		/* Dump to end of buffer. */
    }
    offset = (ptr - base);		/* Offset to failing data. */

    /*
     * NOTE: Rotate parameters are not displayed since we don't have
     * the base data address and can't use global due to AIO design.
     * [ if I get ambitious, I'll correct this in a future release. ]
     */
    Fprintf ("The %scorrect data starts at address %#lx (marked by asterisk '*')\n",
							(expected) ? "" : "in", ptr);
    Fprintf ("Dumping %s Buffer (base = %#lx, offset = %u, limit = %u bytes):\n",
							name, base, offset, limit);
    LogMsg (efp, logLevelError, (PRT_NOIDENT | PRT_NOFLUSH), "\n");

    for (i = 0; i < limit; i++, bptr++) {
	if ((i % field_width) == (size_t) 0) {
	    if (i) fprintf (efp, "\n");
	    LogMsg (efp, logLevelError, (PRT_NOIDENT | PRT_NOFLUSH),
			"%#lx ", (u_long)bptr);
	}
	fprintf (efp, "%c%02x", (bptr == ptr) ? '*' : ' ', *bptr);
    }
    if (i) Fprint ("\n");
    if (expected) {
	LogMsg (efp, logLevelError, (PRT_NOIDENT | PRT_NOFLUSH), "\n");
    }
    (void)fflush(efp);
}

#if defined(TIMESTAMP)

void
display_timestamp(u_char *buffer)
{
    time_t seconds;

    seconds = stoh(buffer, sizeof(seconds));
    /* Note: ctime() appends newline automatically! */
    Fprintf("The data block was written on %s", ctime(&seconds));
    return;
}
#endif /* defined(TIMESTAMP) */

/************************************************************************
 *									*
 * fill_buffer() - Fill Buffer with a Data Pattern.			*
 *									*
 * Description:								*
 *	If a pattern_buffer exists, then this data is used to fill the	*
 * buffer instead of the data pattern specified.			*
 *									*
 * Inputs:	buffer = Pointer to buffer to fill.			*
 *		byte_count = Number of bytes to fill.			*
 *		pattern = Data pattern to fill buffer with.		*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
fill_buffer (	u_char		*buffer,
		size_t		byte_count,
		u_int32		pattern)
{
    register u_char *bptr = buffer;
    register u_char *pptr, *pend;
    register size_t bcount = byte_count;

    pptr = pattern_bufptr;
    pend = pattern_bufend;

    /*
     * Initialize the buffer with a data pattern.
     */
    if ( !prefix_string ) {
	while (bcount--) {
	    *bptr++ = *pptr++;
	    if (pptr == pend) {
		pptr = pattern_buffer;
	    }
	}
    } else {
	register size_t i;
	for (i = 0; i < bcount; ) {
	    if ((i % lbdata_size) == 0) {
		size_t pcount = copy_prefix (bptr, (bcount - i));
		i += pcount;
		bptr += pcount;
		continue;
	    }
	    *bptr++ = *pptr++; i++;
	    if (pptr == pend) {
		pptr = pattern_buffer;
	    }
	}
    }
    pattern_bufptr = pptr;
    return;
}

/************************************************************************
 *									*
 * init_buffer() - Initialize Buffer with a Data Pattern.		*
 *									*
 * Inputs:	buffer = Pointer to buffer to init.			*
 *		count = Number of bytes to initialize.			*
 *		pattern = Data pattern to init buffer with.		*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
init_buffer (	u_char		*buffer,
		size_t		count,
		u_int32		pattern )
{
    register u_char *bptr;
    union {
        u_char pat[sizeof(u_int32)];
        u_int32 pattern;
    } p;
    register size_t i;

    /*
     * Initialize the buffer with a data pattern.
     */
    p.pattern = pattern;
    bptr = buffer;
    for (i = 0; i < count; i++) {
        *bptr++ = p.pat[i & (sizeof(u_int32) - 1)];
    }
    return;
}

#if _BIG_ENDIAN_
/************************************************************************
 *									*
 * init_swapped() - Initialize Buffer with a Swapped Data Pattern.	*
 *									*
 * Inputs:	buffer = Pointer to buffer to init.			*
 *		count = Number of bytes to initialize.			*
 *		pattern = Data pattern to init buffer with.		*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
init_swapped (	u_char		*buffer,
		size_t		count,
		u_int32		pattern )
{
    register u_char *bptr;
    union {
        u_char pat[sizeof(u_int32)];
        u_int32 pattern;
    } p;
    register size_t i;

    /*
     * Initialize the buffer with a data pattern.
     */
    p.pattern = pattern;
    bptr = buffer;
    for (i = count; i ; ) {
        *bptr++ = p.pat[--i & (sizeof(u_int32) - 1)];
    }
    return;
}
#endif /* _BIG_ENDIAN_ */

/************************************************************************
 *									*
 * init_lbdata() - Initialize Data Buffer with Logical Block Data.	*
 *									*
 * Description:								*
 *	This function takes the starting logical block address, and	*
 * inserts it every logical block size bytes, overwriting the first 4	*
 * bytes of each logical block with its' address.			*
 *									*
 * Inputs:	buffer = The data buffer to initialize.			*
 *		count = The data buffer size (in bytes).		*
 *		lba = The starting logical block address.		*
 *		lbsize = The logical block size (in bytes).		*
 *									*
 * Outputs:	Returns the next lba to use.				*
 *									*
 ************************************************************************/
u_int32
init_lbdata (
	u_char		*buffer,
	size_t		count,
	u_int32		lba,
	u_int32		lbsize )
{
    u_char *bptr = buffer;
    register ssize_t i;

    /*
     * Initialize the buffer with logical block data.
     */
    if (prefix_string) {
        register size_t pcount = 0, scount = lbsize;
	/*
	 * The lba is encoded after the prefix string.
	 */
	pcount = MIN(prefix_size, count);
	scount -= pcount;
        for (i = 0; (i+pcount+sizeof(lba)) <= count; ) {
            bptr += pcount;
            htos (bptr, lba, sizeof(lba));
            i += lbsize;
            bptr += scount;
            lba++;
        }
    } else {
        for (i = 0; (i+sizeof(lba)) <= count; ) {
            htos (bptr, lba, sizeof(lba));
            i += lbsize;
            bptr += lbsize;
            lba++;
        }
    }

    return (lba);
}

#if defined(TIMESTAMP)
/************************************************************************
 *									*
 * init_timestamp() - Initialize Data Buffer with a Timestamp.          *
 *									*
 * Description:								*
 *	This function places a timestamp in the first 4 bytes of each   *
 * data block.                                                          *
 *									*
 * Inputs:	buffer = The data buffer to initialize.			*
 *		count = The data buffer size (in bytes).		*
 *		lbsize = The logical block size (in bytes).		*
 *									*
 * Outputs:	Returns the next lba to use.				*
 *									*
 ************************************************************************/
void
init_timestamp (
	u_char		*buffer,
	size_t		count,
	u_int32		lbsize )
{
    u_char *bptr = buffer;
    register ssize_t i;
    register time_t timestamp = time((time_t *)NULL);

    /*
     * Initialize the buffer with a timestamp (in seconds).
     */
    if (prefix_string) {
        register size_t pcount = 0, scount = lbsize;
	/*
	 * The timestamp is encoded after the prefix string.
	 */
	pcount = MIN(prefix_size, count);
	scount -= pcount;
        for (i = 0; (i+pcount+sizeof(timestamp)) <= count; ) {
            bptr += pcount;
            htos (bptr, (large_t)timestamp, sizeof(timestamp));
            i += lbsize;
            bptr += scount;
        }
    } else {
        for (i = 0; (i+sizeof(timestamp)) <= count; ) {
            htos (bptr, (large_t)timestamp, sizeof(timestamp));
            i += lbsize;
            bptr += lbsize;
        }
    }
    return;
}
#endif /* defined(TIMESTAMP) */

#if !defined(INLINE_FUNCS)
/*
 * Calculate the starting logical block number.
 */
u_int32
make_lba(
	struct dinfo	*dip,
	OFF_T		pos )
{
    if (pos == (OFF_T) 0) {
	return ((u_int32) 0);
    } else {
	return (pos / lbdata_size);
    }
}

OFF_T
make_offset(struct dinfo *dip, u_int32 lba)
{
    return ((OFF_T)(lba * lbdata_size));
}

/*
 * Calculate the starting lbdata block number.
 */
u_int32
make_lbdata(
	struct dinfo	*dip,
	OFF_T		pos )
{
    if (pos == (OFF_T) 0) {
	return ((u_int32) 0);
    } else {
	return (pos / lbdata_size);
    }
}

#endif /* !defined(INLINE_FUNCS) */

u_int32
winit_lbdata(
	struct dinfo	*dip,
	OFF_T		pos,
	u_char		*buffer,
	size_t		count,
	u_int32		lba,
	u_int32		lbsize )
{
    if (user_lbdata) {
	/* Using user defined lba, not file position! */
	return (init_lbdata (buffer, count, lba, lbsize));
    } else if (pos == (OFF_T) 0) {
	return (init_lbdata (buffer, count, (u_int32) 0, lbsize));
    } else {
	return (init_lbdata (buffer, count, (pos / lbsize), lbsize));
    }
}

/************************************************************************
 *									*
 * init_iotdata() - Initialize Pattern Buffer with IOT test pattern.	*
 *									*
 * Description:								*
 *	This function takes the starting logical block address, and	*
 * inserts it every logical block size bytes.  The data pattern used	*
 * is the logical block with the constant 0x01010101 added every u_int.	*
 *									*
 *	NOTE: The IOT pattern is stored in the pattern buffer, which	*
 * is assumed to be page aligned, thus there are no alignment problems.	*
 * Note:  Addition of prefix string changes this alignment assumption!  *
 *									*
 *	This implementation does _not_ provide the "best" performance,	*
 * but it does allow the normal 'dt' data flow to be mostly unaffected.	*
 *									*
 * Inputs:	bcount = The data buffer size (in bytes).		*
 *		lba = The starting logical block address.		*
 *		lbsize = The logical block size (in bytes).		*
 *									*
 * Outputs:	Returns the next lba to use.				*
 *                                                                      *
 * Note: If the count is smaller than sizeof(u_int32), then no lba is   *
 * encoded in the buffer.  Instead, we init odd bytes with ~0.          *
 *									*
 ************************************************************************/
u_int32
init_iotdata (
	size_t		bcount,
	u_int32		lba,
	u_int32		lbsize )
{
    register ssize_t count = (ssize_t)bcount;
    register u_int32 lba_pattern;
    /* IOT pattern initialization size. */
    register int iot_icnt = sizeof(lba_pattern);
    register int i;

    if (lbsize == 0) return (lba);

    pattern_bufptr = pattern_buffer;
    /*
     * If the prefix string is a multiple of an unsigned int,
     * we can initialize the buffer using 32-bit words, otherwise
     * we must do so a byte at a time which is slower (of course).
     *
     * Note: This 32-bit fill is possible since the pattern buffer
     * is known to be page aligned!
     *
     * Format: <prefix string><IOT pattern>...
     *
     * Also Note:  The prefix string is copied to the data buffer
     * by fill_buffer(), so don't be mislead by this code.  The
     * IOT pattern bytes is adjusted for the sizeof(prefix_string).
     */
    if (count < iot_icnt) {
        init_buffer(pattern_buffer, count, ~0);
    } else if (prefix_string && (prefix_size & (iot_icnt-1))) {
        register u_char *bptr = pattern_buffer;
        /*
         * Initialize the buffer with the IOT test pattern.
         */
        while ( (count > 0) && (count >= sizeof(lba)) ) {
            /*
             * Process one lbsize'd block at a time.
             *
             * Format: <optional prefix><lba><lba data>...
             */
            lba_pattern = lba++;
            for (i = (lbsize - prefix_size); 
                 ( (i > 0) && (count >= iot_icnt) ); ) {
#if _BIG_ENDIAN_
                init_swapped(bptr, iot_icnt, lba_pattern);
#else /* !_BIG_ENDIAN_ */
                init_buffer(bptr, iot_icnt, lba_pattern);
#endif /* _BIG_ENDIAN_ */
                lba_pattern += 0x01010101;
                i -= iot_icnt;
                bptr += iot_icnt;
                count -= iot_icnt;
            }
        }
        /* Handle any residual count here! */
        if (count && (count < iot_icnt)) {
            init_buffer(bptr, count, ~0);
        }
    } else {
        register int wperb; /* words per lbsize'ed buffer */
        register u_int32 *bptr;

        wperb = (lbsize / iot_icnt);
        bptr = (u_int32 *)pattern_buffer;
        if (prefix_string) {
            /*
             * Adjust counts for the prefix string.
             */
            wperb -= (prefix_size / iot_icnt);
            count -= prefix_size;
        }

        /*
         * Initialize the buffer with the IOT test pattern.
         */
        while ( (count > 0) && (count >= iot_icnt) ) {
            lba_pattern = lba++;
            for (i = 0; (i < wperb) && (count >= iot_icnt); i++) {
#if _BIG_ENDIAN_
                init_swapped((u_char *)bptr++, iot_icnt, lba_pattern);
#else /* !_BIG_ENDIAN_ */
                *bptr++ = lba_pattern;
#endif /* _BIG_ENDIAN_ */
                lba_pattern += 0x01010101;
                count -= iot_icnt;
            }
        }
        /* Handle any residual count here! */
        if (count && (count < iot_icnt)) {
            init_buffer((u_char *)bptr, count, ~0);
        }
    }
    return (lba);
}

/************************************************************************
 *									*
 * init_padbytes() - Initialize pad bytes at end of data buffer.	*
 *									*
 * Inputs:	buffer = Pointer to start of data buffer.		*
 *		offset = Offset to where pad bytes start.		*
 *		pattern = Data pattern to init buffer with.		*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
init_padbytes (	u_char		*buffer,
		size_t		offset,
		u_int32		pattern )
{
	size_t i;
	u_char *bptr;
	union {
	    u_char pat[sizeof(u_int32)];
	    u_int32 pattern;
	} p;

	p.pattern = pattern;
	bptr = buffer + offset;
	for (i = 0; i < PADBUFR_SIZE; i++) {
	    *bptr++ = p.pat[i & (sizeof(u_int32) - 1)];
	}
}

/*
 * copy_prefix() - Copy Prefix String to Buffer.
 *
 * Inputs:
 *	buffer = Pointer to buffer to copy prefix.
 *	bcount = Count of remaining buffer bytes.
 *
 * Implicit Inputs:
 *	prefix_string = The prefix string.
 *	prefix_size = The prefix size.
 *
 * Outputs:
 *	Returns number of prefix bytes copied.
 */
size_t
copy_prefix( u_char *buffer, size_t bcount )
{
    size_t pcount;

    pcount = MIN(prefix_size, bcount);
    (void)memcpy(buffer, prefix_string, pcount);
    return (pcount);
}

/*
 * verify_prefix() - Verify Buffer with Prefix String.
 *
 * Inputs:
 *	dip = The device information pointer.
 *	buffer = Address of buffer to verify.
 *	bcount = Count of remaining buffer bytes.
 *	pcount = Pointer to return prefix count verified.
 *
 * Implicit Inputs:
 *	prefix_string = The prefix string.
 *	prefix_size = The prefix size.
 *
 * Outputs:
 *	pcount gets the prefix string count verified.
 *	Return value is Success or Failure.
 */
int
verify_prefix( struct dinfo *dip, u_char *buffer, size_t bcount, size_t *pcount )
{
    register u_char *bptr = buffer;
    register u_char *pstr = (u_char *)prefix_string;
    register size_t count;
    register int i;
    int status = SUCCESS;

    count = MIN(prefix_size, bcount);

    for (i = 0; (i < count); i++, bptr++, pstr++) {
	if (*bptr != *pstr) {
	    size_t dump_size;
	    ReportCompareError (dip, count, i, *pstr, *bptr);
	    Fprintf ("Mismatch of data pattern prefix: '%s'\n", prefix_string);
	    /* expected */
	    dump_size = CalculateDumpSize (prefix_size);
	    dump_buffer (prefix_str, (u_char *)prefix_string,
				pstr, dump_size, prefix_size, TRUE);
	    /* received */
#if defined(TIMESTAMP)
            if (timestamp_flag) {
                display_timestamp(buffer+count);
            }
#endif /* defined(TIMESTAMP) */
	    dump_size = CalculateDumpSize (bcount);
	    dump_buffer (data_str, buffer, bptr, dump_size, bcount, FALSE);
	    status = FAILURE;
            (void)ExecuteTrigger(dip, "miscompare");
	    break;
	}
    }
    *pcount = count;
    return (status);
}

/************************************************************************
 *									*
 * verify_buffers() - Verify Data Buffers.				*
 *									*
 * Description:								*
 *	Simple verification of two data buffers.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		dbuffer = Data buffer to verify with.			*
 *		vbuffer = Verification buffer to use.			*
 *		count = The number of bytes to compare.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Data Ok/Compare Error.	*
 *									*
 ************************************************************************/
int
verify_buffers(	struct dinfo	*dip,
		u_char		*dbuffer,
		u_char		*vbuffer,
		size_t		count )
{
    u_int32 i;
    u_char *dptr = dbuffer;
    u_char *vptr = vbuffer;

    for (i = 0; (i < count); i++, dptr++, vptr++) {
	if (*dptr != *vptr) {
	    size_t dump_size = CalculateDumpSize (count);
	    ReportCompareError (dip, count, i, *dptr, *vptr);
	    /* expected */
	    dump_buffer (data_str, dbuffer, dptr, dump_size, count, TRUE);
	    /* received */
#if defined(TIMESTAMP)
            if (timestamp_flag) {
                display_timestamp(vbuffer);
            }
#endif /* defined(TIMESTAMP) */
	    dump_buffer (verify_str, vbuffer, vptr, dump_size, count, FALSE);
            (void)ExecuteTrigger(dip, "miscompare");
	    return (FAILURE);
	}
    }
    return (SUCCESS);
}

/************************************************************************
 *									*
 * verify_lbdata() - Verify Logical Block Address in Buffer.            *
 *									*
 * Description:								*
 *	Note: This function is used during read-after-write tests.      *
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		dbuffer = Data buffer to verify with.			*
 *		vbuffer = Verification buffer to use.			*
 *		count = The number of bytes to compare.			*
 *              lba = Pointer to return last lba verified.              *
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = lba Ok/Compare Error. 	*
 *									*
 ************************************************************************/
int
verify_lbdata(	struct dinfo	*dip,
		u_char		*dbuffer,
		u_char		*vbuffer,
		size_t		count,
		u_int32		*lba )
{
    u_int32 i, dlbn = 0, vlbn;
    u_char *dptr = dbuffer;
    u_char *vptr = vbuffer;
    int status = SUCCESS;

    /*
     * Note: With timestamps enabled, we overwrite the lba.
     */
    if (timestamp_flag) { return (status); }
    for (i = 0; (i+sizeof(dlbn) <= count); i += lbdata_size,
		 dptr += lbdata_size, vptr += lbdata_size) {
	if (iot_pattern) {
	    dlbn = get_lbn(dptr);
	    vlbn = get_lbn(vptr);
	} else {
	    dlbn = stoh (dptr, sizeof(dlbn));
	    vlbn = stoh (vptr, sizeof(vlbn));
	}
	if (dlbn != vlbn) {
	    size_t dump_size = CalculateDumpSize (count);
	    ReportLbdataError(dip, *lba, count, i, dlbn, vlbn);
	    /* expected */
	    dump_buffer (data_str, dbuffer, dptr, dump_size, count, TRUE);
	    /* received */
	    dump_buffer (verify_str, vbuffer, vptr, dump_size, count, FALSE);
	    status = FAILURE;
	    break;
	}
    }
    *lba = (dlbn + 1);
    return (status);
}

/************************************************************************
 *									*
 * verify_data() - Verify Data Pattern.					*
 *									*
 * Description:								*
 *	If a pattern_buffer exists, then this data is used to compare	*
 * the buffer instead of the pattern specified.				*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		buffer = Pointer to data to verify.			*
 *		count = The number of bytes to compare.			*
 *		pattern = Data pattern to compare against.		*
 *		lba = Pointer to starting logical block address.	*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Data Ok/Compare Error.	*
 *		lba gets updated with the next lba to verify with.	*
 *									*
 ************************************************************************/
int
verify_data (	struct dinfo	*dip,
		u_char		*buffer,
		size_t		count,
		u_int32		pattern,
		u_int32		*lba )
{
    bool check_lba = (iot_pattern || (lbdata_flag && lbdata_size));

    /*
     * I hate to duplicate code, but the smaller functions
     * optimize better and give *much* better performance.
     */
    if ( !check_lba && !prefix_string ) {
	return ( verify_data_normal(dip, buffer, count, pattern) );
    } else if ( !check_lba && prefix_string ) {
	return ( verify_data_prefix(dip, buffer, count, pattern) );
    } else {
	return ( verify_data_with_lba(dip, buffer, count, pattern, lba) );
    }
}

static int
verify_data_normal(
	struct dinfo	*dip,
	u_char		*buffer,
	size_t		bcount,
	u_int32		pattern )
{
    register size_t i = 0;
    register u_char *vptr = buffer;
    register u_char *pptr = pattern_bufptr;
    register u_char *pend = pattern_bufend;
    register size_t count = bcount;
    bool error = FALSE;
    int status = SUCCESS;

    while ( (i < count) ) {
#if defined(TIMESTAMP)
        /*
         * Skip the timestamp (if enabled).
         */
        if (timestamp_flag && ((i % lbdata_size) == 0)) {
            int p;
            i += sizeof(time_t);
            vptr += sizeof(time_t);
            for (p = 0; (p < sizeof(time_t)); p++) {
                if (++pptr == pend) pptr = pattern_buffer;
            }
        }
#endif /* defined(TIMESTAMP) */
	if (*vptr != *pptr) {
	    error = TRUE;
	    ReportCompareError (dip, count, i, *pptr, *vptr);
	    break;
	} else {
	    i++, pptr++, vptr++;
	    if (pptr == pend) pptr = pattern_buffer;
	}
    }
    if (error) {
	if (dump_flag) {
	    size_t dump_size = CalculateDumpSize (count);
	    if (pattern_buffer) {
		size_t pdump_size = (dump_size < patbuf_size)
					? dump_size : patbuf_size;
		/* expected */
		dump_buffer (pattern_str, pattern_buffer, pptr,
					pdump_size, patbuf_size, TRUE);
	    }
	    /* received */
#if defined(TIMESTAMP)
            if (timestamp_flag) {
                display_timestamp(buffer);
            }
#endif /* defined(TIMESTAMP) */
	    dump_buffer (data_str, buffer, vptr, dump_size, count, FALSE);
	}
        (void)ExecuteTrigger(dip, "miscompare");
	status = FAILURE;
    }
    pattern_bufptr = pptr;
    return (status);
}

static int
verify_data_prefix(
	struct dinfo	*dip,
	u_char		*buffer,
	size_t		bcount,
	u_int32		pattern )
{
    register size_t i = 0;
    register u_char *vptr = buffer;
    register u_char *pptr = pattern_bufptr;
    register u_char *pend = pattern_bufend;
    register size_t count = bcount;
    bool error = FALSE;
    int status = SUCCESS;

    while ( (i < count) ) {
	/*
	 * Verify the prefix string (if any).
	 */
	if (prefix_string && ((i % lbdata_size) == 0)) {
	    size_t pcount;
	    status = verify_prefix (dip, vptr, (count - i), &pcount);
	    if (status == FAILURE) return (status);
	    i += pcount;
	    vptr += pcount;
	    continue;
	}
	if (*vptr != *pptr) {
	    error = TRUE;
	    ReportCompareError (dip, count, i, *pptr, *vptr);
	    break;
	} else {
	    i++, pptr++, vptr++;
	    if (pptr == pend) pptr = pattern_buffer;
	}
    }
    if (error) {
	if (dump_flag) {
	    size_t dump_size = CalculateDumpSize (count);
	    if (pattern_buffer) {
		size_t pdump_size = (dump_size < patbuf_size)
					? dump_size : patbuf_size;
		/* expected */
		dump_buffer (pattern_str, pattern_buffer, pptr,
					pdump_size, patbuf_size, TRUE);
	    }
	    /* received */
#if defined(TIMESTAMP)
            if (timestamp_flag) {
                display_timestamp(buffer);
            }
#endif /* defined(TIMESTAMP) */
	    dump_buffer (data_str, buffer, vptr, dump_size, count, FALSE);
	}
        (void)ExecuteTrigger(dip, "miscompare");
	status = FAILURE;
    }
    pattern_bufptr = pptr;
    return (status);
}

static int
verify_data_with_lba(
	struct dinfo	*dip,
	u_char		*buffer,
	size_t		bcount,
	u_int32		pattern,
	u_int32		*lba )
{
    register size_t i = 0;
    register u_char *vptr = buffer;
    register u_char *pptr = pattern_bufptr;
    register u_char *pend = pattern_bufend;
    register size_t count = bcount;
    register u_int32 lbn, vlbn = *lba;
    bool error = FALSE, lbn_error = FALSE;
    int status = SUCCESS;

    while ( (i < count) ) {
	/*
	 * Handle IOT and Lbdata logical block checks first.
	 */
	if ( ((i % lbdata_size) == 0) ) {
	    /*
	     * Verify the prefix string prior to encoded lba's.
	     */
	    if (prefix_string) {
		size_t pcount;
		status = verify_prefix (dip, vptr, (count - i), &pcount);
		if (status == FAILURE) return (status);
		vptr += pcount;
		if ( (i += pcount) == count) continue;
	    }
	    if ( (i+sizeof(lbn) <= count) ) {
		if (iot_pattern) {
		    vlbn = get_lbn(pptr);
		    lbn = get_lbn(vptr);
		} else {
		    lbn = stoh (vptr, sizeof(lbn));
		}
	        if (!timestamp_flag && (lbn != vlbn)) {
		    error = lbn_error = TRUE;
		    ReportLbdataError (dip, *lba, count, i, vlbn, lbn);
		    break;
		} else {
		    int size;
		    vlbn++;
		    i += sizeof(lbn);
		    vptr += sizeof(lbn);
		    /* Skip past pattern bytes, handling wrapping. */
		    size = sizeof(lbn);
		    while (size--) {
			pptr++;
			if (pptr == pend) pptr = pattern_buffer;
		    }
		}
		continue;
	    }
	}

	if (*vptr != *pptr) {
	    error = TRUE;
	    ReportCompareError (dip, count, i, *pptr, *vptr);
	    break;
	} else {
	    i++, pptr++, vptr++;
	    if (pptr == pend) pptr = pattern_buffer;
	}
    }

    if (error) {
	if (dump_flag) {
	    size_t dump_size = CalculateDumpSize (count);
            if (lbn_error && !iot_pattern) {
                u_int32 elbn = vlbn; /* Can't take address of register. */
                /* expected - yep, real ugly, but gotta be correct! */
                dump_buffer (lba_str, (u_char *)&elbn, (u_char *)&elbn,
                             sizeof(elbn), sizeof(elbn), TRUE); 
            } else if (pattern_buffer) {
		size_t pdump_size = (dump_size < patbuf_size)
					? dump_size : patbuf_size;
		/* expected */
		dump_buffer (pattern_str, pattern_buffer, pptr,
					pdump_size, patbuf_size, TRUE);
	    }
	    /* received */
#if defined(TIMESTAMP)
            if (timestamp_flag) {
                display_timestamp(buffer);
            }
#endif /* defined(TIMESTAMP) */
	    dump_buffer (data_str, buffer, vptr, dump_size, count, FALSE);
	}
        (void)ExecuteTrigger(dip, "miscompare");
	status = FAILURE;
    }
    pattern_bufptr = pptr;
    *lba = vlbn;		/* Pass updated lba back to caller. */
    return (status);
}

/************************************************************************
 *									*
 * verify_padbytes() - Verify Pad Bytes Consistency.			*
 *									*
 * Description:								*
 *	This function simply checks the pad bytes to ensure they	*
 * haven't been overwritten after a read operation.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		buffer = Pointer to start of pad buffer.		*
 *		count = The last record read byte count.		*
 *		pattern = Data pattern to compare against.		*
 *		offset = Offset to where pad bytes start.		*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Data Ok/Compare Error.	*
 *									*
 ************************************************************************/
int
verify_padbytes (
	struct dinfo	*dip,
	u_char		*buffer,
	size_t		count,
	u_int32		pattern,
	size_t		offset )
{
    size_t pbytes, pindex;
    int status;

    /*
     * For short reads, checks inverted data bytes & pad bytes.
     */
    if ( (offset != count) && spad_check) {
	size_t resid = (offset - count);
	pindex = (count & (sizeof(u_int32) - 1));
	pbytes = (resid < PADBUFR_SIZE) ? resid : PADBUFR_SIZE;
	status = dopad_verify (dip, buffer, count, pattern, pbytes, pindex, TRUE);
	if (status == FAILURE) return (status);
    }
    pindex = 0;
    pbytes = PADBUFR_SIZE;
    return (dopad_verify (dip, buffer, offset, pattern, pbytes, pindex, FALSE));
}

static int
dopad_verify (
	struct dinfo	*dip,
	u_char		*buffer,
	size_t		offset,
	u_int32		pattern,
	size_t		pbytes,
	size_t		pindex,
	bool		inverted )
{
	int status = SUCCESS;
	u_char *vptr;
	size_t i;
	union {
	    u_char pat[sizeof(u_int32)];
	    u_int32 pattern;
	} p;

	p.pattern = pattern;
	vptr = buffer + offset;

	/*
	 * NOTE: We could be comparing inverted data on short reads.
	 */
	for (i = pindex; i < (pbytes + pindex); i++, vptr++) {
	    if (*vptr != p.pat[i & (sizeof(u_int32) - 1)]) {
		(void) RecordError();
		Fprintf (
		"Data compare error at %s byte %u in record number %lu\n",
				(inverted) ? "inverted" : "pad",
				(inverted) ? (offset + i) : i,
				(dip->di_records_read + 1));
		ReportDeviceInfo (dip, offset, i, FALSE);
		Fprintf ("Data expected = %#x, data found = %#x, pattern = 0x%08x\n",
			p.pat[i & (sizeof(u_int32) - 1)], *vptr, pattern);
		if (dump_flag) {
		    /*
		     * Limit data dumped for short corrupted records.
		     */
		    size_t dump_size = CalculateDumpSize (offset);
		    dump_buffer (data_str, buffer, vptr, dump_size, data_size, FALSE);
	        } else {
		    Fprintf ("Data buffer pointer = %#lx, buffer offset = %ld\n",
								vptr, offset);
		}
		fflush (efp);
		status = FAILURE;
                (void)ExecuteTrigger(dip, "miscompare");
		break;
	    }
	}
	return (status);
}

/************************************************************************
 *									*
 * process_pfile() - Process a pattern file.				*
 *									*
 * Inputs:	fd   = Pointer to file descriptor.			*
 *		file = Pointer to pattern file name.			*
 *		mode = The mode (read/write) for open.			*
 *									*
 * Outputs:	Returns on success, exits on open failure.		*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
process_pfile (HANDLE *fd, char *file, int mode)
{
	struct stat sb;
	size_t count, size;
	u_char *buffer;

#if defined(__WIN32__)
	mode |= O_BINARY;
#endif /* defined(__WIN32__) */

#if defined(WIN32)
	if( (*fd = CreateFile (file, mode, (FILE_SHARE_READ | FILE_SHARE_WRITE), NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == NoFd) {
#else /* !defined(WIN32) */
	if ( (*fd = open (file, mode)) == FAILURE) {
#endif /* defined(WIN32) */
	    Fprintf ("Error opening pattern file '%s', mode = %o\n",
							file, mode);
	    report_error ("process_pfile", TRUE);
	    exit (exit_status);
	}
#if defined(WIN32)
	if (GetFileType (*fd) == FILE_TYPE_DISK) {
	    size = GetFileSize (*fd, NULL);
	} else {
	    Fprintf ("Expect regular file for pattern file.\n");
	    exit (exit_status);
        }
#else /* !defined(WIN32) */
	if (fstat (*fd, &sb) < 0) {
	    report_error ("fstat", TRUE);
	    exit (exit_status);
	}
	/*
	 * Only support regular files at this time.
	 */
	if ((sb.st_mode & S_IFMT) != S_IFREG) {
	    Fprintf ("Expect regular file for pattern file.\n");
	    exit (exit_status);
	}
	size = (size_t) sb.st_size;
#endif /* defined(WIN32) */
	buffer = (u_char *) myalloc (size, 0);
#if defined(WIN32)
	if(!ReadFile (*fd, buffer, size, &count, NULL)) count = -1;
	if (count != size) {
#else /* !defined(WIN32) */
	if ( (count = read (*fd, buffer, size)) != size) {
#endif /* defined(WIN32) */
	    Fprintf ("Pattern file '%s' read error!\n", file);
	    if ((ssize_t)count == FAILURE) {
#if defined(WIN32)
		report_error ("ReadFile", TRUE);
#else /* !defined(WIN32) */
		report_error ("read", TRUE);
#endif /* defined(WIN32) */
		exit (exit_status);
	    } else {
		LogMsg (efp, logLevelCrit, 0,
			"Attempted to read %d bytes, read only %d bytes.",
							 size, count);
		exit (FATAL_ERROR);
	    }
	}

	patbuf_size = size;
	setup_pattern (buffer, size);
	/*
	 * Currently don't need pattern file open for anything else.
	 */
#if defined(WIN32)
	CloseHandle (*fd); *fd = NoFd;
#else /* !defined(WIN32) */
	(void) close (*fd); *fd = -1;
#endif /* defined(WIN32) */
}

/************************************************************************
 *									*
 * setup_pattern() - Setup pattern variables.				*
 *									*
 * Inputs:	buffer = Pointer to pattern buffer.			*
 *		size = Size of pattern buffer.				*
 *									*
 * Outputs:	Returns on success, exits on open failure.		*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
setup_pattern (u_char *buffer, size_t size)
{
	patbuf_size = size;
	pattern_buffer = buffer;
	pattern_bufptr = buffer;
	pattern_bufend = buffer + size;
	pattern = (u_int32) 0;

	switch (size) {

	    case sizeof(u_char):
		pattern = (u_int32) buffer[0];
		break;

	    case sizeof(u_short):
		pattern = ( ((u_int32)buffer[1] << 8) | (u_int32)buffer[0] );
		break;

	    case 0x03:
		pattern = ( ((u_int32)buffer[2] << 16) | ((u_int32)buffer[1] << 8) |
			    (u_int32) buffer[0] );
		break;

	    default:
		pattern = ( ((u_int32)buffer[3] << 24) | ((u_int32)buffer[2] << 16) |
			    ((u_int32)buffer[1] << 8) | (u_int32)buffer[0]);
		break;
	}
}

/*
 * Copy pattern bytes to pattern buffer with proper byte ordering.
 */
void
copy_pattern (u_int32 pattern, u_char *buffer)
{
	buffer[0] = (u_char) pattern;
	buffer[1] = (u_char) (pattern >> 8);
	buffer[2] = (u_char) (pattern >> 16);
	buffer[3] = (u_char) (pattern >> 24);
}

/*
 * Function to display ASCII time.
 */
char *
bformat_time (char *bp, clock_t time)
{
    u_int hr, min, sec, frac;

    frac = time % hz;
    frac = (frac * 100) / hz;
    time /= hz;
    sec = time % 60; time /= 60;
    min = time % 60;
    if (hr = time / 60) {
        bp += Sprintf(bp, "%dh", hr);
    }
    bp += Sprintf(bp, "%02dm", min);
    bp += Sprintf(bp, "%02d.", sec);
    bp += Sprintf(bp, "%02ds", frac);
    return (bp);
}

void
print_time (FILE *fp, clock_t time)
{
	u_int hr, min, sec, frac;

	frac = time % hz;
	frac = (frac * 100) / hz;
	time /= hz;
	sec = time % 60; time /= 60;
	min = time % 60;
	if (hr = time / 60) {
	    fprintf (fp, "%dh", hr);
	}
	fprintf (fp, "%02dm", min);
	fprintf (fp, "%02d.", sec);
	fprintf (fp, "%02ds\n", frac);
}

void
format_time (clock_t time)
{
	clock_t hr, min, sec, frac;
#if !defined(WIN32)
	frac = (time % hz);
	frac = (frac * 100) / hz;
	time /= hz;
#endif /* !defined(WIN32) */
	sec = time % 60; time /= 60;
	min = time % 60;
	if (hr = time / 60) {
	    Lprintf ("%dh", hr);
	}
	Lprintf ("%02dm", min);
#if defined(WIN32)
	Lprintf ("%02ds\n", sec);
#else /* !defined(WIN32) */
	Lprintf ("%02d.", sec);
	Lprintf ("%02ds\n", frac);
#endif /* defined(WIN32) */
}

#if defined(DEC)
/*
 * Format table() sysinfo time.
 */
void
format_ltime (long time, int tps)
{
	long hr, min, sec, frac;

	/*Lprintf ("%.3f - ", (float)((float)time/(float)tps));*/
	frac = (time % tps);
	frac = (frac * 1000) / tps;
	time /= tps;
	sec = time % 60; time /= 60;
	min = time % 60;
	if (hr = time / 60) {
	    Lprintf ("%dh", hr);
	}
	Lprintf ("%02dm", min);
	Lprintf ("%02d.", sec);
	Lprintf ("%03ds\n", frac);
}
#endif /* defined(DEC) */

/************************************************************************
 *									*
 * seek_file()	Seeks to the specified file offset.			*
 *									*
 * Inputs:	fd      = The file descriptor.				*
 *		records = The number of records.			*
 *		size    = The size of each record.			*
 *		whence  = The method of setting position:		*
 *									*
 *		SEEK_SET (L_SET)  = Set to offset bytes.		*
 *		SEEK_CUR (L_INCR) = Increment by offset bytes.		*
 *		SEEK_END (L_XTND) = Extend by offset bytes.		*
 *									*
 *		offset = (record count * size of each record)		*
 *									*
 * Return Value:							*
 *	Returns file position on Success, (off_t)-1 on Failure.		*
 *									*
 ************************************************************************/
OFF_T
seek_file (HANDLE fd, u_long records, OFF_T size, int whence)
{
    OFF_T pos;

    /*
     * Seek to specifed file position.
     */
#if defined(WIN32)
    if ((pos = SetFilePtr ( fd, (OFF_T)(records * size), whence)) == (OFF_T)-1) {
        Fprintf("SetFilePtr failed (fd %d, offset " FUF ", whence %d)\n",
                fd, (off_t)(records * size), whence);
        report_error ("SetFilePointer", TRUE);
#else /* !defined(WIN32) */
    if ((pos = lseek (fd, (OFF_T)(records * size), whence)) == (OFF_T)-1) {
        Fprintf("lseek failed (fd %d, offset " FUF ", whence %d)\n",
                fd, (off_t)(records * size), whence);
	report_error ("lseek", TRUE);
#endif /* defined(WIN32) */
    }
    return (pos);
}

/*
 * Utility functions to handle file positioning.
 */
OFF_T
seek_position (struct dinfo *dip, OFF_T offset, int whence)
{
    OFF_T pos;
#if defined(WIN32)
    LARGE_INTEGER seek;
#endif /* defined(WIN32) */

#if defined(DEBUG)
    if (Debug_flag) {
        Printf ("attempting lseek (fd=%d, offset=" FUF ", whence=%d)\n",
					dip->di_fd, offset, whence);
    }
#endif /* defined(DEBUG) */

    /*
     * Seek to specifed file position.
     */
#if defined(WIN32)
    if ((pos = SetFilePtr ( dip->di_fd, offset, whence)) == (OFF_T)-1) {
#else /* !defined(WIN32) */
    if ((pos = lseek (dip->di_fd, offset, whence)) == (OFF_T) -1) {
#endif /* defined(WIN32) */
	if (Debug_flag) {
	    Printf ("failed lseek (fd %d, offset " FUF ", whence %d)\n",
					dip->di_fd, offset, whence);
	}
#if defined(WIN32)
	report_error ("SetFilePointer", TRUE);
#else /* !defined(WIN32) */
	report_error ("lseek", TRUE);
#endif /* defined(WIN32) */
	terminate (exit_status);
    }

#if defined(DEBUG)
    if (Debug_flag) {
	Printf ("pos=" FUF " = lseek (fd=%d, offset=%lu, whence=%d)\n",
					pos, dip->di_fd, offset, whence);
    }
#endif /* defined(DEBUG) */

    return (pos);
}

#if !defined(INLINE_FUNCS)

OFF_T
get_position (struct dinfo *dip)
{
#if defined(_BSD)
    return (seek_position (dip, (OFF_T) 0, L_INCR));
#else /* !defined(_BSD) */
    return (seek_position (dip, (OFF_T) 0, SEEK_CUR));
#endif /* defined(_BSD) */
}

#endif /* !defined(INLINE_FUNCS) */

u_int32
get_lba (struct dinfo *dip)
{
    OFF_T pos;
    if ( (pos = get_position(dip)) ) {
	return ( (u_int32)(pos / lbdata_size) );
    } else {
	return ( (u_int32) 0 );
    }
}

OFF_T
incr_position (struct dinfo *dip, OFF_T offset)
{
#if defined(_BSD)
    OFF_T pos = seek_position (dip, offset, L_INCR);
#else /* !defined(_BSD) */
    OFF_T pos = seek_position (dip, offset, SEEK_CUR);
#endif /* defined(_BSD) */

    if (Debug_flag || rDebugFlag) {
	large_t lba = (pos / (OFF_T)dip->di_dsize);
	Printf ("Seeked to block " LUF " (" LXF ") at byte position " FUF "\n",
							lba, lba, pos);
    }
    return (pos);
}

OFF_T
set_position (struct dinfo *dip, OFF_T offset)
{
#if defined(_BSD)
    OFF_T pos = seek_position (dip, offset, L_SET);
#else /* !defined(_BSD) */
    OFF_T pos = seek_position (dip, offset, SEEK_SET);
#endif /* defined(_BSD) */

    if (Debug_flag || rDebugFlag) {
	large_t lba = (pos / (OFF_T)dip->di_dsize);
	Printf ("Seeked to block " LUF " (" LXF ") at byte position " FUF "\n",
							lba, lba, pos);
    }
    return (pos);
}

#if !defined(INLINE_FUNCS)

OFF_T
make_position(struct dinfo *dip, u_int32 lba)
{
    return ( (OFF_T)(lba * lbdata_size));
}

#endif /* !defined(INLINE_FUNCS) */

void
show_position (struct dinfo *dip, OFF_T pos)
{
    if (debug_flag || rDebugFlag) {
	large_t lba = make_lba(dip, pos);
	Printf ("Current file offset is " FUF " (" FXF "), relative lba is %u (%#x)\n",
						pos, pos, lba, lba);
    }
}

u_long
get_random(void)
{
    /*
     * These first 2 functions generate a number in range (0 - 2^31).
     */
#if defined(RAND48)
    return ( (u_long)lrand48() );
#elif defined(RANDOM)
    return ( (u_long)random() );
#else /* Use ANSI rand() below... */
    /*
     * ANSI specifies rand() returns value in range (0 - 32767).
     * NOTE: BSD rand() returns value in range (0 - 2^31) also.
     */
    return ( (u_long)rand() );
#endif /* defined(RAND28) */
}

/*
 * Function to calculate variable length request size.
 */
size_t
get_variable (struct dinfo *dip)
{
    register size_t length;
    register u_long randum;

    randum = get_random();
    length = (size_t)((randum % max_size) + min_size);
    if (dip->di_dsize) {
        if ( (dip->di_dtype->dt_dtype == DT_REGULAR) && !fsalign_flag) {
            length = roundup(length, patbuf_size);
        } else {
	    length = roundup(length, dip->di_dsize);
        }
    }
    if (length > max_size) length = max_size;
    return (length);
}

/*
 * Function to set random number generator seed.
 */
void
set_rseed(u_int seed)
{
#if defined(RAND48)		/* System V */
    srand48 ((long) seed);
#elif defined(RANDOM)		/* BSD? */
    srandom (seed);
#else 				/* ANSI */
    srand (seed);
#endif
}

/*
 * Function to set position for random I/O.
 */
OFF_T
do_random (struct dinfo *dip, bool doseek, size_t xfer_size)
{
    register OFF_T pos, dsize, ralign;
    register large_t randum;
    u_long records;

    dsize = (OFF_T)(dip->di_dsize);
    
    if (dip->di_mode == READ_MODE) {
	records = dip->di_records_read;
    } else {
	records = dip->di_records_written;
    }

    /*
     * Ensure the random alignment size is modulo the device size.
     */
    if ( (dip->di_dtype->dt_dtype == DT_REGULAR) && !fsalign_flag) {
        off_t fsalign = patbuf_size;
        if (user_ralign) {
            ralign = (off_t)((random_align) ? random_align : fsalign);
        } else {
            ralign = fsalign;
        }
        ralign = roundup(ralign, fsalign);
    } else {
        ralign = (off_t)((random_align) ? random_align : dsize);
        ralign = roundup(ralign, dsize);
    }

    randum = (large_t)get_random();

    /*
     * Set position so that the I/O is in the range from file_position to 
     * data_limit and is block size aligned.
     *
     * Since randum only ranges to 2^31, scale the offset by shift value,
     * which is based on the capacity.  See dtread.c: ReadCapacity()
     *
     * Note:  This scaling/rounding/range checking kills performance!
     */
    randum <<= dip->di_rshift;	/* Scale up! */
    pos = (OFF_T)(randum % rdata_limit);
    /*
     * Ensure the upper data limit isn't exceeded.
     */
    while ((pos + xfer_size) >= rdata_limit) {
	pos = ( (pos + xfer_size) % rdata_limit);
    }

    /*
     * Round up and align as necessary.
     */
    if (ralign) pos = roundup(pos, ralign);

    /*
     * Ensure the position is within random range.
     */
    if (pos < file_position) {
	if ((pos + file_position + xfer_size) >= rdata_limit) {
	    pos = (file_position + ((pos + xfer_size) / dsize));
	} else {
	    pos += file_position;
	}
	/* Round up and adjust back down as necessary. (yea, ugly :-) */
	while ( ((pos = roundup(pos, ralign)) + xfer_size) >= rdata_limit) {
	    pos -= (ralign + dsize);
	}
	if (pos < file_position) pos = file_position; /* too low! */
    }

    if (doseek) {
	return (set_position (dip, pos));
    } else {
	/*
	 * Note:  For AIO, we just calculate the random position.
	 */
	if (Debug_flag || rDebugFlag) {
	    large_t lba = (pos / dsize);
	    Printf ("Random position set to " FUF " block " LUF " (" LXF ").\n",
							pos, lba, lba);
	}
	return (pos);
    }
}

/************************************************************************
 *									*
 * skip_records() Skip past specified number of records.		*
 *									*
 * Inputs:	dip	= The device info pointer.			*
 *		records = The number of records.			*
 *		buffer  = The buffer to read into.			*
 *		size    = The size of each record.			*
 *									*
 * Return Value:							*
 *	Returns SUCCESS/FAILURE/WARNING = Ok/Read Failed/Partial Read.	*
 *									*
 ************************************************************************/
int
skip_records (	struct dinfo	*dip,
		u_long		records,
		u_char		*buffer,
		OFF_T		size)
{
    u_long i;
    size_t count;
    int status = SUCCESS;

    /*
     * Skip over the specified record(s).
     */
    for (i = 0; i < records; i++) {
#if defined(WIN32)
	if(!ReadFile (dip->di_fd, buffer, size, &count, NULL)) count = -1;
#else /* !defined(WIN32) */
	count = read (dip->di_fd, buffer, size);
#endif /* defined(WIN32) */
	if ( (status = check_read (dip, count, size)) == FAILURE) {
	    break;
	}
    }
    return (status);
}

/************************************************************************
 *									*
 * myalloc()	Allocate aligned buffer at specified offset.		*
 *									*
 * Description:	This function first gets a page aligned buffer using	*
 *		malloc(), then it returns a pointer offset bytes into	*
 *		the page.  This was done to test page alignment code	*
 *		in device drivers.					*
 *									*
 * Inputs:	size = The size of the buffer to allocate.		*
 *		offset = The offset into the page.			*
 *									*
 * Outputs:	Returns the allocated buffer or 0 for failure.		*
 *									*
 ************************************************************************/
void *
myalloc (size_t size, int offset)
{
	u_char *bp;

#if defined(_BSD)
	if ( (bp = (u_char *) valloc (size + offset)) == (u_char *) 0) {
	    LogMsg (efp, logLevelCrit, 0,
		    "valloc() failed allocating %lu bytes.\n",
						(size + offset));
	    exit (FATAL_ERROR);
	}
#elif defined(_QNX_SOURCE) && !defined(_QNX_32BIT)
	if ( (bp = (u_char *) malloc (size + offset)) == (u_char *) 0) {
	    LogMsg (efp, logLevelCrit, 0,
		    "malloc() failed allocating %u bytes.\n",
						(size + offset));
	    exit (FATAL_ERROR);
	}
#else /* !defined_BSD) || !defined(_QNX_SOURCE) */
	if ( (bp = (u_char *) malloc (size + offset + page_size)) == (u_char *) 0) {
	    LogMsg (efp, logLevelCrit, 0,
		    "malloc() failed allocating %lu bytes.\n",
					(size + offset + page_size));
	    exit (FATAL_ERROR);
	} else {
	    bp = (u_char *)( ((ptr_t)bp + (page_size-1)) &~ (page_size-1) );
	}
#endif /* defined(_BSD) */
	bp += offset;
	if (debug_flag) {
	    Printf (
	"Allocated buffer at address %#lx of %u bytes, using offset %u\n",
							bp, size, offset);
	}
	memset (bp, '\0', size);
	return (bp);
}

/*
 * Allocate memory with appropriate error checking.
 */
void *
Malloc (size_t size)
{
	void *bp;

	if ( (bp = malloc (size)) == NULL) {
	    LogMsg (efp, logLevelCrit, 0,
		    "malloc() failed allocating %u bytes.\n", size);
#if defined(WIN32)
	    exit(ERROR_OUTOFMEMORY);
#else /* !defined(WIN32) */
	    exit (ENOMEM);
#endif /* defined(WIN32) */
	}
	memset (bp, '\0', size);
	return (bp);
}

/************************************************************************
 *									*
 * CvtStrtoValue() - Converts ASCII String into Numeric Value.		*
 *									*
 * Inputs:	nstr = String to convert.				*
 *		eptr = Pointer for terminating character pointer.	*
 *		base = The base used for the conversion.		*
 *									*
 * Outputs:	eptr = Points to terminating character or nstr if an	*
 *			invalid if numeric value cannot be formed.	*
 *									*
 * Return Value:							*
 *		Returns converted number or -1 for FAILURE.		*
 *									*
 ************************************************************************/
u_long
CvtStrtoValue (char *nstr, char **eptr, int base)
{
	u_long n = 0, val;

	if ( (n = strtoul (nstr, eptr, base)) == 0L) {
	    if (nstr == *eptr) {
		n++;
	    }
	}
#ifdef notdef
	if (nstr == *eptr) {
	    return (n);
	}
#endif /* notdef */
	nstr = *eptr;
	for (;;) {

	    switch (*nstr++) {

		case 'k':
		case 'K':			/* Kilobytes */
			n *= KBYTE_SIZE;
			continue;

		case 'g':
		case 'G':			/* Gigibytes */
			n *= GBYTE_SIZE;
			continue;

		case 'm':
		case 'M':			/* Megabytes */
			n *= MBYTE_SIZE;
			continue;

#if defined(QuadIsLong)
	        case 't':
	        case 'T':
		        n *= TBYTE_SIZE;
			continue;
#endif /* defined(QuadIsLong) */

		case 'w':
		case 'W':			/* Word count. */
			n *= sizeof(int);
			continue;

		case 'q':
		case 'Q':			/* Quadword count. */
			n *= sizeof(large_t);
			continue;

		case 'b':
		case 'B':			/* Block count. */
			n *= BLOCK_SIZE;
			continue;

		case 'd':
		case 'D':			/* Device size. */
			n *= device_size;
			continue;

		case 'c':
		case 'C':			/* Core clicks. */
		case 'p':
		case 'P':			/* Page size. */
			n *= page_size;
			continue;

		case 'i':
		case 'I':
			if ( ( ( nstr[0] == 'N' ) || ( nstr[0] == 'n' ) ) &&
			     ( ( nstr[1] == 'F' ) || ( nstr[1] == 'f' ) )) {
			    nstr += 2;
			    n = (u_long)INFINITY;
			    continue;
			} else {
			    goto error;
			}

		case '+':
			n += CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '-':
			n -= CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '*':
		case 'x':
		case 'X':
			n *= CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '/':
			val = CvtStrtoValue (nstr, eptr, base);
			if (val) n /= val;
			nstr = *eptr;
			continue;

		case '%':
			val = CvtStrtoValue (nstr, eptr, base);
			if (val) n %= val;
			nstr = *eptr;
			continue;

		case '~':
			n = ~CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '|':
			n |= CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '&':
			n &= CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '^':
			n ^= CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '<':
			if (*nstr++ != '<') goto error;
			n <<= CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;
			
		case '>':
			if (*nstr++ != '>') goto error;
			n >>= CvtStrtoValue (nstr, eptr, base);
			nstr = *eptr;
			continue;
			
		case ' ':
		case '\t':
			continue;

		case '\0':
			*eptr = --nstr;
			break;

		default:
error:
			n = 0L;
			*eptr = --nstr;
			break;
	    }
	    return (n);
	}
}

/************************************************************************
 *									*
 * CvtStrtoLarge() - Converts ASCII String into Large Value.		*
 *									*
 * Inputs:	nstr = String to convert.				*
 *		eptr = Pointer for terminating character pointer.	*
 *		base = The base used for the conversion.		*
 *									*
 * Outputs:	eptr = Points to terminating character or nstr if an	*
 *			invalid if numeric value cannot be formed.	*
 *									*
 * Return Value:							*
 *		Returns converted number or -1 for FAILURE.		*
 *									*
 ************************************************************************/
large_t
CvtStrtoLarge (char *nstr, char **eptr, int base)
{
	large_t n = 0, val;

#if defined(QuadIsLong) || defined(HP_UX)  
	if ( (n = strtoul (nstr, eptr, base)) == (large_t) 0) {
#elif defined(QuadIsLongLong)
#  if defined(SCO) || defined(__QNXNTO__) || defined(SOLARIS) || defined(AIX) || defined(_NT_SOURCE)
	if ( (n = strtoull (nstr, eptr, base)) == (large_t) 0) {
#elif defined(WIN32)
	if ( (n = strtoul (nstr, eptr, base)) == (large_t) 0) {
#  else /* !defined(SCO) && !defined(__QNXNTO__) && !defined(AIX) && !defined(_NT_SOURCE) */
	if ( (n = strtouq (nstr, eptr, base)) == (large_t) 0) {
#  endif /* defined(SCO) || defined(__QNXNTO__) || defined(SOLARIS) || defined(AIX) || defined(_NT_SOURCE) */
#else /* assume QuadIsDouble */
	if ( (n = strtod (nstr, eptr)) == (large_t) 0) {
#endif
	    if (nstr == *eptr) {
		n++;
	    }
	}
#ifdef notdef
	if (nstr == *eptr) {
	    return (n);
	}
#endif /* notdef */
	nstr = *eptr;
	for (;;) {

	    switch (*nstr++) {

		case 'k':
		case 'K':			/* Kilobytes */
			n *= KBYTE_SIZE;
			continue;

		case 'g':
		case 'G':			/* Gigibytes */
			n *= GBYTE_SIZE;
			continue;

		case 'm':
		case 'M':			/* Megabytes */
			n *= MBYTE_SIZE;
			continue;

	        case 't':
	        case 'T':
		        n *= TBYTE_SIZE;
			continue;

		case 'w':
		case 'W':			/* Word count. */
			n *= sizeof(int);
			continue;

		case 'q':
		case 'Q':			/* Quadword count. */
			n *= sizeof(large_t);
			continue;

		case 'b':
		case 'B':			/* Block count. */
			n *= BLOCK_SIZE;
			continue;

		case 'd':
		case 'D':			/* Device size. */
			n *= device_size;
			continue;

		case 'c':
		case 'C':			/* Core clicks. */
		case 'p':
		case 'P':			/* Page size. */
			n *= page_size;
			continue;

		case 'i':
		case 'I':
			if ( ( ( nstr[0] == 'N' ) || ( nstr[0] == 'n' ) ) &&
			     ( ( nstr[1] == 'F' ) || ( nstr[1] == 'f' ) )) {
			    nstr += 2;
			    n = INFINITY;
			    continue;
			} else {
			    goto error;
			}

		case '+':
			n += CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '-':
			n -= CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '*':
		case 'x':
		case 'X':
			n *= CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '/':
			val = CvtStrtoLarge (nstr, eptr, base);
			if (val) n /= val;
			nstr = *eptr;
			continue;
#if !defined(QuadIsDouble)
		case '%':
			val = CvtStrtoLarge (nstr, eptr, base);
			if (val) n %= val;
			nstr = *eptr;
			continue;

		case '~':
			n = ~CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '|':
			n |= CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '&':
			n &= CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '^':
			n ^= CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;

		case '<':
			if (*nstr++ != '<') goto error;
			n <<= CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;
			
		case '>':
			if (*nstr++ != '>') goto error;
			n >>= CvtStrtoLarge (nstr, eptr, base);
			nstr = *eptr;
			continue;
#endif /* !defined(QuadIsDouble) */
		case ' ':
		case '\t':
			continue;

		case '\0':
			*eptr = --nstr;
			break;

		default:
error:
			n = 0;
			*eptr = --nstr;
			break;
	    }
	    return (n);
	}
}

/************************************************************************
 *									*
 * CvtTimetoValue() - Converts ASCII Time String to Numeric Value.	*
 *									*
 * Inputs:	nstr = String to convert.				*
 *		eptr = Pointer for terminating character pointer.	*
 *									*
 * Outputs:	eptr = Points to terminating character or nstr if an	*
 *			invalid if numeric value cannot be formed.	*
 *									*
 * Return Value:							*
 *		Returns converted number in seconds or -1 for FAILURE.	*
 *									*
 ************************************************************************/
time_t
CvtTimetoValue (char *nstr, char **eptr)
{
	time_t n = 0;
	int base = ANY_RADIX;

	if ( (n = strtoul (nstr, eptr, base)) == 0L) {
	    if (nstr == *eptr) {
		n++;
	    }
	}
#ifdef notdef
	if (nstr == *eptr) {
	    return (n);
	}
#endif /* notdef */
	nstr = *eptr;
	for (;;) {

	    switch (*nstr++) {

		case 'd':
		case 'D':			/* Days */
			n *= SECS_PER_DAY;
			continue;

		case 'h':
		case 'H':			/* Hours */
			n *= SECS_PER_HOUR;
			continue;

		case 'm':
		case 'M':			/* Minutes */
			n *= SECS_PER_MIN;
			continue;

		case 's':
		case 'S':			/* Seconds */
			continue;		/* default */

		case '+':
			n += CvtTimetoValue (nstr, eptr);
			nstr = *eptr;
			continue;

		case '-':
			n -= CvtTimetoValue (nstr, eptr);
			nstr = *eptr;
			continue;

		case '*':
		case 'x':
		case 'X':
			n *= CvtTimetoValue (nstr, eptr);
			nstr = *eptr;
			continue;

		case '/':
			n /= CvtTimetoValue (nstr, eptr);
			nstr = *eptr;
			continue;

		case '%':
			n %= CvtTimetoValue (nstr, eptr);
			nstr = *eptr;
			continue;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			nstr--;
			n += CvtTimetoValue (nstr, eptr);
			nstr = *eptr;
			continue;

		case ' ':
		case '\t':
			continue;

		case '\0':
			*eptr = --nstr;
			break;
		default:
			n = 0L;
			*eptr = --nstr;
			break;
	    }
	    return (n);
	}
}

#if defined(ultrix)
/*
 * This was added since the ULTRIX version returns rounded up whole
 * numbers instead of the scaled clock ticks.  I want the resolution.
 */
/*
 * times() for X/Open/POSIX compatability.
 *
 * Returns the number of (CLK_TCK)ths of a second since the epoch.
 * The value may wrap.
 */

static int
scale (struct timeval *tvp)
{
	/*
	 * convert seconds and microseconds to CLK_TCKs
	 */
	return ((tvp->tv_sec * CLK_TCK) + 
		((tvp->tv_usec * CLK_TCK) / 1000000));
}

clock_t
times (struct tms *tmsp)
{
	struct rusage ru;
	struct timeval tp;

	if (getrusage(RUSAGE_SELF, &ru) < 0)
		return (-1);
	tmsp->tms_utime = scale(&ru.ru_utime);
	tmsp->tms_stime = scale(&ru.ru_stime);
	if (getrusage(RUSAGE_CHILDREN, &ru) < 0)
		return ((clock_t) -1);
	tmsp->tms_cutime = scale(&ru.ru_utime);
	tmsp->tms_cstime = scale(&ru.ru_stime);
	if (gettimeofday(&tp, (struct timezone *)0) < 0)
		return((clock_t) -1);
	return((clock_t) scale(&tp));
}

#endif /* defined(ultrix) */

/*
 * Format & append time string to log file buffer.
 */
/*VARARGS*/
void
Ctime (time_t timer)
{
	char *buf;
	char *bp = log_bufptr;

	buf = ctime(&timer);
	(void) strcpy (bp, buf);
	bp += strlen(bp);
	log_bufptr = bp;
}

/*
 * Record current time of when error occured.
 */
u_long
RecordError(void)
{
    error_time = time((time_t *) 0);
    /* ctime() adds newline '\n' to time stamp. */
    LogMsg (efp, logLevelError, PRT_NOIDENT, "\n");
    LogMsg (efp, logLevelCrit, 0,
	    "Error number %lu occurred on %s",
	    ++error_count, ctime(&error_time));
    end_time = times (&etimes);
    Fprintf ("Elapsed time since beginning of pass: ");
    print_time (efp, end_time - pass_time);
    Fprintf ("Elapsed time since beginning of test: ");
    print_time (efp, end_time - start_time);
    exit_status = FAILURE;
    return (error_count);
}

/*
 * Record current time & record count of when warning occur.
 */
u_long
RecordWarning(u_long record)
{
	error_time = time((time_t *) 0);
	Printf ("Warning on record number %lu, occurred at %s",
			    (record + 1), ctime(&error_time));
	end_time = times (&etimes);
	Printf ("Elapsed time since beginning of pass: ");
	print_time (ofp, end_time - pass_time);
	Printf ("Elapsed time since beginning of test: ");
	print_time (ofp, end_time - start_time);
	return (warning_errors);
}

/*
 * Display failure message to stderr & flush output.
 */
/*VARARGS*/
void
LogMsg (FILE *fp, enum logLevel level, int flags, char *fmtstr, ...)
{
    va_list ap;

    if (hazard_flag && !(flags & PRT_NOLEVEL)) {
	fprintf (fp, "RPCLOG%d:", level);
    }
    if ( !(flags & PRT_NOIDENT) ) {
	if ( ((num_procs || num_slices) && !child_pid) || forked_flag) {
	    fprintf (fp, "%s (%d): ", cmdname, getpid());
	} else {
	    fprintf (fp, "%s: ", cmdname);
	}
    }

    va_start(ap, fmtstr);
    vfprintf (fp, fmtstr, ap);
    va_end(ap);
    if ( !(flags & PRT_NOFLUSH) ) {
	(void) fflush (fp);
    }
    return;
}

void
Fprintf (char *format, ...)
{
	va_list ap;
	FILE *fp = efp;

	if (hazard_flag) {
	    fprintf (fp, "RPCLOG%d:", logLevelError);
	} else if ( ((num_procs || num_slices) && !child_pid) || forked_flag) {
	    fprintf (fp, "%s (%d): ", cmdname, getpid());
	} else {
	    fprintf (fp, "%s: ", cmdname);
	}
	va_start(ap, format);
	vfprintf (fp, format, ap);
	va_end(ap);
	(void) fflush (fp);
}

/*
 * Same as Fprintf except no identifier or log prefix.
 */
/*VARARGS*/
void
Fprint (char *format, ...)
{
	va_list ap;
	FILE *fp = efp;

	va_start(ap, format);
	vfprintf (fp, format, ap);
	va_end(ap);
}

/*
 * Format & append string to log file buffer.
 */
/*VARARGS*/
void
Lprintf (char *format, ...)
{
	va_list ap;
	char *bp = log_bufptr;

	va_start(ap, format);
	vsprintf (bp, format, ap);
	va_end(ap);
	bp += strlen(bp);
	log_bufptr = bp;
}

/*
 * Flush the log buffer and reset the buffer pointer.
 */
void
Lflush(void)
{
	FILE *fp = ofp;
	Fputs (log_buffer, fp);
	fflush (fp);
	log_bufptr = log_buffer;
}

/*
 * Display message to stdout & flush output.
 */
/*VARARGS*/
void
Printf (char *format, ...)
{
	va_list ap;
	FILE *fp = ofp;

	if (hazard_flag) {
	    fprintf (fp, "RPCLOG%d:", logLevelLog);
	} else if ( ((num_procs || num_slices) && !child_pid) || forked_flag) {
	    fprintf (fp, "%s (%d): ", cmdname, getpid());
	} else {
	    fprintf (fp, "%s: ", cmdname);
	}
	va_start(ap, format);
	vfprintf (fp, format, ap);
	va_end(ap);
	(void) fflush (fp);
}

/*
 * Same as Printf except no program name identifier.
 */
/*VARARGS*/
void
Print (char *format, ...)
{
	va_list ap;
	FILE *fp = ofp;

	if (hazard_flag) {
	    fprintf (fp, "RPCLOG%d:", logLevelLog);
	}
	va_start(ap, format);
	vfprintf (fp, format, ap);
	va_end(ap);
}

/*VARARGS*/
int
Sprintf(char *bufptr, char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	(void) vsprintf (bufptr, msg, ap);
	va_end(ap);
	return (strlen (bufptr));
}

int	
vSprintf(char *bufptr, const char *msg, va_list ap)
{
	(void) vsprintf(bufptr, msg, ap);
	return (strlen (bufptr));
}

/************************************************************************
 *									*
 * Fputs()	Common function to Write String to an Output Stream.	*
 *									*
 * Inputs:	str = The string buffer pointer.			*
 *		stream = The file stream to access.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 * 									*
 ************************************************************************/
int
Fputs (char *str, FILE *stream)
{
	int status = SUCCESS;

	(void) fputs ((char *) str, stream);
	if (ferror (stream) != 0) {
	    clearerr (stream);
	    status = FAILURE;
	}
	return (status);
}

#if defined(_QNX_SOURCEx) || defined(SOLARIS) || defined(WIN32)
/*
 * TODO: Could be made a macro for better performance.
 */
void
bzero (char *buffer, size_t length)
{
	memset ((void *)buffer, '\0', length);
}

#endif /* defined(_QNX_SOURCE) || defined(SOLARIS) || defined(WIN32) */

/************************************************************************
 *									*
 * is_Eof() - Check For End Of File Condition.				*
 *									*
 * Description:								*
 *	Detect end of file or end of media.  Here's the presumptions:	*
 *									*
 *  For Writes, we expect a errno (count == -1) and (errno == ENOSPC).	*
 *   For Reads, a (count == 0) indicates end of file, while a		*
 *	(count == -1) and (errno == ENOSPC) indicates end of medium.	*
 *									*
 *	Actually, two file marks normally indicates the end of logical	*
 * tape, while (errno == ENOSPC) normally indicates reading past all of	*
 * the recorded data.  Note, some tapes (QIC) only have one file mark.	*
 *									*
 *	Is this confusing or what?  I'm doing the best I can here :-)	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		count = The count from the last I/O request.		*
 *		status = Optional pointer to status variable.		*
 *									*
 * Return Value:							*
 *		Returns TRUE / FALSE = End of file / Not Eof.		*
 *									*
 ************************************************************************/
int
is_Eof (struct dinfo *dip, size_t count, int *status)
{
    /*
     * We expect writes @ EOF to fail w/count -1, and errno ENOSPC.
     */
    if ( (dip->di_mode == WRITE_MODE) && (count == (size_t) 0) ) {
#if defined(BrokenEOF)
	dip->di_end_of_file = TRUE;
	exit_status = END_OF_FILE;
	return (end_of_file = TRUE);
#else /* !defined(BrokenEOF) */
	return (FALSE);
#endif /* defined(BrokenEOF) */
    }
#if defined(SCO) || defined(HP_UX) || defined(AIX)
    if ( (count == (size_t) 0) ||
	 ( ((ssize_t)count == (ssize_t) -1) &&
	   ((errno == ENOSPC) || (errno == ENXIO)) ) ) {
#elif defined(WIN32)
    if ( (count == (size_t) 0) ||
	 ( (count == -1) &&
	 ( (errno = GetLastError()) == ERROR_DISK_FULL) ) ) {
#else /* !defined(SCO) && !defined(HP_UX) && !defined(AIX) */
    if ( (count == (size_t) 0) ||
	 (((ssize_t)count == (ssize_t) -1) && (errno == ENOSPC)) ) {
#endif /* defined(SCO) || defined(HP_UX) || defined(AIX) */
	large_t data_bytes;
	if (dip->di_mode == READ_MODE) {
	    data_bytes = dip->di_dbytes_read;
	} else {
	    data_bytes = dip->di_dbytes_written;
	}
	if (debug_flag || eDebugFlag) {
	    Printf ("End of %s detected, count = %d, errno = %d [file #%lu, record #%lu]\n",
#if defined(WIN32)
			(count == 0) ? "file" : "media", count, GetLastError(),
#else /* !defined(WIN32) */
			(count == 0) ? "file" : "media", count, errno,
#endif /* defined(WIN32) */
			(dip->di_mode == READ_MODE) ?
			 (dip->di_files_read + 1) : (dip->di_files_written + 1),
			(dip->di_mode == READ_MODE) ?
			 (dip->di_records_read + 1) : (dip->di_records_written + 1));
	}
	if (dip->di_dtype->dt_dtype == DT_TAPE) {
	    if (count == (size_t) 0) {
		/* Two file mark's == end of logical tape. */
		if (dip->di_end_of_file) dip->di_end_of_logical = TRUE;
		if (dip->di_end_of_logical) dip->di_end_of_media = TRUE;
	    } else { /* ENOSPC */
		/* Remember, QIC tapes only have one file mark! */
		dip->di_end_of_logical = TRUE;
		dip->di_end_of_media = TRUE;
	    }
	}
#if defined(SCO) || defined(HP_UX)
	if ( ( ((ssize_t)count == (ssize_t) -1) &&
	       ((errno == ENOSPC) || (errno == ENXIO)) ) &&
	     (data_bytes == (large_t ) 0) ) { /* This is the key... */
#elif defined(WIN32)
	if ( ( (count == -1) &&
	     ( (errno = GetLastError()) == ERROR_DISK_FULL) ) &&
	     (data_bytes == (large_t ) 0) ) { /* This is the key... */
#else /* !defined(SCO) && !defined(HP_UX) */
	if ( (((ssize_t)count == (ssize_t) -1) && (errno == ENOSPC)) &&
	     (data_bytes == (large_t ) 0) ) { /* This is the key... */
#endif /* defined(SCO) || defined(HP_UX) */
	    exit_status = errno;
	    report_error((dip->di_mode == READ_MODE) ? "read" : "write", TRUE);
#if defined(EEI)
	    if ( (dip->di_dtype->dt_dtype == DT_TAPE) && eei_flag) {
		print_mtstatus (dip->di_fd, dip->di_mt, TRUE);
	    }
#endif /* defined(EEI) */
	    if (status) *status = FAILURE;
	} else {
	    exit_status = END_OF_FILE;
	}
#if defined(EEI)
	if ((dip->di_dtype->dt_dtype == DT_TAPE) && eei_flag) {
	    clear_eei_status(dip->di_fd, FALSE);
	}
#endif /* defined(EEI) */
	dip->di_end_of_file = TRUE;
	return (end_of_file = TRUE);
    }
    return (FALSE);
}

/*
 * Used to mimic EOF @ BOM when direction is reverse.
 */
void
set_Eof(struct dinfo *dip)
{
    if (debug_flag || eDebugFlag) {
	Printf ("Beginning of media detected [file #%lu, record #%lu]\n",
		(dip->di_mode == READ_MODE) ?
		 (dip->di_files_read + 1) : (dip->di_files_written + 1),
		(dip->di_mode == READ_MODE) ?
		 dip->di_records_read : dip->di_records_written);
    }
    end_of_file = TRUE;
    exit_status = END_OF_FILE;
    dip->di_end_of_file = TRUE;
    return;
}

void
ReportCompareError (
	struct dinfo	*dip,
	size_t		byte_count,
	u_int		byte_position,
	u_int		expected_data,
	u_int		data_found)
{
    (void) RecordError();
    if (dip->di_dtype->dt_dtype == DT_TAPE) {
	Fprintf ("File #%lu, %s %u in record number %lu\n", (dip->di_files_read + 1),
		compare_error_str, byte_position, (dip->di_records_read + 1));
    } else {
	Fprintf ("%s %u in record number %lu\n",
		compare_error_str, byte_position, (dip->di_records_read + 1));
    }
    ReportDeviceInfo (dip, byte_count, byte_position, FALSE);
    Fprintf ("Data expected = %#x, data found = %#x, byte count = %lu\n",
				expected_data, data_found, byte_count);
#if defined(LOG_DIAG_INFO)
    if (logdiag_flag) {
	char *bp = msg_buffer;
	if (dip->di_dtype->dt_dtype == DT_TAPE) {
	    bp += Sprintf(bp, "%s: File #%lu, %s %u in record number %lu\n", cmdname,
			(dip->di_files_read + 1), compare_error_str,
			byte_position, (dip->di_records_read + 1));
	} else {
	    bp += Sprintf(bp, "%s: %s %u in record number %lu\n", cmdname,
			compare_error_str, byte_position, (dip->di_records_read + 1));
	}
	bp += Sprintf(bp, "%s: Data expected = %#x, data found = %#x, byte count = %lu\n",
				cmdname, expected_data, data_found, byte_count);
	LogDiagMsg(msg_buffer);
    }
#endif /* defined(LOG_DIAG_INFO) */
}

void
ReportDeviceInfo (
	struct dinfo	*dip,
	size_t		byte_count,
	u_int		byte_position,
	bool		eio_error )
{
    /*
     * For disk devices, also report the relative block address.
     */
    if (dip->di_random_access) {
        large_t lba;
	OFF_T current_offset;
	OFF_T starting_offset;
	u_int32 dsize = dip->di_dsize;
	OFF_T block_offset = (byte_position % dsize);

#if defined(AIO)
	if (aio_flag) {
	    starting_offset = current_acb->aio_offset;
	    current_offset = (starting_offset + byte_position);
	} else {
	    current_offset = get_position (dip);
	    starting_offset = (current_offset - byte_count);
	}
#else /* !defined(AIO) */
	current_offset = get_position (dip);
	starting_offset = (current_offset - byte_count);
#endif /* defined(AIO) */

	lba = WhichBlock ((starting_offset + byte_position), dsize);

        dip->di_lba = lba;
        /*
         * Only save the offset for AIO, since our normal read/write
         * functions maintain the file offset themselves.  If we do
         * this here, our next offset will be incorrect breaking lba
         * values when using IOT pattern (for example).
         */
        if (aio_flag) {
            dip->di_offset = current_offset;
        }
        dip->di_position = block_offset;

	LogMsg (efp, logLevelError, PRT_NOFLUSH,
		"Relative block number where the error occurred is " LUF ","
                " position " FUF, lba, (starting_offset + byte_position));
	if (block_offset) {
	    LogMsg (efp, logLevelError, (PRT_NOIDENT|PRT_NOLEVEL),
		    " (offset %lu)\n", (u_int)block_offset);
	} else {
	    LogMsg (efp, logLevelError, (PRT_NOIDENT|PRT_NOLEVEL), "\n");
	}
#if defined(LOG_DIAG_INFO)
	if (logdiag_flag) {
	    char *bp = msg_buffer;
	    bp += Sprintf(bp,
		  "%s: Relative block number where the error occurred is " LUF ",",
		  " position " FUF, cmdname, lba, (starting_offset + byte_position));
	    if (block_offset) {
		bp += Sprintf(bp, " (offset %lu)\n", (u_int)block_offset);
  	    } else {
                bp += Sprintf(bp, "\n");
            }
	    LogDiagMsg(msg_buffer);
	}
#endif /* defined(LOG_DIAG_INFO) */

	/*
	 * Seek past the erroring block, so we can continue our I/O.
	 */
	if (eio_error) {
	    current_offset += dsize;
	    if (dip->di_mode == READ_MODE) {
		dip->di_fbytes_read += dsize;
	    } else {
		dip->di_fbytes_written += dsize;
	    }
	    (void) set_position (dip, current_offset);

	    /*
	     * When copying data, properly position the output device too.
	     */
	    if ( (io_mode != TEST_MODE)		&&
		 (dip != output_dinfo)		&&
		 output_dinfo->di_random_access ) {
		/*
		 * NOTE: Output device could be at a different offset.
		 */
		struct dinfo *odip = output_dinfo;
		OFF_T output_offset = get_position(odip);
		output_offset += dsize;
		if (dip->di_mode == READ_MODE) {
		    dip->di_fbytes_read += dsize;
		} else {
		    dip->di_fbytes_written += dsize;
		}
		(void) set_position (odip, output_offset);
	    }
	}
#if defined(EEI)
    } else if ( (dip->di_dtype->dt_dtype == DT_TAPE) && eei_flag) {
	print_mtstatus (dip->di_fd, dip->di_mt, TRUE);
#endif /* defined(EEI) */
    }
}

/************************************************************************
 *									*
 * ReportLbDataError() - Report Logical Block Data Compare Error.	*
 *									*
 * Inputs:	dip = The device info structure.			*
 *		lba = The starting logical block address.		*
 *		byte_count = The byte count of the last request.	*
 *		byte_position = Data buffer index where compare failed.	*
 *		expected_data = The expected data.			*
 *		data_found = The incorrect data found.			*
 *									*
 * Return Value: Void.							*
 *									*
 ************************************************************************/
void
ReportLbdataError (
	struct dinfo	*dip,
        u_int32		lba,
	u_int32		byte_count,
	u_int32		byte_position,
	u_int32		expected_data,
	u_int32		data_found )
{
    (void) RecordError();
    if (dip->di_dtype->dt_dtype == DT_TAPE) {
	Fprintf ("File #%lu, %s %u in record number %u\n", (dip->di_files_read + 1),
		compare_error_str, byte_position, (dip->di_records_read + 1));
    } else {
	Fprintf ("%s %u in record number %u\n",
		compare_error_str, byte_position, (dip->di_records_read + 1));
    }

    ReportDeviceInfo (dip, byte_count, byte_position, FALSE);

    Fprintf ("Block expected = %u (0x%08x), block found = %u (0x%08x), count = %u\n",
		expected_data, expected_data, data_found, data_found, byte_count);

#if defined(LOG_DIAG_INFO)
    if (logdiag_flag) {
	char *bp = msg_buffer;
	if (dip->di_dtype->dt_dtype == DT_TAPE) {
	    bp += Sprintf(bp, "%s: File #%lu, %s %u in record number %lu\n", cmdname,
			(dip->di_files_read + 1), compare_error_str, byte_position,
			(dip->di_records_read + 1));
	} else {
	    bp += Sprintf(bp, "%s: %s %u in record number %lu\n", cmdname,
			compare_error_str, byte_position, (dip->di_records_read + 1));
	}
	bp += Sprintf(bp, "%s: Block expected = %u (0x%08x), block found = %u (0x%08x), count = %u\n",
		cmdname, expected_data, expected_data, data_found, data_found, byte_count);
	LogDiagMsg(msg_buffer);
    }
#endif /* defined(LOG_DIAG_INFO) */
}

/*
 * Check for all hex characters in string.
 */
int
IS_HexString (char *s)
{
	if ( (*s == '0') &&
	     ((*(s+1) == 'x') || (*(s+1) == 'X')) ) {
	    s += 2;	/* Skip over "0x" or "0X" */
	}
	while (*s) {
	    if ( !isxdigit((int)*s++) ) return (FALSE);
	}
	return (TRUE);
}

/*
 * FmtKeepAlive() - Format Keepalive Message.
 *
 * Special Format Characters:
 *
 *      %b = The bytes read or written.
 *      %B = The total bytes read and written.
 *      %c = The count of records this pass.
 *      %C = The total records for this test.
 *	%d = The device name.
 *	%D = The real device name.
 *      %e = The number of errors.
 *      %E = The error limit.
 *      %f = The files read or written.
 *      %F = The total files read and written.
 *	%h = The host name.
 *	%H = The full host name.
 *      %i = The I/O mode ("read" or "write").
 *      %k = The kilobytes this pass.
 *      %K = The total kilobytes this test.
 *      %l = The logical blocks read or written.
 *      %L = The total blocks read and written.
 *      %m = The megabytes this pass.
 *      %M = The total megabytes this test.
 *	%p = The pass count.
 *	%P = The pass limit.
 *      %r = The records read this pass.
 *      %R = The total records read this test.
 *      %s = The seconds this pass.
 *      %S = The total seconds this test.
 *      %t = The pass elapsed time.
 *      %T = The total elapsed time.
 *	%u = The user (login) name.
 *      %w = The records written this pass.
 *      %W = The total records written this test.
 *
 * Performance Keywords:
 *      %bps  = The bytes per second.
 *      %lbps = The blocks per second.
 *      %kbps = The kilobytes per second.
 *      %mbps = The megabytes per second.
 *      %iops = The I/O's per second.
 *      %spio = The seconds per I/O.
 *
 * Lowercase means per pass stats, while uppercase means total stats.
 *
 * Inputs:
 *	dip = The device information pointer.
 *	keepalivefmt = Keepalive formal control string.
 *	buffer = Buffer for formatted message.
 *
 * Outputs:
 *	Returns SUCCESS or FAILURE.
 */
int
FmtKeepAlive (struct dinfo *dip, char *keepalivefmt, char *buffer)
{
  char    *from = keepalivefmt;
  char    *to = buffer;
  ssize_t length = strlen(keepalivefmt);

  while (length-- > 0) {
    bool full_info = FALSE;
    /*
     * Running out of single characters, use key words for performance.
     */
    if (*from == '%') {
      char *key = (from + 1);
      /*
       * The approach taken is: lower = pass, upper = total
       */
      if (strncasecmp(key, "bps", 3) == 0) {
        int secs;
        large_t bytes;
        bool pass_stats = (strncmp(key, "bps", 3) == 0);
        bytes = GetStatsValue(dip, ST_BYTES, pass_stats, &secs);
        if (secs) {
          to += Sprintf(to, "%.3f", ((double)bytes / (double)secs));
        } else {
          to += Sprintf(to, "0.000");
        }
        length -= 3;
        from += 4;
        continue;
      } else if (strncasecmp(key, "lbps", 4) == 0) {
        int secs;
        large_t blocks;
        bool pass_stats = (strncmp(key, "lbps", 4) == 0);
        blocks = GetStatsValue(dip, ST_BLOCKS, pass_stats, &secs);
        if (secs && dip->di_dsize) {
          to += Sprintf(to, "%.3f", ((double)blocks / (double)secs));
        } else {
          to += Sprintf(to, "0.000");
        }
        length -= 4;
        from += 5;
        continue;
      } else if (strncasecmp(key, "kbps", 4) == 0) {
        int secs;
        large_t bytes;
        bool pass_stats = (strncmp(key, "kbps", 4) == 0);
        bytes = GetStatsValue(dip, ST_BYTES, pass_stats, &secs);
        if (secs) {
          to += Sprintf(to, "%.3f", ((double)bytes / (double)KBYTE_SIZE) / secs);
        } else {
          to += Sprintf(to, "0.000");
        }
        length -= 4;
        from += 5;
        continue;
      } else if (strncasecmp(key, "mbps", 4) == 0) {
        int secs;
        large_t bytes;
        bool pass_stats = (strncmp(key, "mbps", 4) == 0);
        bytes = GetStatsValue(dip, ST_BYTES, pass_stats, &secs);
        if (secs) {
          to += Sprintf(to, "%.3f", ((double)bytes / (double)MBYTE_SIZE) / secs);
        } else {
          to += Sprintf(to, "0.000");
        }
        length -= 4;
        from += 5;
        continue;
      } else if (strncasecmp(key, "iops", 4) == 0) {
        int secs;
        u_long records;
        bool pass_stats = (strncmp(key, "iops", 4) == 0);
        records = GetStatsValue(dip, ST_RECORDS, pass_stats, &secs);
        if (secs) {
          to += Sprintf(to, "%.3f", ((double)records / (double)secs));
        } else {
          to += Sprintf(to, "0.000");
        }
        length -= 4;
        from += 5;
        continue;
      } else if (strncasecmp(key, "spio", 4) == 0) {
        int secs;
        u_long records;
        bool pass_stats = (strncmp(key, "spio", 4) == 0);
        records = GetStatsValue(dip, ST_RECORDS, pass_stats, &secs);
        if (records) {
          to += Sprintf(to, "%.4f", ((double)secs / (double)records));
        } else {
          to += Sprintf(to, "0.0000");
        }
        length -= 4;
        from += 5;
        continue;
      }
    }
    switch (*from) {
      case '%': {
        if (length) {
          switch (*(from + 1)) {
            case 'b': {
              if (raw_flag) {
                to += Sprintf(to, LUF,
                              (dip->di_dbytes_read + dip->di_dbytes_written));
              } else if (dip->di_mode == READ_MODE) {
                to += Sprintf(to, LUF, dip->di_dbytes_read);
              } else {
                to += Sprintf(to, LUF, dip->di_dbytes_written);
              }
              break;
            }
            case 'B': {
              to += Sprintf(to, LUF,
                    (total_bytes + dip->di_dbytes_read + dip->di_dbytes_written));
              break;
            }
            case 'c': {
              if (raw_flag) {
                to += Sprintf(to, "%lu",
                              (dip->di_records_read + dip->di_records_written));
              } else if (dip->di_mode == READ_MODE) {
                to += Sprintf(to, "%lu", dip->di_records_read);
              } else {
                to += Sprintf(to, "%lu", dip->di_records_written);
              }
              break;
            }
            case 'C': {
              to += Sprintf(to, LUF,
                    (total_records + total_partial +
                     dip->di_records_read + dip->di_records_written));
              break;
            }
            case 'd': {
              to += Sprintf(to, "%s", dip->di_dname);
              break;
            }
            case 'D': {
              if ( dip->di_device ) { /* Only if known. */
                to += Sprintf(to, "%s", dip->di_device);
              } else {
                struct dtype *dtp = dip->di_dtype;
                to += Sprintf(to, "%s", dtp->dt_type);
              }
              break;
            }
            case 'e': {
              to += Sprintf(to, "%lu",
                            (dip->di_read_errors + dip->di_write_errors));
              break;
            }
            case 'E': {
              to += Sprintf(to, "%lu", error_limit);
              break;
            }
            case 'f': {
              if (raw_flag) {
                to += Sprintf(to, "%lu",
                              (dip->di_files_read + dip->di_files_written));
              } else if (dip->di_mode == READ_MODE) {
                to += Sprintf(to, "%lu", dip->di_files_read);
              } else {
                to += Sprintf(to, "%lu", dip->di_files_written);
              }
              break;
            }
            case 'F': {
              to += Sprintf(to, LUF,
                    (total_files + dip->di_files_read + dip->di_files_written));
              break;
            }
            case 'H':
              full_info = TRUE;
              /* FALL THROUGH */
            case 'h': {
              char *p, hostname[MAXHOSTNAMELEN];
              if ( gethostname(hostname, sizeof(hostname)) ) {
                perror("gethostname()");
                return(FAILURE);
              }
              if ( !full_info ) {
                if (p = strchr(hostname, '.')) {
                  *p = '\0';
                }
              }
              to += Sprintf(to, "%s", hostname);
              break;
            }
            case 'i': {
              if (raw_flag) {
                to += Sprintf(to, "raw");
              } else if (dip->di_mode == READ_MODE) {
                to += Sprintf(to, "read");
              } else {
                to += Sprintf(to, "write");
              }
              break;
            }
            case 'k': {
              if (raw_flag) {
                to += Sprintf(to, "%.3f",
                              (((double)dip->di_dbytes_read + (double)dip->di_dbytes_written)
                              / (double)KBYTE_SIZE));
              } else if (dip->di_mode == READ_MODE) {
                to += Sprintf(to, "%.3f",
                              ((double)dip->di_dbytes_read / (double)KBYTE_SIZE));
              } else {
                to += Sprintf(to, "%.3f",
                              ((double)dip->di_dbytes_written / (double)KBYTE_SIZE));
              }
              break;
            }
            case 'K': {
              to += Sprintf(to, "%.3f",
                            (((double)total_bytes +
                              (double)dip->di_dbytes_read + (double)dip->di_dbytes_written)
                            / (double)MBYTE_SIZE));
              break;
            }
            case 'l': {
              if (dip->di_dsize <= 1) { /* Device without a size, tape, etc. */
                  to += Sprintf(to, "<n/a>");
              } else if (raw_flag) {
                to += Sprintf(to, LUF,
                              ((dip->di_dbytes_read + dip->di_dbytes_written)
                              / dip->di_dsize));
              } else if (dip->di_mode == READ_MODE) {
                to += Sprintf(to, LUF, (dip->di_dbytes_read / dip->di_dsize));
              } else {
                to += Sprintf(to, LUF, (dip->di_dbytes_written / dip->di_dsize));
              }
              break;
            }
            case 'L': {
              if (dip->di_dsize <= 1) { /* Device without a size, tape, etc. */
                  to += Sprintf(to, "<n/a>");
              } else {
                  to += Sprintf(to, LUF,
              ((total_bytes + dip->di_dbytes_read + dip->di_dbytes_written) / dip->di_dsize));
              }
              break;
            }
            case 'm': {
              if (raw_flag) {
                to += Sprintf(to, "%.3f",
                              (((double)dip->di_dbytes_read + (double)dip->di_dbytes_written)
                              / (double)MBYTE_SIZE));
              } else if (dip->di_mode == READ_MODE) {
                to += Sprintf(to, "%.3f",
                              ((double)dip->di_dbytes_read / (double)MBYTE_SIZE));
              } else {
                to += Sprintf(to, "%.3f",
                              ((double)dip->di_dbytes_written / (double)MBYTE_SIZE));
              }
              break;
            }
            case 'M': {
              to += Sprintf(to, "%.3f",
                            (((double)total_bytes +
                              (double)dip->di_dbytes_read + (double)dip->di_dbytes_written)
                            / (double)MBYTE_SIZE));
              break;
            }
            case 'p': {
              to += Sprintf(to, "%lu", pass_count);
              break;
            }
            case 'P': {
              to += Sprintf(to, "%lu", pass_limit);
              break;
            }
            case 'r': {
              to += Sprintf(to, "%lu", dip->di_records_read);
              break;
            }
            case 'R': {
              to += Sprintf(to, LUF,
                    (total_records_read + total_partial_reads + dip->di_records_read));
              break;
            }
            case 's': {
              int secs;
              end_time = times (&etimes);
              secs = ((end_time - pass_time) / hz);
              to += Sprintf(to, "%d", secs);
              break;
            }
            case 'S': {
              int secs;
              end_time = times (&etimes);
              secs = ((end_time - start_time) / hz);
              to += Sprintf(to, "%d", secs);
              break;
            }
            case 't': {
              clock_t at;
              end_time = times (&etimes);
              at = end_time - pass_time;
              to = bformat_time(to, at);
              break;
            }
            case 'T': {
              clock_t at;
              end_time = times (&etimes);
              at = end_time - start_time;
              to = bformat_time(to, at);
              break;
            }
            case 'u': {
#if defined(WIN32)
	      DWORD size = 256;
	      TCHAR buf[256];
#endif
              char *username = NULL;
             
#if defined(WIN32)
	      GetUserName(buf, &size);
	      username = buf;
#else /* if !defined(WIN32) */
              username = getlogin();
#endif 
	      if (username) {
                to += Sprintf(to, "%s", username);
              } else {
#if defined(WIN32)
		Fprintf("GetUserName failed !\n");
#else /*if !defined(WIN32) */
                perror("getlogin()");
#endif
              }
              break;
            }
            case 'w': {
              to += Sprintf(to, "%lu", dip->di_records_written);
              break;
            }
            case 'W': {
              to += Sprintf(to, LUF,
                    (total_records_written + total_partial_writes + dip->di_records_written));
              break;
            }
            default: {
              *to++ = *from;
              *to++ = *(from + 1);
              break;
            }
          } /* end switch */
          length--;
          from += 2;
          break;
        } else { /* !length */
          *to++ = *from++;
        } /* end if length */
        break;
      } /* end case '%' */
      case '\\': {
        if (length) {
          switch (*(from + 1)) {
            case 'n': {
              to += Sprintf(to, "\n");
              break;
            }
            case 't': {
              to += Sprintf(to, "\t");
              break;
            }
            default: {
              *to++ = *from;
              *to++ = *(from + 1);
              break;
            }
          } /* end switch */
          length--;
          from += 2;
          break;
        } else { /* !length */
          *to++ = *from++;
        } /* end if length */
        break;
      } /* end case '\' */
      default: {
        *to++ = *from++;
        break;
      }
    }
  }
  *to = '\0';       /* NULL terminate! */
  return(SUCCESS);
}

/*
 * GetStatsValue() - Simple function to obtain stats values.
 *
 * Inputs:
 *    dip = The device information pointer.
 *    stv = The stats value to obtain.
 *    pass_stats = Boolean true if pass stats.
 *    secs = Optional pointer to store seconds.
 *
 * Return Value:
 *    Returns the stats value (64 bits).
 */
large_t
GetStatsValue(struct dinfo *dip, stats_value_t stv, bool pass_stats, int *secs)
{
   large_t value;

   switch (stv) {
     case ST_BYTES: {
       if (pass_stats) {
          if (raw_flag) {
            value = (dip->di_dbytes_read + dip->di_dbytes_written);
          } else if (dip->di_mode == READ_MODE) {
            value = dip->di_dbytes_read;
          } else {
            value = dip->di_dbytes_written;
          }
        } else {
          value = (total_bytes + dip->di_dbytes_read + dip->di_dbytes_written);
        }
        break;
     }
     case ST_BLOCKS: {
       value = GetStatsValue(dip, ST_BYTES, pass_stats, secs);
       if (dip->di_dsize) {
         value /= dip->di_dsize; /* Convert to logical blocks. */
       }
       break;
     }
     case ST_FILES: {
       if (pass_stats) {
         if (raw_flag) {
           value = (dip->di_files_read + dip->di_files_written);
         } else if (dip->di_mode == READ_MODE) {
           value = dip->di_files_read;
         } else {
           value = dip->di_files_written;
         }
       } else {
         value = (total_files + dip->di_files_read + dip->di_files_written);
       }
       break;
     }
     case ST_RECORDS: {
       if (pass_stats) {
         if (raw_flag) {
           value = (dip->di_records_read + dip->di_records_written);
         } else if (dip->di_mode == READ_MODE) {
           value = dip->di_records_read;
         } else {
           value = dip->di_records_written;
         }
       } else {
         value = (total_records + total_partial +
                  dip->di_records_read + dip->di_records_written);
       }
       break;
     }
     default:
       Fprintf("Invalid stats value request, %d\n", stv);
       abort();
   }
   if (secs) {
     end_time = times(&etimes);
     if (pass_stats) {
       *secs = ((end_time - pass_time) / hz);
     } else {
       *secs = ((end_time - start_time) / hz); 
     }
   }
   return (value);
}

/*
 * FmtPrefix() - Format the Prefix String.
 *
 * Special Format Characters:
 *
 *	%d = The device name.
 *	%D = The real device name.
 *	%h = The host name.
 *	%H = The full host name.
 *	%p = The process ID.
 *	%P = The parent process ID.
 *	%u = The user (login) name.
 *
 * Inputs:
 *	dip = The device information pointer.
 *	prefix = Pointer to prefix address.
 *	psize = Pointer to prefix length;
 *
 * Outputs:
 *	Returns SUCCESS or FAILURE.
 *	Prefix string and size are updated.
 */
int
FmtPrefix (struct dinfo *dip, char **prefix, int *psize)
{
    char	*from = *prefix;
    char	*buffer, *to;
    int		length = *psize;
#if defined(WIN32)
    int		len = MAXHOSTNAMELEN;
    DWORD size = 256;
    TCHAR buf[256];
    char *username;
#endif /* defined(WIN32) */
    if (strstr (from, "%") == NULL) {
        /* See comments below. */
        if (io_type != RANDOM_IO) {
	    return (SUCCESS);
        }
    }
    buffer = to = Malloc(KBYTE_SIZE);
    while (length--) {
	bool full_info = FALSE;
	switch (*from) {
	    case '%':
		if (length) {
		    switch (*(from + 1)) {
			case 'd': {
			    to += Sprintf(to, "%s", dip->di_dname);
			    break;
			}
			case 'D': {
			    if ( dip->di_device ) { /* Only if known. */
				to += Sprintf(to, "%s", dip->di_device);
			    } else {
				struct dtype *dtp = dip->di_dtype;
				to += Sprintf(to, "%s", dtp->dt_type);
			    }
			    break;
			}
			case 'H':
			    full_info = TRUE;
			    /* FALL THROUGH */
			case 'h': {
			    char *p, hostname[MAXHOSTNAMELEN];
#if defined(WIN32)
			    if (( GetComputerName(hostname, &len)) == 0) {
			        Fprintf("GetComputerName failed!\n");
#else /* !defined(WIN32) */
			    if ( gethostname(hostname, sizeof(hostname)) ) {
				perror("gethostname()");
#endif /* defined(WIN32) */
				return (FAILURE);
			    }
			    if ( !full_info ) {
				if (p = strchr(hostname, '.')) {
				    *p = '\0';
				}
			    }
			    to += Sprintf(to, "%s", hostname);
			    break;
			}
			case 'p': {
			    pid_t pid = getpid();
			    to += Sprintf(to, "%d", pid);
			    break;
			}
			case 'P': {
#if defined(WIN32)
			    LogMsg (efp,logLevelError,0,
				   "option P currently not supported\n");
			    exit(FAILURE);
#else /* !defined(WIN32) */
			    pid_t ppid = getppid();
			    to += Sprintf(to, "%d", ppid);
#endif /* defined(WIN32) */
			    break;
			}
			case 'u': {
#if defined(WIN32)
			    GetUserName(buf, &size);
			    username = buf;
#else /* !defined(WIN32) */
			    char *username = getlogin();
#endif /* defined(WIN32) */
			    if (username) {
				to += Sprintf(to, "%s", username);
			    } else {
#if defined(WIN32)
				Fprintf("GetUserName failed !\n");
#else /* !defined(WIN32) */
				perror("getlogin()");
#endif /* defined(WIN32) */
			    }
			    break;
			}
			default: {
			    *to++ = *from;
			    *to++ = *(from + 1);
			    break;
			}
		    }
		    length--;
		    from += 2;
		    break;
		}
		/* FALLTHROUGH */
	    default: {
		*to++ = *from++;
		break;
	    }
	}
    }
    free (*prefix);
    *psize = strlen(buffer)+1; /* plus NULL! */
    /*
     * To avoid problems with random I/O, make the prefix string
     * modulo the lba (iot or lbdata) or our 4 byte data pattern.
     * Otherwise, random I/O fails with a partial pattern.
     */
    if (io_type == RANDOM_IO) {
        *psize = roundup(*psize, sizeof(u_int32));
    }
    *prefix = Malloc(*psize);
    (void)strcpy(*prefix, buffer);
    free (buffer);
    return (SUCCESS);
}

/*
 * String copy with special mapping.
 */
int
StrCopy (void *to_buffer, void *from_buffer, size_t length)
{
	u_char *to = (u_char *) to_buffer;
	u_char *from = (u_char *) from_buffer;
	u_char c, key;
	int count = 0;

	while ( length ) {
	    c = *from++; length--;
	    if ( (c != '^') && (c != '\\') ) {
		*to++ = c; count++;
		continue;
	    }
	    if (length == 0) {
		*to++ = c; count++;
		continue;
	    }
	    if (c == '^') {			/* control/X */
		c = *from++; length--;
		*to++ = (c & 037); count++;
		continue;
	    }
	    c = *from++; length--;
	    if      (c == 'a')	key = '\007';	/* alert (bell) */
	    else if (c == 'b')	key = '\b';	/* backspace */
	    else if ( (c == 'e') || (c == 'E') )
				key = '\033';	/* escape */
	    else if (c == 'f')	key = '\f';	/* formfeed */
	    else if (c == 'n')	key = '\n';	/* newline */
	    else if (c == 'r')	key = '\r';	/* return */
	    else if (c == 't')	key = '\t';	/* tab */
	    else if (c == 'v')	key = '\v';	/* vertical tab */
	    else if ( (length >= 2) &&
		      ((c == 'x') || (c == 'X')) ) { /* hex */
		int cnt;
		u_char val = 0;
		for (cnt = 0, key = 0; cnt < 2; cnt++) {
		    c = *from++; count--;
		    if ( isxdigit(c) ) {
			if ( ('c' >= '0') && (c <= '9') )
			    val = (c - '0');
			else if ( ('c' >= 'a') && (c <= 'f') )
			    val = 10 + (c - 'a');
			else if ( ('c' >= 'A') && (c <= 'F') )
			    val = 10 + (c - 'A');
		    } else {
			key = c;
			break;
		    }
		    key = (key << 4) | val;
		}
	    } else if ( (length >= 3) &&
			((c >= '0') && (c <= '7')) ) { /* octal */
		int cnt;
		for (cnt = 0, key = 0; cnt < 3; cnt++) {
		    key = (key << 3) | (c - '0');
		    c = *from++; length--;
		    if (c < '0' || c > '7') break;
		}
	    } else {
		key = c;	/* Nothing special here... */
	    }
	    *to++ = key; count++;
	}
	return (count);
}

/************************************************************************
 *									*
 * stoh() - Convert SCSI bytes to Host short/int/long format.		*
 *									*
 * Description:								*
 *	This function converts SCSI (big-endian) byte ordering to the	*
 * format necessary by this host.					*
 *									*
 * Inputs:	bp = Pointer to SCSI data bytes.			*
 *		size = The conversion size in bytes.			*
 *									*
 * Return Value:							*
 *		Returns a long value with the proper byte ordering.	*
 *									*
 ************************************************************************/
large_t
stoh (u_char *bp, size_t size)
{
	large_t value = 0L;

	switch (size) {

	    case sizeof(u_char):
		value = (u_long) bp[0];
		break;

	    case sizeof(u_short):
		value = ( ((u_long)bp[0] << 8) | (u_long)bp[1] );
		break;

	    case 0x03:
		value = ( ((u_long)bp[0] << 16) | ((u_long)bp[1] << 8) |
			  (u_long) bp[2] );
		break;

	    case sizeof(u_int):
		value = ( ((u_long)bp[0] << 24) | ((u_long)bp[1] << 16) |
			  ((u_long)bp[2] << 8) | (u_long)bp[3]);
		break;

	    case 0x05:
		value = ( ((large_t)bp[0] << 32L) | ((large_t)bp[1] << 24) |
			  ((large_t)bp[2] << 16) | ((large_t)bp[3] << 8) |
			  (large_t)bp[4] );
		break;

	    case 0x06:
		value = ( ((large_t)bp[0] << 40L) | ((large_t)bp[1] << 32L) |
			  ((large_t)bp[2] << 24) | ((large_t)bp[3] << 16) |
			  ((large_t)bp[4] << 8) | (large_t)bp[5] );
		break;

	    case 0x07:
		value = ( ((large_t)bp[0] << 48L) | ((large_t)bp[1] << 40L) |
			  ((large_t)bp[2] << 32L) | ((large_t)bp[3] << 24) |
			  ((large_t)bp[4] << 16) | ((large_t)bp[5] << 8) |
			  (large_t)bp[6] );
		break;

	    case sizeof(large_t):
		value = ( ((large_t)bp[0] << 56L) | ((large_t)bp[1] << 48L) |
			  ((large_t)bp[2] << 40L) | ((large_t)bp[3] << 32L) |
			  ((large_t)bp[4] << 24) | ((large_t)bp[5] << 16) |
			  ((large_t)bp[6] << 8) | (large_t)bp[7] );
		break;

	    default:
		Fprintf (bad_conversion_str, size);
		break;

	}
	return (value);
}

/************************************************************************
 *									*
 * htos() - Convert Host short/int/long format to SCSI bytes.		*
 *									*
 * Description:								*
 *	This function converts host values to SCSI (big-endian) byte	*
 * ordering.								*
 *									*
 * Inputs:	bp = Pointer to SCSI data bytes.			*
 *		value = The numeric value to convert.			*
 *		size = The conversion size in bytes.			*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
htos (u_char *bp, large_t value, size_t size)
{
	switch (size) {

	    case sizeof(u_char):
		bp[0] = (u_char) value;
		break;

	    case sizeof(u_short):
		bp[0] = (u_char) (value >> 8);
		bp[1] = (u_char) value;
		break;

	    case 0x03:
		bp[0] = (u_char) (value >> 16);
		bp[1] = (u_char) (value >> 8);
		bp[2] = (u_char) value;
		break;

	    case sizeof(u_int):
		bp[0] = (u_char) (value >> 24);
		bp[1] = (u_char) (value >> 16);
		bp[2] = (u_char) (value >> 8);
		bp[3] = (u_char) value;
		break;

	    case 0x05:
		bp[0] = (u_char) (value >> 32L);
		bp[1] = (u_char) (value >> 24);
		bp[2] = (u_char) (value >> 16);
		bp[3] = (u_char) (value >> 8);
		bp[4] = (u_char) value;
		break;

	    case 0x06:
		bp[0] = (u_char) (value >> 40L);
		bp[1] = (u_char) (value >> 32L);
		bp[2] = (u_char) (value >> 24);
		bp[3] = (u_char) (value >> 16);
		bp[4] = (u_char) (value >> 8);
		bp[5] = (u_char) value;
		break;

	    case 0x07:
		bp[0] = (u_char) (value >> 48L);
		bp[1] = (u_char) (value >> 40L);
		bp[2] = (u_char) (value >> 32L);
		bp[3] = (u_char) (value >> 24);
		bp[4] = (u_char) (value >> 16);
		bp[5] = (u_char) (value >> 8);
		bp[6] = (u_char) value;
		break;

	    case sizeof(large_t):
		bp[0] = (u_char) (value >> 56L);
		bp[1] = (u_char) (value >> 48L);
		bp[2] = (u_char) (value >> 40L);
		bp[3] = (u_char) (value >> 32L);
		bp[4] = (u_char) (value >> 24);
		bp[5] = (u_char) (value >> 16);
		bp[6] = (u_char) (value >> 8);
		bp[7] = (u_char) value;
		break;

	    default:
		Fprintf (bad_conversion_str, size);
		break;

	}
}

#if defined(DEC)

#include <dec/binlog/binlog.h>

extern int binlogmsg(int msg_class, char *msg_buffer);

/*
 * LogDiagMsg() - Log a Generic Diagnostic Message.
 *
 * Inputs:
 *	msg - Message to log (not to exceed 1024 bytes).
 *
 * Notes:
 *	The binlogmsg() function writes to /dev/kbinlog, so this API
 * normally requires running as root, or setuid root.  Changing the
 * /dev/kbinlog permissions is NFG, we still get an EIO error :-)
 */
void
LogDiagMsg(char *msg)
{
    int error;
    /*
     * TODO?: Later, we may wish opt for the event logger (EVM).
     */
    if (logdiag_flag) {
	error = binlogmsg (ELMSGT_DIAG, msg);
	if (error && (debug_flag || Debug_flag)) {
	    perror("binlogmsg failed");
	}
    }
    return;
}

#else /* !defined(DEC) */

void
LogDiagMsg(char *msg)
{
    return;
}

#endif /* defined(DEC) */

enum trigger_type
check_trigger_type (char *str)
{
    if (strcmp(str, "br") == 0) {
        return (TRIGGER_BR);
    } else if (strcmp(str, "bdr") == 0) {
        return (TRIGGER_BDR);
    } else if (strcmp(str, "seek") == 0) {
        return (TRIGGER_SEEK);
    } else if (strncmp(str, "cmd:", 4) == 0) {
        trigger_cmd = &str[4];
        return (TRIGGER_CMD);
    } else {
        LogMsg (efp, logLevelCrit, 0,
                "Valid trigger types are: br, bdr, seek, or cmd:string\n");
        return (TRIGGER_INVALID);
    }
}

int
ExecuteTrigger (struct dinfo *dip, ...)
{
    char cmd[STRING_BUFFER_SIZE];
    enum trigger_type trigger = dip->di_trigger;
    int status = TRIGACT_CONTINUE;

    if (( (trigger == TRIGGER_BR) ||
          (trigger == TRIGGER_BDR) ||
          (trigger == TRIGGER_SEEK) ) &&
        (dip->di_dtype->dt_dtype != DT_DISK) ) {
        LogMsg (efp, logLevelWarn, 0,
                "Trigger requires a raw disk to execute Scu!\n");
        return (status);
    }
    switch (trigger) {
        case TRIGGER_NONE:
            return (status);
            /*NOTREACHED*/
            break;

        case TRIGGER_BR:
            (void)sprintf(cmd, "scu -f %s br", dip->di_dname);
            break;

        case TRIGGER_BDR:
            (void)sprintf(cmd, "scu -f %s bdr", dip->di_dname);
            break;

        case TRIGGER_SEEK: {
            (void)sprintf(cmd, "scu -f %s seek lba " LUF,
                          dip->di_dname, dip->di_lba);
            break;
        }
        case TRIGGER_CMD: {
            va_list ap;
            char *op;
            va_start(ap, dip);
            op = va_arg(ap, char *);
            va_end(ap);
            /*
             * Format: cmd dname op dsize offset position lba errno
             */
            (void)sprintf(cmd, "%s %s %s %u " FUF " %u " LUF " %d",
                          trigger_cmd,
                          dip->di_dname, op, dip->di_dsize,
                          dip->di_offset, dip->di_position,
                          dip->di_lba, dip->di_errno);
            break;
        }
        default:
            LogMsg (efp, logLevelCrit, 0,
                    "Invalid trigger type detected, type = %d\n", trigger);
            terminate(FATAL_ERROR);
            /*NOTREACHED*/
            break;
    }
    Printf("Executing: %s\n", cmd);
    status = system(cmd);
    status = WEXITSTATUS(status);
    if (status) Printf("Trigger exited with status %d!\n", status);
    return (status);
}

#if defined(WIN32)

LPVOID
error_msg(void)
{
    LPVOID msgbuf;
   if( FormatMessage (
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
			(LPSTR) &msgbuf,
			0,NULL) == 0) {
        report_error("Format message",TRUE);
	return 0;
    }
    else
	return(msgbuf);
}

OFF_T
SetFilePtr (HANDLE hf, OFF_T distance, DWORD MoveMethod)
{
  LARGE_INTEGER seek;
  seek.QuadPart = distance;
  seek.LowPart = SetFilePointer (hf, seek.LowPart, &seek.HighPart, MoveMethod);
  if(seek.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    seek.QuadPart = -1;
  return (OFF_T)seek.QuadPart;
}

#endif /* defined(WIN32) */
