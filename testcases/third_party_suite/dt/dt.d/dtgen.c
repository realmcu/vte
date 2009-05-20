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
 * Module: dtgen.c
 * Author: Robin T. Miller
 * Date: September 8, 1993
 *
 * Description:
 * Generic test functions for the 'dt' program.
 *
 * Modification History:
 *
 * February 24th, 2001 by Robin Miller.
 * Add conditionalization for QNX RTP (Neutrino).
 *
 * February 6th, 2001 by Robin Miller.
 * Handle multiple slices to the same output file, by ensuring the
 * file disposition is set to "keep".  Otherwise, the first completing
 * process deletes the in-use file, which is generally not desriable.
 *
 * avoid unexpected failures.  This *is* permitted for input files.
 *
 * January 29th, 2001 by Robin Miller.
 * Added validity checks to ensure transfer counts are modulo the
 * device size, to avoid unexpected failures.
 *
 * January 25th, 2001 by Robin Miller.
 * Added validate options function for checking options after the
 * device information has been setup (i.e., device size, etc).
 *
 * January 15th, 2001 by Robin Miller.
 * Add error checks to seek_file() and skip_file() calls.
 * If reopen failures occur, don't terminate, return the error.
 *
 * January 14th, 2001 by Robin Miller.
 * Added support for multiple volumes option.
 *
 * January 12th, 2001 by Robin Miller.
 * When reading multiple tape files, issue a forward space file
 * mark command between tape files.  For systems which don't support
 * tape operations, we'll use continue to use the read_eof() method.
 *
 * December 30th, 2000 by Robin Miller.
 * Make changes to build using MKS/NuTCracker product.
 *
 * November 8th, 2000 by Robin Miller.
 * Add check for ignoring device close failures.  In this case,
 * the error is displayed as a warning instead of a failure.
 *
 * July 14th, 2000 by Robin Miller.
 * In flush_file(), control sync'ing of data using fsync flag.
 *
 * May 8th, 2000 by Robin Miller.
 * Honor the di_closing flag, to avoid a race condition with the
 * close function being called (again) while still closing, from the
 * terminate() routine called by the runtime= alarm, or signals.
 * [ damn, fixed this in 1994, broke it during recent re-write :-) ]
 *
 * April 18th, 2000 by Robin Miller.
 * Added messages for open/close failures doing EEI reset tests.
 * Modified calls to report_error() to ensure error count bumped.
 *
 * March 28th, 2000 by Robin Miller.
 * Modified calls to file position functions which now accept a
 * device information parameter.
 *
 * February 19th, 2000 by Robin Miller.
 * When EEI resets are enabled, retry open/reopen failures attempts.
 * Also, if we're processing EEI resets, don't fail on close().
 *
 * February 17th, 2000 by Robin Miller.
 * Adding better support for multi-volume tape testing.
 *
 * January 22nd, 2000 by Robin Miller.
 * Added support for Cygwin tape devices for Windows/NT.
 *
 * August 2, 1999 by Robin Miller.
 * Don't do fsync() to tape devices to avoid errors on Linux.
 *
 * May 27, 1999 by Robin Miller.
 * Adding support for micro-second delays.
 *
 * February 26, 1999 by Robin Miller.
 * Adjust the file count AFTER writing a file mark, so our tape
 * repositioning code will work if the Reset occurs during the WFM.
 *
 * December 21, 1998 by Robin Miller.
 * Updates necessary to match my tape API changes.
 * Save device/file open flags in device info struct.
 *
 * December 17, 1998 by Robin Miller.
 * For DUNIX tape devices, clear persistent EEI data after opens.
 *
 * April 28, 1998 by Robin Miller.
 * For WIN32/NT, or in O_BINARY into open flags to force binary
 * mode (the default is text mode).
 *
 * April 7, 1998 by Robin Miller.
 * Fix problem with attempting reopen of stdin/stdout.
 *
 * March 27, 1997 by Ali Eghlima.
 *      Added cluster support, so more than one process or one system
 *      can access a resource. dlm are being used to synchronize all
 *      access.
 *
 * December 9, 1995 by Robin Miller.
 * Allow writing tape file marks on QNX Operating System.
 *
 * September 23, 1994 by Robin Miller.
 * Make changes necessary to build on QNX 4.21 release.  This
 * required changing O_DSYNCH to O_DSYNC, and O_FSYNCH to O_SYNC.
 *
 * May 19, 1994 by Robin Miller.
 * Altered logic in close_file() & reopen_file() functions to set
 * the file descriptor to NoFd prior to calling close() to actually
 * perform the close operation.  This avoids a problem where if the
 * user types Ctrl/C while the close is in progress, the signal handler
 * gets invoked immediately after the system call, the fd doesn't
 * get marked closed, and our termination code then attempts to
 * close an already closed file descriptor and reports the error:
 *  "dt: 'close' - Bad file number"
 */
