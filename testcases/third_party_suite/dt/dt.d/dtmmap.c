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
 * Module:	dtmmap.c
 * Author:	Robin T. Miller
 * Date:	September 4, 1993
 *
 * Description:
 *	Functions to do memory mapped I/O for 'dt' program.
 *
 * Modification History:
 *
 * January 24th, 2001 by Robin Miller.
 *	Add support for variable I/O requests sizes.
 *
 * April 25th, 2000 by Robin Miller.
 *	Broke the MMAP write function badly during some cleanup (sorry!).
 *
 * July 29, 1999 by Robin Miller.
 *	Merge in changes made to compile on FreeBSD.
 *
 * July 22nd, 1999 by Robin Miller.
 *	Added support for IOT (DJ's) test pattern.
 * 
 * May 27, 1999 by Robin Miller.
 *	Added support for micro-second delays.
 *
 * January 9, 1998 by Robin.
 *	Don't initialize data buffer being written for "disable=compare"
 * which yields better performance.  Also correct a problem referencing
 * data due to compiler optimization (reference_data() use static volatile).
 * [ Changes requested by Marcus Barrow, thanks! ]
 *
 * November 19, 1995 by Robin Miller.
 *	Removed old Sun/OS #ifdef'ed out code, and added support for
 *	logical block data option (yes, valid for use w/data files).
 */
#if defined(MMAP)

#include "dt.h"
#include <limits.h>
#include <sys/mman.h>
#include <sys/stat.h>

#if !defined(MAP_FILE)
#  define MAP_FILE	0
#endif /* !defined(MAP_FILE) */

/*
 * Forward References:
 */
void reference_data (u_char *buffer, size_t count);

/*
 * Declare the memory mapped test functions.
 */
struct dtfuncs mmap_funcs = {
    /*	tf_open,		tf_close,		tf_initialize,	  */
	open_file,		close_file,		nofunc,
    /*  tf_start_test,		tf_end_test,				  */
	mmap_file,		nofunc,
    /*	tf_read_file,		tf_read_data,		tf_cancel_reads,  */
	read_file,		mmap_read_data,		nofunc,
    /*	tf_write_file,		tf_write_data,		tf_cancel_writes, */
	write_file,		mmap_write_data,	nofunc,
    /*	tf_flush_data,		tf_verify_data,		tf_reopen_file,   */
	mmap_flush,		verify_data,		mmap_reopen_file,
    /*	tf_startup,		tf_cleanup,		tf_validate_opts  */
	nofunc,			nofunc,			mmap_validate_opts
};

/************************************************************************
 *									*
 * mmap_file()	Memory map the input or output file.			*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
mmap_file (struct dinfo *dip)
{
	int fd = dip->di_fd;
	int status = SUCCESS;

	/*
	 * For memory mapped I/O, map the file to a buffer.
	 */
	if (dip->di_mode == READ_MODE) {
	    mmap_buffer = (u_char *) mmap ((char *) 0, data_limit,
		PROT_READ, (MAP_FILE | MAP_PRIVATE), fd, (off_t) 0);
	} else { /* Output file */
	    /*
	     * Set the output file to the specified limit before
	     * memory mapping the file.
	     */
	    if (ftruncate (fd, data_limit) < 0) {
		report_error ("ftruncate", TRUE);
		exit (exit_status);
	    }
	    mmap_buffer = (u_char *) mmap ((caddr_t) 0, data_limit,
	(PROT_READ | PROT_WRITE), (MAP_FILE | MAP_SHARED), fd, (off_t) 0);
	}
	if (mmap_buffer == (u_char *) -1) {
	    report_error ("mmap", TRUE);
	    exit (exit_status);
	}
	mmap_bufptr = mmap_buffer;

	/*
	 * File positioning options currently ignored... maybe later.
	 */

	return (status);
}

