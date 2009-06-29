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
 * Module:	dtwrite.c
 * Author:	Robin T. Miller
 *
 * Description:
 *	Write routines for generic data test program.
 */
#include "dt.h"
#if !defined(_QNX_SOURCE)
#  include <sys/file.h>
#endif /* !defined(_QNX_SOURCE) */

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
 * writing into that area (which is now deemed protected).
 *
 * November 17th, 2003 by Robin Miller.
 *	Breakup output to stdout or stderr, rather than writing
 * all output to stderr.  If output file is stdout ('-') or a log
 * file is specified, then all output reverts to stderr.
 *
 * September 27th, 2003 by Robin Miller.
 *      Added support for AIX.
 *
 * June 25th, 2001 by Robin Miller.
 *	Restructured code associated with Tru64 Unix EEI, so we obtain
 * the EEI status for all tape errors, not just EIO errors.
 *
 * February 24th, 2001 by Robin Miller.
 *	Add conditionalization for QNX RTP (Neutrino).
 *
 * January 26th, 2001 by Robin Miller.
 *	Added support for reverse writing.
 *
 * January 24th, 2001 by Robin Miller.
 *	Add support for variable I/O requests sizes.
 *
 * January 14th, 2001 by Robin Miller.
 *	Added support for multiple volumes option.
 *
 * December 30th, 2000 by Robin Miller.
 *	Make changes to build using MKS/NuTCracker product.
 *
 * March 28th, 2000 by Robin Miller.
 *	Modified calls to file position functions which now accept a
 * device information parameter.
 *
 * February 18th, 2000 by Robin Miller.
 *	Fix a problem where the records written value was not updated
 * when the data limit was reached first.
 *
 * January 22nd, 2000 by Robin Miller.
 *	Added support for Cygwin tape devices for Windows/NT.
 *
 * January 6th, 2000 by Robin Miller.
 *	Added support for multi-volume media.
 *
 * January 1st, 2000 by Robin Miller.
 *	Added read after write support.
 *
 * December 30th, 1999 by Robin Miller.
 *	Modify call to do_random() to pass the transfer size.
 *	Fix lbdata problem when using step option (wrong lba).
 *
 * July 22nd, 1999 by Robin Miller.
 *	Added support for IOT (DJ's) test pattern.
 * 
 * May 27, 1999 by Robin Miller.
 *	Added support for micro-second delays.
 *
 * March 1, 1999 by Robin Miller.
 *	For tapes when Debug is enabled, report the file number.
 *
 * January 18, 1999 by Robin Miller.
 *	Modified logic in check_write() of partial record writes, so
 * these warnings can be turned off by "disable=verbose".  This change
 * is also consistent with logic done in check_read().
 *
 * December 21, 1998 by Robin Miller.
 *	Add hooks to handle tape device resets (DUNIX specific).
 *
 * October 26, 1998 by Robin Miller.
 *	When random I/O and lbdata options are both enabled, use the
 * file offset seeked to as the starting lbdata address.
 *
 * January 9, 1998 by Robin.
 *	Don't initialize data buffer being written for "disable=compare"
 * which yields better performance.
 *
 * September 6, 1996 by Robin Miller.
 *	Modified write_record() to properly check write errors!.
 *
 * February 28, 1996 by Robin Miller.
 *	Added function for copying records to device or file.
 *	Modified logic so write errors honor users' error limit.
 *
 * February 23, 1996 by Robin Miller.
 *	Only report partial record warning for sequential I/O testing.
 *	Random I/O can position us towards the end of media often, and
 *	partial transfers are likely especially with large block sizes.
 *
 * July 15, 1995 by Robin Miller.
 *	Fix end of media error handling (ENOSPC), and cleanup code.
 *
 * September 17, 1993 by Robin Miller.
 *	Report record number on warning errors (for debug).
 *
 * September 4, 1993 by Robin Miller.
 *	Moved memory mapped I/O logic to seperate module.
 *
 * Septemeber 1, 1993 by Robin Miller.
 *	Add ability to write variable record sizes.
 *
 * August 31, 1993 by Robin Miller.
 *	Rotate starting data buffer address through sizeof(long).
 *
 * August 27, 1993 by Robin MIller.
 *	Added support for DEC OSF/1 POSIX Asynchronous I/O (AIO).
 *
 * August 18, 1992 by Robin Miller.
 *	If "step=" option was specified, then seek that many bytes
 *	before the next write request (for disks).
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
 *	If "skip=n" option is specified, then skip that many records
 *	before starting to write.  The "skip=n" option skips records
 *	by reading, while "seek=n" seeks past records (for disks).
 */

/************************************************************************
 *									*
 * write_data() - Write specified data to the output file.		*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Ok/Error.			*
 *									*
 ************************************************************************/
int
write_data (struct dinfo *dip)
{
    register ssize_t count;
    register size_t bsize, dsize;
    int status = SUCCESS;
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
	    (void)set_position(dip, (off_t)rdata_limit);
	}
	lba = get_lba(dip);
	dip->di_offset = get_position(dip);
    } else {
	lba = make_lbdata (dip, dip->di_offset);
    }

    /*
     * Now write the specifed number of records.
     */
    while ( (error_count < error_limit) &&
	    (dip->di_fbytes_written < data_limit) &&
	    (dip->di_records_written < record_limit) ) {

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
	if ( (dip->di_fbytes_written + dsize) > data_limit) {
	    bsize = (size_t)(data_limit - dip->di_fbytes_written);
	} else {
	    bsize = dsize;
	}

	if (io_dir == REVERSE) {
	    bsize = MIN((dip->di_offset-file_position), bsize);
	    dip->di_offset = set_position(dip, (off_t)(dip->di_offset - bsize));
	} else if (io_type == RANDOM_IO) {
	    dip->di_offset = do_random (dip, TRUE, bsize);
	}

        if (debug_flag && (bsize != dsize) && !variable_flag) {
            Printf ("Record #%lu, Writing a partial record of %lu bytes...\n",
                                    (dip->di_records_written + 1), bsize);
        }

	if (iot_pattern || lbdata_flag) {
	    lba = make_lbdata (dip, (off_t)(dip->di_volume_bytes + dip->di_offset));
	}

	/*
	 * If requested, rotate the data buffer through ROTATE_SIZE
	 * bytes to force various unaligned buffer accesses.
	 */
	if (rotate_flag) {
	    data_buffer = (base_buffer + (rotate_offset++ % ROTATE_SIZE));
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
	    lba = init_lbdata (data_buffer, bsize, lba, lbdata_size);
	}

	if (Debug_flag) {
	    u_int32 iolba = NO_LBA;
            off_t iopos = (off_t) 0;
	    if (dip->di_random_access) {
                iopos = get_position(dip);
		iolba = (iopos / dip->di_dsize);
	    } else if (lbdata_flag || iot_pattern) {
                iopos = (off_t)(dip->di_volume_bytes + dip->di_offset);
		iolba = make_lbdata (dip, iopos);
	    }
            report_record(dip, (dip->di_files_written + 1), (dip->di_records_written + 1),
				iolba, iopos, WRITE_MODE, data_buffer, bsize);
	}

	count = write_record (dip, data_buffer, bsize, dsize, &status);
	if (end_of_file) break;	/* Stop writing at end of file. */
	if ( (status == FAILURE) && (error_count >= error_limit) ) break;

	/*
	 * If we had a partial transfer, perhaps due to an error, adjust
	 * the logical block address in preparation for the next request.
	 */
	if (iot_pattern && ((size_t)count < bsize)) {
	    size_t resid = (bsize - count);
	    lba -= howmany(resid, lbdata_size);
	}

	if ( (count > (ssize_t) 0) && raw_flag) {
	    status = write_verify(dip, data_buffer, count, dsize, dip->di_offset);
	    if ( (status == FAILURE) && (error_count >= error_limit) ) break;
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

	dip->di_records_written++;
	dip->di_volume_records++;

	if (io_dir == FORWARD) {
	    dip->di_offset += count;	/* Maintain our own position too! */
	} else if ( (io_type == SEQUENTIAL_IO) &&
		    (dip->di_offset == (off_t) file_position) ) {
	    set_Eof(dip);
	    break;
	}

	if (step_offset) {
	    if (io_dir == FORWARD) {
		dip->di_offset = incr_position (dip, step_offset);
	    } else if ((dip->di_offset -= step_offset) <= (off_t) file_position) {
		set_Eof(dip);
		break;
	    }
	}
    }
    return (status);
}

