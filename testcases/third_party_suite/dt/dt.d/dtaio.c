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
 * Module:	dtaio.c
 * Author:	Robin T. Miller
 * Date:	August 26, 1993
 *
 * Description:
 *	Functions to handle POSIX Asynchronous I/O requests for 'dt' program.
 */

#if defined(AIO)

#include "dt.h"
#include <aio.h>
#include <limits.h>
#include <sys/stat.h>

#if !defined(AIO_PRIO_DFL)
#  define AIO_PRIO_DFL	0		/* Default scheduling priority. */
#endif /* !defined(AIO_PRIO_DFL) */

/*
 * Modification History:
 *
 * January 26th, 2001 by Robin Miller.
 *	Added support for reverse reading and writing.
 *
 * January 24th, 2001 by Robin Miller.
 *	Add support for variable I/O requests sizes.
 *
 * January 14th, 2001 by Robin Miller.
 *	Added support for multiple volumes option.
 *	Fixed multi-volume write w/lbdata option problem.
 *
 * October 4th, 2000 by Robin Miller.
 *	Update is_Eof() to accept ENXIO for AIO reads @ EOM on
 * SCO UnixWare systems.  All other systems return ENOSPC or zero.
 *
 * May 9th, 2000 by Robin Miller.
 *	Ensure the closing flag gets reset in dtaio_close_file() before
 * calling close_file(), or the file descriptor won't get closed!
 *
 * May 8th, 2000 by Robin Miller.
 *	Honor the di_closing flag, to avoid a race condition with the
 * close function being called again while still closing, from the
 * terminate() routine called by the runtime= alarm, or signals.
 *
 * February 17th, 2000 by Robin Miller.
 *	Adding better support for multi-volume tape testing.  Mainly,
 * make it work with writing multiple tape files, rather than one file.
 *
 * January 6th, 2000 by Robin Miller.
 *	Added support for multi-volume media.
 *	Added a couple missing aio_return() calls.
 *
 * December 30th, 1999 by Robin Miller.
 *	Modify call to do_random() to pass the transfer size.
 *
 * November 10th, 1999 by Robin Miller.
 *	If aio_return() fails, report device information.
 *
 * August 7th, 1999 by Robin Miller.
 *	Minor mods to support AIO on SCO UnixWare 7.1.
 *
 * July 29, 1999 by Robin Miller.
 *	Merge in changes made to compile on FreeBSD.
 *
 * July 22nd, 1999 by Robin Miller.
 *   o	Added support for IOT (DJ's) test pattern.
 *   o	Fixed problem writing wrong starting lba, when lbdata
 *	and random I/O options were enabled.
 * 
 * July 5th, 1999 by Robin Miller.
 *	Cleanup of compilation warnings on Linux.
 *
 * May 27, 1999 by Robin Miller.
 *	Added support for micro-second delays.
 *
 * March 1, 1999 by Robin Miller.
 *	For tapes when Debug is enabled, report the file number.
 *
 * January 13, 1998 by Robin Miller.
 *	Add support for restarting I/O's after EEI reset recovery.
 *	Modified dtaio_waitall() to optionally adjust data/file counts.
 *
 * December 21, 1998 by Robin Miller.
 *	Updates necessary to match tape API changes.
 *
 * November 16, 1998 by Robin Miller.
 *	Added pointer to current AIO control block for error reporting.
 *
 * October 26, 1998 by Robin Miller.
 *   o	Fix incorrect record number displayed when Debug is enabled.
 *   o	Don't exit read/write loops when processing partial records.
 *   o	Fix problem in write function, where short write processing,
 *	caused us not to write sufficent data bytes (actually, the
 *	file loop in write_file() caused dtaio_write_data() to be
 *	called again, and we'd actually end up writing too much!
 *   o	When random I/O and lbdata options are both enabled, use the
 *	file offset seeked to as the starting lbdata address.
 *
 * March 20, 1998 by Robin Miller.
 *	Update counts in dtaio_waitall() for accurate statistics.
 *
 * January 28, 1998 by Robin Miller.
 *	Add dtaio_close() function, to wait for queued I/O's when we're
 *	aborting, to avoid kernel I/O rundown problem, which panic's
 *	the system if we close the CAM disk driver device descriptor,
 *	prior to AIO's completing (fixed in steelos by Anton Verhulst).
 *
 * January 9, 1998 by Robin Miller.
 *	Don't initialize data buffer being written for "disable=compare"
 * which yields better performance.
 *
 * April 3, 1997 by Robin Miller.
 *	Removed use of undocumented AIO_SEEK_CUR in aio_offset.
 *	Also fixed bug where random I/O offset was clobbered, thus
 *	resulting in sequential I/O.
 *
 * February 28, 1996 by Robin Miller.
 *	Added support for copying and verifying device/files.
 *	Modified logic so read errors honor users' error limit.
 *	[ NOTE: Copy and verify operations are done sequentially. ]
 *
 * November 11, 1995 by Robin Miller.
 *	Fix bug with init'ing and performing pad byte verification.
 *	This caused variable length reads with small increment values
 *	to report an (invalid) pad byte data compare error. e.g.:
 *
 *	% dt of=/dev/rmt0h min=10k max=64k incr=1 pattern=incr
 *
 * July 17, 1995 by Robin Miller.
 *	Conditionalize aio_suspend() via "#if defined(POSIX_4D11)" for
 * earlier POSIX drafts so only one copy of this file is necessary.
 * [ NOTE: This is accomplished via -DPOSIX_4D11 in our Makefile. ]
 *
 * July 15, 1995 by Robin Miller.
 *	Fix end of media error handling (ENOSPC), and cleanup code.
 *
 * July 14, 1995 by Robin Miller.
 *	Add logic to allow rotating through 1st ROTATE_SIZE byes of buffers.
 *	[ This option was being silently ignored before, and nobody knew. ]
 *
 * April 15, 1994 by Wayne Casagrande.
 *	Update aiosuspend() interface, which now takes different arguments
 * due to POSIX standard changing.
 *
 * January 20, 1994 by Robin Miller.
 *	When initializing the data buffer, don't do the entire buffer since
 * init'ing large buffer (e.g. 100m) using min, max, and incr options cause
 * excessive paging and VERY poor performance.
 *
 * October 15, 1993 by Robin Miller.
 *	Sorry folks, major screw up on my part.  I forgot to redefine the
 * test function (tf_write_data) field to point a the dtaio_write_data()
 * function, so... synchronous writes were still being done (damn!!!).
 * Also fixed bug when writing to stop looping when end of file reached.
 */