#include "dt.h"
#if defined(_QNX_SOURCE)
#  include <fcntl.h>
#else /* !defined(_QNX_SOURCE) */
#  include <sys/file.h>
#endif /* defined(_QNX_SOURCE) */

/*
 * Declare the generic (default) test functions.
 */
struct dtfuncs generic_funcs = {
    /* tf_open,  tf_close,  tf_initialize,   */
 open_file,  close_file,  initialize,
    /*  tf_start_test,  tf_end_test,      */
 init_file,  nofunc,
    /* tf_read_file,  tf_read_data,  tf_cancel_reads,  */
 read_file,  read_data,  nofunc,
    /* tf_write_file,  tf_write_data,  tf_cancel_writes, */
 write_file,  write_data,  nofunc,
    /* tf_flush_data,  tf_verify_data,  tf_reopen_file,   */
 flush_file,  verify_data,  reopen_file,
    /* tf_startup,  tf_cleanup,  tf_validate_opts  */
 nofunc,   nofunc,   validate_opts
};

#if defined(MUNSA)
dlm_status_t gen_stat;
#endif /* defined(MUNSA) */

/************************************************************************
 *         *
 * open_file() - Open an input/output file for read/write.  *
 *         *
 * Description:        *
 * This function does the default (generic) open processing. *
 *         *
 * Inputs: dip = The device information pointer.   *
 *  oflags = The device/file open flags.   *
 *         *
 * Return Value:       *
 *  Returns 0 / -1 = SUCCESS / FAILURE.   *
 *         *
 ************************************************************************/
int
open_file (struct dinfo *dip, int oflags)
{
    char *file = dip->di_dname;
    int status = SUCCESS;
#if defined(EEI)
    int open_retries = EEI_OPEN_RETRIES;
#endif

    if ( (strlen(file) == 1) && (*file == '-') ) {
 if (debug_flag) {
     Fprintf ("Dup'ing standard %s...\n",
  (dip->di_ftype == INPUT_FILE) ? "input" : "output");
 }
 if (dip->di_ftype == INPUT_FILE) {
     stdin_flag = TRUE;
     dip->di_fd = dup (fileno(stdin));
 } else {
     stdout_flag = TRUE;
     dip->di_fd = dup (fileno(stdout));
     verify_flag = FALSE;
 }
 if (dip->di_fd < 0) {
     Fprintf ("dup -> ");
     report_error (file, TRUE);
     status = FAILURE;
 }
    } else {
#if defined(__WIN32__)
 oflags |= O_BINARY;
#endif /* defined(__WIN32__) */
#if defined(EEI)
retry:
#endif
 if (debug_flag) {
     Fprintf (
  "Attempting to open %s file '%s', open flags = %#o (%#x)...\n",
  (dip->di_ftype == INPUT_FILE) ? "input" : "output",
       file, oflags, oflags);
 }
 dip->di_oflags = oflags;
 if (dip->di_ftype == INPUT_FILE) {
     dip->di_fd = open (file, oflags);
 } else {
     dip->di_fd = open (file, oflags, 0666);
 }
 if (dip->di_fd < 0) {
#if defined(EEI)
     if ( (errno == EIO) && eei_resets &&
   (dip->di_dtype &&
    (dip->di_dtype->dt_dtype == DT_TAPE)) ) {
  if (--open_retries) {
      if (verbose_flag) {
   Fprintf("Warning, ignoring open error and retrying...\n");
      }
      goto retry;
  }
     }
#endif /* defined(EEI) */
     Fprintf ("open -> ");
     report_error (file, TRUE);
     status = FAILURE;
 }
    }

    end_of_file = FALSE;
    dip->di_end_of_file = FALSE;
    dip->di_end_of_media = FALSE;
    dip->di_end_of_logical = FALSE;

    if ( (status != FAILURE) && debug_flag) {
 Fprintf ("%s file '%s' successfully opened, fd = %d\n",
  (dip->di_ftype == INPUT_FILE) ? "Input" : "Output",
      file, dip->di_fd);
    }
    return (status);
}

