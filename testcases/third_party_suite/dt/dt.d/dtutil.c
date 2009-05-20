/****************************************************************************
 *             *
 *     COPYRIGHT (c) 1990 - 2000       *
 *      This Software Provided       *
 *         By         *
 *     Robin's Nest Software Inc.       *
 *          2 Paradise Lane     *
 *          Hudson, NH 03051        *
 *          (603) 883-2355        *
 *             *
 * Permission to use, copy, modify, distribute and sell this software and   *
 * its documentation for any purpose and without fee is hereby granted,     *
 * provided that the above copyright notice appear in all copies and that   *
 * both that copyright notice and this permission notice appear in the     *
 * supporting documentation, and that the name of the author not be used    *
 * in advertising or publicity pertaining to distribution of the software   *
 * without specific, written prior permission.        *
 *             *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,     *
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN     *
 * NO EVENT SHALL HE BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL   *
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR    *
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS  *
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF   *
 * THIS SOFTWARE.           *
 *             *
 ****************************************************************************/
/*
 * Module: dtutil.c
 * Author: Robin T. Miller
 *
 * Description:
 * Utility routines for generic data test program.
 */
#include "dt.h"
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#if defined(USE_STDARG)
#  include <string.h>
#  include <stdarg.h>
#else /* !defined(USE_STDARG) */
#  include <varargs.h>
#  if defined(sun)
#    include <sys/time.h>
#  else /* !defined(sun) */
#    include <sys/ioctl.h>
#  endif /* !defined(sun) */
#  include <sys/param.h>
#  include <sys/resource.h>
#endif /* defined(USE_STDARG) */
#include <sys/stat.h>

#if defined(sun)
#  define strtoul strtol
#endif /* defined(sun) */

#if defined(ultrix)
extern void *valloc(size_t size);
#endif /* defined(ultrix) */