/*
 * Forward References:
 */
#if 0
static void dtaio_checkdevice(struct dinfo *dip);
#endif
static int dtaio_wait(struct dinfo *dip, struct aiocb *acbp);
static int dtaio_waitall(struct dinfo *dip, bool canceling);
static int dtaio_wait_reads(struct dinfo *dip);
static int dtaio_wait_writes(struct dinfo *dip);
static int dtaio_process_read(struct dinfo *dip, struct aiocb *acbp);
static int dtaio_process_write(struct dinfo *dip, struct aiocb *acbp);

#define AIO_BUFS	8		/* Default number of AIO buffers. */
#define AIO_NotQed	-1		/* AIO request not queued flag.	*/

int	aio_bufs = AIO_BUFS;		/* The number of AIO buffers.	*/
int	aio_index;			/* Index to AIO control block.	*/
volatile off_t aio_offset;		/* AIO offset (we maintain).	*/
v_large	aio_data_bytes;			/* Total data bytes per pass.	*/
v_large	aio_file_bytes;			/* # of tape bytes processed.	*/
vu_long	aio_record_count;		/* # of records to processed.	*/
u_int32	aio_lba;			/* AIO logical block address.	*/

/*
 * The following variables are meant to be used with tape devices to
 * backup unprocessed files and/or records due to read-ahead, to be
 * repositioned prior to the next test or before closing the tape.
 */
u_long	aio_data_adjust;		/* # of data bytes to adjust.	*/
u_long	aio_file_adjust;		/* # of tape files to adjust.	*/
u_long	aio_record_adjust;		/* # of tape record to adjust.	*/

struct aiocb	*acbs;			/* Pointer to AIO control blocks. */
u_char		**aiobufs;		/* Pointer to base buffer addrs.  */
struct aiocb	*current_acb;		/* Current acb for error reports. */

/*
 * Declare the POSIX Asynchronous I/O test functions.
 */
struct dtfuncs aio_funcs = {
    /*	tf_open,		tf_close,		tf_initialize,	  */
	open_file,		dtaio_close_file,	dtaio_initialize,
    /*  tf_start_test,		tf_end_test,				  */
	init_file,		nofunc,
    /*	tf_read_file,		tf_read_data,		tf_cancel_reads,  */
	read_file,		dtaio_read_data,	dtaio_cancel_reads,
    /*	tf_write_file,		tf_write_data,		tf_cancel_writes, */
	write_file,		dtaio_write_data,	nofunc,
    /*	tf_flush_data,		tf_verify_data,		tf_reopen_file,   */
	flush_file,		verify_data,		reopen_file,
    /*	tf_startup,		tf_cleanup,		tf_validate_opts  */
	nofunc,			nofunc,			validate_opts
};

/************************************************************************
 *									*
 * dtaio_close_file() - Close an open file descriptor.			*
 *									*
 * Description:								*
 *	This function does the AIO file descriptor close processing.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
dtaio_close_file (struct dinfo *dip)
{
    int status = SUCCESS;

    if (dip->di_closing || (dip->di_fd == NoFd)) {
	return (status);		/* Closing or not open. */
    }
    /*
     * Avoid cancel'ing I/O more than once using the closing flag.
     * We can get called again by alarm expiring or signal handler.
     */
    dip->di_closing = TRUE;
    (void) dtaio_cancel (dip);
    status = dtaio_waitall (dip, FALSE);
    dip->di_closing = FALSE;
    return (close_file (dip));
}

/*
 * Allocate and initialize AIO data structures.
 */