/************************************************************************
 *         *
 * close_file() - Close an open file descriptor.   *
 *         *
 * Description:        *
 * This function does the default (generic) close processing. *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Return Value:       *
 *  Returns 0 / -1 = SUCCESS / FAILURE.   *
 *         *
 ************************************************************************/
int
close_file (struct dinfo *dip)
{
    char *file = dip->di_dname;
    int status = SUCCESS;

    if (dip->di_closing || (dip->di_fd == NoFd)) {
 return (status);  /* Closing or not open. */
    }

    dip->di_closing = TRUE;
    if (cdelay_count) {   /* Optional close delay */
 mySleep (cdelay_count);
    }
    if (debug_flag) {
 Fprintf ("Closing file '%s', fd = %d...\n", file, dip->di_fd);
    }
    if ((status = close (dip->di_fd)) == FAILURE) {
 if (cerrors_flag) {
     report_error ("close", TRUE);
 } else if (verbose_flag) {
     status = SUCCESS;
     Fprintf("Warning, close failed, errno = %d - %s\n",
     errno, strerror(errno));
     Fprintf("Warning, ignoring close failure and continuing...\n");
 }
#if defined(EEI)
 if ( (errno == EIO) && eei_resets &&
      (dip->di_dtype->dt_dtype == DT_TAPE) ) {
     if (dip->di_proc_eei) {
  if (verbose_flag) {
      Fprintf("Warning, ignoring close failure and continuing...\n");
  }
  error_count--;
  status = SUCCESS;
     }
     /* No retries, we're already closed! */
 }
#endif /* defined(EEI) */
    }
    dip->di_fd = NoFd;
    dip->di_closing = FALSE;
    return (status);
}

/************************************************************************
 *         *
 * reopen_file() - Close and reopen file descriptor.   *
 *         *
 * Inputs: dip = The device information pointer.   *
 *  oflags = The device/file open flags.   *
 *         *
 * Return Value:       *
 *  Returns 0 / -1 = SUCCESS / FAILURE.   *
 *         *
 ************************************************************************/
int
reopen_file (struct dinfo *dip, int oflags)
{
    struct dtfuncs *dtf = dip->di_funcs;
    char *file = dip->di_dname;
    int status = SUCCESS;
#if defined(EEI)
    int open_retries = EEI_OPEN_RETRIES;
#endif

    /*
     * For stdin or stdout, do not attempt close/open.
     */
    if ( (strlen(file) == 1) && (*file == '-') ) return (status);

    /*
     * For tape, we must close & reopen to get to BOT,
     * an lseek() won't do the trick.
     */
    if (dip->di_fd != NoFd) {  /* If not already closed... */
 status = (*dtf->tf_close)(dip);
    }
    if (edelay_count) {   /* Optional end delay. */
 mySleep (edelay_count);
    }

    end_of_file = FALSE;
    dip->di_end_of_file = FALSE;
    dip->di_end_of_media = FALSE;
    dip->di_end_of_logical = FALSE;

#if defined(__WIN32__)
    oflags |= O_BINARY;
#endif /* defined(__WIN32__) */
#if defined(EEI)
retry:
#endif
    if (debug_flag) {
 Fprintf ("Attempting to reopen file '%s', open flags = %#o (%#x)...\n",
       file, oflags, oflags);
    }

    dip->di_oflags = oflags;
    if ( (dip->di_fd = open (file, oflags)) == FAILURE) {
#if defined(EEI)
 if ( (dip->di_dtype->dt_dtype == DT_TAPE) &&
      (errno == EIO) && eei_resets) {
     if (--open_retries) {
  if (verbose_flag) {
      Fprintf("Warning, ignoring open error and retrying...\n");
  }
  goto retry;
     }
 }
#endif /* defined(EEI) */
 report_error ("reopen", TRUE);
 return (FAILURE);
    }

    if (debug_flag) {
 Fprintf ("File '%s' successfully reopened, fd = %d\n",
      file, dip->di_fd);
    }
#if defined(EEI)
    if ( (status == SUCCESS) && eei_flag &&
  (dip->di_dtype->dt_dtype == DT_TAPE) ) {
 clear_eei_status(dip->di_fd, FALSE);
    }
#endif /* defined(EEI) */
    return (status);
}