/*
 * Modification History:
 *
 * February 24th, 2001 by Robin Miller.
 * Add conditionalization for QNX RTP (Neutrino).
 *
 * January 24th, 2001 by Robin Miller.
 * Add support for variable I/O requests sizes.
 * Conditionalize some functions to allow INLINE macros.
 * Modifications to allow IOT test pattern to honor the lba
 * size variable, rather than hardcoding IOT to 512 byte sectors.
 *
 * January 15th, 2001 by Robin Miller.
 * Don't terminate() on failures, return error to caller.
 * Note: seek functions still call terminate()... more work!
 *
 * January 2nd, 2001 by Robin Miller.
 *      Make changes to build using MKS/NuTCracker product.
 *
 * October 4th, 2000 by Robin Miller.
 * Update is_Eof() to accept ENXIO for AIO reads @ EOM on
 * SCO UnixWare systems.  All other systems return ENOSPC or zero.
 *
 * April 25th, 2000 by Robin Miller.
 * Added an expect flag to dump_buffer to help with formatting.
 *
 * April 18th, 2000 by Robin Miller.
 * Modified calls to report_error() to ensure error count bumped.
 *
 * March 28th, 2000 by Robin Miller.
 * Modified file position functions to accept device information
 * parameter, so necessary debug could be added to these funcitons.
 * Also, only scale the random position upwards by the device size
 * every other record, so low blocks gets utilized more often.
 * Note:  These changes degrade random I/O performance slightly.
 * [ All debug needs ripped out to obtain better performance, ]
 *
 * February 17th, 2000 by Robin Miller.
 * Adding better support for multi-volume tape testing.  Mainly,
 * make it work with writing multiple tape files, rather than one file.
 *
 * February 16th, 2000 by Robin Miller.
 * Set exit_status to FAILURE in RecordError() function.  This
 * is necessary to catch any error being logged, since test functions
 * no longer call terminate() with FAILURE status.
 *
 * January 22nd, 2000 by Robin Miller.
 * Add check for Cygwin device names for Windows/NT.
 *
 * January 4th, 2000 by Robin Miller.
 * Major cleanup in verify_data().  Now reports IOT block number.
 * Added verify_buffers() and verify_lbdata() functions for raw.
 *
 * December 31st, 1999 by Robin Miller.
 * Modify do_random() to ensure rlimit is not exceeded when the lower
 * file position is factored in, to stay within desired lba range.
 *
 * December 30th, 1999 by Robin Miller.
 * Modify do_random() to take into consideration the transfer size,
 * to ensure the requested random I/O data limit is not exceeded.
 * Added get_lba() function to return the current logical block address.
 *
 * November 11th, 1999 by Robin Miller.
 * Add LogDiagMsg() to log diagnostic information to event logger.
 *
 * November 10th, 1999 by Robin Miller.
 * Update make_lbdata() & winit_lbdata() so if the IOT test pattern is
 * selected, and the device block size is NOT 512 bytes, we force 1 block.
 * Note: This only affects Tru64 Unix, since we obtain the real sector size.
 *
 * November 9th, 1999 by Robin Miller.
 * Remove use of '%p' in dump_buffer(), since it's use is inconsistent.
 * With Cygnus Solutions, '%p' displays a leading '0x', other OS's don't!
 *
 * August 26th, 1999 by Robin Miller.
 * Report an error for ENOSPC, if no data has been transferred.
 * Previously, this error was only reported for tape devices.
 *
 * August 6th, 1999 by Robin Miller.
 *      Better parameterizing of "long long" printf formatting.
 *
 * July 29, 1999 by Robin Miller.
 * Merge in changes made to compile on FreeBSD.
 *
 * July 22nd, 1999 by Robin Miller.
 * Added support for IOT (DJ's) test pattern.
 *
 * July 7th, 1999 by Robin Miller.
 * Modify CvtStrtoValue() to check for recursive calls which return
 * zero (0) for '/' and '%', which cause a divide by zero fault/core dump.
 *
 * June 28, 1999 by Robin Miller.
 * For 32-bit systems, added CvtStrtoLarge() function to
 * permit double or long long values, since u_long is too small.
 *
 * May 27, 1999 by Robin Miller.
 * Added support for micro-second delays.
 *
 * April 8, 1999 by Robin Miller.
 * Added format_ltime() to format table() sysinfo times.
 *
 * January 7, 1999 by Robin Miller.
 * Removed fflush() in Fprintf() function which caused intermixed
 * output when multiple processes were running (i.e., serial line testing).
 *
 * December 21, 1998 by Robin Miller.
 * - update Malloc() to clear allocated memory.
 * - for DUNIX, updates to handle resets for tapes.
 *
 * December 16, 1998 by Robin Miller.
 * Merge in changes made by George Bittner:
 * - modify do_random(), use random value as a block number
 *   instead of a byte offset, for testing larger disks/files.
 * - ensure the random offset is within starting file position
 *   and the end of the disk/partition.
 *
 * November 19, 1998, by Robin Miller.
 * Fix problem where wrong lbdata lba was returned for offset 0.
 * This required updates to make_lbdata() and winit_lbdata() functions.
 * [ sorry folks, major brain burp!!! ]
 *
 * November 16, 1998 by Robin Miller.
 * For AIO, report the relative block from saved AIO control block.
 *
 * October 29, 1998 by Robin Miller.
 * Implement a random I/O data limit, instead of using the normal
 * data limit variable (not good to dual purpose this value).
 *
 * October 26, 1998 by Robin Miller.
 * Add functions make_lbdata() and winit_lbdata() which handle
 * using both random I/O and lbdata options.  The file offset seeked
 * to is used as the starting lbdata address.
 *
 * April 28, 1998 by Robin Miller.
 * For WIN32/NT, or in O_BINARY into open flags to force binary
 * mode (the default is text mode).
 *
 * May 16, 1997 by Robin Miller.
 * Modified macro used in ReportDeviceInfo() which rounded block
 * up, causing the erroring block to be off by one, when data
 * compare errors were not at the beginning of a block.  Also,
 * report the block offset (byte within block) when non-modulo.
 *
 * May 14, 1997 by Robin Miller.
 * If we encounter a ENOSPC error and no data has been transferred,
 * flag this as an error.  This normally indicates a zero length
 * partition or the user may have seek'ed past eom or eop.
 *
 * March 27, 1997 by Ali Eghlima.
 *      Fix call to report_error() to reflect "unlink" failed.
 *
 * March 7, 1997 by Robin Miller.
 * In ReportDeviceInfo(), when we're doing a copy/verify
 * operation, allow output device to be a different offset.
 *
 * February 28, 1996 by Robin Miller.
 * Modified ReportDeviceInfo() so we seek past disk errors.
 * This action allows testing to continue with "errors=n".
 *
 * February 21, 1996 by Robin Miller.
 * Added do_random() function for random I/O to disks.
 *
 * December 19, 1995 by Robin Miller.
 * Conditionalize for Linux Operating System.
 *
 * Novemeber 18, 1995 by Robin Miller.
 * Removing unaligned data access (ade) test code (code cleanup).
 *
 * November 11, 1995 by Robin Miller.
 * Fix bug with verifying pad bytes.  Basically, the previous logic,
 * while valid for short reads, was incorrect when the pad bytes did
 * *not* start on a modulo sizeof(int) boundary. This caused variable
 * length reads with small increment values to report an (invalid)
 * pad byte data compare error.  NOTE: Only occurred w/pattern file.
 *
 * July 22, 1995 by Robin Miller.
 * Finally, correctly dump data buffers with context (yes, I made
 * sure I tested min/max limits via fault insertion this time).
 *
 * July 15, 1995 by Robin Miller.
 * Add is_Eof() to handle end of media (ENOSPC), and cleanup code.
 *
 * July 7, 1995 by Robin Miller.
 * Correctly dump the pattern buffer on data compare errors.
 * When reporting a compare error, also display the byte count.
 * This latter information is useful for variable length records.
 *
 * July 6, 1995 by Robin Miller.
 * Changed a number of fprintf's to Fprintf so the child PID gets
 * reported during error processing (how did I miss this stuff?).
 *
 * September 23, 1994 by Robin Miller.
 *      Make changes necessary to build on QNX 4.21 release.
 *
 * January 20, 1994 by Robin Miller.
 * When checking the pad bytes, don't do the entire buffer since very
 * large buffers (e.g. 100m) may have been specified using min, max,
 * and incr options which cause excessive paging and poor performance.
 *
 * October 28, 1993 by Robin Miller.
 * For multiple processes, display the PID to differentiate output.
 *
 * September 15, 1993 by Robin Miller.
 * Limit pattern buffer dumping to size of pattern buffer.
 *
 * September 17, 1993 by Robin Miller.
 * Added RecordWarning() function to reporting record number and
 * time stamp on warning errors (useful for debugging purposes).
 * More limiting of data & pattern buffer dumping (less bytes).
 *
 * September 3, 1993 by Robin Miller.
 * Allow "inf" or "INF" keywords to set maximum counts.
 *
 * September 2, 1993 by Robin Miller.
 * Added device specific information.
 *
 * September 1, 1993 by Robin Miller.
 * Report proper record number during errors (add partial records).
 * Limit data dumped when data verify errors occur (short records).
 *
 * August 27, 1993 by Robin MIller.
 * Added support for DEC OSF/1 POSIX Asynchronous I/O (AIO).
 *
 * August 17, 1993 by Robin Miller.
 * Added function RecordError() to record when an error occurs,
 * and added function Ctime() to append date/time string to the
 * log buffer.
 *
 * August 10, 1993 by Robin Miller.
 * Added verify_padbuf() function to check pad buffer bytes after
 * read operations to ensure they weren't overwritten.
 *
 * August 4, 1993 by Robin Miller.
 * Added various printing functions to simplify error reporting.
 *
 * September 11, 1992 by Robin Miller.
 * Added additional debug information when reopening a device.
 *
 * September 5, 1992 by Robin Miller.
 * Initial port to QNX 4.1 Operating System.
 *
 * May 22, 1992 by Robin Miller.
 * Control / force kernel address data exception via flag.
 *
 * March 11, 1992 by Robin Miller.
 * Changes necessary for port to 64-bit Alpha architecture.
 *
 * October 16, 1990 by Robin Miller.
 * Added myalloc() memory allocation function to allocate and
 * align the buffer address using the alignment value.
 *
 * October 9, 1990 by Robin Miller.
 * Use variable hz instead of HZ define for clock ticks per second
 * so user can specify different value via "hz=ticks" option.
 *
 * August 21, 1990 by Robin Miller.
 * Changed exit status so scripts can detect and handle errors
 * based on the exit code.  If not success, fatal error, or end
 * of file/tape, the exit code is the error number (errno).
 *
 * August 21, 1990 by Robin Miller.
 * Added function dump_buffer() to dump buffer in hex bytes.
 *
 * August 8, 1990 by Robin Miller.
 * Added functions seek_file() and skip_records().  Modified seek
 * file function to avoid lseek() system call overhead (see code).
 *
 * Changed malloc() to valloc() to align buffer on page boundry.
 * On some archetectures, this results on better performance to
 * raw devices since the DMA is done directly to the users' buffer.
 *
 * April 11, 1990 by Robin Miller.
 * Added function mmap_file() to memory map a file.  Added munmap()
 * syscall to reopen_file() which gets called for multiple passes.
 * This allows us to time the mmap() as well as the munmap() code.
 *
 * March 21, 1990 by Robin Miller.
 * Added function delete_file() which checks for a regular file
 * and then unlinks it.
 *
 */

/*
 * Forward References:
 */
static size_t CalculateDumpSize(size_t size);
static int dopad_verify (
   struct dinfo *dip,
   u_char  *buffer,
   size_t  offset,
   u_int32  pattern,
   size_t  pbytes,
   size_t  pindex,
   bool  inverted );
int vSprintf(char *bufptr, const char *msg, va_list ap);

#if 0
static char *pad_str =  "Pad";
#endif
static char *data_str =  "Data";
static char *pattern_str = "Pattern";
static char *verify_str = "Verify";

static char *compare_error_str = "Data compare error at byte";
static char *bad_conversion_str =
   "Warning: unexpected conversion size of %d bytes.";

/************************************************************************
 *         *
 * delete_file() - Delete the specified file.    *
 *         *
 *  This function ensures the file specified is a regular *
 *  file before doing an unlink() to delete the file. *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Outputs: Returns SUCCESS/FAILURE = File Delete/Not Deleted. *
 *         *
 ************************************************************************/