/************************************************************************
 *									*
 * mmap_flush()	Flush memory map file data to permanent storage.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
mmap_flush (struct dinfo *dip)
{
	int status = SUCCESS;

	/*
	 * Sync out modified pages & invalid the address range to
	 * force them to be obtained from the file system during the
	 * read pass.
	 */
	if (dip->di_mode == WRITE_MODE) {
	    if (msync ((caddr_t)mmap_buffer, dip->di_dbytes_written,
					 MS_INVALIDATE) == FAILURE) {
		report_error ("msync", TRUE);
		terminate (errno);
	    }
	}
	return (status);
}

/************************************************************************
 *									*
 * mmap_reopen_file() - Reopen memory mapped input or output file.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		oflags = The device/file open flags.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
mmap_reopen_file (struct dinfo *dip, int oflags)
{
    /*
     * For memory mapped files, remove the mappings before closing
     * the file.
     */
    if (mmap_flag) {
	if (munmap ((caddr_t)mmap_buffer, data_limit) == FAILURE) {
	    report_error ("munmap", TRUE);
	    terminate (errno);
	}
	mmap_bufptr = mmap_buffer = (u_char *) 0;
    }

    return (reopen_file (dip, oflags));
}

/************************************************************************
 *									*
 * mmap_validate_opts() - Validate Memory Mapped Test Options.		*
 *									*
 * Description:								*
 *	This function verifies the options specified for memory mapped	*
 * file testing are valid.						*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Return Value:							*
 *		Returns SUCESS / FAILURE = Valid / Invalid Options.	*
 *									*
 ************************************************************************/
int
mmap_validate_opts (struct dinfo *dip)
{
    int status = SUCCESS;

    /*
     * For memory mapped I/O, ensure the user specified a limit, and
     * that the block size is a multiple of the page size (a MUST!).
     */
    if (mmap_flag) {
	if (data_limit == INFINITY) {
	    Fprintf ("You must specify a data limit for memory mapped I/O.\n");
	    status = FAILURE;
	} else if (block_size % page_size) {
	    Fprintf (
	"Please specify a block size modulo of the page size (%d).\n", page_size);
	    status = FAILURE;
	} else if (aio_flag) {
	    Fprintf ("Cannot enable async I/O with memory mapped I/O.\n");
	    status = FAILURE;
	} else {
	    status = validate_opts (dip);
	}
    }
    return (status);
}

/************************************************************************
 *									*
 * mmap_read_data() - Read and optionally verify memory mapped data.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Ok/Error.			*
 *									*
 ************************************************************************/
int
mmap_read_data (struct dinfo *dip)
{
	ssize_t count;
	size_t bsize, dsize;
	int status = SUCCESS;
	u_int32 lba = lbdata_addr;
	struct dtfuncs *dtf = dip->di_funcs;

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

	    if (rdelay_count) {			/* Optional read delay.	*/
		mySleep (rdelay_count);
	    }

	    /*
	     * If data limit was specified, ensure we don't exceed it.
	     */
	    if ( (dip->di_fbytes_read + dsize) > data_limit) {
		bsize = (data_limit - dip->di_fbytes_read);
		if (debug_flag) {
		    Fprintf ("Reading partial record of %d bytes...\n", bsize);
		}
	    } else {
		bsize = dsize;
	    }

	    count = bsize;			/* Paged in by system.	*/

	    if ((io_mode == TEST_MODE) && compare_flag) {
		if (iot_pattern) {
		    lba = init_iotdata (count, lba, lbdata_size);
		}
	    }

	    /*
	     * Stop reading when end of file is reached.
	     */
	    if (count == (ssize_t) 0) {		/* Pseudo end of file. */
		if (debug_flag) {
		    Fprintf ("End of memory mapped file detected...\n");
		}
		end_of_file = TRUE;
		exit_status = END_OF_FILE;
		break;
	    } else {
		dip->di_dbytes_read += count;
		dip->di_fbytes_read += count;
	        if ((status = check_read (dip, count, bsize)) == FAILURE) {
		    break;
		}
	    }

	    if (count == dsize) {
		records_processed++;
	    } else {
		partial_records++;
	    }

	    /*
	     * Verify the data (unless disabled).
	     */
	    if (compare_flag) {
		status = (*dtf->tf_verify_data)(dip, mmap_bufptr, count, pattern, &lba);
	    } else {
		/*
		 * Must reference the data to get it paged in.
		 */
		reference_data (mmap_bufptr, count);
	    }
	    mmap_bufptr += count;

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

	    if ( (dip->di_fbytes_read >= data_limit) ||
		 (++dip->di_records_read >= record_limit) ) {
		break;
	    }

#ifdef notdef
	/*
	 * Can't do this right now... if it's not mapped via mmap(), you'll
	 * get a "Segmentation Fault" and core dump.  Need more logic...
	 */
	    if (step_offset) mmap_bufptr += step_offset;
#endif
	}
	return (status);
}