int
dtaio_initialize (struct dinfo *dip)
{
    struct aiocb *acbp;
    size_t size = (sizeof(struct aiocb) * aio_bufs);
    int index;
    int status = SUCCESS;

#if 0
    /*
     * This check isn't being done, since when linked with libaio.a,
     * AIO is mimiced via multiple threads to any device, not just
     * character devices as with libaioraw.a
     */
    dtaio_checkdevice (dip);
#endif

    if ( (dip->di_dtype->dt_dtype == DT_TAPE) && raw_flag && (aio_bufs > 1) ) {
	Fprintf("Sorry, tapes are limited to 1 AIO with raw option!\n");
	aio_bufs = 1;
	size = (sizeof(struct aiocb) * aio_bufs);
    }

    aio_index = 0;
    aio_offset = (off_t) 0;
    if (acbs == NULL) {
	acbs = (struct aiocb *) Malloc (size);
	bzero ((char *) acbs, size);
	if (rotate_flag) {
	    size_t psize = (aio_bufs * sizeof(u_char *));
	    aiobufs = (u_char **) Malloc (psize);
	    bzero ((char *) aiobufs, psize);
	}
    }
    for (index = 0, acbp = acbs; index < aio_bufs; index++, acbp++) {
	if (acbp->aio_buf == NULL) {
	    acbp->aio_buf = myalloc (data_size, align_offset);
	    if (rotate_flag) {
		aiobufs[index] = (u_char *) acbp->aio_buf;
	    }
	}
	acbp->aio_fildes = AIO_NotQed;
	acbp->aio_offset = (off_t) 0;
	acbp->aio_nbytes = block_size;
	acbp->aio_reqprio = AIO_PRIO_DFL;	/* Set default priority. */
#if defined(SCO)
	/*
	 * Note: The AIO manual recommends setting AIO_RAW, but when
	 *       this is set, EINVAL is returned by aio_read/aio_write!
	 */
	acbp->aio_flags = 0;			/* Must be zero to work! */
	acbp->aio_sigevent.sigev_notify = SIGEV_NONE;
#if 0
	acbp->aio_flags = 0; /*AIO_RAW;*/	/* Required on SVR4.2(?) */
	/*
	 * This signaling method did not exist with the first release
	 * of POSIX AIO.  Perhaps I'll add this completion method in
	 * a future release.  Note: Tru64 Unix now supports this too!
	 */
	acbp->aio_sigevent.sigev_signo = /* use with SIGEV_SIGNAL */;
	acbp->aio_sigevent.sigev_notify = SIGEV_CALLBACK;
	acbp->aio_sigevent.sigev_func = my_aio_completion_function;
	acbp->aio_sigevent.sigev_value = acbp;
#endif
#endif /* defined(SCO) */
	/*
	 * Use first buffer allocated for initial skip reads, etc.
	 */
	if (index == 0) data_buffer = (u_char *) acbp->aio_buf;
    }
    return (status);
}

#if 0
static void
dtaio_checkdevice (struct dinfo *dip)
{
    struct stat sb;

    /*
     * Asynchronous I/O is for character devices *only*.
     *    [ Is this true for all operating systems? ]
     */
    if (fstat (dip->di_fd, &sb) == FAILURE) {
	report_error("fstat", FALSE);
	exit (FATAL_ERROR);
    }
    if (!S_ISCHR(sb.st_mode) ) {
	Fprintf("'%s' is NOT a character device, cannot use asynchronous I/O.\n",
								dip->di_dname);
	exit (FATAL_ERROR);
    }
    return;
}
#endif /* 0 */

/*
 * Cancel outstanding I/O on the specified file descriptor.
 */
int
dtaio_cancel (struct dinfo *dip)
{
    int status;

    /*
     * This is not a very useful operation on DEC OSF/1 at this time,
     * since the drivers do *not* contain a cancel entry point.
     * So... you cannot actually cancel outstanding I/O requests.
     */
    if ((status = aio_cancel (dip->di_fd, NULL)) == FAILURE) {
	/*
	 * aio_cancel() returns EBADF if we never issued any AIO!
	 */
	if (errno != EBADF) {
	    report_error ("aio_cancel", TRUE);
	}
	return (status);
    }
    if (debug_flag) {

	switch (status) {

	    case AIO_ALLDONE:
		Fprintf ("All requests completed before cancel...\n");
		break;

	    case AIO_CANCELED:
		Fprintf ("Outstanding requests were canceled...\n");
		break;

	    case AIO_NOTCANCELED:
		Fprintf ("Outstanding (active?) request NOT canceled...\n");
		break;

	    default:
		Fprintf ("Unexpected status of %d from aio_cancel()...\n", status);
		break;
	}
    }
    return (status);
}

int
dtaio_cancel_reads (struct dinfo *dip)
{
    int status;
    struct dtype *dtp = dip->di_dtype;

    aio_data_adjust = aio_file_adjust = aio_record_adjust = 0L;
    (void) dtaio_cancel (dip);
    status = dtaio_waitall (dip, TRUE);
    if (aio_file_adjust && (dtp->dt_dtype == DT_TAPE) ) {
	daddr_t count = (daddr_t)aio_file_adjust;
	/*
	 * Tapes are tricky... we must backup prior to the
	 * last file(s) we processed, then forward space over
	 * its' file mark to be properly positioned (yuck!!!).
	 */
	if (end_of_file) count++;
	status = DoBackwardSpaceFile (dip, count);
	if (status == SUCCESS) {
	    status = DoForwardSpaceFile (dip, (daddr_t) 1);
	}
    } else if (aio_record_adjust && (dtp->dt_dtype == DT_TAPE) ) {
	/*
	 * If we've read partially into the next file, backup.
	 */
	status = DoBackwardSpaceFile (dip, (daddr_t) 1);
	if (status == SUCCESS) {
	    status = DoForwardSpaceFile (dip, (daddr_t) 1);
	}
    }
    return (status);
}

#if defined(EEI)