/************************************************************************
 *         *
 * initialize() - Do the default program initialization.  *
 *         *
 * Description:        *
 * This function does the default (generic) test initialization. *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Outputs: Always SUCCESS right now.    *
 *         *
 ************************************************************************/
int
initialize (struct dinfo *dip)
{
    if (!data_buffer) {
 base_buffer = data_buffer = myalloc (data_size, align_offset);
    }
    return (SUCCESS);
}

/************************************************************************
 *         *
 * init_file() - Initial file processing.    *
 *         *
 * Description:        *
 * This function is used to process options before starting tests. *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Outputs: Always SUCCESS right now.    *
 *         *
 ************************************************************************/
int
init_file (struct dinfo *dip)
{
    int status = SUCCESS;
    int fd = dip->di_fd;

    /*
     * If the lba option is specified, and we're a disk device,
     * then setup a file position to be seek'ed to below.
     */
    if ( lbdata_addr && !user_position &&
  ((dip->di_dtype->dt_dtype == DT_DISK) ||
   (dip->di_dtype->dt_dtype == DT_BLOCK)) ) {
 file_position = make_position(dip, lbdata_addr);
 if ( (io_type == RANDOM_IO) && (rdata_limit <= file_position) ) {
     Fprintf ("Please specify a random data limit > lba file position!\n");
     exit (FATAL_ERROR);
 }
    }

    /*
     * Seek to specified offset (if requested).
     */
    if (file_position) {
 last_position = set_position (dip, file_position);
    }

    /*
     * Seek to specified record (if requested).
     */
    if (seek_count) {
#if defined(_BSD)
 last_position = seek_file (fd, seek_count, block_size, L_INCR);
#else /* !defined(_BSD) */
 last_position = seek_file (fd, seek_count, block_size, SEEK_CUR);
#endif /* defined(_BSD) */
 if (last_position == (off_t) FAILURE) return (FAILURE);
 show_position (dip, last_position);
    }

    /*
     * Skip over input record(s) (if requested).
     */
    if (skip_count) {
 status = skip_records (dip, skip_count, data_buffer, block_size);
 if (debug_flag && (status != FAILURE)) {
     Fprintf ("Successfully skipped %d records.\n", skip_count);
 }
    }
    return (status);
}

/************************************************************************
 *         *
 * flush_file() - Flush file data to disk.    *
 *         *
 * Description:        *
 * This function is used to flush the file data to disk after the *
 * write pass of each test.      *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Return Value:       *
 *  Returns SUCCESS / FAILURE = Ok / Sync Failed.  *
 *         *
 ************************************************************************/
int
flush_file (struct dinfo *dip)
{
    int status = SUCCESS;
    int fd = dip->di_fd;

    /*
     * Ensure data is sync'ed to disk file.
     */
    if ( fsync_flag ) {
 if (debug_flag) {
     Fprintf ("Flushing data to file '%s'...\n", dip->di_dname);
 }
#if defined(_QNX_SOURCE)
 if ((status = fcntl (fd, F_SETFL, O_DSYNC)) < 0) {
     report_error("F_SETFL", TRUE);
 }
#else /* !defined(_QNX_SOURCE) */
 if ((status = fsync (fd)) < 0) { /* Force data to disk. */
     report_error ("fsync", TRUE);
 }
#endif /* !defined(_QNX_SOURCE) */
    }
    return (status);
}

/************************************************************************
 *         *
 * read_file - Read and optionally verify data in the test file. *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Return Value:       *
 *  Returns SUCCESS/FAILURE = Ok / Error.   *
 *         *
 ************************************************************************/