int
delete_file (struct dinfo *dip)
{
    int status;

    if (debug_flag) {
 Fprintf("Deleting file '%s'...\n", dip->di_dname);
    }
    if ( (status = unlink(dip->di_dname)) == FAILURE) {
 report_error ("unlink", TRUE);
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
 *         *
 * CalculateDumpSize() - Calculate the number of data bytes to dump. *
 *         *
 * Description:        *
 * For non-memory mapped files, we'll dump the pad bytes.  These *
 * pad bytes do not exist for memory mapped files which are directly *
 * mapped to memory addresses.      *
 *         *
 * Inputs: size = The size of the buffer to dump.   *
 *         *
 * Outputs: Size of buffer to dump.     *
 *         *
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
 *         *
 * dump_buffer() Dump data buffer in hex bytes.    *
 *         *
 * Inputs: name = The buffer name being dumped.   *
 *  base = Base pointer of buffer to dump.   *
 *  ptr  = Pointer into buffer being dumped.  *
 *  dump_size = The size of the buffer to dump.  *
 *  bufr_size = The maximum size of this buffer.  *
 *  expected = Boolean flag (True = Expected).  *
 *         *
 * Return Value:       *
 *  Void.       *
 *         *
 ************************************************************************/
void
dump_buffer ( char  *name,
  u_char  *base,
  u_char  *ptr,
  size_t  dump_size,
  size_t  bufr_size,
  bool  expected )
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
     limit = (bend - bptr); /* Dump to end of buffer. */
 }
 offset = (ptr - base);  /* Offset to failing data. */
 /*
  * NOTE: Rotate parameters are not displayed since we don't have
  * the base data address and can't use global due to AIO design.
  * [ if I get ambitious, I'll correct this in a future release. ]
  */
 Fprintf ("The incorrect data starts at address %#lx (marked by asterisk '*')\n",
         ptr);
 Fprintf ("Dumping %s Buffer (base = %#lx, offset = %u, limit = %u bytes):\n\n",
       name, base, offset, limit);
 for (i = 0; i < limit; i++, bptr++) {
     if ((i % field_width) == (size_t) 0) {
  if (i) fprintf (stderr, "\n");
  fprintf (stderr, "%#lx ", (u_long)bptr);
     }
     fprintf (stderr, "%c%02x", (bptr == ptr) ? '*' : ' ', *bptr);
 }
 if (i) Fprint ("\n");
 if (expected) Fprint ("\n");
}

/************************************************************************
 *         *
 * fill_buffer() - Fill Buffer with a Data Pattern.   *
 *         *
 * Description:        *
 * If a pattern_buffer exists, then this data is used to fill the *
 * buffer instead of the data pattern specified.   *
 *         *
 * Inputs: buffer = Pointer to buffer to fill.   *
 *  byte_count = Number of bytes to fill.   *
 *  pattern = Data pattern to fill buffer with.  *
 *         *
 * Return Value:       *
 *  Void.       *
 *         *
 ************************************************************************/
void
fill_buffer ( u_char  *buffer,
  size_t  byte_count,
  u_int32  pattern)
{
 u_char *bptr = buffer;
 size_t i;

 /*
  * Initialize the buffer with a data pattern.
  */
 if (!pattern_buffer) {
     union {
  u_char pat[sizeof(u_int32)];
  u_int32 pattern;
     } p;

     p.pattern = pattern;
     for (i = 0; i < byte_count; i++) {
  *bptr++ = p.pat[i & (sizeof(u_int32) - 1)];
     }
 } else {
     u_char *pptr, *pend;

     pptr = pattern_bufptr;
     pend = pattern_bufend;
     for (i = 0; i < byte_count; i++) {
  *bptr++ = *pptr++;
  if (pptr == pend) {
      pptr = pattern_buffer;
  }
     }
     pattern_bufptr = pptr;
 }
}

/************************************************************************
 *         *
 * init_buffer() - Initialize Buffer with a Data Pattern.  *
 *         *
 * Inputs: buffer = Pointer to buffer to init.   *
 *  count = Number of bytes to initialize.   *
 *  pattern = Data pattern to init buffer with.  *
 *         *
 * Return Value:       *
 *  Void.       *
 *         *
 ************************************************************************/
void
init_buffer ( u_char  *buffer,
  size_t  count,
  u_int32  pattern )
{
 u_char *bptr;
 union {
     u_char pat[sizeof(u_int32)];
     u_int32 pattern;
 } p;
 size_t i;

 /*
  * Initialize the buffer with a data pattern.
  */
 p.pattern = pattern;
 bptr = buffer;
 for (i = 0; i < count; i++) {
     *bptr++ = p.pat[i & (sizeof(u_int32) - 1)];
 }
}

/************************************************************************
 *         *
 * init_lbdata() - Initialize Data Buffer with Logical Block Data. *
 *         *
 * Description:        *
 * This function takes the starting logical block address, and *
 * inserts it every logical block size bytes, overwriting the first 4 *
 * bytes of each logical block with its' address.   *
 *         *
 * Inputs: buffer = The data buffer to initialize.   *
 *  count = The data buffer size (in bytes).  *
 *  lba = The starting logical block address.  *
 *  lbsize = The logical block size (in bytes).  *
 *         *
 * Outputs: Returns the next lba to use.    *
 *         *
 ************************************************************************/
u_int32
init_lbdata (
 u_char  *buffer,
 size_t  count,
 u_int32  lba,
 u_int32  lbsize )
{
 u_char *bptr = buffer;

 /*
  * Initialize the buffer with logical block data.
  */
 while ( ((ssize_t)count > 0) && (count >= sizeof(lba)) ) {
     htos (bptr, lba, sizeof(lba));
     bptr += lbsize;
     count -= lbsize;
     lba++;
 }
 return (lba);
}

#if !defined(INLINE_FUNCS)
/*
 * Calculate the starting logical block number.
 */
u_int32
make_lba(
 struct dinfo *dip,
 off_t  pos )
{
    if (pos == (off_t) 0) {
 return ((u_int32) 0);
    } else {
 return (pos / lbdata_size);
    }
}

off_t
make_offset(struct dinfo *dip, u_int32 lba)
{
    return ((off_t)(lba * lbdata_size));
}

/*
 * Calculate the starting lbdata block number.
 */
u_int32
make_lbdata(
 struct dinfo *dip,
 off_t  pos )
{
    if (pos == (off_t) 0) {
 return ((u_int32) 0);
    } else {
 return (pos / lbdata_size);
    }
}

#endif /* !defined(INLINE_FUNCS) */

u_int32
winit_lbdata(
 struct dinfo *dip,
 off_t  pos,
 u_char  *buffer,
 size_t  count,
 u_int32  lba,
 u_int32  lbsize )
{
    if (user_lbdata) {
 /* Using user defined lba, not file position! */
 return (init_lbdata (buffer, count, lba, lbsize));
    } else if (pos == (off_t) 0) {
 return (init_lbdata (buffer, count, (u_int32) 0, lbsize));
    } else {
 return (init_lbdata (buffer, count, (pos / lbsize), lbsize));
    }
}

/************************************************************************
 *         *
 * init_iotdata() - Initialize Data Buffer with IOT test pattern. *
 *         *
 * Description:        *
 * This function takes the starting logical block address, and *
 * inserts it every logical block size bytes.  The data pattern used *
 * is the logical block with the constant 0x01010101 added every u_int. *
 *         *
 * NOTE: The IOT pattern is stored in the pattern buffer, which *
 * is assumed to be page aligned, thus there are no alignment problems *
 * on Alpha/Mips.  Remember, the data buffer may be unaligned :-) *
 *         *
 * This implementation does _not_ provide the "best" performance, *
 * but it does allow the normal 'dt' data flow to be mostly unaffected. *
 *         *
 * Inputs: count = The data buffer size (in bytes).  *
 *  lba = The starting logical block address.  *
 *  lbsize = The logical block size (in bytes).  *
 *         *
 * Outputs: Returns the next lba to use.    *
 *         *
 ************************************************************************/