/************************************************************************
 *									*
 * check_write() - Check status of last write operation.		*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		count = Number of bytes read.				*
 *		size  = Number of bytes expected.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE/WARNING = Ok/Error/Warning	*
 *									*
 ************************************************************************/
int
check_write (struct dinfo *dip, ssize_t count, size_t size)
{
    int status = SUCCESS;

    if ((size_t)count != size) {
	if (count == FAILURE) {
	    report_error ("write", FALSE);
	    ReportDeviceInfo (dip, 0, 0, (bool)(errno == EIO));
            ExecuteTrigger(dip, "write");
	} else {
	    /*
	     * For writes at end of file or writes at end of block
	     * devices, we'll write less than the requested count.
	     * In this case, we'll treat this as a warning since
	     * this is to be expected.
	     *
	     * NOTE:  The raw device should be used for disks.
	     */
	    if ( (debug_flag || verbose_flag || ((size_t)count > size)) &&
		 (io_mode == TEST_MODE) /*&& (io_type == SEQUENTIAL_IO)*/ ) {
		Printf(
	"WARNING: Record #%lu, attempted to write %lu bytes, wrote only %lu bytes.\n",
					(dip->di_records_written + 1), size, count);
	    }
	    if ((size_t)count < size) {	/* Partial write is a warning. */
		warning_errors++;
		return (WARNING);
	    }
	    ReportDeviceInfo (dip, count, 0, FALSE);
	}
	(void)RecordError();
	dip->di_write_errors++;
	status = FAILURE;
    }
    return (status);
}