static int
dtaio_restart(struct dinfo *dip, struct aiocb *first_acbp)
{
    struct aiocb *acbp = first_acbp;
    size_t bsize;
    ssize_t count, adjust;
    int index, error, status = SUCCESS;

    /*
     * Find starting index of this AIO request.
     */
    for (index = 0; index < aio_bufs; index++) {
	if (first_acbp == &acbs[index]) break;
    }
    if (index == aio_bufs) abort(); /* Should NEVER happen! */

    /*
     * Now, wait for and restart all previously active I/O's.
     */
    do {
	/*
	 * Assumes the first request was already waited for!
	 */
	if (Debug_flag) {
	    Fprintf ("Restarting request for acbp at %#lx...\n", acbp);
	}
	if (dip->di_mode == READ_MODE) {
	    if ( (error = aio_read (acbp)) == FAILURE) {
		acbp->aio_fildes = AIO_NotQed;
		report_error ("aio_read", TRUE);
		return (error);
	    }
	} else {
	    if ( (error = aio_write (acbp)) == FAILURE) {
		acbp->aio_fildes = AIO_NotQed;
		report_error ("aio_write", TRUE);
		return (error);
	    }
	}
	if (++index == aio_bufs) index = 0;
	if (index == aio_index) break;

	acbp = &acbs[index];
	if (acbp->aio_fildes == AIO_NotQed) abort();

	error = dtaio_wait (dip, acbp);
	(void) aio_return (acbp);

    } while (TRUE);

    return (status);
}

#endif /* defined(EEI) */

