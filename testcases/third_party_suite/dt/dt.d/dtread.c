static char *whatHeader = "@(#) dt.d/dtread.c /main/3 Jan_18_15:13";
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
 * Module:	dtread.c
 * Author:	Robin T. Miller
 *
 * Description:
 *	Read routines for generic data test program.
 */
#include "dt.h"
#if !defined(_QNX_SOURCE) && !defined(WIN32)
#  include <sys/file.h>
#endif /* !defined(_QNX_SOURCE) && !defined(WIN32) */
#include <sys/stat.h>

/*
 * Modification History:
 *
 * October 21st, 2004 by Robin Miller.
 *      For variable record lengths, ensure we prime the first size
 * to ensure it meets device size alignment requirements.
 *
 * June 23rd, 2004 by Robin Miller.
 *      Added support for triggers on corruption.
 *
 * February 13th, 2004 by Robin Miller.
 *      Factor in the file position when doing reverse I/O, to avoid
 * reading into that area (which is now deemed protected).
 *
 * November 17th, 2003 by Robin Miller.
 *	Breakup output to stdout or stderr, rather than writing
 * all output to stderr.  If output file is stdout ('-') or a log
 * file is specified, then all output reverts to stderr.
 *
 * October 8th, 2003 by Robin Miller.
 *	On AIX, accept ENXIO for I/O's pass EOF.
 *
 * March 4th, 2003 by Robin Miller.
 *      Add EOF support for older SunOS release.  This means reads
 * past EOF return EIO, but continue on this to find real capacity.
 *
 * November 20th, 2002 by Robin Miller.
 *	Updated FindCapacity() to expect ENXIO for reads past EOM.
 *
 * June 25th, 2001 by Robin Miller.
 *	Restructured code associated with Tru64 Unix EEI, so we obtain
 * the EEI status for all tape errors, not just EIO errors.
 *
 * January 28th, 2001 by Robin Miller.
 *	Allow FindCapacity() to be called prior to the file being opened
 * The device capacity is necessary early on for the new slices option.
 *
 * January 26th, 2001 by Robin Miller.
 *	Added support for reverse reading.
 *
 * January 24th, 2001 by Robin Miller.
 *	Add support for variable I/O requests sizes.
 *
 * January 14th, 2001 by Robin Miller.
 *	Added support for multiple volumes option.
 *
 * January 2nd, 2001 by Robin Miller.
 *	Remove check for block or character device in FindCapacity(), since
 * that check is already done in mainline code, so it's a duplicate check.
 *
 * December 30th, 2000 by Robin Miller.
 *	Make changes to build using MKS/NuTCracker product.
 *
 * October 2nd, 2000 by Robin Miller.
 *	Update FindCapacity() to accept ENXIO for reads at EOM on
 * SCO UnixWare systems.
 *
 * April 2nd, 2000 by Robin Miller.
 *	Updated FindCapacity(): printf -> Fprintf so messages go to
 * log file, and add/change casts on large_t values (nothing big :-).
 *
 * March 28th, 2000 by Robin Miller.
 *	Modified calls to file position functions which now accept a
 * device information parameter.
 *
 * March 27th, 2000 by Robin Miller.
 *	Modified FindCapacity() to continue on read() errors, to handle
 * broken Linux ATAPI (IDE) driver, which returns EIO on errors past the
 * end of media (damn!).
 *
 * February 18th, 2000 by Robin Miller.
 *	Fix a problem where the records read value was not updated
 * when the data limit was reached first.
 *
 * February 17th, 2000 by Robin Miller.
 *	Adding better support for multi-volume tape testing.  Mainly,
 * make it work with writing multiple tape files, rather than one file.
 *
 * January 17th, 2000 by Robin Miller.
 *	Added checks @ EOF with/multi-volume enabled, so Copy/Verify
 * operations properly prompt for the next volume.  This allows 'dt'
 * to be used as a general purpose multi-volume tool w/other utilities.
 *
 * January 6th, 2000 by Robin Miller.
 *	Added support for multi-volume media.
 *
 * December 30th, 1999 by Robin Miller.
 *	Modify call to do_random() to pass the transfer size.
 *	Fix lbdata problem when using step option (wrong lba).
 *
 * August 6th, 1999 by Robin Miller.
 *      Better parameterizing of "long long" printf formatting.
 *
 * July 22nd, 1999 by Robin Miller.
 *	Added support for IOT (DJ's) test pattern.
 * 
 * May 27, 1999 by Robin Miller.
 *	Adding support for micro-second delays.
 *
 * March 1, 1999 by Robin Miller.
 *	For tapes when Debug is enabled, report the file number.
 *
 * December 21, 1998 by Robin Miller.
 *	Add hooks to handle tape device resets (DUNIX specific).
 *
 * October 29, 1998 by Robin Miller.
 *	Implement a random I/O data limit, instead of using the normal
 * data limit variable (not good to dual purpose this value).
 *
 * October 26, 1998 by Robin Miller.
 *	When random I/O and lbdata options are both enabled, use the
 * file offset seeked to as the starting lbdata address.
 *
 * April 28, 1998 by Robin Miller.
 *	For WIN32/NT, or in O_BINARY into open flags to force binary
 *	mode (the default is text mode).
 *
 * February 29, 1996 by Robin Miller.
 *	Added FindCapacity() function to obtain capacity for random
 *	access devices.  Must set limits for random I/O.
 *
 * February 28, 1996 by Robin Miller.
 *	Added support for copying and verifying device/files.
 *	Modified logic so read errors honor users' error limit.
 *
 * February 23, 1996 by Robin Miller.
 *	Only report partial record warning for sequential I/O testing.
 *	Random I/O can position us towards the end of media often, and
 *	partial transfers are likely especially with large block sizes.
 *
 * November 11, 1995 by Robin Miller.
 *	Fix bug with init'ing and performing pad byte verification.
 *	This caused variable length reads with small increment values
 *	to report an (invalid) pad byte data compare error. e.g.:
 *
 *	% dt of=/dev/rmt0h min=10k max=64k incr=1 pattern=incr
 *
 * July 15, 1995 by Robin Miller.
 *	Fix end of media error handling (ENOSPC), and cleanup code.
 *
 * January 20, 1994 by Robin Miller.
 *	When initializing the data buffer, don't do the entire buffer since
 * init'ing large buffer (e.g. 100m) using min, max, and incr options cause
 * excessive paging and VERY poor performance.
 *
 * September 17, 1993 by Robin Miller.
 *	Report record number on warning errors (for debug).
 *
 * September 4, 1993 by Robin Miller.
 *	Moved memory mapped I/O logic to seperate module.
 *
 * Septemeber 1, 1993 by Robin Miller.
 *	Add ability to read variable record sizes.
 *
 * August 31, 1993 by Robin Miller.
 *	Rotate starting data buffer address through sizeof(long).
 *
 * August 27, 1993 by Robin MIller.
 *	Added support for DEC OSF/1 POSIX Asynchronous I/O (AIO).
 *
 * August 18, 1992 by Robin Miller.
 *	If "step=" option was specified, then seek that many bytes
 *	before the next read request (for disks).
 *
 * August 10, 1993 by Robin Miller.
 *	Added initializing and checking of buffer pad bytes to ensure
 *	data corruption does *not* occur at the end of read buffers.
 *
 * August 5, 1993 by Robin Miller.
 *	Added support for reading multiple tape files.
 *
 * September 11, 1992 by Robin Miller.
 *	Ensure data limit specified by user is not exceeded, incase
 *	the block size isn't modulo the data limit.
 *
 * September 5, 1992 by Robin Miller.
 *	Initial port to QNX 4.1 Operating System.
 *
 * August 21, 1990 by Robin Miller.
 *	Changed exit status so scripts can detect and handle errors
 *	based on the exit code.  If not success, fatal error, or end
 *	of file/tape, the exit code is the error number (errno).
 *
 * August 7, 1990 by Robin Miller.
 *	If "seek=n" option is specified, then seek that many records
 *	before starting to read.  The "skip=n" option skips records
 *	by reading, while "seek=n" seeks past records.
 */