/************************************************************************
 *									*
 * reference_data() - Reference Data of Memory Mapped File.		*
 *									*
 * Description:								*
 *	This function simply references each data byte to force pages	*
 * to be mapped in by the system (memory mapped file I/O).		*
 *									*
 * Inputs:	buffer = Data buffer to reference.			*
 *		count = Number of bytes to reference.			*
 *									*
 * Return Value:							*
 *		Void.							*
 *									*
 ************************************************************************/
void
reference_data (u_char *buffer, size_t count)
{
	size_t i = (size_t) 0;
	u_char *bptr = buffer;
	static volatile u_char data;

	while (i++ < count) {
		data = *bptr++;
	}
}

/************************************************************************
 *									*
 * mman_write_data() - Write data to memory mapped output file.		*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Outputs:	Returns SUCCESS/FAILURE = Ok/Error.			*
 *									*
 ************************************************************************/
int
mmap_write_data (struct dinfo *dip)
{
	ssize_t count;
	size_t bsize, dsize;
	int status = SUCCESS;
	u_int32 lba = lbdata_addr;

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
	while ( (dip->di_fbytes_written < data_limit) &&
		(dip->di_records_written < record_limit) ) {

	    if (wdelay_count) {			/* Optional write delay	*/
		mySleep (wdelay_count);
	    }

	    /*
	     * If data limit was specified, ensure we don't exceed it.
	     */
	    if ( (dip->di_fbytes_written + dsize) > data_limit) {
		bsize = (data_limit - dip->di_fbytes_written);
		if (debug_flag) {
		    Fprintf ("Writing partial record of %d bytes...\n",
								bsize);
		}
	    } else {
		bsize = dsize;
	    }

	    count = bsize;
	    if ((io_mode == TEST_MODE) && compare_flag) {
	        if (iot_pattern) {
		    lba = init_iotdata(count, lba, lbdata_size);
		}
		fill_buffer (mmap_bufptr, count, pattern);
	    }

	    /*
	     * Initialize the logical block data (if enabled).
	     */
	    if (lbdata_flag && lbdata_size && !iot_pattern) {
		lba = init_lbdata (mmap_bufptr, count, lba, lbdata_size);
	    }

	    mmap_bufptr += count;
	    dip->di_dbytes_written += count;
	    dip->di_fbytes_written += count;

	    /*
	     * Stop writing when end of file is reached.
	     */
	    if (count == (ssize_t) 0) {		/* Pseudo end of file. */
		if (debug_flag) {
		    Fprintf ("End of memory mapped file reached...\n");
		}
		end_of_file = TRUE;
		exit_status = END_OF_FILE;
		break;
	    }

	    if ((status = check_write (dip, count, bsize)) == FAILURE) {
		break;
	    } else {
		if (count == dsize) {
		    records_processed++;
		} else {
		    partial_records++;
		}
	    }

	    /*
	     * For variable length records, adjust the next record size.
	     */
	    if (min_size) {
		dsize += incr_count;
		if (dsize > max_size) dsize = min_size;
	    }

	    ++dip->di_records_written;

#ifdef notdef
	/*
	 * Can't do this right now... if it's not mapped via mmap(), you'll
	 * get a "Segmentation Fault" and core dump.  Need more logic...
	 */
	    if (step_offset) mmap_bufptr += step_offset;
#endif
	}
	return (status);
}

#endif /* defined(MMAP) */