u_int32
init_iotdata (
 size_t  count,
 u_int32  lba,
 u_int32  lbsize )
{
    u_int32 *bptr, lba_pattern;
    int i, bperb;

    if (lbsize == 0) return (lba);
    bperb = (lbsize / sizeof(u_int));
    bptr = (u_int32 *)pattern_buffer;
    pattern_bufptr = pattern_buffer;
    /*
     * Initialize the buffer with the IOT test pattern.
     */
    while ( ((ssize_t)count > 0) && (count >= sizeof(lba)) ) {
        lba_pattern = lba++; /* Start with logical block address. */
        for (i = 0; (i < bperb) && ((ssize_t)count > 0); i++) {
     *bptr++ = lba_pattern;
     lba_pattern += 0x01010101;
     count -= sizeof(u_int32);
 }
    }
    return (lba);
}

/************************************************************************
 *         *
 * init_padbytes() - Initialize pad bytes at end of data buffer. *
 *         *
 * Inputs: buffer = Pointer to start of data buffer.  *
 *  offset = Offset to where pad bytes start.  *
 *  pattern = Data pattern to init buffer with.  *
 *         *
 * Return Value:       *
 *  Void.       *
 *         *
 ************************************************************************/
void
init_padbytes ( u_char  *buffer,
  size_t  offset,
  u_int32  pattern )
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

/************************************************************************
 *         *
 * verify_buffers() - Verify Data Buffers.    *
 *         *
 * Description:        *
 * Simple verification of two data buffers.   *
 *         *
 * Inputs: dip = The device information pointer.   *
 *  dbuffer = Data buffer to verify with.   *
 *  vbuffer = Verification buffer to use.   *
 *  count = The number of bytes to compare.   *
 *         *
 * Outputs: Returns SUCCESS/FAILURE = Data Ok/Compare Error. *
 *         *
 ************************************************************************/
int
verify_buffers( struct dinfo *dip,
  u_char  *dbuffer,
  u_char  *vbuffer,
  size_t  count )
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
     dump_buffer (verify_str, vbuffer, vptr, dump_size, count, FALSE);
     return (FAILURE);
 }
    }
    return (SUCCESS);
}

int
verify_lbdata( struct dinfo *dip,
  u_char  *dbuffer,
  u_char  *vbuffer,
  size_t  count,
  u_int32  *lba )
{
    u_int32 i, dlbn, vlbn;
    u_char *dptr = dbuffer;
    u_char *vptr = vbuffer;
    int status = SUCCESS;

    for (i = 0; (i+sizeof(dlbn) < count); i += lbdata_size,
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
 *         *
 * verify_data() - Verify Data Pattern.     *
 *         *
 * Description:        *
 * If a pattern_buffer exists, then this data is used to compare *
 * the buffer instead of the pattern specified.    *
 *         *
 * Inputs: dip = The device information pointer.   *
 *  buffer = Pointer to data to verify.   *
 *  count = The number of bytes to compare.   *
 *  pattern = Data pattern to compare against.  *
 *  lba = Pointer to starting logical block address. *
 *         *
 * Outputs: Returns SUCCESS/FAILURE = Data Ok/Compare Error. *
 *  lba gets updated with the next lba to verify with. *
 *         *
 ************************************************************************/
int
verify_data ( struct dinfo *dip,
  u_char  *buffer,
  size_t  count,
  u_int32  pattern,
  u_int32  *lba )
{
    u_int32 i = 0;
    bool error = FALSE;
    u_char *vptr = buffer;
    u_char *pptr = NULL, *pend;
    u_int32 lbn, vlbn = *lba;
    int status = SUCCESS;
    union {
 u_char pat[sizeof(u_int32)];
 u_int pattern;
    } p;

    p.pattern = pattern;
    if (pattern_buffer) {
 pptr = pattern_bufptr;
 pend = pattern_bufend;
    }

    /*
     * TODO:  This function is woefully overloaded! :-)
     */
    while ( (i < count) && !error ) {
 /*
  * Handle IOT and Lbdata logical block checks first.
  */
 if ( (iot_pattern || lbdata_flag && lbdata_size) &&
      ((i % lbdata_size) == 0) && (i+sizeof(lbn) <= count) ) {
     if (iot_pattern) {
  vlbn = get_lbn(pptr);
  lbn = get_lbn(vptr);
     } else {
  lbn = stoh (vptr, sizeof(lbn));
     }
     if (lbn != vlbn) {
  error = TRUE;
  ReportLbdataError (dip, *lba, count, i, vlbn, lbn);
  continue;
     } else {
  vlbn++;
  i += sizeof(lbn);
  vptr += sizeof(lbn);
  /* Skip past pattern bytes, handling wrapping. */
  if (pptr) {
      int size = sizeof(lbn);
      while (size--) {
   pptr++;
   if (pptr == pend) pptr = pattern_buffer;
      }
  }
  continue;
     }
 } /* end of IOT/Lbdata check */

 if (pattern_buffer) {
     if (*vptr != *pptr) {
  error = TRUE;
  ReportCompareError (dip, count, i, *pptr, *vptr);
     } else {
  i++, pptr++, vptr++;
  if (pptr == pend) pptr = pattern_buffer;
     }
 } else { /* check one of 4 pattern bytes */
     if (*vptr != p.pat[i & (sizeof(u_int32) - 1)]) {
  error = TRUE;
  ReportCompareError (dip, count, i,
    p.pat[(sizeof(u_int32) - 1)], *vptr);
     } else {
  i++, vptr++;
     }
 }
    } /* end of while... */

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
     dump_buffer (data_str, buffer, vptr, dump_size, count, FALSE);
 }
 status = FAILURE;
    }
    *lba = vlbn;
    if (pattern_buffer) pattern_bufptr = pptr;
    return (status);
}

/************************************************************************
 *         *
 * verify_padbytes() - Verify Pad Bytes Consistency.   *
 *         *
 * Description:        *
 * This function simply checks the pad bytes to ensure they *
 * haven't been overwritten after a read operation.   *
 *         *
 * Inputs: dip = The device information pointer.   *
 *  buffer = Pointer to start of pad buffer.  *
 *  count = The last record read byte count.  *
 *  pattern = Data pattern to compare against.  *
 *  offset = Offset to where pad bytes start.  *
 *         *
 * Outputs: Returns SUCCESS/FAILURE = Data Ok/Compare Error. *
 *         *
 ************************************************************************/
int
verify_padbytes (
 struct dinfo *dip,
 u_char  *buffer,
 size_t  count,
 u_int32  pattern,
 size_t  offset )
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
 struct dinfo *dip,
 u_char  *buffer,
 size_t  offset,
 u_int32  pattern,
 size_t  pbytes,
 size_t  pindex,
 bool  inverted )
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
  fflush (stderr);
  status = FAILURE;
  break;
     }
 }
 return (status);
}

/************************************************************************
 *         *
 * process_pfile() - Process a pattern file.    *
 *         *
 * Inputs: fd   = Pointer to file descriptor.   *
 *  file = Pointer to pattern file name.   *
 *  mode = The mode (read/write) for open.   *
 *         *
 * Outputs: Returns on success, exits on open failure.  *
 *         *
 * Return Value:       *
 *  Void.       *
 *         *
 ************************************************************************/