static int
dtaio_wait (struct dinfo *dip, struct aiocb *acbp)
{
    int error, status;

    if (Debug_flag) {
	Fprintf ("Waiting for acbp at %#lx to complete...\n", acbp);
    }
    /*
     * Loop waiting for an I/O request to complete.
     */
    while ((error = aio_error (acbp)) == EINPROGRESS) {
#if defined(POSIX_4D11)
	if ((status = aio_suspend (1, (const struct aiocb **)&acbp)) == FAILURE) {
#else /* Beyond draft 11... */
	if ((status = aio_suspend ((const struct aiocb **)&acbp,1,NULL)) == FAILURE) {
#endif /* defined(POSIX_4D11) */
	    if (errno != EINTR) {
		report_error ("aio_suspend", TRUE);
		return (status);
	    }
	}
    }
    if ( (error == FAILURE) && !terminating_flag) {
	report_error ("aio_error", TRUE);
    }
    return (error);
}

static int
dtaio_waitall(struct dinfo *dip, bool canceling)
{
    struct aiocb *acbp;
    size_t bsize;
    ssize_t count, adjust;
    int index, error, status = SUCCESS;

    /*
     * During EEI reset handling, don't touch the active requests,
     * since dtaio_restart() needs this state to restart reqeusts.
     */
    if (dip->di_proc_eei) return (status);
    /*
     * Loop waiting for all I/O requests to complete.
     */
    for (index = 0; index < aio_bufs; index++) {
	acbp = &acbs[aio_index];
	if (++aio_index == aio_bufs) aio_index = 0;
	if (acbp->aio_fildes == AIO_NotQed) continue;
	if ( (error = dtaio_wait (dip, acbp))) {
	    status = error;
	    if (status == FAILURE) {
		acbp->aio_fildes = AIO_NotQed;
		continue;	/* aio_error() failed! */
	    }
	}
	count = aio_return (acbp);
	acbp->aio_fildes = AIO_NotQed;
	errno = error;
	if ( (count == FAILURE) && !dip->di_closing && !terminating_flag) {
	    /*
	     * End of media is handled below.
	     */
#if defined(SCO)
	    if ( (error != ENOSPC) && (error != ENXIO) ) {
#else /* !defined(SCO) */
	    if (error != ENOSPC) {
#endif /* defined(SCO) */
		current_acb = acbp;
		report_error ("aio_return", TRUE);
		ReportDeviceInfo (dip, acbp->aio_nbytes, 0, (errno == EIO));
		status = FAILURE;
		/* adjust counts below */
	    }
	} else if (error) {
	    count = FAILURE;
	}

	bsize = acbp->aio_nbytes;

	/*
	 * Adjust for short records or no data transferred.
	 */
	if (count == FAILURE) {
	    aio_data_bytes -= bsize;
	    aio_file_bytes -= bsize;
	} else if (adjust = (bsize - count)) {
	    if (debug_flag) {
		Fprintf("Adjusting byte counts by %d bytes...\n", adjust);
	    }
	    aio_data_bytes -= adjust;
	    aio_file_bytes -= adjust;
	}

	/*
	 * Count files or records to adjust after I/O's complete.
	 */
	if ( is_Eof (dip, count, (int *) 0) ) {
	    if (!dip->di_end_of_media) aio_file_adjust++;
	} else if (count > (ssize_t) 0) {
	    aio_record_adjust++;
	    /*
	     * Adjust counts for total statistics.
	     */
	    if (!canceling) {
		if (dip->di_mode == READ_MODE) {
		    dip->di_dbytes_read += count;
		    dip->di_fbytes_read += count;
		} else {
		    dip->di_dbytes_written += count;
		    dip->di_fbytes_written += count;
		}
		aio_data_adjust += count;
		if (count == bsize) {
		    records_processed++;
		} else {
		    partial_records++;
		}
	    }
	}
    }
    return (status);
}

/*
 * Function to wait for and process read requests.
 */
static int
dtaio_wait_reads (struct dinfo *dip)
{
    struct aiocb *acbp;
    int index, error, status = SUCCESS;

    /*
     * Loop waiting for all I/O requests to complete.
     */
    for (index = 0; index < aio_bufs; index++) {
	acbp = &acbs[aio_index];
	if (++aio_index == aio_bufs) aio_index = 0;
	if (acbp->aio_fildes == AIO_NotQed) continue;
	
	if ( (error = dtaio_process_read (dip, acbp)) == FAILURE) {
	    status = error;
	}
	if ( end_of_file ||
	     (dip->di_records_read >= record_limit) || (dip->di_fbytes_read >= data_limit) ) {
	    break;
	}
    }
    return (status);
}

/*
 * Function to wait for and process write requests.
 */
static int
dtaio_wait_writes (struct dinfo *dip)
{
    struct aiocb *acbp;
    int index, error, status = SUCCESS;

    /*
     * Loop waiting for all I/O requests to complete.
     */
    for (index = 0; index < aio_bufs; index++) {
	acbp = &acbs[aio_index];
	if (++aio_index == aio_bufs) aio_index = 0;
	if (acbp->aio_fildes == AIO_NotQed) continue;
	
	if ( (error = dtaio_process_write (dip, acbp)) == FAILURE) {
	    status = error;
	    if (error_count >= error_limit) break;
	}
    }
    return (status);
}

/************************************************************************
 *									*
 * dtaio_read_data() - Read and optionally verify data read.		*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Ok/Error.			*
 *									*
 ************************************************************************/
int
dtaio_read_data (struct dinfo *dip)
{
    struct aiocb *acbp;
    int error, status = SUCCESS;
    size_t bsize, dsize;

    if (dip->di_random_access) {
	if (io_dir == REVERSE) {
	    (void)set_position(dip, (off_t)rdata_limit);
	}
	aio_lba = get_lba(dip);
	aio_offset = get_position(dip);
    } else {
	aio_offset = dip->di_offset;
	aio_lba = make_lbdata (dip, aio_offset);
    }
    aio_data_bytes = aio_file_bytes = aio_record_count = 0;

    /*
     * For variable length records, initialize to minimum record size.
     */
    if (min_size) {
	dsize = min_size;
    } else {
	dsize = block_size;
    }

    /*
     * Now read and optionally verify the input records.
     */
    while ( (error_count < error_limit) &&
	    (dip->di_fbytes_read < data_limit) &&
	    (dip->di_records_read < record_limit) ) {

	if (volumes_flag && (multi_volume >= volume_limit) &&
		  (dip->di_volume_records >= volume_records)) {
	    dip->di_volume_records = volume_records;
	    break;
	}

	/*
	 * Two loops are used with AIO.  The inner loop queues requests up
	 * to the requested amount, and the outer loop checks the actual
	 * data processed.  This is done mainly for tapes to handle short
	 * reads & to efficiently handle multiple tape files.
	 */
	while ( (error_count < error_limit) &&
		(aio_record_count < record_limit) &&
		(aio_file_bytes < data_limit) ) {

	    if (volumes_flag && (multi_volume >= volume_limit) &&
		      (dip->di_volume_records >= volume_records)) {
		break;
	    }

	    if (rdelay_count) {			/* Optional read delay.	*/
		mySleep (rdelay_count);
	    }

	    /*
	     * If data limit was specified, ensure we don't exceed it.
	     */
	    if ( (aio_file_bytes + dsize) > data_limit) {
		bsize = (data_limit - aio_file_bytes);
		if (debug_flag && !variable_flag) {
		    Fprintf ("Record #%lu, Reading a partial record of %lu bytes...\n",
					(aio_record_count + 1), bsize);
		}
	    } else {
		bsize = dsize;
	    }

	    acbp = &acbs[aio_index];
	    /*
	     * If requested, rotate the data buffer through ROTATE_SIZE bytes
	     * to force various unaligned buffer accesses.
	     */
	    if (rotate_flag) {
		data_buffer = aiobufs[aio_index];
		data_buffer += (rotate_offset++ % ROTATE_SIZE);
		acbp->aio_buf = data_buffer;
	    } else {
		data_buffer = (u_char *) acbp->aio_buf;
	    }

	    acbp->aio_fildes = dip->di_fd;
	    acbp->aio_nbytes = bsize;

	    if (io_dir == REVERSE) {
		/*debug*/ if (!aio_offset) abort(); /*debug*/
		bsize = MIN(aio_offset, bsize);
		aio_offset = (off_t)(aio_offset - bsize);
	    }

	    if (io_type == RANDOM_IO) {
		acbp->aio_offset = do_random (dip, FALSE, bsize);
	    } else {
		acbp->aio_offset = aio_offset;
	    }

	    /*
	     * If we'll be doing a data compare after the read, then
	     * fill the data buffer with the inverted pattern to ensure
	     * the buffer actually gets written into (driver debug mostly).
	     */
	    if ((io_mode == TEST_MODE) && compare_flag) {
		init_buffer (data_buffer, bsize, ~pattern);
		init_padbytes (data_buffer, bsize, ~pattern);
	    }

	    if (Debug_flag) {
		u_int32 lba = NO_LBA;
		if (dip->di_random_access || lbdata_flag || iot_pattern) {
		    lba = make_lbdata(dip, (dip->di_volume_bytes + acbp->aio_offset));
		}
		report_record(dip, (dip->di_files_read + 1), (aio_record_count + 1),
			lba, READ_MODE, (void *)acbp->aio_buf, acbp->aio_nbytes);
	    }
	    if ( (error = aio_read (acbp)) == FAILURE) {
		acbp->aio_fildes = AIO_NotQed;
		report_error ("aio_read", TRUE);
		return (error);
	    }

	    /*
	     * Must adjust record/data counts here to avoid writing
	     * too much data, even though the writes are incomplete.
	     */
	    aio_data_bytes += bsize;
	    aio_file_bytes += bsize;
	    aio_record_count++;

	    if (io_dir == FORWARD) {
		aio_offset += bsize;
	    }

	    if (step_offset) {
		if (io_dir == FORWARD) {
		    aio_offset += step_offset;
		} else if ((aio_offset -= step_offset) <= (off_t) 0) {
		    aio_offset = (off_t) 0;
		}
	    }

	    /*
	     * For variable length records, adjust the next record size.
	     */
	    if (min_size) {
		if (variable_flag) {
		    dsize = get_variable (dip);
		} else {
		    dsize += incr_count;
		    if (dsize > max_size) dsize = min_size;
		}
	    }

	    /*
	     * Always ensure the next control block has completed.
	     */
	    if (++aio_index == aio_bufs) aio_index = 0;
	    if ( (io_dir == REVERSE) && (aio_offset == (off_t) 0) ) {
		break;
	    }
	    acbp = &acbs[aio_index];
	    if (acbp->aio_fildes == AIO_NotQed) continue; /* Never Q'ed. */

	    if ( (status = dtaio_process_read (dip, acbp)) == FAILURE) {
		return (status);
	    }
	    if ( end_of_file ) return (status);
	}
	/*
	 * We get to this point after we've Q'ed enough requests to
	 * fulfill the requested record and/or data limit.  We now
	 * wait for these Q'ed requests to complete, adjusting the
	 * global transfer statistics appropriately which reflects
	 * the actual data processed.
	 */
	status = dtaio_wait_reads(dip);
	if ( end_of_file ) break;	/* Stop reading at end of file. */
    }
    return (status);
}

/************************************************************************
 *									*
 * dtaio_process_read() - Process AIO reads & optionally verify data.	*
 *									*
 * Description:								*
 *	This function does waits for the requested AIO read request,	*
 * checks the completion status, and optionally verifies the data read.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		acbp = The AIO control block.				*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE/WARNING = Ok/Error/Warning.	*
 *									*
 ************************************************************************/
static int
dtaio_process_read (struct dinfo *dip, struct aiocb *acbp)
{
    struct dtfuncs *dtf = dip->di_funcs;
    size_t bsize, dsize;
    ssize_t count, adjust;
    int error, status = SUCCESS;

#if defined(EEI)
retry:
#endif
    current_acb = acbp;
    error = dtaio_wait (dip, acbp);
    count = aio_return (acbp);

#if defined(EEI)
    if ( (error == EIO) && (dip->di_dtype->dt_dtype == DT_TAPE) ) {
	if (eei_resets) {
	    if ( HandleTapeResets(dip) ) {
		int error = dtaio_restart(dip, acbp);
		if (error) return (error);
		goto retry;
	    }
	} else if (eei_flag) {
	    (void) get_eei_status(dip->di_fd, dip->di_mt);
	}
    }
#endif /* defined(EEI) */

    acbp->aio_fildes = AIO_NotQed;
    errno = error;

    if (volumes_flag && (multi_volume >= volume_limit) &&
	      (dip->di_volume_records == volume_records)) {
	return (SUCCESS);
    }

    if (count == FAILURE) {
	/*
	 * End of media is handled below.
	 */
#if defined(SCO)
	if ( (error != ENOSPC) && (error != ENXIO) ) {
#else /* !defined(SCO) */
	if (error != ENOSPC) {
#endif /* defined(SCO) */
	    report_error ("aio_return", TRUE);
	    ReportDeviceInfo (dip, acbp->aio_nbytes, 0, (errno == EIO));
	    return (FAILURE);
	}
    } else if (error) {
	count = FAILURE;
    }

    bsize = acbp->aio_nbytes;
    data_buffer = (u_char *)acbp->aio_buf;

    if (min_size) {
	dsize = bsize;
    } else {
	dsize = block_size;
    }

    /*
     * Adjust for short records or no data transferred.
     */
    if (count == FAILURE) {
	aio_data_bytes -= bsize;
	aio_file_bytes -= bsize;
    } else if (adjust = (bsize - count)) {
	if (debug_flag) {
	    Fprintf("Adjusting byte counts by %d bytes...\n", adjust);
	}
	aio_data_bytes -= adjust;
	aio_file_bytes -= adjust;
    }

    /*
     * Process end of file/media conditions and handle multi-volume.
     */
    if ( is_Eof (dip, count, &status) ) {
	if (multi_flag) {
	    if ( (dip->di_dtype->dt_dtype == DT_TAPE) &&
		 !dip->di_end_of_logical ) {
		return (status);	/* Expect two file marks @ EOM. */
	    }
	    status = HandleMultiVolume (dip);
	    aio_record_count = dip->di_records_read;
	    /*aio_file_bytes = dip->di_dbytes_read;*/
	    aio_offset = (off_t) 0;
	}
	return (status);
    } else {
	dip->di_end_of_file = FALSE;	/* Reset saved end of file state. */
	if (count > (ssize_t) 0) {
	    dip->di_dbytes_read += count;
	    dip->di_fbytes_read += count;
	    dip->di_vbytes_read += count;
	    if (count == dsize) {
		records_processed++;
	    } else {
		partial_records++;
	    }
	    dip->di_offset = (acbp->aio_offset + count);
	}
	if ((status = check_read (dip, count, bsize)) == FAILURE) {
	    if (error_count >= error_limit) return (status);
	} else if (io_mode == COPY_MODE) {
	    status = copy_record (output_dinfo, data_buffer, count);
	    if ( (error_count >= error_limit) || end_of_file) return (status);
	} else if (io_mode == VERIFY_MODE) {
	    status = verify_record (output_dinfo, data_buffer, count);
	    if ( (error_count >= error_limit) || end_of_file) return (status);
	}
    }

    /*
     * Verify the data (unless disabled).
     */
    if ( (status != FAILURE) && compare_flag && (io_mode == TEST_MODE)) {
	ssize_t vsize = count;
	if (lbdata_flag || iot_pattern) {
	    aio_lba = make_lbdata(dip, (dip->di_volume_bytes + acbp->aio_offset));
	    if (iot_pattern) {
		aio_lba = init_iotdata (vsize, aio_lba, lbdata_size);
	    }
	}
	status = (*dtf->tf_verify_data)(dip, data_buffer, vsize, pattern, &aio_lba);
	/*
	 * Verify the pad bytes (if enabled).
	 */
	if ( (status == SUCCESS) && pad_check) {
	    (void) verify_padbytes (dip, data_buffer, vsize, ~pattern, bsize);
	}
    }
    dip->di_records_read++;
    dip->di_volume_records++;

    if ( ((io_dir == REVERSE) && (acbp->aio_offset == (off_t) 0)) ||
	 (step_offset && ((acbp->aio_offset - step_offset) <= (off_t) 0)) ) {
	set_Eof(dip);
    }
    return (status);
}

/************************************************************************
 *									*
 * dtaio_write_data() - Write specified data to the output file.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Ok/Error.			*
 *									*
 ************************************************************************/
int
dtaio_write_data (struct dinfo *dip)
{
    struct aiocb *acbp;
    int error, status = SUCCESS;
    size_t bsize, dsize;
    u_int32 lba = lbdata_addr;

    if (dip->di_random_access) {
	if (io_dir == REVERSE) {
	    (void)set_position(dip, (off_t)rdata_limit);
	}
	aio_lba = lba = get_lba(dip);
	aio_offset = get_position(dip);
    } else {
	aio_offset = dip->di_offset;
	aio_lba = lba = make_lbdata (dip, aio_offset);
    }
    aio_data_bytes = aio_file_bytes = aio_record_count = 0;

    /*
     * For variable length records, initialize to minimum record size.
     */
    if (min_size) {
	dsize = min_size;
    } else {
	dsize = block_size;
    }

    /*
     * Now write the specifed number of records.
     */
    while ( (error_count < error_limit) &&
	    (dip->di_fbytes_written < data_limit) &&
	    (dip->di_records_written < record_limit) ) {

	if (volumes_flag && (multi_volume >= volume_limit) &&
		  (dip->di_volume_records >= volume_records)) {
	    dip->di_volume_records = volume_records;
	    break;
	}

	/*
	 * Two loops are used with AIO.  The inner loop queues requests up
	 * to the requested amount, and the outer loop checks the actual
	 * data processed.  This is done to handle short reads, which can
	 * happen frequently with random I/O and large block sizes.
	 */
	while ( (error_count < error_limit) &&
		(aio_record_count < record_limit) &&
		(aio_file_bytes < data_limit) ) {

	    if (volumes_flag && (multi_volume >= volume_limit) &&
		      (dip->di_volume_records >= volume_records)) {
		break;
	    }

	    if (wdelay_count) {			/* Optional write delay	*/
		mySleep (wdelay_count);
	    }

	    /*
	     * If data limit was specified, ensure we don't exceed it.
	     */
	    if ( (aio_file_bytes + dsize) > data_limit) {
		bsize = (data_limit - aio_file_bytes);
		if (debug_flag && !variable_flag) {
		    Fprintf ("Record #%lu, Writing a partial record of %d bytes...\n",
						(aio_record_count + 1), bsize);
		}
	    } else {
		bsize = dsize;
	    }

	    acbp = &acbs[aio_index];
	    /*
	     * If requested, rotate the data buffer through ROTATE_SIZE bytes
	     * to force various unaligned buffer accesses.
	     */
	    if (rotate_flag) {
		data_buffer = aiobufs[aio_index];
		data_buffer += (rotate_offset++ % ROTATE_SIZE);
		acbp->aio_buf = data_buffer;
	    } else {
		data_buffer = (u_char *) acbp->aio_buf;
	    }

	    acbp->aio_fildes = dip->di_fd;
	    acbp->aio_nbytes = bsize;

	    if (io_dir == REVERSE) {
		/*debug*/ if (!aio_offset) abort(); /*debug*/
		bsize = MIN(aio_offset, bsize);
		aio_offset = (off_t)(aio_offset - bsize);
	    }

	    if (io_type == RANDOM_IO) {
		acbp->aio_offset = do_random (dip, FALSE, bsize);
	    } else {
		acbp->aio_offset = aio_offset;
	    }

	    if (iot_pattern || lbdata_flag) {
		lba = make_lbdata (dip, (dip->di_volume_bytes + acbp->aio_offset));
	    }

	    /*
	     * Initialize the data buffer with a pattern.
	     */
	    if ((io_mode == TEST_MODE) && compare_flag) {
	        if (iot_pattern) {
		    lba = init_iotdata(bsize, lba, lbdata_size);
		}
		fill_buffer (data_buffer, bsize, pattern);
	    }

	    /*
	     * Initialize the logical block data (if enabled).
	     */
	    if (lbdata_flag && lbdata_size && !iot_pattern) {
		lba = winit_lbdata (dip, (dip->di_volume_bytes + acbp->aio_offset),
					data_buffer, bsize, lba, lbdata_size);
	    }

	    if (Debug_flag) {
		u_int32 lba = NO_LBA;
		if (dip->di_random_access || lbdata_flag || iot_pattern) {
		    lba = make_lbdata(dip, (dip->di_volume_bytes + acbp->aio_offset));
		}
		report_record(dip, (dip->di_files_written + 1), (aio_record_count + 1),
			lba, WRITE_MODE, (void *)acbp->aio_buf, acbp->aio_nbytes);
	    }

	    if ( (error = aio_write (acbp)) == FAILURE) {
		acbp->aio_fildes = AIO_NotQed;
		report_error ("aio_write", TRUE);
		return (error);
	    }

	    /*
	     * Must adjust record/data counts here to avoid writing
	     * too much data, even though the writes are incomplete.
	     */
	    aio_data_bytes += bsize;
	    aio_file_bytes += bsize;
	    aio_record_count++;

	    if (io_dir == FORWARD) {
		aio_offset += bsize;
	    } 

	    if (step_offset) {
		if (io_dir == FORWARD) {
		    aio_offset += step_offset;
		} else if ((aio_offset -= step_offset) <= (off_t) 0) {
		    aio_offset = (off_t) 0;
		}
	    }

	    /*
	     * For variable length records, adjust the next record size.
	     */
	    if (min_size) {
		if (variable_flag) {
		    dsize = get_variable (dip);
		} else {
		    dsize += incr_count;
		    if (dsize > max_size) dsize = min_size;
		}
	    }

	    /*
	     * Always ensure the next control block has completed.
	     */
	    if (++aio_index == aio_bufs) aio_index = 0;
	    if ( (io_dir == REVERSE) && (aio_offset == (off_t) 0) ) {
		break;
	    }
	    acbp = &acbs[aio_index];
	    if (acbp->aio_fildes == AIO_NotQed) continue; /* Never Q'ed. */

	    if ( (status = dtaio_process_write (dip, acbp)) == FAILURE) {
		return (status);
	    }
	    if (end_of_file) break;
	}
	/*
	 * We get to this point after we've Q'ed enough requests to
	 * fulfill the requested record and/or data limit.  We now
	 * wait for these Q'ed requests to complete, adjusting the
	 * global transfer statistics appropriately which reflects
	 * the actual data processed.
	 */
	status = dtaio_wait_writes(dip);
	if (end_of_file) break;
    }
    return (status);
}

/************************************************************************
 *									*
 * dtaio_process_write() - Process AIO write requests.			*
 *									*
 * Description:								*
 *	This function does waits for the requested AIO write request	*
 * and checks the completion status.					*
 *									*
 * Inputs:	dip = The device info pointer.				*
 *		acbp = The AIO control block.				*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE/WARNING = Ok/Error/Partial.	*
 *									*
 ************************************************************************/
static int
dtaio_process_write (struct dinfo *dip, struct aiocb *acbp)
{
    size_t bsize, dsize;
    ssize_t count, adjust;
    int error, status = SUCCESS;

#if defined(EEI)
retry:
#endif
    current_acb = acbp;
    error = dtaio_wait (dip, acbp);
    count = aio_return (acbp);

#if defined(EEI)
    if ( (error == EIO) && (dip->di_dtype->dt_dtype == DT_TAPE) ) {
	if (eei_resets) {
	    if ( HandleTapeResets(dip) ) {
		dtaio_restart(dip, acbp);
		goto retry;
	    }
	} else if (eei_flag) {
	    (void) get_eei_status(dip->di_fd, dip->di_mt);
	}
    }
#endif /* defined(EEI) */

    acbp->aio_fildes = AIO_NotQed;
    errno = error;

    if (volumes_flag && (multi_volume >= volume_limit) &&
	      (dip->di_volume_records == volume_records)) {
	return (SUCCESS);
    }

    if (count == FAILURE) {
#if defined(SCO)
	if ( (error != ENOSPC) && (error != ENXIO) ) {
#else /* !defined(SCO) */
	if (error != ENOSPC) {
#endif /* defined(SCO) */
	    report_error ("aio_return", TRUE);
	    ReportDeviceInfo (dip, acbp->aio_nbytes, 0, (errno == EIO));
	    return (FAILURE);
	}
    } else if (error) {
	count = FAILURE;
    }

    bsize = acbp->aio_nbytes;

    if (min_size) {
	dsize = bsize;	/* Can lead to wrong partial record count :-) */
    } else {
	dsize = block_size;
    }

    /*
     * Adjust for short records or no data transferred.
     */
    if (count == FAILURE) {
	aio_data_bytes -= bsize;
	aio_file_bytes -= bsize;
    } else if (adjust = (bsize - count)) {
	aio_data_bytes -= adjust;
	aio_file_bytes -= adjust;
    }

    if (count > (ssize_t) 0) {
	dip->di_dbytes_written += count;
	dip->di_fbytes_written += count;
	dip->di_vbytes_written += count;
    }

    /*
     * Process end of file/media conditions and handle multi-volume.
     */
    if ( is_Eof (dip, count, &status) ) {
	if (multi_flag) {
	    status = HandleMultiVolume (dip);
	    aio_record_count = dip->di_records_written;
	    /*aio_file_bytes = dip->di_dbytes_written;*/
	    aio_offset = (off_t) 0;
	}
	return (status);
    }

    if (count > (ssize_t) 0) {
	if (count == dsize) {
	    records_processed++;
	} else {
	    partial_records++;
	}
	dip->di_offset = (acbp->aio_offset + count);
    }
    if ((status = check_write (dip, count, bsize)) == FAILURE) {
	if (error_count >= error_limit) return (status);
    }

    if ( (status != FAILURE) && raw_flag) {
	status = write_verify(dip, (u_char *)acbp->aio_buf, count, dsize, acbp->aio_offset);
	if ( (status == FAILURE) && (error_count >= error_limit) ) {
	    return (status);
	}
    }
    dip->di_records_written++;
    dip->di_volume_records++;

    if ( ((io_dir == REVERSE) && (acbp->aio_offset == (off_t) 0)) ||
	 (step_offset && ((acbp->aio_offset - step_offset) <= (off_t) 0)) ) {
	set_Eof(dip);
    }
    return (status);
}

#endif /* defined(AIO) */