int
read_file (struct dinfo *dip)
{
    int status;
    struct dtfuncs *dtf = dip->di_funcs;

#if defined(MUNSA)
    if (munsa_flag) {
 if (debug_flag) {
     Fprintf ("converting to dlm NL-> %d \n",
     input_munsa_lock_type);
 }
 gen_stat = dlm_cvt(&lkid, input_munsa_lock_type,
     NULL, 0, 0, 0, NULL, 0);
 if (gen_stat !=  DLM_SUCCESS) {
     Fprintf ("dlm_cvt failed\n");
     dlm_error(&lkid, gen_stat);      /* exit with FATAL ERROR */
 }

 if (debug_flag) {
     Fprintf ("done, converting to dlm NL-> %d \n",
     input_munsa_lock_type);
 }
    } /* end if(munsa_flag) .... */
#endif /* defined(MUNSA) */

    dip->di_offset = make_offset(dip, lbdata_addr);

    /*
     * Loop reading/comparing data until we've detected end of file
     * or we've reached the data limit or record limit.
     */
    do {     /* Read/compare data. */
 dip->di_fbytes_read =(v_large) 0;
 dip->di_records_read = (v_large) 0;
read_some_more:
 if ((status = (*dtf->tf_read_data)(dip)) == FAILURE) break;

 if (volumes_flag && (multi_volume >= volume_limit) &&
    (dip->di_volume_records >= volume_records)) {
     break;
 }
 /*
  * Handle reading multiple tapes and multiple files.
  */
 if (end_of_file && multi_flag &&
     (dip->di_dtype->dt_dtype == DT_TAPE) &&
     ((dip->di_records_read != record_limit) &&
      (dip->di_fbytes_read != data_limit)) ) {
     /* Check for logical end of tape. */
     (void) (*dtf->tf_cancel_reads)(dip);
     if ((status = read_eom(dip)) != SUCCESS) break;
     if ( !end_of_file ) goto read_some_more;
 }
 if (end_of_file) dip->di_files_read++;
 if ( (dip->di_dtype->dt_dtype == DT_TAPE) &&
      (file_limit && (dip->di_files_read < file_limit)) ) {
     /*
      * Normally, we handle EOF conditions on the fly, but when lbdata
      * or the IOT pattern is enabled, we must cancel outstanding I/O's
      * so that aio_offset (for LBA) is accurate on subsequent files.
      */
     if (lbdata_flag || iot_pattern) {
  (void) (*dtf->tf_cancel_reads)(dip);
     }
     /*
      * An exact record or data limit keeps us short of file marks,
      * so we read and check for an expected end of file here.
      */
     if (!end_of_file) {
#if defined(__NUTC__) || defined(__QNXNTO__)
  if ((status = read_eof(dip)) != SUCCESS) break;
#else /* !defined(__NUTC__) && !defined(__QNXNTO__) */
  status = DoForwardSpaceFile (dip, (daddr_t) 1);
  if (status != SUCCESS) break;
#endif /* defined(__NUTC__) || defined(__QNXNTO__) */
  if (++dip->di_files_read == file_limit) break;
     }
     dip->di_fbytes_read =(v_large) 0;
     dip->di_records_read = (v_large) 0;
     end_of_file = FALSE;
     continue;
 }
    } while ( !end_of_file && (error_count < error_limit) &&
       (dip->di_records_read < record_limit) &&
       (dip->di_fbytes_read < data_limit) );

    /*
     * We cancel the reads here incase multiple files were being
     * read, so reads continue while we process each file mark.
     */
    (void) (*dtf->tf_cancel_reads)(dip);
    return (status);
}

/************************************************************************
 *         *
 * write_file() - Write data to the test file/device.   *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Return Value:       *
 *  Returns SUCCESS/FAILURE = Ok/Error.   *
 *         *
 ************************************************************************/