void
process_pfile (int *fd, char *file, int mode)
{
 struct stat sb;
 size_t count, size;
 u_char *buffer;

#if defined(__WIN32__)
 mode |= O_BINARY;
#endif /* defined(__WIN32__) */
 if ( (*fd = open (file, mode)) == FAILURE) {
     Fprintf ("Error opening pattern file '%s', mode = %o\n",
       file, mode);
     report_error ("process_pfile", TRUE);
     exit (exit_status);
 }
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
 buffer = (u_char *) myalloc (size, 0);
 if ( (count = read (*fd, buffer, size)) != size) {
     Fprintf ("Pattern file '%s' read error!\n", file);
     if ((ssize_t)count == FAILURE) {
  report_error ("read", TRUE);
  exit (exit_status);
     } else {
  Fprintf ("Attempted to read %d bytes, read only %d bytes.",
        size, count);
  exit (FATAL_ERROR);
     }
 }

 patbuf_size = size;
 setup_pattern (buffer, size);
 /*
  * Currently don't need pattern file open for anything else.
  */
 (void) close (*fd); *fd = -1;
}

/************************************************************************
 *         *
 * setup_pattern() - Setup pattern variables.    *
 *         *
 * Inputs: buffer = Pointer to pattern buffer.   *
 *  size = Size of pattern buffer.    *
 *         *
 * Outputs: Returns on success, exits on open failure.  *
 *         *
 * Return Value:       *
 *  Void.       *
 *         *
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
void
print_time (clock_t time)
{
 u_int hr, min, sec, frac;

 frac = time % hz;
 frac = (frac * 100) / hz;
 time /= hz;
 sec = time % 60; time /= 60;
 min = time % 60;
 if (hr = time / 60) {
     fprintf (stderr, "%dh", hr);
 }
 fprintf (stderr, "%02dm", min);
 fprintf (stderr, "%02d.", sec);
 fprintf (stderr, "%02ds\n", frac);
}

void
format_time (clock_t time)
{
 clock_t hr, min, sec, frac;

 frac = (time % hz);
 frac = (frac * 100) / hz;
 time /= hz;
 sec = time % 60; time /= 60;
 min = time % 60;
 if (hr = time / 60) {
     Lprintf ("%dh", hr);
 }
 Lprintf ("%02dm", min);
 Lprintf ("%02d.", sec);
 Lprintf ("%02ds\n", frac);
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
 *         *
 * seek_file() Seeks to the specified file offset.   *
 *         *
 * Inputs: fd      = The file descriptor.    *
 *  records = The number of records.   *
 *  size    = The size of each record.   *
 *  whence  = The method of setting position:  *
 *         *
 *  SEEK_SET (L_SET)  = Set to offset bytes.  *
 *  SEEK_CUR (L_INCR) = Increment by offset bytes.  *
 *  SEEK_END (L_XTND) = Extend by offset bytes.  *
 *         *
 *  offset = (record count * size of each record)  *
 *         *
 * Return Value:       *
 * Returns file position on Success, (off_t)-1 on Failure.  *
 *         *
 ************************************************************************/
off_t
seek_file (int fd, u_long records, off_t size, int whence)
{
    off_t pos;

    /*
     * Seek to specifed file position.
     */
    if ((pos = lseek (fd, (off_t)(records * size), whence)) == (off_t)-1) {
 report_error ("lseek", TRUE);
    }
    return (pos);
}

/*
 * Utility functions to handle file positioning.
 */
off_t
seek_position (struct dinfo *dip, off_t offset, int whence)
{
    off_t pos;

#if defined(DEBUG)
    if (Debug_flag) {
        Fprintf ("attempting lseek (fd=%d, offset=%lu, whence=%d)\n",
     dip->di_fd, offset, whence);
    }
#endif /* defined(DEBUG) */

    /*
     * Seek to specifed file position.
     */
    if ((pos = lseek (dip->di_fd, offset, whence)) == (off_t) -1) {
 if (Debug_flag) {
     Fprintf ("failed lseek (fd %d, offset " FUF " whence %d)\n",
     dip->di_fd, offset, whence);
 }
 report_error ("lseek", TRUE);
 terminate (exit_status);
    }

#if defined(DEBUG)
    if (Debug_flag) {
 Fprintf ("pos=%lu = lseek (fd=%d, offset=%lu, whence=%d)\n",
     pos, dip->di_fd, offset, whence);
    }
#endif /* defined(DEBUG) */

    return (pos);
}

#if !defined(INLINE_FUNCS)

off_t
get_position (struct dinfo *dip)
{
#if defined(_BSD)
    return (seek_position (dip, (off_t) 0, L_INCR));
#else /* !defined(_BSD) */
    return (seek_position (dip, (off_t) 0, SEEK_CUR));
#endif /* defined(_BSD) */
}

#endif /* !defined(INLINE_FUNCS) */

u_int32
get_lba (struct dinfo *dip)
{
    off_t pos;
    if ( (pos = get_position(dip)) ) {
 return ( (u_int32)(pos / lbdata_size) );
    } else {
 return ( (u_int32) 0 );
    }
}

off_t
incr_position (struct dinfo *dip, off_t offset)
{
#if defined(_BSD)
    off_t pos = seek_position (dip, offset, L_INCR);
#else /* !defined(_BSD) */
    off_t pos = seek_position (dip, offset, SEEK_CUR);
#endif /* defined(_BSD) */

    if (Debug_flag || rDebugFlag) {
 u_int32 lba = (pos / (off_t)dip->di_dsize);
 Fprintf ("Seeked to block %lu (%#lx) at byte position " FUF "\n",
       lba, lba, pos);
    }
    return (pos);
}

off_t
set_position (struct dinfo *dip, off_t offset)
{
#if defined(_BSD)
    off_t pos = seek_position (dip, offset, L_SET);
#else /* !defined(_BSD) */
    off_t pos = seek_position (dip, offset, SEEK_SET);
#endif /* defined(_BSD) */

    if (Debug_flag || rDebugFlag) {
 u_int32 lba = (pos / (off_t)dip->di_dsize);
 Fprintf ("Seeked to block %lu (%#lx) at byte position " FUF ".\n",
       lba, lba, pos);
    }
    return (pos);
}

#if !defined(INLINE_FUNCS)

off_t
make_position(struct dinfo *dip, u_int32 lba)
{
    return ( (off_t)(lba * lbdata_size));
}

#endif /* !defined(INLINE_FUNCS) */