/************************************************************************
 *									*
 * read_data() - Read and optionally verify data read.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Ok/Error.			*
 *									*
 ************************************************************************/
int
read_data (struct dinfo *dip)
{
    register ssize_t count;
    register size_t bsize, dsize;
    int status = SUCCESS;
    struct dtfuncs *dtf = dip->di_funcs;
    u_int32 lba;

    /*
     * For variable length records, initialize to minimum record size.
     */
    if (min_size) {
        if (variable_flag) {
            dsize = get_variable (dip);
        } else {
	    dsize = min_size;
        }
    } else {
	dsize = block_size;
    }
    if (dip->di_random_access) {
	if (io_dir == REVERSE) {
	    (void)set_position(dip, (OFF_T)rdata_limit);
	}
	lba = get_lba(dip);
	dip->di_offset = get_position(dip);
    } else {
	lba = make_lbdata (dip, dip->di_offset);
    }

    /*
     * Now read and optionally verify the input records.
     */
    while ( (error_count < error_limit) &&
	    (dip->di_fbytes_read < data_limit) &&
	    (dip->di_records_read < record_limit) ) {

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
	if ( (dip->di_fbytes_read + dsize) > data_limit) {
	    bsize = (size_t)(data_limit - dip->di_fbytes_read);
	} else {
	    bsize = dsize;
	}

	if (io_dir == REVERSE) {
	    bsize = MIN((dip->di_offset-file_position), bsize);
	    dip->di_offset = set_position(dip, (OFF_T)(dip->di_offset - bsize));
	} else if (io_type == RANDOM_IO) {
	    dip->di_offset = do_random (dip, TRUE, bsize);
	}

        if (debug_flag && (bsize != dsize) && !variable_flag) {
            Printf ("Record #%lu, Reading a partial record of %lu bytes...\n",
                                    (dip->di_records_read + 1), bsize);
        }

	if (iot_pattern || lbdata_flag) {
	    lba = make_lbdata (dip, (OFF_T)(dip->di_volume_bytes + dip->di_offset));
	}

	/*
	 * If requested, rotate the data buffer through ROTATE_SIZE bytes
	 * to force various unaligned buffer accesses.
	 */
	if (rotate_flag) {
	    data_buffer = (base_buffer + (rotate_offset++ % ROTATE_SIZE));
	}

	/*
	 * If we'll be doing a data compare after the read, then
	 * fill the data buffer with the inverted pattern to ensure
	 * the buffer actually gets written into (driver debug mostly).
	 */
	if ((io_mode == TEST_MODE) && compare_flag) {
	    init_buffer (data_buffer, bsize, ~pattern);
	    init_padbytes (data_buffer, bsize, ~pattern);
	    if (iot_pattern) {
		lba = init_iotdata (bsize, lba, lbdata_size);
	    }
	}

	if (Debug_flag) {
	    large_t iolba = NO_LBA;
            off_t iopos = (off_t) 0;
	    if (dip->di_random_access) {
                iopos = get_position(dip);
		iolba = (iopos / dip->di_dsize);
	    } else if (lbdata_flag || iot_pattern) {
                iopos = (OFF_T)(dip->di_volume_bytes + dip->di_offset);
		iolba = make_lbdata (dip, iopos);
	    }
	    report_record(dip, (dip->di_files_read + 1), (dip->di_records_read + 1),
				iolba, iopos, READ_MODE, data_buffer, bsize);
	}

	count = read_record (dip, data_buffer, bsize, dsize, &status);
	if (end_of_file) break;		/* Stop reading at end of file. */

	if (status == FAILURE) {
	    if (error_count >= error_limit) break;
	} else if (io_mode == COPY_MODE) {
	    status = copy_record (output_dinfo, data_buffer, count);
	    if ( (error_count >= error_limit) || end_of_file) break;
	} else if (io_mode == VERIFY_MODE) {
	    status = verify_record (output_dinfo, data_buffer, count);
	    if ( (error_count >= error_limit) || end_of_file) break;
	}

	/*
	 * Verify the data (unless disabled).
	 */
	if ( (status != FAILURE) && compare_flag && (io_mode == TEST_MODE) ) {
	    ssize_t vsize = count;
	    status = (*dtf->tf_verify_data)(dip, data_buffer, vsize, pattern, &lba);
	    /*
	     * Verify the pad bytes (if enabled).
	     */
	    if ( (status == SUCCESS) && pad_check) {
		(void) verify_padbytes (dip, data_buffer, vsize, ~pattern, bsize);
	    }
	}

	/*
	 * If we had a partial transfer, perhaps due to an error, adjust
	 * the logical block address in preparation for the next request.
	 */
	if (iot_pattern && ((size_t)count < bsize)) {
	    size_t resid = (bsize - count);
	    lba -= howmany(resid, lbdata_size);
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

	dip->di_records_read++;
	dip->di_volume_records++;

	if (io_dir == FORWARD) {
	    dip->di_offset += count;	/* Maintain our own position too! */
	} else if ( (io_type == SEQUENTIAL_IO) &&
		    (dip->di_offset == (OFF_T) file_position) ) {
	    set_Eof(dip);
	    break;
	}

	if (step_offset) {
	    if (io_dir == FORWARD) {
		dip->di_offset = incr_position (dip, step_offset);
	    } else if ((dip->di_offset -= step_offset) <= (OFF_T) file_position) {
		set_Eof(dip);
		break;
	    }
	}
    }
    return (status);
}

/************************************************************************
 *									*
 * check_read() - Check status of last read operation.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		count = Number of bytes read.				*
 *		size  = Number of bytes expected.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE/WARNING = Ok/Error/Warning	*
 *									*
 ************************************************************************/
int
check_read (struct dinfo *dip, ssize_t count, size_t size)
{
    int status = SUCCESS;

    if ((size_t)count != size) {
	if (count == FAILURE) {
#if defined(WIN32)
	    report_error ("ReadFile", FALSE);
	    ReportDeviceInfo (dip, 0, 0, (bool)((errno=GetLastError()) == ERROR_IO_DEVICE));
#else /* !defined(WIN32) */
	    report_error ("read", FALSE);
	    ReportDeviceInfo (dip, 0, 0, (bool)(errno == EIO));
#endif /* defined(WIN32) */
            (void)ExecuteTrigger(dip, "read");
	} else {
	    /*
	     * For reads at end of file or reads at end of block
	     * devices, we'll read less than the requested count.
	     * In this case, we'll treat this as a warning since
	     * this is to be expected.  In the case of tape, the
	     * next read will indicate end of tape (in my driver).
	     *
	     * NOTE:  The raw device should be used for disks.
	     */
	    if ( (debug_flag || verbose_flag || ((size_t)count > size)) &&
		 (io_mode == TEST_MODE) /*&& (io_type == SEQUENTIAL_IO)*/ ) {
		Printf(
	"WARNING: Record #%lu, attempted to read %lu bytes, read only %lu bytes.\n",
						(dip->di_records_read + 1), size, count);
	    }
	    if ((size_t)count < size) {	/* Partial read is a warning. */
		warning_errors++;
		return (WARNING);
	    }
	    ReportDeviceInfo (dip, count, 0, FALSE);
	}
	(void)RecordError();
	dip->di_read_errors++;
	status = FAILURE;
    }
    return (status);
}

/*
 * This function is envoked when reading multiple tape files, to
 * position past an expected file mark.  This is especially important
 * when using the lbdata or iot options, since encountering an expected
 * EOF throws off the offset being maintained, resulting in an lba error.
 */
int
read_eof(struct dinfo *dip)
{
    ssize_t count;
    size_t bsize = block_size;
    int status = SUCCESS;

    if (debug_flag) {
	Printf("Processing end of file... [file #%lu, record #%lu]\n",
			(dip->di_files_read + 1), (dip->di_records_read + 1));
    }
    dip->di_eof_processing = TRUE;
    count = read_record (dip, data_buffer, bsize, bsize, &status);
    dip->di_eof_processing = FALSE;
    if (!end_of_file) {
	Fprintf("ERROR: File %lu, Record %lu, expected EOF was NOT detected!\n",
		(dip->di_files_read + 1), (dip->di_records_read + 1));
	ReportDeviceInfo (dip, count, 0, FALSE);
	(void)RecordError();
	dip->di_read_errors++;
	status = FAILURE;
    }
    return (status);
}

/*
 * This function is called after EOF is detected, to read the next record
 * which checks for reaching the end of logical tape (i.e. two successive
 * file marks).  For multi-volume tapes, the user will be prompted for the
 * next volume via read_record(), and the end of file flag gets reset when
 * the tape is re-open'ed.
 */
int
read_eom(struct dinfo *dip)
{
    ssize_t count;
    size_t bsize = block_size;
    int status = SUCCESS;

    if (debug_flag) {
	Printf("Processing end of media... [file #%lu, record #%lu]\n",
			(dip->di_files_read + 1), (dip->di_records_read + 1));
    }
    dip->di_eom_processing = TRUE;
    count = read_record (dip, data_buffer, bsize, bsize, &status);
    dip->di_eom_processing = FALSE;

    if (multi_flag) {
	if (end_of_file) {
	    Fprintf("ERROR: File %lu, Record %lu, expected EOM was NOT detected!\n",
			(dip->di_files_read + 1), (dip->di_records_read + 1));
	    ReportDeviceInfo (dip, count, 0, FALSE);
	    (void)RecordError();
	    return (FAILURE);
	}
    } else if ( !dip->di_end_of_logical ) {
	Fprintf("ERROR: File %lu, Record %lu, expected EOM was NOT detected!\n",
		(dip->di_files_read + 1), (dip->di_records_read + 1));
	ReportDeviceInfo (dip, count, 0, FALSE);
	(void)RecordError();
	dip->di_read_errors++;
	return (FAILURE);
    }
    return (SUCCESS);	/* We don't care about the read status! */
}

/************************************************************************
 *									*
 * read_record() - Read record from device or file.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		buffer = The data buffer to read into.			*
 *		bsize = The number of bytes read.			*
 *		dsize = The users' requested size.			*
 *		status = Pointer to status variable.			*
 *									*
 * Outputs:	status = SUCCESS/FAILURE/WARNING = Ok/Error/Warning	*
 *		Return value is number of bytes from read() request.	*
 *									*
 ************************************************************************/
ssize_t
read_record (	struct dinfo	*dip,
		u_char		*buffer,
		size_t		bsize,
		size_t		dsize,
		int		*status )
{
    ssize_t count;

retry:
    *status = SUCCESS;
    if (noprog_flag) { dip->di_initiated_time = time((time_t *)0); }
#if defined(WIN32)
   if (!ReadFile (dip->di_fd, buffer, bsize, &count, NULL)) { count = -1; }
#else /* !defined(WIN32) */
    count = read (dip->di_fd, buffer, bsize);
#endif /* defined(WIN32) */
    if (noprog_flag) { dip->di_initiated_time = (time_t) 0; }

#if defined(EEI)
    if ( (count == FAILURE) &&
	 (dip->di_dtype->dt_dtype == DT_TAPE) ) {
	if ( (errno == EIO) && eei_resets) {
	    if ( HandleTapeResets(dip) ) {
		goto retry;
	    }
	} else if (eei_flag) {
	    (void) get_eei_status(dip->di_fd, dip->di_mt);
	}
    }
#endif /* defined(EEI) */

    /*
     * Allow terminal reads to continue on end of file (eof).
     * [ NOTE: This allows reads with timeouts to continue. ]
     */
    if ( count || (dip->di_dtype->dt_dtype != DT_TERMINAL) ) {
	if ( is_Eof (dip, count, status) ) {
	    if (multi_flag &&
		(!stdin_flag || (dip->di_ftype == OUTPUT_FILE)) ) {
		if ( (dip->di_dtype->dt_dtype == DT_TAPE) &&
		     !dip->di_end_of_logical ) {
		    return (count);	/* Expect two file marks @ EOM. */
		}
		*status = HandleMultiVolume (dip);
		dip->di_offset = (OFF_T) 0;
		if ( !dip->di_eof_processing && !dip->di_eom_processing ) {
		    if (*status == SUCCESS) goto retry;
		}
	    }
	    return (count);	/* Stop reading at end of file. */
	}
    }
    if ( dip->di_eof_processing || dip->di_eom_processing ) {
	return (count);
    }
    dip->di_end_of_file = FALSE;	/* Reset saved end of file state. */

    /*
     * If something was read, adjust counts and statistics.
     */
    if (count > (ssize_t) 0) {
	dip->di_dbytes_read += count;
	dip->di_fbytes_read += count;
	dip->di_vbytes_read += count;
	if ((size_t)count == dsize) {
	    dip->di_full_reads++;
	} else {
	    dip->di_partial_reads++;
	}
    }

    *status = check_read (dip, count, bsize);

    return (count);
}

/************************************************************************
 *									*
 * verify_record() - Verify record with selected output device/file.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		buffer = The data buffer to compare.			*
 *		bsize = The number of bytes read.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE/WARNING = Ok/Error/Warning	*
 *									*
 ************************************************************************/
int
verify_record (	struct dinfo	*dip,
		u_char		*buffer,
		size_t		bsize )
{
    struct dtfuncs *dtf = dip->di_funcs;
    ssize_t count;
    int status;
    u_int32 lba = lbdata_addr;

    /*
     * TODO: Re-write this using the verify buffer (when I have time).
     */
    count = read_record (dip, pattern_buffer, bsize, bsize, &status);
    if ( (status == FAILURE) || end_of_file) return (status);

    /*
     * I realize this is real ugly, but I wanted to use existing code.
     */
    patbuf_size = count;
    pattern_bufptr = pattern_buffer;
    pattern_bufend = pattern_buffer + count;

    status = (*dtf->tf_verify_data)(dip, buffer, count, pattern, &lba);
    /* TODO: Get this into read_record() where it belongs! */
    dip->di_records_read++;
    return (status);
}

/************************************************************************
 *									*
 * FindCapacity() - Find capacity of a random access device.		*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Outputs:	Fills in device capacity and data limit on success.	*
 *									*
 * Return Value: Returns SUCCESS/FAILURE/WARNING = Ok/Error/Warning	*
 *									*
 ************************************************************************/
int
FindCapacity (struct dinfo *dip)
{
    u_int32 dsize = dip->di_dsize;
    OFF_T lba, max_seek = (MAX_SEEK - dsize);
    long adjust = ((250 * MBYTE_SIZE) / dsize);
    int attempts = 0;
    ssize_t count, last;
    u_char *buffer;
    HANDLE fd, saved_fd = NoFd;
    int status = SUCCESS;
    bool temp_fd = FALSE;

    /*
     * Use the user specified capacity (if specified).
     */
    if (user_capacity) {
	lba = (OFF_T)(user_capacity / dsize);
	goto set_capacity;
    } else if (dip->di_data_limit) {
	return (status);
    }

    if (debug_flag || Debug_flag || rDebugFlag) {
	Printf ("Attempting to calculate capacity via seek/read algorithm...\n");
    }
    /*
     * If the device is open in write mode, open another
     * file descriptor for reading.
     */
    if ( (dip->di_fd == NoFd) || (dip->di_mode == WRITE_MODE) ) {
	temp_fd = TRUE;
	saved_fd = dip->di_fd;
#if defined(WIN32)
	if ( (fd = CreateFile (dip->di_dname, GENERIC_READ, (FILE_SHARE_READ | FILE_SHARE_WRITE), NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == NoFd) {
#elif defined(__WIN32__)
	if ( (fd = open (dip->di_dname, (O_RDONLY|O_BINARY))) < 0) {
#else /* !defined(__WIN32__) */
	if ( (fd = open (dip->di_dname, O_RDONLY)) < 0) {
#endif /* defined(__WIN32__) */
#if defined(WIN32)
	    report_error ("FindCapacity() CreateFile", FALSE);
#else /* !defined(WIN32) */
	    report_error ("FindCapacity() open", FALSE);
#endif /* defined(WIN32) */
	    return (FAILURE);
	}
	dip->di_fd = fd;
    }

    buffer = (u_char *) malloc (dsize);
    if (buffer == NULL) return (FAILURE);
    /*
     * Algorthim:
     *	There maybe a better may, but this works...
     */
    lba = adjust;
    adjust /= 2;

    while (TRUE) {
	attempts++;
#if defined(DEBUG)
	Printf("lba = " LUF ", adjust = %lu\n", lba, adjust);
#endif
	/*
	 * We cannot seek past the maximum allowable file position,
	 * otherwise lseek() fails, and 'dt' exits.  So, we must
	 * limit seeks appropriately, and break out of this loop
	 * if we hit the upper seek limit.
	 */
	if ( (OFF_T)(lba * dsize) < (OFF_T) 0 ) {
	    lba = (max_seek / dsize);
	}
	(void) set_position (dip, (OFF_T)(lba * dsize));
#if defined(WIN32)
	if(!ReadFile(dip->di_fd, buffer, dsize, &count, NULL)) count = -1;
	if( count == dsize) {
#else /* !defined(WIN32) */
	if ( (count = read (dip->di_fd, buffer, dsize)) == (ssize_t)dsize) {
#endif /* defined(WIN32) */
	    if (lba == (OFF_T)(max_seek / dsize)) break;
	    lba += adjust;
	    if (adjust == 1) break;
#if defined(SCO) || defined(HP_UX) || defined(AIX)
	} else if ( (count == 0) ||
		    ( (count < 0) &&
		      ((errno == ENOSPC) || (errno == ENXIO)) ) ) {
#elif defined(BrokenEOF)
	} else if ( (count == 0) ||
		    ( (count < 0) &&
		      ((errno == ENOSPC) || (errno == EIO)) ) ) {
#elif defined(WIN32)
	} else if ( (count == 0) ||
		    ((count < 0) && 
		    ((errno = GetLastError()) == ERROR_DISK_FULL) ) ) { 
#else /* !defined(SCO) */
	} else if ( (count == 0) ||
		    ( (count < 0) && (errno == ENOSPC) ) ) {
#endif /* defined(SCO) || defined(HP_UX) || defined(AIX) */
	    if (last) adjust /= 2;
	    if (!adjust) adjust++;
	    lba -= adjust;
	} else {
#if defined(WIN32)
	    report_error ("FindCapacity() ReadFile", FALSE);
#else /* !defined(WIN32) */
	    report_error ("FindCapacity() read", FALSE);
#endif /* defined(WIN32) */
	    status = FAILURE;
	    break;
	}
	last = count;
    }
    free (buffer);
    if (temp_fd) {
#if defined(WIN32)
	CloseHandle(dip->di_fd);
#else /* !defined(WIN32) */
	(void) close (dip->di_fd);
#endif /* defined(WIN32) */
	dip->di_fd = saved_fd;
    } else {
	(void) set_position (dip, (OFF_T) 0);
    }

    /*
     * If the read failed, set the lba to the last successful read,
     * and continue.  Won't be perfect, but at least we can run.
     * Note: Workaround for problem seen on Linux w/ATAPI CD-ROM's.
     */
    if (status == FAILURE) {
#if 1
#if defined(DEBUG)
	Printf("failed, last good lba was " LUF ", adjust was %ld\n",
							 lba, adjust);
#endif /* defined(DEBUG) */
	lba -= adjust;
	exit_status = SUCCESS;
#else
	return (status);	/* Return the failure! */
#endif
    }

#if defined(DEBUG)
    Printf ("read attempts was %d, the max lba is " LUF "\n", attempts, lba);
#endif /* defined(DEBUG) */

set_capacity:
    dip->di_capacity = lba;
    dip->di_data_limit = (large_t)(lba * dsize);

    if (!record_limit) record_limit = INFINITY;
    if (data_limit == INFINITY) data_limit = dip->di_data_limit;

    /*
     * The proper data limit is necessary for random I/O processing.
     */
    if ( (io_dir == REVERSE) || (io_type == RANDOM_IO) ) {
	if ( (rdata_limit == (large_t)0) || (rdata_limit > dip->di_data_limit) ) {
	    rdata_limit = dip->di_data_limit;
	}
	if (debug_flag || Debug_flag || rDebugFlag || (status == FAILURE)) {
/* TODO: Cleanup this mess! */
#if !defined(__GNUC__) && ( defined(_NT_SOURCE) || defined(WIN32) )
    /* Avoid:  error C2520: conversion from unsigned __int64 to double not implemented, use signed __int64 */
	    Printf ("Random data limit set to " LUF " bytes (%.3f Mbytes), " LUF " blocks.\n",
		     rdata_limit, ((double)(slarge_t)rdata_limit/(double)MBYTE_SIZE), (rdata_limit / dsize));
#else /* !defined(_NT_SOURCE) */
	    Printf ("Random data limit set to " LUF " bytes (%.3f Mbytes), " LUF " blocks.\n",
		rdata_limit, ((double)rdata_limit/(double)MBYTE_SIZE), (rdata_limit / dsize));
#endif /* !defined(__GNUC__) && ( defined(_NT_SOURCE) || defined(WIN32) ) */
	}
	if (rdata_limit <= file_position) {
	    LogMsg (efp, logLevelCrit, 0,
		    "Please specify a random data limit > file position!\n");
	    exit (FATAL_ERROR);
	}
    } else if (debug_flag || Debug_flag || (status == FAILURE)) {
#if !defined(__GNUC__) && ( defined(_NT_SOURCE) || defined(WIN32) )
    /* Avoid:  error C2520: conversion from unsigned __int64 to double not implemented, use signed __int64 */
	    Printf ("Data limit set to " LUF " bytes (%.3f Mbytes), " LUF " blocks.\n",
			dip->di_data_limit,
			((double)(slarge_t)dip->di_data_limit / (double)MBYTE_SIZE),
			 (dip->di_data_limit / dsize));
#else /* !defined(_NT_SOURCE) */
	    Printf ("Data limit set to " LUF " bytes (%.3f Mbytes), " LUF " blocks.\n",
		dip->di_data_limit, ((double)dip->di_data_limit/(double)MBYTE_SIZE),
				     (dip->di_data_limit / dsize));
#endif /* !defined(__GNUC__) && ( defined(_NT_SOURCE) || defined(WIN32) ) */
	if (file_position > dip->di_data_limit) {
	    LogMsg (efp, logLevelCrit, 0,
		    "Please specify a file position < media capacity!\n");
	    exit (FATAL_ERROR);
	}
    }

    /*
     * Calculate a random I/O offset shift value for generating offsets
     * greather than 1TB.  This is necessary since the random number
     * generate returns a maximum of 31 bits.
     * Note: Shift value used for speed (avoid multiply).
     * See dtutil.c: do_random() for more details on usage!
     */
    dip->di_rshift = 8;		/* log2(512)-1 shift value. */
    /* Now adjust based on capacity: 1TB = 1, 2TB = 2, etc. */
    dip->di_rshift += howmany(dip->di_data_limit, TBYTE_SIZE);
    return (SUCCESS);
}