/************************************************************************
 *									*
 * copy_record() - Copy record to device or file.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		buffer = The data buffer to write.			*
 *		bsize = The number of bytes to write.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE/WARNING = Ok/Error/Warning	*
 *									*
 ************************************************************************/
int
copy_record (	struct dinfo	*dip,
		u_char		*buffer,
		size_t		bsize )
{
    ssize_t count;
    int status;

    count = write_record (dip, buffer, bsize, bsize, &status);
    /* TODO: Get this into write_record() where it belongs! */
    if (count > (ssize_t) 0) dip->di_records_written++;
    return (status);
}

/************************************************************************
 *									*
 * write_record() - Write record to device or file.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		buffer = The data buffer to write.			*
 *		bsize = The number of bytes to write.			*
 *		dsize = The users' requested size.			*
 *		status = Pointer to status variable.			*
 *									*
 * Outputs:	status = SUCCESS/FAILURE/WARNING = Ok/Error/Warning	*
 *		Return value is number of bytes from write() request.	*
 *									*
 ************************************************************************/
ssize_t
write_record(
	struct dinfo	*dip,
	u_char		*buffer,
	size_t		bsize,
	size_t		dsize,
	int		*status )
{
    ssize_t count;

retry:
    *status = SUCCESS;
    count = write (dip->di_fd, buffer, bsize);

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

    if ( is_Eof (dip, count, status) ) {
	if (multi_flag) {
	    *status = HandleMultiVolume (dip);
	    dip->di_offset = (off_t) 0;
	    if (*status == SUCCESS) goto retry;
	}
    } else {
	if (count > (ssize_t) 0) {
	    dip->di_dbytes_written += count;
	    dip->di_fbytes_written += count;
	    dip->di_vbytes_written += count;
	    if ((size_t)count == dsize) {
		records_processed++;
	    } else {
		partial_records++;
	    }
	}
	*status = check_write (dip, count, bsize);
    }
    return (count);
}

/************************************************************************
 *									*
 * write_verify() - Verify the record just written.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		buffer = The data buffer written.			*
 *		bsize = The number of bytes written.			*
 *		dsize = The users' requested size.			*
 *		pos = The starting device/file position.		*
 *									*
 * Outputs:	status = SUCCESS/FAILURE/WARNING = Ok/Error/Warning	*
 *									*
 ************************************************************************/