void
show_position (struct dinfo *dip, off_t pos)
{
    if (debug_flag || rDebugFlag) {
 u_int32 lba = make_lba(dip, pos);
 Fprintf ("Current file offset is " FUF " (" FXF "), relative lba is %u (%#x)\n",
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
    size_t length;
    u_long randum;

    randum = get_random();
    length = (size_t)((randum % max_size) + min_size);
    if (dip->di_dsize) {
 length = roundup(length, dip->di_dsize);
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
#if defined(RAND48)  /* System V */
    srand48 ((long) seed);
#elif defined(RANDOM)  /* BSD? */
    srandom (seed);
#else  /* ANSI */
    srand (seed);
#endif
}

/*
 * Function to set position for random I/O.
 */
off_t
do_random (struct dinfo *dip, bool doseek, size_t xfer_size)
{
    off_t pos, dsize, ralign;
    u_long randum, records;

    dsize = (off_t)(dip->di_dsize);
    ralign = (off_t)((random_align) ? random_align : dsize);
    if (dip->di_mode == READ_MODE) {
 records = dip->di_records_read;
    } else {
 records = dip->di_records_written;
    }

    /*
     * Ensure the random alignment size is modulo the device size.
     */
    if ( (ralign % dsize) != 0) ralign = roundup(ralign,dsize);

    randum = get_random();

    /*
     * Set position so that the I/O is in the range from file_position to
     * data_limit and is block size aligned.
     *
     * Since randum only ranges to 2^31, treat it as a block number to
     * allow random access to more blocks in a large device or file.
     *
     * Note:  This scaling/rounding/range checking kills performance!
     */
    if (records & 0x01) {
 randum *= dsize; /* Scale upwards every other record. */
    }
    pos = (randum % (u_long)rdata_limit);
    /*
     * Ensure the upper data limit isn't exceeded.
     */
    while ((pos + xfer_size) >= rdata_limit) {
 pos = ( (pos + xfer_size) % (u_long)rdata_limit);
    }

    /*
     * Round up and align as necessary.
     */
    pos = roundup(pos, ralign);

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
     u_int32 lba = (pos / dsize);
     Fprintf ("Random position set to " FUF " block %lu (%#lx).\n",
       pos, lba, lba);
 }
 return (pos);
    }
}

/************************************************************************
 *         *
 * skip_records() Skip past specified number of records.  *
 *         *
 * Inputs: dip = The device info pointer.   *
 *  records = The number of records.   *
 *  buffer  = The buffer to read into.   *
 *  size    = The size of each record.   *
 *         *
 * Return Value:       *
 * Returns SUCCESS/FAILURE/WARNING = Ok/Read Failed/Partial Read. *
 *         *
 ************************************************************************/
int
skip_records ( struct dinfo *dip,
  u_long  records,
  u_char  *buffer,
  off_t  size)
{
    u_long i;
    size_t count;
    int status = SUCCESS;

    /*
     * Skip over the specified record(s).
     */
    for (i = 0; i < records; i++) {
 count = read (dip->di_fd, buffer, size);
 if ( (status = check_read (dip, count, size)) == FAILURE) {
     break;
 }
    }
    return (status);
}

/************************************************************************
 *         *
 * myalloc() Allocate aligned buffer at specified offset.  *
 *         *
 * Description: This function first gets a page aligned buffer using *
 *  malloc(), then it returns a pointer offset bytes into *
 *  the page.  This was done to test page alignment code *
 *  in device drivers.     *
 *         *
 * Inputs: size = The size of the buffer to allocate.  *
 *  offset = The offset into the page.   *
 *         *
 * Outputs: Returns the allocated buffer or 0 for failure.  *
 *         *
 ************************************************************************/
void *
myalloc (size_t size, int offset)
{
 u_char *bp;

#if defined(_BSD)
 if ( (bp = (u_char *) valloc (size + offset)) == (u_char *) 0) {
     Fprintf ("valloc() failed allocating %lu bytes.\n",
      (size + offset));
     exit (FATAL_ERROR);
 }
#elif defined(_QNX_SOURCE) && !defined(_QNX_32BIT)
 if ( (bp = (u_char *) malloc (size + offset)) == (u_char *) 0) {
     Fprintf ("malloc() failed allocating %u bytes.\n",
      (size + offset));
     exit (FATAL_ERROR);
 }
#else /* !defined_BSD) || !defined(_QNX_SOURCE) */
 if ( (bp = (u_char *) malloc (size + offset + page_size)) == (u_char *) 0) {
     Fprintf ("malloc() failed allocating %lu bytes.\n",
     (size + offset + page_size));
     exit (FATAL_ERROR);
 } else {
     bp = (u_char *)( ((ptr_t)bp + (page_size-1)) &~ (page_size-1) );
 }
#endif /* defined(_BSD) */
 bp += offset;
 if (debug_flag) {
     Fprintf (
 "Allocated buffer at address %#lx of %u bytes, using offset %u\n",
       bp, size, offset);
 }
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
     Fprintf ("malloc() failed allocating %u bytes.\n", size);
     exit (ENOMEM);
 }
 memset (bp, '\0', size);
 return (bp);
}

/************************************************************************
 *         *
 * CvtStrtoValue() - Converts ASCII String into Numeric Value.  *
 *         *
 * Inputs: nstr = String to convert.    *
 *  eptr = Pointer for terminating character pointer. *
 *  base = The base used for the conversion.  *
 *         *
 * Outputs: eptr = Points to terminating character or nstr if an *
 *   invalid if numeric value cannot be formed. *
 *         *
 * Return Value:       *
 *  Returns converted number or -1 for FAILURE.  *
 *         *
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
  case 'K':   /* Kilobytes */
   n *= KBYTE_SIZE;
   continue;

  case 'g':
  case 'G':   /* Gigibytes */
   n *= GBYTE_SIZE;
   continue;

  case 'm':
  case 'M':   /* Megabytes */
   n *= MBYTE_SIZE;
   continue;

#if defined(QuadIsLong)
         case 't':
         case 'T':
          n *= TBYTE_SIZE;
   continue;
#endif /* defined(QuadIsLong) */

  case 'w':
  case 'W':   /* Word count. */
   n *= sizeof(int);
   continue;

  case 'q':
  case 'Q':   /* Quadword count. */
   n *= sizeof(large_t);
   continue;

  case 'b':
  case 'B':   /* Block count. */
   n *= BLOCK_SIZE;
   continue;

  case 'd':
  case 'D':   /* Device size. */
   n *= device_size;
   continue;

  case 'c':
  case 'C':   /* Core clicks. */
  case 'p':
  case 'P':   /* Page size. */
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
 *         *
 * CvtStrtoLarge() - Converts ASCII String into Large Value.  *
 *         *
 * Inputs: nstr = String to convert.    *
 *  eptr = Pointer for terminating character pointer. *
 *  base = The base used for the conversion.  *
 *         *
 * Outputs: eptr = Points to terminating character or nstr if an *
 *   invalid if numeric value cannot be formed. *
 *         *
 * Return Value:       *
 *  Returns converted number or -1 for FAILURE.  *
 *         *
 ************************************************************************/