int
write_file (struct dinfo *dip)
{
    int status;
    struct dtfuncs *dtf = dip->di_funcs;

#if defined(MUNSA)
    if (munsa_flag) {
 if (debug_flag) {
     Fprintf ("converting to dlm NL-> %d .\n",
     output_munsa_lock_type);
 }

 gen_stat = dlm_cvt(&lkid, output_munsa_lock_type,
     NULL, 0, 0, 0, NULL, 0);
 if (gen_stat !=  DLM_SUCCESS) {
     Fprintf ("dlm_cvt failed\n");
     dlm_error(&lkid, gen_stat);  /* exit with FATAL ERROR */
 }
 if (debug_flag) {
     Fprintf ("done, converting to dlm NL-> %d .\n",
     output_munsa_lock_type);
 }
    }  /*  end if(munsa_flag)...  */
#endif /* defined(MUNSA) */

    dip->di_offset = make_offset(dip, lbdata_addr);

    /*
     * Loop writing data until end of media or data limit reached.
     */
    do {     /* Write data pattern. */
 if (raw_flag) {
     dip->di_fbytes_read =(v_large) 0;
     dip->di_records_read = (v_large) 0;
 }
 dip->di_fbytes_written =(v_large) 0;
 dip->di_records_written = (v_large) 0;
 if ((status = (*dtf->tf_write_data)(dip)) == FAILURE) break;

 if (volumes_flag && (multi_volume >= volume_limit) &&
    (dip->di_volume_records >= volume_records)) {
     break;
 }
 /*
  * Handle writing multiple tape files.
  */
 if ( (dip->di_dtype->dt_dtype == DT_TAPE) &&
      (file_limit && (dip->di_files_written < file_limit)) ) {
     /*
      * For tapes, write a file mark for all but last file.
      * The last file mark(s) are written when closing tape.
      */
#if !defined(__NUTC__) && !defined(__QNXNTO__)
     if ((dip->di_files_written + 1) < file_limit) {
  status = DoWriteFileMark (dip, (daddr_t) 1);
  if (status != SUCCESS) break;
     }
#endif /* !defined(__NUTC__) && !defined(__QNXNTO__) */
     if (++dip->di_files_written < file_limit) {
  dip->di_fbytes_written =(v_large) 0;
  dip->di_records_written = (v_large) 0;
  continue;   /* Write the next file. */
     }
 }
    } while ( !end_of_file && (error_count < error_limit) &&
       (dip->di_records_written < record_limit) &&
       (dip->di_fbytes_written < data_limit) );

    return (status);
}

/************************************************************************
 *         *
 * validate_opts() - Generic Validate Option Test Criteria.  *
 *         *
 * Description:        *
 * This function verifies the options specified are valid for the *
 * test criteria selected.      *
 *         *
 * Inputs: dip = The device information pointer.   *
 *         *
 * Return Value:       *
 *  Returns SUCESS / FAILURE = Valid / Invalid Options. *
 *         *
 ************************************************************************/
int
validate_opts (struct dinfo *dip)
{
  if (bypass_flag) return (SUCCESS);

  if (dip->di_fd == NoFd) {
    /*
     * Validation checks *before* the device is open.
     */
    if (min_size) {
 size_t value;
 char *emsg = NULL;
 if (dip->di_dsize > min_size) {
     value = min_size; emsg = "min size";
 } else if (dip->di_dsize > max_size) {
     value = max_size; emsg = "max size";
 }
 if (emsg) {
     Fprintf(
    "Please specify %s (%u) greater than device size %u of bytes.\n",
     emsg, value, dip->di_dsize);
     return (FAILURE);
 }
    }
  } else {
    /*
     * Validation checks *after* the device is open.
     */
    if ( (dip->di_dtype->dt_dtype == DT_REGULAR) &&
  (dip->di_ftype == OUTPUT_FILE)          &&
  num_slices && (dispose_mode == DELETE_FILE) ) {
      dispose_mode = KEEP_FILE;
      if (verbose_flag) {
 Fprintf("Warning: Multiple slices to same file, setting dispose=keep!\n");
      }
    }
    if ( (io_dir == REVERSE) || (io_type == RANDOM_IO) ) {
      if ( !dip->di_random_access ) {
 Fprintf(
    "Random I/O or reverse direction, is only valid for random access device!\n");
 return (FAILURE);
      }
      if ( (dip->di_dtype->dt_dtype == DT_REGULAR) && !user_capacity ) {
 Fprintf(
    "Please specify a data limit, record count, or capacity for random I/O.\n");
 return (FAILURE);
      }
    }
    if (dip->di_random_access) {
 size_t value;
 char *emsg = NULL;
 if (block_size % dip->di_dsize) {
     value = block_size; emsg = "block size";
 } else if (min_size && (min_size % dip->di_dsize)) {
     value = min_size; emsg = "min size";
 } else if (max_size && (max_size % dip->di_dsize)) {
     value = max_size; emsg = "max size";
 } else if (incr_count && (incr_count % dip->di_dsize)) {
     value = incr_count; emsg = "incr count";
 }
 if (emsg) {
     Fprintf(
     "Please specify a %s (%u) modulo the device size of %u bytes!\n",
     emsg, value, dip->di_dsize);
     return (FAILURE);
 }
    }
  }
  return (SUCCESS);
}