int
write_verify(
	struct dinfo	*dip,
	u_char		*buffer,
	size_t		bsize,
	size_t		dsize,
	off_t		pos )
{
    u_char *vbuffer = verify_buffer;
    ssize_t count;
    u_int32 lba = 0;
    int status = SUCCESS;

    if (rdelay_count) {			/* Optional read delay.	*/
	mySleep (rdelay_count);
    }

    if (dip->di_dtype->dt_dtype == DT_TAPE) {
#if !defined(__NUTC__) && !defined(__QNXNTO__) && !defined(AIX)
	status = DoBackwardSpaceRecord(dip, 1);
	if (status) return (status);
#endif /* !defined(__NUTC__) && !defined(__QNXNTO__) && !defined(AIX) */
    } else { /* assume random access */
	off_t npos = set_position (dip, pos);
	if (npos != pos) {
	    Fprintf("ERROR: Wrong seek position, (npos " FUF " != pos)" FUF "!\n",
						npos, (pos - bsize));
	    return (FAILURE);
	}
    }
    if (iot_pattern || lbdata_flag) {
	lba = make_lbdata(dip, (off_t)(dip->di_volume_bytes + pos));
    }

    if (rotate_flag) {
	vbuffer = (verify_buffer + ((rotate_offset -1) % ROTATE_SIZE));
    }

    /*
     * If we'll be doing a data compare after the read, then
     * fill the data buffer with the inverted pattern to ensure
     * the buffer actually gets written into (driver debug mostly).
     */
    if ((io_mode == TEST_MODE) && compare_flag) {
	init_buffer (vbuffer, bsize, ~pattern);
	init_padbytes (vbuffer, bsize, ~pattern);
	if (iot_pattern) {
	    lba = init_iotdata (bsize, lba, lbdata_size);
	}
    }

    if (Debug_flag) {
	u_int32 iolba = NO_LBA;
        off_t iopos = (off_t) 0;
	if (dip->di_random_access) {
            iopos = get_position(dip);
	    iolba = (iopos / dip->di_dsize);
	} else if (lbdata_flag || iot_pattern) {
            iopos = (off_t)(dip->di_volume_bytes + pos);
	    iolba = make_lbdata (dip, iopos);
	}
	report_record(dip, (dip->di_files_read + 1), (dip->di_records_read + 1),
			iolba, iopos, READ_MODE, vbuffer, bsize);
    }

    count = read_record (dip, vbuffer, bsize, dsize, &status);
    if (end_of_file) {
	report_error ("read", FALSE);
	ReportDeviceInfo (dip, 0, 0, FALSE);
	(void)RecordError();
	if (dip->di_dtype->dt_dtype != DT_TAPE) {
	    (void) set_position (dip, pos);
	}
	return (FAILURE);
    }

    /*
     * Verify the data (unless disabled).
     */
    if ( (status != FAILURE) && compare_flag && (io_mode == TEST_MODE) ) {
	ssize_t vsize = count;
	if (iot_pattern || lbdata_flag) {
	    status = verify_lbdata(dip, buffer, vbuffer, vsize, &lba);
	}
	if (status == SUCCESS) {
	    status = verify_buffers(dip, buffer, vbuffer, vsize);
	}
	/*
	 * Verify the pad bytes (if enabled).
	 */
	if ( (status == SUCCESS) && pad_check) {
	    (void) verify_padbytes (dip, vbuffer, vsize, ~pattern, bsize);
	}
    }

    /*
     * We expect to read as much as we wrote, or else we've got a problem!
     */
    if ((size_t)count < bsize) {
	/* check_read() reports info regarding the short record read. */
	ReportDeviceInfo (dip, count, 0, FALSE);
	(void)RecordError();
	status = FAILURE;
	if (dip->di_dtype->dt_dtype != DT_TAPE) {
	    (void) set_position (dip, pos);
	}
    }
    dip->di_records_read++;
    return (status);
}