large_t
CvtStrtoLarge (char *nstr, char **eptr, int base)
{
 large_t n = 0, val;

#if defined(QuadIsLong)
 if ( (n = strtoul (nstr, eptr, base)) == (large_t) 0) {
#elif defined(QuadIsLongLong)
#  if defined(SCO) || defined(__QNXNTO__)
 if ( (n = strtoull (nstr, eptr, base)) == (large_t) 0) {
#  else /* !defined(SCO) && !defined(__QNXNTO__) */
 if ( (n = strtouq (nstr, eptr, base)) == (large_t) 0) {
#  endif /* defined(SCO) || defined(__QNXNTO__) */
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
  case 'K':   /* Kilobytes */
   n *= KBYTE_SIZE;
   continue;

  case 'g':
  case 'G':   /* Gigibytes */
   n *= GBYTE_SIZE;
   continue;

  case 'm':
  case 'M':   /* Megabytes */
   n *= MBYTE_SIZE;
   continue;

         case 't':
         case 'T':
          n *= TBYTE_SIZE;
   continue;

  case 'w':
  case 'W':   /* Word count. */
   n *= sizeof(int);
   continue;

  case 'q':
  case 'Q':   /* Quadword count. */
   n *= sizeof(large_t);
   continue;

  case 'b':
  case 'B':   /* Block count. */
   n *= BLOCK_SIZE;
   continue;

  case 'd':
  case 'D':   /* Device size. */
   n *= device_size;
   continue;

  case 'c':
  case 'C':   /* Core clicks. */
  case 'p':
  case 'P':   /* Page size. */
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
 *         *
 * CvtTimetoValue() - Converts ASCII Time String to Numeric Value. *
 *         *
 * Inputs: nstr = String to convert.    *
 *  eptr = Pointer for terminating character pointer. *
 *         *
 * Outputs: eptr = Points to terminating character or nstr if an *
 *   invalid if numeric value cannot be formed. *
 *         *
 * Return Value:       *
 *  Returns converted number in seconds or -1 for FAILURE. *
 *         *
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
  case 'D':   /* Days */
   n *= SECS_PER_DAY;
   continue;

  case 'h':
  case 'H':   /* Hours */
   n *= SECS_PER_HOUR;
   continue;

  case 'm':
  case 'M':   /* Minutes */
   n *= SECS_PER_MIN;
   continue;

  case 's':
  case 'S':   /* Seconds */
   continue;  /* default */

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
 Fprint ("\n");
 Fprintf ("Error number %lu occurred on %s",
   ++error_count, ctime(&error_time));
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
 Fprintf ("Warning on record number %lu, occurred at %s",
       (record + 1), ctime(&error_time));
 return (warning_errors);
}

/*
 * Display failure message to stderr & flush output.
 */
/*VARARGS*/
void
#if defined(USE_STDARG)
Fprintf (char *format, ...)
#else /* !defined(USE_STDARG) */
Fprintf (va_alist)
va_dcl
#endif /* defined(USE_STDARG) */
{
 va_list ap;
 FILE *fp = stderr;

#if defined(USE_STDARG)
 va_start(ap, format);
#else /* !defined(USE_STDARG) */
 char *fmt;
 va_start(ap);
#endif /* defined(USE_STDARG) */
 if ( ((num_procs || num_slices) && !child_pid) || forked_flag) {
     fprintf (fp, "%s (%d): ", cmdname, getpid());
 } else {
     fprintf (fp, "%s: ", cmdname);
 }
#if defined(USE_STDARG)
 vfprintf (fp, format, ap);
#else /* !defined(USE_STDARG) */
 fmt = va_arg(ap, char *);
 vfprintf (fp, fmt, ap);
#endif /* defined(USE_STDARG) */
 va_end(ap);
 (void) fflush (fp);
}

/*
 * Same as Fprintf except no program name identifier.
 */
/*VARARGS*/
void
#if defined(USE_STDARG)
Fprint (char *format, ...)
#else /* !defined(USE_STDARG) */
Fprint (va_alist)
va_dcl
#endif /* defined(USE_STDARG) */
{
 va_list ap;
 FILE *fp = stderr;

#if defined(USE_STDARG)
 va_start(ap, format);
 vfprintf (fp, format, ap);
#else /* !defined(USE_STDARG) */
 char *fmt;
 va_start(ap);
 fmt = va_arg(ap, char *);
 vfprintf (fp, fmt, ap);
#endif /* defined(USE_STDARG) */
 va_end(ap);
}

/*
 * Format & append string to log file buffer.
 */
/*VARARGS*/
void
#if defined(USE_STDARG)
Lprintf (char *format, ...)
#else /* !defined(USE_STDARG) */
Lprintf (va_alist)
va_dcl
#endif /* defined(USE_STDARG) */
{
 va_list ap;
 char *bp = log_bufptr;

#if defined(USE_STDARG)
 va_start(ap, format);
 vsprintf (bp, format, ap);
#else /* !defined(USE_STDARG) */
 char *fmt;
 va_start(ap);
 fmt = va_arg(ap, char *);
 vsprintf (bp, fmt, ap);
#endif /* defined(USE_STDARG) */
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
 FILE *fp = stderr;
 Fputs (log_buffer, fp);
 fflush (fp);
 log_bufptr = log_buffer;
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
 *         *
 * Fputs() Common function to Write String to an Output Stream. *
 *         *
 * Inputs: str = The string buffer pointer.   *
 *  stream = The file stream to access.   *
 *         *
 * Return Value:       *
 *  Returns 0 / -1 = SUCCESS / FAILURE.   *
 *       *
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

#if defined(_QNX_SOURCEx) || defined(SOLARIS)
/*
 * TODO: Could be made a macro for better performance.
 */
void
bzero (char *buffer, size_t length)
{
 memset ((void *)buffer, '\0', length);
}

#endif /* defined(_QNX_SOURCE) || defined(SOLARIS) */

/************************************************************************
 *         *
 * is_Eof() - Check For End Of File Condition.    *
 *         *
 * Description:        *
 * Detect end of file or end of media.  Here's the presumptions: *
 *         *
 *  For Writes, we expect a errno (count == -1) and (errno == ENOSPC). *
 *   For Reads, a (count == 0) indicates end of file, while a  *
 * (count == -1) and (errno == ENOSPC) indicates end of medium. *
 *         *
 * Actually, two file marks normally indicates the end of logical *
 * tape, while (errno == ENOSPC) normally indicates reading past all of *
 * the recorded data.  Note, some tapes (QIC) only have one file mark. *
 *         *
 * Is this confusing or what?  I'm doing the best I can here :-) *
 *         *
 * Inputs: dip = The device information pointer.   *
 *  count = The count from the last I/O request.  *
 *  status = Optional pointer to status variable.  *
 *         *
 * Return Value:       *
 *  Returns TRUE / FALSE = End of file / Not Eof.  *
 *         *
 ************************************************************************/
int
is_Eof (struct dinfo *dip, size_t count, int *status)
{
    /*
     * We expect writes @ EOF to fail w/count -1, and errno ENOSPC.
     */
    if ( (dip->di_mode == WRITE_MODE) && (count == (size_t) 0) ) {
 return (FALSE);
    }
#if defined(SCO)
    if ( (count == (size_t) 0) ||
  ( ((ssize_t)count == (ssize_t) -1) &&
    ((errno == ENOSPC) || (errno == ENXIO)) ) ) {
#else /* !defined(SCO) */
    if ( (count == (size_t) 0) ||
  (((ssize_t)count == (ssize_t) -1) && (errno == ENOSPC)) ) {
#endif /* defined(SCO) */
 large_t data_bytes;
 if (dip->di_mode == READ_MODE) {
     data_bytes = dip->di_dbytes_read;
 } else {
     data_bytes = dip->di_dbytes_written;
 }
 if (debug_flag || eDebugFlag) {
     Fprintf ("End of %s detected, count = %d, errno = %d [file #%lu, record #%lu]\n",
   (count == 0) ? "file" : "media", count, errno,
   (dip->di_mode == READ_MODE) ?
    (dip->di_files_read + 1) : (dip->di_files_written + 1),
   (dip->di_mode == READ_MODE) ?
    (dip->di_records_read + 1) : (dip->di_records_written + 1));
 }
#if defined(EEI)
 if ((dip->di_dtype->dt_dtype == DT_TAPE) && eei_flag) {
     clear_eei_status(dip->di_fd, FALSE);
 }
#endif /* defined(EEI) */
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
#if defined(SCO)
 if ( ( ((ssize_t)count == (ssize_t) -1) &&
        ((errno == ENOSPC) || (errno == ENXIO)) ) &&
      (data_bytes == (large_t ) 0) ) { /* This is the key... */
#else /* !defined(SCO) */
 if ( (((ssize_t)count == (ssize_t) -1) && (errno == ENOSPC)) &&
      (data_bytes == (large_t ) 0) ) { /* This is the key... */
#endif /* defined(SCO) */
     exit_status = errno;
     report_error((dip->di_mode == READ_MODE) ? "read" : "write", TRUE);
     if (status) *status = FAILURE;
 } else {
     exit_status = END_OF_FILE;
 }
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
 Fprintf ("Beginning of media detected [file #%lu, record #%lu]\n",
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
 struct dinfo *dip,
 size_t  byte_count,
 u_int  byte_position,
 u_int  expected_data,
 u_int  data_found)
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
 struct dinfo *dip,
 size_t  byte_count,
 u_int  byte_position,
 bool  eio_error )
{
    /*
     * For disk devices, also report the relative block address.
     */
    if (dip->di_random_access) {
 off_t current_offset;
 off_t starting_offset;
 u_int32 dsize = dip->di_dsize;
 off_t block_offset = (byte_position % dsize);
 u_long lba;

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
 Fprintf("Relative block number where the error occcured is %lu", lba);
 if (block_offset) {
     Fprint (" (offset %lu)\n", block_offset);
 } else {
     Fprint ("\n");
 }
#if defined(LOG_DIAG_INFO)
 if (logdiag_flag) {
     char *bp = msg_buffer;
     bp += Sprintf(bp,
   "%s: Relative block number where the error occcured is %lu\n",
   cmdname, lba);
     if (block_offset) {
  --bp;
  bp += Sprintf(bp, " (offset %lu)\n", block_offset);
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
     if ( (io_mode != TEST_MODE)  &&
   (dip != output_dinfo)  &&
   output_dinfo->di_random_access ) {
  /*
   * NOTE: Output device could be at a different offset.
   */
  struct dinfo *odip = output_dinfo;
  off_t output_offset = get_position(odip);
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
 if (eio_error) print_mtstatus (dip->di_fd, dip->di_mt, TRUE);
#endif /* defined(EEI) */
    }
}

/************************************************************************
 *         *
 * ReportLbDataError() - Report Logical Block Data Compare Error. *
 *         *
 * Inputs: dip = The device info structure.   *
 *  lba = The starting logical block address.  *
 *  byte_count = The byte count of the last request. *
 *  byte_position = Data buffer index where compare failed. *
 *  expected_data = The expected data.   *
 *  data_found = The incorrect data found.   *
 *         *
 * Return Value: Void.       *
 *         *
 ************************************************************************/
void
ReportLbdataError (
 struct dinfo *dip,
        u_int32  lba,
 u_int32  byte_count,
 u_int32  byte_position,
 u_int32  expected_data,
 u_int32  data_found )
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

    Fprintf ("Block expected = %u (0x%x), block found = %u (0x%x), count = %u\n",
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
 bp += Sprintf(bp, "%s: Block expected = %u (0x%x), block found = %u (0x%x), count = %u\n",
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
     s += 2; /* Skip over "0x" or "0X" */
 }
 while (*s) {
     if ( !isxdigit((int)*s++) ) return (FALSE);
 }
 return (TRUE);
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
     if (c == '^') {   /* control/X */
  c = *from++; length--;
  *to++ = (c & 037); count++;
  continue;
     }
     c = *from++; length--;
     if      (c == 'a') key = '\007'; /* alert (bell) */
     else if (c == 'b') key = '\b'; /* backspace */
     else if ( (c == 'e') || (c == 'E') )
    key = '\033'; /* escape */
     else if (c == 'f') key = '\f'; /* formfeed */
     else if (c == 'n') key = '\n'; /* newline */
     else if (c == 'r') key = '\r'; /* return */
     else if (c == 't') key = '\t'; /* tab */
     else if (c == 'v') key = '\v'; /* vertical tab */
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
  key = c; /* Nothing special here... */
     }
     *to++ = key; count++;
 }
 return (count);
}

/************************************************************************
 *         *
 * stoh() - Convert SCSI bytes to Host short/int/long format.  *
 *         *
 * Description:        *
 * This function converts SCSI (big-endian) byte ordering to the *
 * format necessary by this host.     *
 *         *
 * Inputs: bp = Pointer to SCSI data bytes.   *
 *  size = The conversion size in bytes.   *
 *         *
 * Return Value:       *
 *  Returns a long value with the proper byte ordering. *
 *         *
 ************************************************************************/
u_long
stoh (u_char *bp, size_t size)
{
 u_long value = 0L;

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

#if defined(MACHINE_64BITS)
 /*
  * These sizes, require "long long" (64 bits), and since most
  * compilers don't support this, I've only defined for Alpha.
  */
     case 0x05:
  value = ( ((u_long)bp[0] << 32L) | ((u_long)bp[1] << 24) |
     ((u_long)bp[2] << 16) | ((u_long)bp[3] << 8) |
     (u_long)bp[4] );
  break;

     case 0x06:
  value = ( ((u_long)bp[0] << 40L) | ((u_long)bp[1] << 32L) |
     ((u_long)bp[2] << 24) | ((u_long)bp[3] << 16) |
     ((u_long)bp[4] << 8) | (u_long)bp[5] );
  break;

     case 0x07:
  value = ( ((u_long)bp[0] << 48L) | ((u_long)bp[1] << 40L) |
     ((u_long)bp[2] << 32L) | ((u_long)bp[3] << 24) |
     ((u_long)bp[4] << 16) | ((u_long)bp[5] << 8) |
     (u_long)bp[6] );
  break;
     case sizeof(u_long):
  /*
   * NOTE: Compiler dependency? If I don't cast each byte
   * below to u_long, the code generated simply ignored the
   * bytes [0-3] and sign extended bytes [4-7].  Strange.
   */
  value = ( ((u_long)bp[0] << 56L) | ((u_long)bp[1] << 48L) |
     ((u_long)bp[2] << 40L) | ((u_long)bp[3] << 32L) |
     ((u_long)bp[4] << 24) | ((u_long)bp[5] << 16) |
     ((u_long)bp[6] << 8) | (u_long)bp[7] );
  break;
#endif /* defined(MACHINE_64BITS) */

     default:
  Fprintf ("%s\n", bad_conversion_str, size);
  break;

 }
 return (value);
}

/************************************************************************
 *         *
 * htos() - Convert Host short/int/long format to SCSI bytes.  *
 *         *
 * Description:        *
 * This function converts host values to SCSI (big-endian) byte *
 * ordering.        *
 *         *
 * Inputs: bp = Pointer to SCSI data bytes.   *
 *  value = The numeric value to convert.   *
 *  size = The conversion size in bytes.   *
 *         *
 * Return Value:       *
 *  Void.       *
 *         *
 ************************************************************************/
void
htos (u_char *bp, u_long value, size_t size)
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

#if defined(MACHINE_64BITS)
 /*
  * These sizes, require "long long" (64 bits), and since most
  * compilers don't support this, I've only defined for Alpha.
  */
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

     case sizeof(u_long):
  bp[0] = (u_char) (value >> 56L);
  bp[1] = (u_char) (value >> 48L);
  bp[2] = (u_char) (value >> 40L);
  bp[3] = (u_char) (value >> 32L);
  bp[4] = (u_char) (value >> 24);
  bp[5] = (u_char) (value >> 16);
  bp[6] = (u_char) (value >> 8);
  bp[7] = (u_char) value;
  break;
#endif /* defined(MACHINE_64BITS) */

     default:
  Fprintf ("%s\n", bad_conversion_str, size);
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
 * msg - Message to log (not to exceed 1024 bytes).
 *
 * Notes:
 * The binlogmsg() function writes to /dev/kbinlog, so this API
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
