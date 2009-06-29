#if defined(EEI)
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
 * Module:	dteei.c
 * Author:	Robin T. Miller
 * Date:	September 13, 1997
 *
 * Description:
 *	This file contains functions for support of DEC EEI interface.
 *
 * Modification History:
 *
 * November 17th, 2003 by Robin Miller.
 *	Breakup output to stdout or stderr, rather than writing
 * all output to stderr.  If output file is stdout ('-') or a log
 * file is specified, then all output reverts to stderr.
 *
 * April 21st, 2000 by Robin Miller.
 *	Converted tabs to spaces in all messages, so formatting is not
 * screwed up when messages are prefixed by a timestamp or process ID.
 *
 * April 10th, 2000 by Robin Miller.
 *	For Wave4, added logic to handle EEI_DEVPATH_FAILURE, so we try
 * to re-open the tape device to 1) find another path (multi-pathing) or
 * 2) force DRD failover (find another server for the tape).
 *
 * February 19th, 2000 by Robin Miller.
 *	While processing a reset condition, if another operation errors
 * with no EEI status (EEI_NO_STATUS), then retry that operation.  This
 * may be a driver related problem, but we wish to recover from this.
 * If not compiled for Tru64 clusters, retry target selection timeouts.
 *
 * February 14th, 2000 by Robin Miller.
 *	Merge Mike Gilmore's changes:
 *	- change the data format from DEVIOCGET to DEVGETINFO.
 *
 * March 25, 1999 by Robin Miller.
 *	Fix problem with my fileno going negative (bad check).
 *
 * February 25, 1999 by Robin Miller.
 *	Add support for multiple reset conditions.
 *
 * December 21, 1998 by Robin Miller.
 *	Add HandleTapeResets() to reposition the tape after resets.
 *
 * March 11, 1998 by George Bittner.
 *	Remove unnecessary include files for building in steelos.
 *
 */

#include "dt.h"

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/ioctl_compat.h>
#include <sys/mtio.h>
#include <io/common/iotypes.h>
#include <io/common/devio.h>
#include <io/common/devgetinfo.h>
#include <io/cam/cam.h>
#include <io/cam/scsi_all.h>
#include <io/cam/scsi_status.h>
#if !defined(STEELOS)
#  include <io/common/kds.h>
#  include <io/cam/camdb.h>
#  include <io/cam/cam_tape.h>
#endif /* !defined(STEELOS) */

/*
 * These three defines are, unfortunately, not defined in the CAM code
 * because they are always used only as bit fields in the entire sense
 * data structure.  All we have available is the sense key itself.
 */
#define	ILI		0x20
#define	EOM		0x40
#define	FILE_MARK	0x80

#define DNL		0			/* Disable newline.	*/
#define PNL		1			/* Print newline.	*/

/*
 * Declare CAM Debugging Definitions:
 */
#define CDBG_BRIEF	0		/* Brief message text. */
#define CDBG_FULL	1		/* Full message text. */

/*
 * Additional Sense Code Table Entry Format:
 */
struct sense_entry {
	u_char	sense_code;		/* Additional sense code.	*/
	u_char	sense_qualifier;	/* Sense code qualifier.	*/
	char	*sense_message;		/* Error message text.		*/
};

static char *not_valid_str	= "<Not Valid>";

long	mt_blkno;			/* Starting tape block number.	*/
long	mt_fileno;			/* Starting tape file number.	*/

/*
 * Forward References:
 */
bool is_ResetCondition(struct dinfo *dip, DEV_EEI_STATUS *eei);
bool is_StatusRetryable(struct dinfo *dip, DEV_EEI_STATUS *eei);
void print_eei(DEV_EEI_STATUS *eei);
void print_devio(int fd);
void print_erreg(short erreg);
void print_stat(long stat);
void print_category(long stat, device_info_t *devinfop);

void PrintNumeric(char *field_str, u_long numeric_value, int nl_flag);
void PrintDecimal(char *field_str, u_long numeric_value, int nl_flag);
void PrintHex(char *field_str, u_long numeric_value, int nl_flag);
void PrintAscii(char *field_str, char *ascii_str, int nl_flag);
void PrintFields(u_char *bptr, int length);

char *cdbg_SenseKeyTable[];
char *cdbg_CamStatus(u_char cam_status, int report_format);
char *cdbg_ScsiStatus(u_char scsi_status, int report_format);
char *cdbg_SenseMessage(struct all_req_sns_data *sdp);
char *cdbg_EEIStatus(u_int eei_status, int report_format);

/*
 * HandleTapeResets() - Handle Tape Reset Conditions.
 *
 * Description:
 * 	This function is called to do reset processing for tape devices.
 * The idea is to reposition the tape to the file/record position prior
 * to the reset, then retry the operation.
 *
 * Inputs:	dip = The device information pointer.
 *
 * Return Value:
 *		TRUE/FALSE = Retry Operation/Error the Request.
 *
 */
bool
HandleTapeResets(struct dinfo *dip)
{
	int status;
	int fd = dip->di_fd;
	struct mtget mtget, *mt;
	int saved_errno;
	u_long fileno, files, records;

	if (dip->di_proc_eei) {
	    mt = &mtget;
	    memset(mt, '\0', sizeof(*mt));
	} else {
	    mt = dip->di_mt;
	}
	saved_errno = errno;
	status = get_eei_status(fd, mt);
	if (status != ESUCCESS) {
	    errno = saved_errno;
	    return (FALSE);
	}

	if (dip->di_mode == READ_MODE) {
	    files = dip->di_files_read;
	    records = dip->di_records_read;
	} else {
	    files = dip->di_files_written;
	    records = dip->di_records_written;
	}
	if (dip->di_proc_eei) {
	    if (debug_flag) print_mtstatus(fd, mt, FALSE);
	    /*
	     * See if we should retry this EEI status.
	     */
	    if ( is_StatusRetryable(dip, &mt->eei) ) {
		if (dip->di_eei_retries-- == 0) {
		    dip->di_eei_retries = EEI_RETRIES;
		    return (FALSE);		/* Exhausted retries. */
		}
		if (verbose_flag) {
		    Printf("Retry %d after %s status...\n",
			    (EEI_RETRIES - dip->di_eei_retries),
			    cdbg_EEIStatus(mt->eei.status, CDBG_BRIEF));
		}
		(void)sleep(dip->di_eei_sleep);
		return (TRUE);		/* Retry please! */
	    } else if ( is_ResetCondition(dip, &mt->eei) ) {
		return (FALSE);		/* Don't retry. */
	    } else {
		if (!debug_flag) print_mtstatus(fd, mt, TRUE);
		return (FALSE);		/* Don't retry. */
	    }
	}

	if ( is_StatusRetryable(dip, &mt->eei) ) {
	    if (verbose_flag) {
		Printf("Retrying after %s status...\n",
				cdbg_EEIStatus(mt->eei.status, CDBG_BRIEF));
	    }
	    (void)sleep(dip->di_eei_sleep);
	    return (TRUE);
	} else if ( !is_ResetCondition(dip, &mt->eei) ) {
	    print_mtstatus(fd, mt, TRUE);
	    return (FALSE);
	}

	dip->di_proc_eei = TRUE;		/* For recursion. */
	dip->di_eei_retries = EEI_RETRIES;

	if (verbose_flag) {
	    Printf("Processing reset condition (%s) - file %lu, record %lu\n",
				cdbg_EEIStatus(mt->eei.status, CDBG_BRIEF),
					mt->mt_fileno, mt->mt_blkno);
	}

	if (debug_flag) print_mtstatus(fd, mt, FALSE);

	/*
	 * When writing tape files, the last file mark gets written
	 * when closing the tape, so adjust file count appropriately.
	 */
	fileno = files;
	if ( (dip->di_mode == WRITE_MODE) &&
	     (files == file_limit) ) {
	    if (fileno) fileno--;
	}

	/*
	 * Sanity check the block and file counts.
	 */
	if (mt->mt_fileno != (fileno + mt_fileno)) {
	    /* NFG if the no-rewind device is used! */
	    Fprintf(
	"File count sanity check failed, mt_fileno = %ld, my count = %ld\n",
			mt->mt_fileno, (fileno + mt_fileno));
#if 0
	    return (FALSE);
#endif
	    /* Adjust and continue below... */
	    mt->mt_fileno = (fileno + mt_fileno);
	}

	/*
	 * ??? mt_blkno doesn't always match my counts ???
	 */
	if ( mt->mt_blkno != (records + mt_blkno) ) {
	    Fprintf(
	"Record count sanity check failed, mt_blkno = %ld, my count = %ld\n",
		mt->mt_blkno, (records + mt_blkno));
#if 0
	    return (FALSE);
#endif
	    /* Adjust and continue below... */
	    mt->mt_blkno = (records + mt_blkno);
	}

reposition:
	dip->di_reset_condition = FALSE;

#if defined(STEELOS)
    if (dip->di_devpath_failure) {
	dip->di_devpath_failure = FALSE;
	/* We terminate if we can't re-open the device. */
	(void) (*dip->di_funcs->tf_reopen_file)(dip, dip->di_oflags);
    } else {
	/*
	 * Rewind and reposition the tape.
	 */
	status = DoRewindTape(dip);
	if (status) {
	    if (dip->di_reset_condition) goto reposition;
	    dip->di_proc_eei = FALSE;
	    return (FALSE);
	}

	/*
	 * Bring the tape online, driver calls ctape_online(), to force
	 * auto-density selection to be done.  Otherwise, we may proceed
	 * with the wrong tape density.
	 */
	status = DoTapeOnline(dip);
	if (status) {
	    if (dip->di_reset_condition) goto reposition;
	    dip->di_proc_eei = FALSE;
	    return (FALSE);
	}
    }
#else /* !defined(STEELOS) */
	/*
	 * NOTE:  Re-open the tape (assumes we're using rewind device),
	 * and reposition the tape.  The re-open of the tape is necessary
	 * to clear state flags (CTAPE_UNIT_ATTEN_STATE & CTAPE_RESET_STATE)
	 * in the CAM tape driver.  If this is NOT done, the close function
	 * won't write file marks. [ Fixed in Steel, not earlier releases! ]
	 */
	(void) (*dip->di_funcs->tf_reopen_file)(dip, dip->di_oflags);
	/*
	 * NOTE: reopen of the tape also brings the tape online.
	 */
#endif /* !defined(STEELOS) */

	/*
	 * Finally, reposition to the correct file and block numbers.
	 */
	if (mt->mt_fileno) {
	    long fileno = mt->mt_fileno;
	    if (verbose_flag) {
		Printf("Positioning to file %lu...\n", fileno);
	    }
	    status = DoForwardSpaceFile(dip, fileno);
	    if (status) {
		if (dip->di_reset_condition) goto reposition;
		dip->di_proc_eei = FALSE;
		return (FALSE);
	    }
	}

	if (mt->mt_blkno) {
	    long record = MIN(mt->mt_blkno, records);
	    if (record) {
		if (verbose_flag) {
		    Printf("Positioning to record %lu...\n", record);
		}
		status = DoForwardSpaceRecord(dip, record);
		if (status) {
		    if (dip->di_reset_condition) goto reposition;
		    dip->di_proc_eei = FALSE;
		    return (FALSE);
		}
	    }
	}
	dip->di_proc_eei = FALSE;
	/*
	 * Don't need to clear EEI status here, since this function is
	 * called recursively when tape movement IOCTL errors occur.
	 */
	return (TRUE);
}

bool
is_ResetCondition(
	struct dinfo	*dip,
	DEV_EEI_STATUS	*eei )
{
    /*
     * For Reference:
     *
     *    EEI_DEVPATH_RESET == ( CAM_SCSI_BUS_RESET || ASCQ_PON_RESET
     *						    || ASCQ_POWER_ON
     *						    || ASCQ_BUS_RESET
     *						    || ASCQ_BDR_RESET
     *						    || ASCQ_FIRMWARE_REBOOT )
     *
     *    EEI_CNTBUSY_FAILURE == CAM_BUSY
     *
     * A status of CAM_BUSY indicates the SIM/HBA is in recovery,
     * and usually that recovery is the result of reset handling.
     * So in the case of tapes, we reposition on CAM_BUSY too.
     *
     * NOTE: CAM_BDR_SENT belongs with EEI_DEVPATH_RESET :-)
     */
    if ( (eei->status == EEI_DEVPATH_RESET)		||
	 (eei->status == EEI_CNTBUSY_FAILURE)		||
	 (eei->status == EEI_TAPE_POS_LOST)		||
	 (eei->arch.cam.cam_status == CAM_BDR_SENT)	) {
	dip->di_reset_condition = TRUE;
	(void) sleep (EEI_RESET);
	return (TRUE);
    } else if (eei->status == EEI_DEVSTATE_FAILURE) {
	struct all_req_sns_data *sdp;
	sdp = (struct all_req_sns_data *)&eei->arch.cam.scsi_sense;
	if ( (eei->flags & EEI_SCSI_SENSE_VALID) &&
	     (sdp->sns_key == ALL_UNIT_ATTEN) ) {
	    dip->di_reset_condition = TRUE;
	    return (TRUE);	/* Result of reset or power on. */
	}
    }
#if defined(STEELOS) && defined(MUNSA)
    /*
     * On Wave4, a we must catch path failures and attempt to re-open
     * the tape, so we:
     *
     *    1) the tape driver looks for another path (multi-pathing)
     *    2) DRD will find a new server for this tape device.
     *
     * Again, for reference, here's the EEI_DEVPATH_FAILURE mapping:
     *
     *    EEI_DEVPATH_FAILURE == ( CAM_PATH_INVALID	||
     *				   CAM_DEV_NOT_THERE	||
     *				   CAM_SEL_TIMEOUT	||
     *				   CAM_NO_HBA		||
     *				   CAM_LUN_INVALID	||
     *				   CAM_TID_INVALID	)
     *
     */
    if (eei->status == EEI_DEVPATH_FAILURE) {
	dip->di_devpath_failure = TRUE;
	dip->di_reset_condition = TRUE;
	return (TRUE);		/* Re-open to find next path. */
    }
#endif /* defined(STEELOS) && defined(MUNSA) */

    return (FALSE);
}

bool
is_StatusRetryable(
	struct dinfo	*dip,
	DEV_EEI_STATUS	*eei )
{
    /*
     * Personally, I'm starting to think using EEI status mappings
     * is problematic, since its' groupings are subjective, and the
     * actual underlying status codes are hidden, so confusing :-)
     *
     * For Reference:
     *
     *     EEI_DEVBUSY_FAILURE == SCSI_STAT_BUSY
     *
     *     EEI_ABORTIO_FAILURE == ( CAM_REQ_ABORTED || CAM_CMD_TIMEOUT )
     */
    if ( (eei->status == EEI_DEVBUSY_FAILURE) ||
	 (eei->status == EEI_ABORTIO_FAILURE) ) {
	return (TRUE);			/* Retry please! */
    } else if ( (eei->status == EEI_NO_STATUS) && dip->di_proc_eei) {
	/*
	 * Occasionally, while processing a reset condition on V4.0F, the
	 * first mtop (rewind) is error'ed out with no EEI status.  This is
	 * probably broken in the tape driver, but like any good application
	 * we must try our best to workaround these anomalies :-)
	 */
	return (TRUE);			/* Retry please! */
    }

    switch (eei->arch.cam.cam_status) {
	/*
	 * Most of these CAM status' are mapped to EEI_DEVBIO_FAILURE,
	 * but this grouping also contains status codes which should
	 * NOT be retried. (IMHO)
	 *
	 * At this time, EEI_DEVBIO_FAILURE also includes CAM_BDR_SENT
	 * which is used for Reset detection above :-)
	 */
	case CAM_SCSI_BUSY:		/* SCSI bus busy.		*/
	case CAM_CMD_TIMEOUT:		/* Command timeout.		*/
	case CAM_MSG_REJECT_REC:	/* Message reject received.	*/
	case CAM_UNCOR_PARITY:		/* Uncorrectable parity error.	*/
	case CAM_DATA_RUN_ERR:		/* Data overrun/underrun error.	*/
	case CAM_SEQUENCE_FAIL:		/* Target bus phase sequence.	*/
	case CAM_UNEXP_BUSFREE:		/* Unexpected BUS free.		*/
	/*
	 * This status is NOT retried for cluster systems, since this
	 * is the primary status code used to initiate service failover.
	 * [ Changed my mind... we should NOT normally get this error. ]
	 */
#if 0 /* !defined(MUNSA) */
	case CAM_SEL_TIMEOUT:		/* Target selection timeout.	*/
#endif /* !defined(MUNSA) */
	    return (TRUE);		/* Retry these please! */

	default:
	    return (FALSE);		/* Don't retry. */
    }
}

/*
 * Check for retryable EEI status on non-open tape device.
 */
bool
check_eei_status(struct dinfo *dip, bool retry)
{
	struct mtget mt;
	char *file = dip->di_dname;
	int oflags = (dip->di_oflags | O_NONBLOCK);
	int fd, status;
	int saved_errno = errno;

	if (debug_flag) {
	    Printf (
		"Attempting to open %s file '%s', open flags = %#o...\n",
			(dip->di_ftype == INPUT_FILE) ? "input" : "output",
								file, oflags);
	}

	if ( (fd = open (file, oflags)) == FAILURE) {
	    report_error ("check_eei_status open", FALSE);
	    errno = saved_errno;
	    return (FALSE);
	}

	status = get_eei_status(fd, &mt);
	if (status != ESUCCESS) {
	    (void) close (fd);
	    errno = saved_errno;
	    return (FALSE);
	}

	if (debug_flag) print_mtstatus(fd, &mt, FALSE);

	(void) close (fd);
	errno = saved_errno;

	if (!retry) return (FALSE);

	if ( is_ResetCondition(dip, &mt.eei) ||
	     is_StatusRetryable(dip, &mt.eei) ) {
	    return (TRUE);
	} else {
	    return (FALSE);
	}
}

/*
 * Clear Tape EEI Status - Necessary since EEI is persistent.
 */
void
clear_eei_status(int fd, bool startup)
{
	struct mtget mt;
	int status;

	if (debug_flag) Printf("Clearing EEI data...\n");

	if ( get_eei_status(fd, &mt) != ESUCCESS) return;

	if (debug_flag) print_mtstatus(fd, &mt, FALSE);

	if (startup) {
	    /*
	     * blkno/fileno == -1 when invalid!
	     */
	    if ( (mt_blkno = mt.mt_blkno) < 0L) {
		mt_blkno = 0L;
	    }
	    if ( (mt_fileno = mt.mt_fileno) < 0L) {
		mt_fileno = 0L;
	    }
	}
	return;
}

int
get_eei_status(int fd, struct mtget *mt)
{
	int status;

	if ( (status = ioctl(fd, MTIOCGET, (char *)mt)) < 0) {
	    report_error ("MTIOCGET failed", FALSE);
	}
	return (status);
}

/*
 * Print out tape status based on deviocget and mtiocget.
 */
void
print_mtstatus(int fd, struct mtget *mt, bool print_all)
{
	/*
	 * When processing EEI Reset requests, we do not display
	 * the DEVIOCGET or DEVGETINFO, since these IOCTL's issue
	 * SCSI commands which can interfere with our recovery.
	 * [ Driver does Mode Sense to determine current density ]
	 */
	if ( print_all ) {
	    print_devio(fd);
	}
	print_mtio(fd, mt);
}

/*
 * Display the contents of the mtget struct.
 * Args: fd a file descriptor of the already opened tape device.
 */
void
print_mtio(int fd, struct mtget *mt)
{
	Fprint("\n");
	Fprint("MTIOCGET ELEMENT        CONTENTS\n");
	Fprint("----------------        --------\n");
	Fprint("mt_type                 ");
	switch(mt->mt_type) {
	case MT_ISTS:
		Fprint("MT_ISTS\n");
		break;
	case MT_ISHT:
		Fprint("MT_ISHT\n");
		break;
	case MT_ISTM:
		Fprint("MT_ISTM\n");
		break;
	case MT_ISMT:
		Fprint("MT_ISMT\n");
		break;
	case MT_ISUT:
		Fprint("MT_ISUT\n");
		break;
	case MT_ISTMSCP:
		Fprint("MT_ISTMSCP\n");
		break;
	case MT_ISST:
		Fprint("MT_ISST\n");
		break;
	case MT_ISSCSI:
		Fprint("MT_ISSCSI\n");
		break;
	default:
		Fprint("Unknown mt_type = 0x%x\n", mt->mt_type);
	}

	if ( mt->mt_type != MT_ISSCSI ) {
		Fprint("mt_dsreg                0x%X\n", mt->mt_dsreg);
		Fprint("mt_erreg                0x%X\n\n", mt->mt_erreg);
		Fprint("mt_resid                %d\n", mt->mt_resid);
		return;
	}

	/*
	 * The rest deals with functionality built into the SCSI tape
	 * driver which does not exist in TMSCP.
	 */

	Fprint("mt_dsreg                0x%X\n", mt->mt_dsreg);
	if (mt->mt_dsreg)
		print_stat(mt->mt_dsreg);
	Fprint("mt_erreg                0x%X ", mt->mt_erreg);
	print_erreg(mt->mt_erreg);
	Fprint("mt_resid                %d\n", mt->mt_resid);
	Fprint("mt_fileno               %ld %s\n",
		mt->mt_fileno, (mt->mt_fileno < 0L) ? "(invalid)" : "");
	Fprint("mt_blkno                %ld %s\n",
		mt->mt_blkno, (mt->mt_blkno < 0L) ? "(invalid)" : "");

	/*
	 * Display the Extended Error Information (EEI) Status.
	 */
	print_eei(&mt->eei);
	Fprint("\n");
	return;
}

#if defined(ZULUOS)

void print_status( v1_device_info_t *p_info );

/*
 * Display the contents of the device_info struct.
 * Args: fd a file descriptor of the already opened tape device.
 */
void
print_devio(int fd)
{
	int			status;
	device_info_t		devinfo;
	v1_device_info_t	*p_info;

	/*
	 *  Attempt to use preferred DEVGETINFO, if it fails: use DEVIOCGET
	 */
	status = ioctl( fd, DEVGETINFO, &devinfo );
	if ( status || ( devinfo.version != VERSION_1 ) ) {
	    struct devget devget;

	    status = ioctl( fd, DEVIOCGET, (void *)&devget );
	    if( status ) {
		report_error ("DEVIOCGET failed", FALSE);
		return;
	    }
	    devio_2_devgetinfo( &devget, &devinfo );
	}
	/*
	 *  At this point we have a valid devinfo struct
	 */
	p_info = &devinfo.v1;

	/*
	 * We keep the "status" information EXACTLY the same as before
	 * as there are most likely are user entities outthere that parse
	 * the "status" output. We'll follow the normal output with the
	 * additional devgetinfo information, if applicable.
	 */
	Fprint( "\n" );
	Fprint( "DEVIOGET ELEMENT       CONTENTS\n" );
	Fprint( "----------------       --------\n" );
	Fprint( "category               " );
	switch( p_info->category ) {

	    case  DEV_TAPE :
		Fprint( "DEV_TAPE\n" );
		break;
	    case  DEV_DISK :
		Fprint( "DEV_DISK\n" );
		break;
	    case  DEV_TERMINAL :
		Fprint( "DEV_TERMINAL\n" );
		break;
	    case  DEV_PRINTER :
		Fprint( "DEV_PRINTER\n" );
		break;
	    case  DEV_SPECIAL :
		Fprint( "DEV_SPECIAL\n" );
		break;
	    default :
		Fprint( "UNDEFINED VALUE (0x%x)\n", p_info->category );
		break;
	}
	Fprint( "bus                    " );
	switch( p_info->bus ) {

	    case  DEV_UB :
		Fprint( "DEV_UB\n" );
		break;
	    case  DEV_QB :
		Fprint( "DEV_QB\n" );
		break;
	    case  DEV_MB :
		Fprint( "DEV_MB\n" );
		break;
	    case  DEV_BI :
		Fprint( "DEV_BI\n" );
		break;
	    case  DEV_CI :
		Fprint( "DEV_CI\n" );
		break;
	    case  DEV_NB :
		Fprint( "DEV_NB\n" );
		break;
	    case  DEV_MSI :
		Fprint( "DEV_MSI\n" );
		break;
	    case  DEV_SCSI :
		Fprint( "DEV_SCSI\n" );
		break;
	    case  DEV_UNKBUS :
		Fprint( "DEV_UNKBUS\n" );
		break;
	    default :
		Fprint( "UNDEFINED VALUE (0x%x)\n", p_info->bus );
		break;
	}
	Fprint( "interface              %s\n", p_info->interface );
	Fprint( "device                 %s\n", p_info->device );
	Fprint( "adpt_num               %d\n", p_info->businfo.adpt_num );
	Fprint( "nexus_num              %d\n", p_info->businfo.nexus_num );
	Fprint( "bus_num                %d\n", p_info->businfo.bus_num );
	Fprint( "ctlr_num               %d\n", p_info->businfo.ctlr_num );
	Fprint( "slave_num              %d\n", p_info->businfo.slave_num );
	Fprint( "dev_name               %s\n", p_info->dev_name );
	Fprint( "unit_num               %d\n", p_info->businfo.unit_num );
	Fprint( "soft_count             %u\n", p_info->soft_count );
	Fprint( "hard_count             %u\n", p_info->hard_count );
	/*
	 *  New info status fields:
	 */
	print_status( p_info );
	return;
}

/*
 *  Dissect the status field of the device_info struct
 */
void
print_status( v1_device_info_t *p_info )
{
    v1_disk_dev_info_t	*p_disk;
    v1_tape_dev_info_t	*p_tape;

    switch( p_info->category ) {

      case  DEV_DISK :
        p_disk = &p_info->devinfo.disk;
        if( !p_disk->status ) {
            break;
	}
	Fprint( "\n" );
        Fprint( "DEVGETINFO ELEMENT      CONTENTS\n" );
        Fprint( "------------------      --------\n" );
        Fprint( "status                  0x%x\n", p_disk->status );
        if( p_disk->status ) {
            Fprint( "                        " );
            if( p_disk->status & DKDEV_OFFLINE )
                Fprint( "OFFLINE " );
            if( p_disk->status & DKDEV_WRTPROT )
                Fprint( "WRTPROT " );
            if( p_disk->status & DKDEV_RDONLY )
                Fprint( "RDONLY " );
            if( p_disk->status & DKDEV_REMOVABLE )
                Fprint( "REMOVABLE " );
            if( p_disk->status & DKDEV_REMOVABLE )
                Fprint( "REMOVABLE " );
	}
        Fprint( "part_num                %d\n", p_disk->part_num );
        Fprint( "media_changes           %d\n", p_disk->media_changes );
        Fprint( "class                   %s\n", p_disk->class==DKDEV_CLS_FLPY_GENERIC ? "GENERIC"
                                              : p_disk->class==DKDEV_CLS_FLPY_3_DD2S ? "3_DD2S"
                                              : p_disk->class==DKDEV_CLS_FLPY_3_HD2S ? "3_HD2S"
                                              : p_disk->class==DKDEV_CLS_FLPY_3_ED2S ? "3_ED2S"
                                              : p_disk->class==DKDEV_CLS_FLPY_5_LD2S ? "5_LD2S"
                                              : p_disk->class==DKDEV_CLS_FLPY_5_DD1S ? "5_DD1S"
                                              : p_disk->class==DKDEV_CLS_FLPY_5_DD2S ? "5_DD2S"
                                              : p_disk->class==DKDEV_CLS_FLPY_5_HD2S ? "5_HD2S"
                                              : "<unrecognized>" );
        break;

      case  DEV_TAPE :
        p_tape = &p_info->devinfo.tape;
        if ( !p_tape->unit_status && !p_tape->media_status ) {
            break;
	}
        Fprint( "\n" );
        Fprint( "DEVGETINFO ELEMENT      CONTENTS\n");
        Fprint( "------------------      --------\n");
        Fprint( "media_status            0x%x\n", p_tape->media_status );
        if( p_tape->media_status ) {
            Fprint( "                        " );
            if( p_tape->media_status & TPDEV_BOM )
                Fprint( "BOM " );
            if( p_tape->media_status & TPDEV_EOM )
                Fprint( "EOM " );
            if( p_tape->media_status & TPDEV_WRTPROT )
                Fprint( "WrtProt " );
            if( p_tape->media_status & TPDEV_BLANK )
                Fprint( "Blank " );
            if( p_tape->media_status & TPDEV_WRITTEN )
                Fprint( "Written " );
            if( p_tape->media_status & TPDEV_SOFTERR )
                Fprint( "SoftERR " );
            if( p_tape->media_status & TPDEV_HARDERR )
                Fprint( "HardERR " );
            if( p_tape->media_status & TPDEV_DONE )
                Fprint( "Done " );
            if( p_tape->media_status & TPDEV_RETRY )
                Fprint( "Retry " );
            if( p_tape->media_status & TPDEV_ERASED )
                Fprint( "Erased " );
            if( p_tape->media_status & TPDEV_TPMARK )
                Fprint( "TPmark " );
            if( p_tape->media_status & TPDEV_SHRTREC )
                Fprint( "ShortRec " );
            if( p_tape->media_status & TPDEV_RDOPP )
                Fprint( "RD_Opposite " );
            if( p_tape->media_status & TPDEV_RWDING )
                Fprint( "Rewinding " );
            if( p_tape->media_status & TPDEV_POS_VALID )
                Fprint( "POS_VALID " );
            Fprint( "\n" );
	}
        Fprint( "unit_status             0x%x\n", p_tape->unit_status );
        if( p_tape->unit_status ) {
            Fprint( "                        " );
            if( p_tape->unit_status & TPDEV_READY )
                Fprint( "Ready " );
              else
                Fprint( "Offline " );
            if( p_tape->unit_status & TPDEV_NO_EOM )
                Fprint( "No_EOM_Warn " );
            if( p_tape->unit_status & TPDEV_HAS_LOADER )
                Fprint( "Loader " );
            if( p_tape->unit_status & TPDEV_1FM_ONCLOSE )
                Fprint( "1_FM_Close " );
              else
                Fprint( "2_FM_Close " );
            if( p_tape->unit_status & TPDEV_REW_ONCLOSE )
                Fprint( "Rewind " );
              else
                Fprint( "NO_Rewind " );
            if( p_tape->unit_status & TPDEV_COMPACTING )
                Fprint( "Compacting " );
            if( p_tape->unit_status & TPDEV_COMPRESSING )
                Fprint( "Compacting " );
            if( p_tape->unit_status & TPDEV_BUFFERED )
                Fprint( "Buffered " );
            Fprint( "\n" );
	}
        Fprint( "record_size             %ld\n", p_tape->recordsz );
        Fprint( "density (current)       %ld BPI\n", p_tape->density_bpi );
        Fprint( "density (on write)      %ld BPI\n", p_tape->density_bpi_wrt );
        if( p_tape->media_status & TPDEV_POS_VALID ) {
            Fprint( "Filemark Cnt            %ld\n", p_tape->fm_cnt );
            Fprint( "Record Cnt              %ld\n", p_tape->position );
	} else {
            Fprint( "Filemark Cnt            <Not Valid>\n" );
            Fprint( "Record Cnt              <Not Valid>\n" );
	}
        Fprint( "Class                   %d - ", ((int )p_tape->class ) & 0xFF );
        switch( p_tape->class ) {
          case  TPDEV_CLS_DLT :
            Fprint( "DLT (tk )\n" );
            break;
          case  TPDEV_CLS_RDAT :
            Fprint( "RDAT\n" );
            break;
          case  TPDEV_CLS_9TRK :
            Fprint( "9TRK\n" );
            break;
          case  TPDEV_CLS_QIC :
            Fprint( "QIC\n" );
            break;
          case  TPDEV_CLS_8MM :
            Fprint( "8MM\n" );
            break;
          case  TPDEV_CLS_3480 :
            Fprint( "3480\n" );
            break;
          case  TPDEV_CLS_UNKNOWN :
            Fprint( "<unspecified>\n" );
            break;
          default :
            Fprint( "<unrecognized>\n" );
            break;
	}
        break;
    }
    return;
}

#else /* !defined(ZULUOS) */

/*
 * Display the contents of the deviocget struct.
 * Args: fd a file descriptor of the already opened tape device.
 */
void
print_devio(int fd)
{
	struct devget	devinf;
	device_info_t	*devinfop;

	/*
	 * First, attempt to use the DEVGETINFO ioctl (preferred),
	 * if that fails, try the backward-compatibility ioctl
	 * DEVIOCGET.
	 */

	/* malloc this as this is a large structure */
	devinfop = (device_info_t *)malloc(sizeof(device_info_t));

	if (!(devinfop) || 
		(ioctl(fd, DEVGETINFO, (char *)devinfop) < 0) ||
		(devinfop->version != VERSION_1) || /* what a joke! */
		(devgetinfo_2_devio(devinfop, &devinf) != ESUCCESS)) {

		/* DEVGETINFO failure */
		if (devinfop)
			free(devinfop);
		devinfop = (device_info_t *)NULL;

		if (ioctl(fd, DEVIOCGET, (char *)&devinf) < 0) {
			report_error ("DEVIOCGET failed", FALSE);
			return;
		}
	}
	/*
	 * at this point we have a valid devinf struct and possibly
	 * a valid device_info struct.
	 */

	/*
	 * We keep the "status" information EXACTLY the same as before
	 * as there are most likely are user entities outthere that parse
	 * the "status" output. We'll follow the normal output with the
	 * additional devgetinfo information, if applicable.
	 */

	Fprint("\n");
	Fprint("DEVIOGET ELEMENT        CONTENTS\n");
	Fprint("----------------        --------\n");
	Fprint("category                ");
	switch(devinf.category) {
	    case DEV_TAPE:
		Fprint("DEV_TAPE\n");
		break;
	    case DEV_DISK:
		Fprint("DEV_DISK\n");
		break;
	    case DEV_TERMINAL:
		Fprint("DEV_TERMINAL\n");
		break;
	    case DEV_PRINTER:
		Fprint("DEV_PRINTER\n");
		break;
	    case DEV_SPECIAL:
		Fprint("DEV_SPECIAL\n");
		break;
	    default:
		Fprint("UNDEFINED VALUE (0x%x)\n", devinf.category);
		break;
	}
	Fprint("bus                     ");
	switch(devinf.bus) {
	    case DEV_UB:
		Fprint("DEV_UB\n");
		break;
	    case DEV_QB:
		Fprint("DEV_QB\n");
		break;
	    case DEV_MB:
		Fprint("DEV_MB\n");
		break;
	    case DEV_BI:
		Fprint("DEV_BI\n");
		break;
	    case DEV_CI:
		Fprint("DEV_CI\n");
		break;
	    case DEV_NB:
		Fprint("DEV_NB\n");
		break;
	    case DEV_MSI:
		Fprint("DEV_MSI\n");
		break;
	    case DEV_SCSI:
		Fprint("DEV_SCSI\n");
		break;
	    case DEV_UNKBUS:
		Fprint("DEV_UNKBUS\n");
		break;
	    default:
		Fprint("UNDEFINED VALUE (0x%x)\n", devinf.bus);
		break;
	}
	Fprint("interface               %s\n", devinf.interface);
	Fprint("device                  %s\n", devinf.device);
	Fprint("adpt_num                %d\n", devinf.adpt_num);
	Fprint("nexus_num               %d\n", devinf.nexus_num);
	Fprint("bus_num                 %d\n", devinf.bus_num);
	Fprint("ctlr_num                %d\n", devinf.ctlr_num);
	Fprint("slave_num               %d\n", devinf.slave_num);
	Fprint("dev_name                %s\n", devinf.dev_name);
	Fprint("unit_num                %d\n", devinf.unit_num);
	Fprint("soft_count              %u\n", devinf.soft_count);
	Fprint("hard_count              %u\n", devinf.hard_count);
	Fprint("stat                    0x%X\n", devinf.stat);
	if (devinf.stat) print_stat(devinf.stat);
	Fprint("category_stat           0x%X\n", devinf.category_stat);
	if (devinf.category_stat) print_category(devinf.category_stat,devinfop);

	/* now print any additional DEVGETINFO data */
	if (devinfop) {
		v1_tape_dev_info_t *tapep;

		tapep = (v1_tape_dev_info_t *)&devinfop->v1.devinfo;

		Fprint("\n");
		Fprint("DEVGETINFO ELEMENT      CONTENTS\n");
		Fprint("------------------      --------\n");
		Fprint("media_status            0x%x\n", tapep->media_status);
		if (tapep->media_status) {
		    Fprint("                        ");
		    if (tapep->media_status & TPDEV_BOM)
			Fprint("BOM ");
		    if (tapep->media_status & TPDEV_EOM)
			Fprint("EOM ");
		    if (tapep->media_status & TPDEV_WRTPROT)
			Fprint("WrtProt ");
		    if (tapep->media_status & TPDEV_BLANK)
			Fprint("Blank ");
		    if (tapep->media_status & TPDEV_WRITTEN)
			Fprint("Written ");
		    if (tapep->media_status & TPDEV_SOFTERR)
			Fprint("SoftERR ");
		    if (tapep->media_status & TPDEV_HARDERR)
			Fprint("HardERR ");
		    if (tapep->media_status & TPDEV_DONE)
			Fprint("Done ");
		    if (tapep->media_status & TPDEV_RETRY)
			Fprint("Retry ");
		    if (tapep->media_status & TPDEV_ERASED)
			Fprint("Erased ");
		    if (tapep->media_status & TPDEV_TPMARK)
			Fprint("TPmark ");
		    if (tapep->media_status & TPDEV_SHRTREC)
			Fprint("ShortRec ");
		    if (tapep->media_status & TPDEV_RDOPP)
			Fprint("RD_Opposite ");
		    if (tapep->media_status & TPDEV_RWDING)
			Fprint("Rewinding ");
		    if (tapep->media_status & TPDEV_POS_VALID)
			Fprint("POS_VALID ");
		    Fprint("\n");
		}
		Fprint("unit_status             0x%x\n", tapep->unit_status);
		if (tapep->unit_status) {
		    Fprint("                        ");
		    if (tapep->unit_status & TPDEV_READY)
			Fprint("Ready ");
		    else
			Fprint("Offline ");
		    if (tapep->unit_status & TPDEV_NO_EOM)
			Fprint("No_EOM_Warn ");
		    if (tapep->unit_status & TPDEV_HAS_LOADER)
			Fprint("Loader ");
		    if (tapep->unit_status & TPDEV_1FM_ONCLOSE)
			Fprint("1_FM_Close ");
		    else
			Fprint("2_FM_Close ");
		    if (tapep->unit_status & TPDEV_REW_ONCLOSE)
			Fprint("Rewind ");
		    else
			Fprint("NO_Rewind ");
		    if (tapep->unit_status & TPDEV_COMPACTING)
			Fprint("Compacting ");
		    if (tapep->unit_status & TPDEV_COMPRESSING)
			Fprint("Compacting ");
		    if (tapep->unit_status & TPDEV_BUFFERED)
			Fprint("Buffered ");
		    Fprint("\n");
		}
		Fprint("record_size             %ld\n", tapep->recordsz);
		Fprint("density (current)       %ld BPI\n", tapep->density_bpi);
		Fprint("density (on write)      %ld BPI\n", tapep->density_bpi_wrt);
		if (tapep->media_status & TPDEV_POS_VALID) {
			Fprint("Filemark Cnt            %ld\n", tapep->fm_cnt);
			Fprint("Record Cnt              %ld\n", tapep->position);
		} else {
			Fprint("Filemark Cnt            %s\n", not_valid_str);
			Fprint("Record Cnt              %s\n", not_valid_str);
		}
		Fprint("Class                   %d - ", ((int)tapep->class) & 0xFF);
		switch (tapep->class) {
		case TPDEV_CLS_DLT:
			Fprint("DLT (tk)\n");
			break;
		case TPDEV_CLS_RDAT:
			Fprint("RDAT\n");
			break;
		case TPDEV_CLS_9TRK:
			Fprint("9TRK\n");
			break;
		case TPDEV_CLS_QIC:
			Fprint("QIC\n");
			break;
		case TPDEV_CLS_8MM:
			Fprint("8MM\n");
			break;
		case TPDEV_CLS_3480:
			Fprint("3480\n");
			break;
		case TPDEV_CLS_UNKNOWN:
			Fprint("<unspecified>\n");
			break;
		default:
			Fprint("<unrecognized>\n");
			break;
		}
		free(devinfop);
	}
	return;
}

#endif /* defined(ZULUOS) */

/*
 * Decode and display the EEI status:
 */
void
print_eei(DEV_EEI_STATUS *eei)
{
	if (eei->version == 0) {
	    PrintAscii ("EEI Information", not_valid_str, PNL);
	    return;
	}
	Fprint("\nCAM Extended Error Information:\n\n");
	PrintHex ("EEI Version", eei->version, PNL);
	PrintAscii ("EEI Status", "", DNL);
	print_eei_status (eei->status);

	PrintHex ("EEI Valid Flags", eei->flags, DNL);
	Fprint (" - ");
	if (eei->flags & EEI_CAM_STATUS_VALID)
		Fprint ("CAM_STATUS ");
	if (eei->flags & EEI_SCSI_STATUS_VALID)
		Fprint ("SCSI_STATUS ");
	if (eei->flags & EEI_SCSI_SENSE_VALID)
		Fprint ("SCSI_SENSE ");
	if (eei->flags & EEI_CAM_DATA_VALID)
		Fprint ("CAM_DATA");
	Fprint("\n");

	PrintAscii ("CAM Status", "", DNL);
	if (eei->flags & EEI_CAM_STATUS_VALID) {
	    print_cam_status (eei->arch.cam.cam_status);
	} else {
	    Fprint ("%s\n", not_valid_str);
	}

	PrintAscii ("SCSI Status", "", DNL);
	if (eei->flags & EEI_SCSI_STATUS_VALID) {
	    print_scsi_status(eei->arch.cam.scsi_status);
	} else {
	    Fprint ("%s\n", not_valid_str);
	}

	if (eei->flags & EEI_SCSI_SENSE_VALID) {
	    print_sense_data (eei->arch.cam.scsi_sense);
	} else {
	    PrintAscii ("SCSI Sense Data", not_valid_str, PNL);
	}
}

/*
 * Dissect the mt_erreg field of the mtget structure
 */
void
print_erreg(short erreg)
{
	Fprint ("(%s)\n", cdbg_SenseKeyTable[erreg&0xF]);
	if (erreg & 0xE0) {	/* check these three flags */
		Fprint("\t\t\t");
		if (erreg & FILE_MARK)
			Fprint("FILE_MARK ");
		if (erreg & EOM)
			Fprint("EOM ");
		if (erreg & ILI)
			Fprint("ILI ");
		Fprint("\n");
	}
}

/*
 * Display the EEI status code:
 */
void
print_eei_status(u_int eei_status)
{
	Fprint ("%s (%#x) - %s\n",
		cdbg_EEIStatus(eei_status, CDBG_BRIEF),
		eei_status, cdbg_EEIStatus(eei_status, CDBG_FULL));
}

/*
 * Dissect the cam_status field of the mtget structure
 */
void
print_cam_status(u_int cam_status)
{
	Fprint ("%s (%#x) - %s\n",
		cdbg_CamStatus(cam_status, CDBG_BRIEF),
		cam_status, cdbg_CamStatus(cam_status, CDBG_FULL));
}

/*
 * Dissect the scsi_status field of the mtget structure
 */
void
print_scsi_status(u_char scsi_status)
{
	Fprint ("%s (%#x) - %s\n",
		cdbg_ScsiStatus(scsi_status, CDBG_BRIEF),
		scsi_status, cdbg_ScsiStatus(scsi_status, CDBG_FULL));
}

/*
 * Print the SCSI sense data.
 */
void
print_sense_data(u_char *scsi_sense_ptr)
{
	struct all_req_sns_data *sdp = (struct all_req_sns_data *) scsi_sense_ptr;
	int sense_length = (int) sdp->addition_len + 8;
	u_short ascq;
	u_int info;

	Fprint ("\nSCSI Request Sense Information:\n\n");
	PrintHex ("Error Code", sdp->error_code, DNL);
	if (sdp->error_code == ALL_IMED_ERR_CODE) {
	    Fprint (" (Current Error)\n");
	} else if (sdp->error_code == ALL_DEFER_ERR_CODE) {
	    Fprint (" (Deferred Error)\n");
	} else if (sdp->error_code == ALL_VENDOR_SPECIFIC) {
	    Fprint (" (Vendor Specific)\n");
	} else {
	    Fprint ("\n");
	}
	PrintHex ("Valid Bit", sdp->valid, DNL);
	Fprint ("%s\n", (sdp->valid) ? " (Information field is valid)" : "");
	PrintHex ("Segment Number", sdp->segment, PNL);
	PrintHex ("Sense Key", sdp->sns_key, DNL);
	Fprint (" (%s)\n", cdbg_SenseKeyTable[sdp->sns_key]);
	PrintHex ("Illegal Length", sdp->ili, PNL);
	PrintHex ("End Of Media", sdp->eom, PNL);
	PrintHex ("File Mark", sdp->filemark, PNL);
	info = (u_int)( ((u_int)sdp->info_byte3 << 24) +
			((u_int)sdp->info_byte2 << 16) +
			((u_int)sdp->info_byte1 << 8) +
			 (sdp->info_byte0) );
	PrintHex ("Information Field", info, (info) ? DNL : PNL);
	if (info) Fprint (" (%d)\n", info);
	PrintHex ("Additional Sense Length", sdp->addition_len, PNL);

	if ( (sense_length -= 8) > 0) {
	    info = (u_int)( ((u_int)sdp->cmd_specific3 << 24) +
			    ((u_int)sdp->cmd_specific2 << 16) +
			    ((u_int)sdp->cmd_specific1 << 8) +
			     (sdp->cmd_specific0) );
	    PrintHex ("Command Specific Information", info, (info) ? DNL : PNL);
	    if (info) Fprint (" (%d)\n", info);
	    sense_length -= 4;
	}

	if (sense_length > 0) {
	    char *ascq_msg = cdbg_SenseMessage (sdp);
	    ASCQ_TO_USHORT (sdp, ascq);
	    PrintAscii ("Additional Sense Code/Qualifier", "", DNL);
	    Fprint ("(%#x, %#x) = %s\n", sdp->asc, sdp->asq,
					(ascq_msg) ? ascq_msg : "Unknown");
	    sense_length -= 2;
	}

	if (sense_length-- > 0) {
	    PrintHex ("Field Replaceable Unit Code", sdp->fru, PNL);
	}

	/*
	 * Sense specific sense data.
	 */
	if (sense_length > 0) {
	    struct all_sks_ill_req *sksi;
	    sksi = &sdp->sense_specific.sks_ill_req;
	    if (sksi->sksv && (sdp->sns_key == ALL_ILLEGAL_REQ) ) {
		u_short field_ptr = ((sksi->field_ptr1 << 8) + sksi->field_ptr0);
		PrintHex ("Bit Pointer to Field in Error", sksi->bit_pointer,
					(sksi->bit_pointer) ? DNL : PNL);
		if (sksi->bpv) {
		    Fprint (" (valid, bit %u)\n", (sksi->bit_pointer + 1));
		}
		PrintHex ("Error Field Command/Data (C/D)", sksi->c_or_d, DNL);
		Fprint (" (%s)\n", (sksi->c_or_d) ? "Illegal parameter in CDB bytes"
						  : "Illegal parameter in Data Sent");
		PrintHex ("Byte Pointer to Field in Error", field_ptr,
						(field_ptr) ? DNL : PNL);
		if (field_ptr) {
		    Fprint (" (byte %u)\n", (field_ptr + 1)); /* zero-based */
		}
		sense_length -= sizeof(*sksi);
	    } else if (sksi->sksv && ( (sdp->sns_key == ALL_RECOVER_ERR) ||
				       (sdp->sns_key == ALL_HARDWARE_ERR) ||
				       (sdp->sns_key == ALL_MEDIUM_ERR) ) ) {
		struct all_sks_retry_cnt *sksr;
		sksr = &sdp->sense_specific.sks_retry_cnt;
		PrintHex ("Actual Retry Count",
			    ((sksr->retry_cnt1 << 8) + sksr->retry_cnt0), PNL);
		sense_length -= sizeof(*sksr);
	    } else if (sksi->sksv && (sdp->sns_key == ALL_NOT_READY) &&
				     (ascq == ASCQ_LUN_NRDY_FMT) ) {
		u_short progress;
		struct all_sks_prog_cnt *sksp;
		sksp = &sdp->sense_specific.sks_prog_cnt;
		progress = ((sksp->progress1 << 8) + sksp->progress0);
		PrintDecimal ("Format Progress Indication", progress, DNL);
		/*Fprint (" (%d%% complete)\n", FormatProgress(progress));*/
		sense_length -= sizeof(*sksp);
	    } else {
		PrintAscii ("Sense Specific Bytes", "", DNL);
		PrintFields ((u_char *) &sdp->sense_specific,
					sizeof(sdp->sense_specific));
		sense_length -= sizeof(sdp->sense_specific);
	    }
	}
	/*
	 * Additional sense bytes (if any);
	 */
	if (sense_length > 0) {
	    PrintAscii ("Additional Sense Bytes", "", DNL);
	    PrintFields ((u_char *)sdp->additional_sense.other_sns,
							sense_length);
	}
}

/*
 * Dissect the stat field of a devio structure
 *  or the mt_dsreg field of the mtget structure
 */
void
print_stat(long stat)
{
	Fprint("                        ");
	if (stat & DEV_BOM)
		Fprint("DEV_BOM ");
	if (stat & DEV_EOM)
		Fprint("DEV_EOM ");
	if (stat & DEV_OFFLINE)
		Fprint("DEV_OFFLINE ");
	if (stat & DEV_WRTLCK)
		Fprint("DEV_WRTLCK ");
	if (stat & DEV_BLANK)
		Fprint("DEV_BLANK ");
	if (stat & DEV_WRITTEN)
		Fprint("DEV_WRITTEN ");
	if (stat & DEV_CSE)
		Fprint("DEV_CSE ");
	if (stat & DEV_SOFTERR)
		Fprint("DEV_SOFTERR ");
	if (stat & DEV_HARDERR)
		Fprint("DEV_HARDERR ");
	if (stat & DEV_DONE)
		Fprint("DEV_DONE ");
	if (stat & DEV_RETRY)
		Fprint("DEV_RETRY ");
	if (stat & DEV_ERASED)
		Fprint("DEV_ERASED ");
#if defined(STEELOS)
	if (stat & DEV_TAPE_MARK)
		Fprint("TPMARK ");
	if (stat & DEV_SHORT_REC)
		Fprint("SHRTREC ");
	if (stat & DEV_TPMRK_PEND)
		Fprint("TPMARK_PENDING ");
	if (stat & DEV_REWINDING)
		Fprint("REWINDING ");
	if (stat & DEV_READ_OPP)
		Fprint("READING_OPPOSITE ");
#else /* !defined(STEELOS) */
	if (stat & CTAPE_TPMARK)
		Fprint("TPMARK ");
	if (stat & CTAPE_SHRTREC)
		Fprint("SHRTREC ");
	if (stat & CTAPE_TPMARK_PENDING)
		Fprint("TPMARK_PENDING ");
	if (stat & CTAPE_REWINDING)
		Fprint("REWINDING ");
#endif /* defined(STEELOS) */
	Fprint("\n");
}

/*
 * Dissect the category_stat field of a devio structure
 *
 * With the advent of the DEVGETINFO ioctl, we may get densities that are
 * unknown with the DEV_xxxBPI bitmasks. Thus, rather than simply
 * stating "unspecified density", if the devinfop exists and has a
 * non-zero density, we'll dummy up a DEV_xxxBPI name.
 */
void
print_category(long stat, device_info_t *devinfop)
{
	Fprint("                        ");
	if (stat & DEV_TPMARK)
		Fprint("DEV_TPMARK ");
	if (stat & DEV_SHRTREC)
		Fprint("DEV_SHRTREC ");
	if (stat & DEV_RDOPP)
		Fprint("DEV_RDOPP ");
	if (stat & DEV_RWDING)
		Fprint("DEV_RWDING ");
	if (stat & DEV_LOADER)
		Fprint("DEV_LOADER ");

	if (stat & DEV_800BPI) {
		Fprint("DEV_800BPI");
	} else if (stat & DEV_1600BPI) {
		Fprint("DEV_1600BPI");
	} else if (stat & DEV_6250BPI) {
		Fprint("DEV_6250BPI");
	} else if (stat & DEV_6666BPI) {
		Fprint("DEV_6666BPI");
	} else if (stat & DEV_10240BPI) {
		Fprint("DEV_10240BPI");
	} else if (stat & DEV_38000BPI) {
		Fprint("DEV_38000BPI");
#ifdef DEV_38000_CP
	} else if (stat & DEV_38000_CP) {
		Fprint("DEV_38000_CP");
	} else if (stat & DEV_76000BPI) {
		Fprint("DEV_76000BPI");
	} else if (stat & DEV_76000_CP) {
		Fprint("DEV_76000_CP");
#endif /* DEV_38000_CP */
	} else if (stat & DEV_8000_BPI) {
		Fprint("DEV_8000_BPI");
	} else if (stat & DEV_10000_BPI) {
		Fprint("DEV_10000_BPI");
	} else if (stat & DEV_16000_BPI) {
		Fprint("DEV_16000_BPI");
	} else if (stat & DEV_54000_BPI) {
		Fprint("DEV_54000_BPI");
	} else if (stat & DEV_61000_BPI) {
		Fprint ("DEV_61000_BPI");
	} else if (stat & DEV_45434_BPI) {
		Fprint ("DEV_45434_BPI");
	} else if (stat & DEV_42500_BPI) {
		Fprint ("DEV_42500_BPI");
	} else if (stat & DEV_62500_BPI) {
		Fprint ("DEV_62500_BPI");
	} else if (stat & DEV_40640_BPI) {
		Fprint ("DEV_40640_BPI");
	} else if (stat & DEV_36000_BPI) {
		Fprint ("DEV_36000_BPI");
	} else if (stat & DEV_81630_BPI) {
		Fprint ("DEV_81630_BPI");
	} else {
		if (devinfop && devinfop->v1.devinfo.tape.density_bpi) {
			Fprint("DEV_%ld_%s",
				devinfop->v1.devinfo.tape.density_bpi,
				((devinfop->v1.devinfo.tape.unit_status &
					TPDEV_COMPACTING) ? "CP" : "BPI"));
		} else
			Fprint("<unspecified density>");
	}
	Fprint("\n");
}

/*
 * Strings used by Common Printing Functions.
 */
#define ASCII_FIELD	"%34.34s: %s"
#define EMPTY_FIELD	"%36.36s%s"
#define NUMERIC_FIELD	"%34.34s: %lu"
#define HEX_FIELD	"%34.34s: %#lx"
#define FIELD_WIDTH	36		/* The field width (see above).	*/
#define DEFAULT_WIDTH	80		/* Default tty display width.	*/

int OutputRadix = DEC_RADIX;		/* Default to decimal output.	*/

void
PrintNumeric (char *field_str, u_long numeric_value, int nl_flag)
{
	char *Fprint_str;

	if (OutputRadix == HEX_RADIX) {
		Fprint_str = HEX_FIELD;
	} else {
		Fprint_str = NUMERIC_FIELD;
	}
	Fprint (Fprint_str, field_str, numeric_value);
	if (nl_flag) Fprint ("\n");
}

void
PrintDecimal (char *field_str, u_long numeric_value, int nl_flag)
{
	char *Fprint_str = NUMERIC_FIELD;

	Fprint (Fprint_str, field_str, numeric_value);
	if (nl_flag) Fprint ("\n");
}

void
PrintHex (char *field_str, u_long numeric_value, int nl_flag)
{
	char *Fprint_str = HEX_FIELD;

	Fprint (Fprint_str, field_str, numeric_value);
	if (nl_flag) Fprint ("\n");
}

void
PrintAscii (char *field_str, char *ascii_str, int nl_flag)
{
	int length = strlen(field_str);
	char *Fprint_str = ((length) ? ASCII_FIELD : EMPTY_FIELD);

	Fprint (Fprint_str, field_str, ascii_str);
	if (nl_flag) Fprint ("\n");
}

void
PrintFields (u_char *bptr, int length)
{
	int field_entrys = ((DEFAULT_WIDTH - FIELD_WIDTH) / 3) - 1;
	int count = 0;

	while (count < length) {
	    if ((++count % field_entrys) == 0) {
		Fprint("%02x\n", *bptr++);
		if (count < length) PrintAscii("", "", DNL);
	    } else {
		Fprint ("%02x ", *bptr++);
	    }
	}
	if (count % field_entrys) Fprint("\n");
}

/*
 * CAM Status Code Table.
 */
struct CAM_StatusTable {
	u_char	cam_status;
	caddr_t	status_msg_brief;
	caddr_t	status_msg_full;
} cam_StatusTable[] = {
    {	CAM_REQ_INPROG,		"CAM_REQ_INPROG",
				"CCB request is in progress"		},
    {	CAM_REQ_CMP ,		"CAM_REQ_CMP",
				"CCB request completed w/out error"	},
    {	CAM_REQ_ABORTED,	"CAM_REQ_ABORTED",
				"CCB request aborted by the host"	},
    {	CAM_UA_ABORT,		"CAM_UA_ABORT",
				"Unable to Abort CCB request"		},
    {	CAM_REQ_CMP_ERR,	"CAM_REQ_CMP_ERR",
				"CCB request completed with an error"	},
    {	CAM_BUSY,		"CAM_BUSY",
				"CAM subsystem is busy"			},
    {	CAM_REQ_INVALID,	"CAM_REQ_INVALID",
				"CCB request is invalid"		},
    {	CAM_PATH_INVALID,	"CAM_PATH_INVALID",
				"ID supplied is invalid"		},
    {	CAM_DEV_NOT_THERE,	"CAM_DEV_NOT_THERE",
				"device not installed/there"		},
    {	CAM_UA_TERMIO,		"CAM_UA_TERMIO",
				"Unable to Terminate I/O CCB request"	},
    {	CAM_SEL_TIMEOUT,	"CAM_SEL_TIMEOUT",
				"Target selection timeout"		},
    {	CAM_CMD_TIMEOUT,	"CAM_CMD_TIMEOUT",
				"Command timed out",			},
    {	CAM_MSG_REJECT_REC,	"CAM_MSG_REJECT_REC",
				"reject received"			},
    {	CAM_SCSI_BUS_RESET,	"CAM_SCSI_BUS_RESET",
				"bus reset sent/received"		},
    {	CAM_UNCOR_PARITY,	"CAM_UNCOR_PARITY",
				"parity error occurred"			},
    {	CAM_AUTOSENSE_FAIL,	"CAM_AUTOSENSE_FAIL",
				"Request sense cmd fail"		},
    {	CAM_NO_HBA,		"CAM_NO_HBA",
				"No HBA detected Error"			},
    {	CAM_DATA_RUN_ERR,	"CAM_DATA_RUN_ERR",
				"overrun/underrun error"		},
    {	CAM_UNEXP_BUSFREE,	"CAM_UNEXP_BUSFREE",
				"Unexpected bus free"			},
    {	CAM_SEQUENCE_FAIL,	"CAM_SEQUENCE_FAIL",
				"bus phase sequence failure"		},
    {	CAM_CCB_LEN_ERR,	"CAM_CCB_LEN_ERR",
				"CCB length supplied is inadequate"	},
    {	CAM_PROVIDE_FAIL,	"CAM_PROVIDE_FAIL",
				"to provide requ. capability"		},
    {	CAM_BDR_SENT,		"CAM_BDR_SENT",
				"A SCSI BDR msg was sent to target"	},
    {	CAM_REQ_TERMIO,		"CAM_REQ_TERMIO",
				"CCB request terminated by the host"	},
    {	CAM_HBA_ERR,		"CAM_HBA_ERR",
				"Unrecoverable host bus adaptor error"	},
    {	CAM_BUS_RESET_DENIED,	"CAM_BUS_RESET_DENIED",
				"bus reset denied"			},
    {	CAM_MUNSA_REJECT,	"CAM_MUNSA_REJECT",
				"rejecting device"			},
    {	CAM_IDE,		"CAM_IDE",
				"Initiator Detected Error Received"	},
    {	CAM_RESRC_UNAVAIL,	"CAM_RESRC_UNAVAIL",
				"Resource unavailable"			},
    {	CAM_UNACKED_EVENT,	"CAM_UNACKED_EVENT",
				"Unacknowledged event by host"		},
    {	CAM_MESSAGE_RECV,	"CAM_MESSAGE_RECV",
				"Msg received in Host Target Mode"	},
    {	CAM_INVALID_CDB,	"CAM_INVALID_CDB",
				"Invalid CDB recvd in HT Mode"		},
    {	CAM_LUN_INVALID,	"CAM_LUN_INVALID",
				"LUN supplied is invalid"		},
    {	CAM_TID_INVALID,	"CAM_TID_INVALID",
				"Target ID supplied is invalid"		},
    {	CAM_FUNC_NOTAVAIL,	"CAM_FUNC_NOTAVAIL",
				"Requested function is not available"	},
    {	CAM_NO_NEXUS,		"CAM_NO_NEXUS",
				"Nexus is not established"		},
    {	CAM_IID_INVALID,	"CAM_IID_INVALID",
				"The initiator ID is invalid"		},
    {	CAM_CDB_RECVD,		"CAM_CDB_RECVD",
				"The SCSI CDB has been received"	},
    {	CAM_LUN_ALLREADY_ENAB,	"CAM_LUN_ALLREADY_ENAB",
				"LUN is already enabled"		},
    {	CAM_SCSI_BUSY,		"CAM_SCSI_BUSY",
				"SCSI bus busy"				}
};
static int cam_StatusEntrys =
		sizeof(cam_StatusTable) / sizeof(cam_StatusTable[0]);

char *
cdbg_CamStatus (u_char cam_status, int report_format)
{
	struct CAM_StatusTable *cst = cam_StatusTable;
	I32 entrys;

	for (entrys = 0; entrys < cam_StatusEntrys; cst++, entrys++) {
	    if (cst->cam_status == (cam_status&CAM_STATUS_MASK)) {
		if (report_format == CDBG_BRIEF) {
		    return (cst->status_msg_brief);
		} else {
		    return (cst->status_msg_full);
		}
	    }
	}
	return ((report_format) ? "Unknown CAM Status" : "Unknown");
}

/*
 * CAM Status Code Table.
 */
struct SCSI_StatusTable {
	u_char	scsi_status;
	caddr_t	status_msg_brief;
	caddr_t	status_msg_full;
} scsi_StatusTable[] = {
    {	SCSI_STAT_GOOD,			"SCSI_STAT_GOOD",
	/* 0x00 */ "Command successfully completed"		},
    {	SCSI_STAT_CHECK_CONDITION,	"SCSI_STAT_CHECK_CONDITION",
	/* 0x02 */ "Error, exception, or abnormal condition"	},
    {	SCSI_STAT_CONDITION_MET,	"SCSI_STAT_CONDITION_MET",
	/* 0x04 */ "Requested operation satisifed"		},
    {	SCSI_STAT_BUSY,			"SCSI_STAT_BUSY",
	/* 0x08 */ "Target is BUSY"				},
    {	SCSI_STAT_INTERMEDIATE,		"SCSI_STAT_INTERMEDIATE",
	/* 0x10 */ "Linked commands successfully completed"	},
    {	SCSI_STAT_INTER_COND_MET,	"SCSI_STAT_INTER_COND_MET",
	/* 0x14 */ "Intermediate-Condition met"			},
    {	SCSI_STAT_RESERVATION_CONFLICT,	"SCSI_STAT_RESERVATION_CONFLICT",
	/* 0x18 */ "Target reservation conflict"		},
    {	SCSI_STAT_COMMAND_TERMINATED,	"SCSI_STAT_COMMAND_TERMINATED",
	/* 0x22 */ "Command terminated by Terminate I/O Message"},
    {	SCSI_STAT_QUEUE_FULL,		"SCSI_STAT_QUEUE_FULL",
	/* 0x28 */ "Command tag queue is full"			}
};
static int scsi_StatusEntrys =
		sizeof(scsi_StatusTable) / sizeof(scsi_StatusTable[0]);

char *
cdbg_ScsiStatus (u_char scsi_status, int report_format)
{
	struct SCSI_StatusTable *cst = scsi_StatusTable;
	I32 entrys;

	for (entrys = 0; entrys < scsi_StatusEntrys; cst++, entrys++) {
	    if (cst->scsi_status == scsi_status) {
		if (report_format == CDBG_BRIEF) {
		    return (cst->status_msg_brief);
		} else {
		    return (cst->status_msg_full);
		}
	    }
	}
	return ((report_format) ? "Unknown SCSI Status" : "Unknown");
}

/*
 * Sense Key Error Table.
 */
char *cdbg_SenseKeyTable[] = {
	"NO SENSE - No error or no sense information",		/* 0x00 */
	"RECOVERED ERROR - Recovery action performed",		/* 0x01 */
	"NOT READY - Logical unit is NOT ready",		/* 0x02 */
	"MEDIUM ERROR - Nonrecoverable medium error",		/* 0x03 */
	"HARDWARE ERROR - Nonrecoverable hardware error",	/* 0x04 */
	"ILLEGAL REQUEST - Illegal request or CDB parameter",	/* 0x05 */
	"UNIT ATTENTION - Medium changed or target reset",	/* 0x06 */
	"DATA PROTECT - Data protected from this operation",	/* 0x07 */
	"BLANK CHECK - No-data condition occured",		/* 0x08 */
	"Vendor Specific",					/* 0x09 */
	"COPY ABORTED - Copy command was aborted",		/* 0x0a */
	"ABORTED COMMAND - Target aborted command",		/* 0x0b */
	"EQUAL - Search satisfied with equal comparison",	/* 0x0c */
	"VOLUME OVERFLOW - Physical end of media detected",	/* 0x0d */
	"MISCOMPARE - Source and medium data differ",		/* 0x0e */
	"RESERVED",						/* 0x0f */
};

/*
 * Sense Code/Qualifier Table:
 */
struct sense_entry SenseCodeTable[] = {
/*
                     Table 7-41: ASC and ASCQ Assignments
--------------------------------------------------------------------------

                 ASC AND ASCQ ASSIGNMENTS

      D          = Direct Access Device
       T         = Sequential Access Device
        L        = Printer Device
         P       = Processor Device
          W      = Write Once Read Multiple Device
           R     = Read Only (CD-ROM) Device
            S    = Scanner Device
             O   = Optical Memory Device
              M  = Media Changer Device
               C = Communication Device
     BYTE
   12    13     DTLPWRSOMC    DESCRIPTION
   --    --                   -------------------------------------------- */
{ 0x13, 0x00, /*D   W  O  */ "Address mark not found for data field" },
{ 0x12, 0x00, /*D   W  O  */ "Address mark not found for ID field" },
{ 0x00, 0x11, /*     R    */ "Audio play operation in progress" },
{ 0x00, 0x12, /*     R    */ "Audio play operation paused" },
{ 0x00, 0x14, /*     R    */ "Audio play operation stopped due to error" },
{ 0x00, 0x13, /*     R    */ "Audio play operation successfully completed" },
{ 0x00, 0x04, /* T    S   */ "Beginning-of-partition/medium detected" },
{ 0x14, 0x04, /* T        */ "Block sequence error" },
{ 0x30, 0x02, /*DT  WR O  */ "Cannot read medium - incompatible format" },
{ 0x30, 0x01, /*DT  WR O  */ "Cannot read medium - unknown format" },
{ 0x52, 0x00, /* T        */ "Cartridge fault" },
{ 0x3F, 0x02, /*DTLPWRSOMC*/ "Changed operating definition" },
{ 0x11, 0x06, /*    WR O  */ "Circ unrecovered error" },
{ 0x30, 0x03, /*DT        */ "Cleaning cartridge installed" },
{ 0x4A, 0x00, /*DTLPWRSOMC*/ "Command phase error" },
{ 0x2C, 0x00, /*DTLPWRSOMC*/ "Command sequence error" },
{ 0x2F, 0x00, /*DTLPWRSOMC*/ "Commands cleared by another initiator" },
{ 0x2B, 0x00, /*DTLPWRSO C*/ "Copy cannot execute since host cannot disconnect" },
{ 0x41, 0x00, /*D         */ "Data path failure (should use 40 nn)" },
{ 0x4B, 0x00, /*DTLPWRSOMC*/ "Data phase error" },
{ 0x11, 0x07, /*    W  O  */ "Data resychronization error" },
{ 0x16, 0x00, /*D   W  O  */ "Data synchronization mark error" },
{ 0x19, 0x00, /*D      O  */ "Defect list error" },
{ 0x19, 0x03, /*D      O  */ "Defect list error in grown list" },
{ 0x19, 0x02, /*D      O  */ "Defect list error in primary list" },
{ 0x19, 0x01, /*D      O  */ "Defect list not available" },
{ 0x1C, 0x00, /*D      O  */ "Defect list not found" },
{ 0x32, 0x01, /*D   W  O  */ "Defect list update failure" },
#if 0
{ 0x40, 0xNN, /*DTLPWRSOMC*/ "Diagnostic failure on component nn (80h-ffh)" },
#endif
{ 0x40, 0x00, /*DTLPWRSOMC*/ "Diagnostic failure on component 0 (80h-ffh)" },
{ 0x63, 0x00, /*     R    */ "End of user area encountered on this track" },
{ 0x00, 0x05, /* T    S   */ "End-of-data detected" },
{ 0x14, 0x03, /* T        */ "End-of-data not found" },
{ 0x00, 0x02, /* T    S   */ "End-of-partition/medium detected" },
{ 0x51, 0x00, /* T     O  */ "Erase failure" },
{ 0x0A, 0x00, /*DTLPWRSOMC*/ "Error log overflow" },
{ 0x11, 0x02, /*DT  W SO  */ "Error too long to correct" },
{ 0x03, 0x02, /* T        */ "Excessive write errors" },
{ 0x3B, 0x07, /*  L       */ "Failed to sense bottom-of-form" },
{ 0x3B, 0x06, /*  L       */ "Failed to sense top-of-form" },
{ 0x00, 0x01, /* T        */ "Filemark detected" },
{ 0x14, 0x02, /* T        */ "Filemark or setmark not found" },
{ 0x09, 0x02, /*    WR O  */ "Focus servo failure" },
{ 0x31, 0x01, /*D L    O  */ "Format command failed" },
{ 0x58, 0x00, /*       O  */ "Generation does not exist" },
{ 0x1C, 0x02, /*D      O  */ "Grown defect list not found" },
{ 0x00, 0x06, /*DTLPWRSOMC*/ "I/O process terminated" },
{ 0x10, 0x00, /*D   W  O  */ "Id CRC or ECC error" },
{ 0x22, 0x00, /*D         */ "Illegal function (should use 20 00, 24 00, or 26 00)" },
{ 0x64, 0x00, /*     R    */ "Illegal mode for this track" },
{ 0x28, 0x01, /*        M */ "Import or export element accessed" },
{ 0x30, 0x00, /*DT  WR OM */ "Incompatible medium installed" },
{ 0x11, 0x08, /* T        */ "Incomplete block read" },
{ 0x48, 0x00, /*DTLPWRSOMC*/ "Initiator detected error message received" },
{ 0x3F, 0x03, /*DTLPWRSOMC*/ "Inquiry data has changed" },
{ 0x44, 0x00, /*DTLPWRSOMC*/ "Internal target failure" },
{ 0x3D, 0x00, /*DTLPWRSOMC*/ "Invalid bits in identify message" },
{ 0x2C, 0x02, /*      S   */ "Invalid combination of windows specified" },
{ 0x20, 0x00, /*DTLPWRSOMC*/ "Invalid command operation code" },
{ 0x21, 0x01, /*        M */ "Invalid element address" },
{ 0x24, 0x00, /*DTLPWRSOMC*/ "Invalid field in CDB" },
{ 0x26, 0x00, /*DTLPWRSOMC*/ "Invalid field in parameter list" },
{ 0x49, 0x00, /*DTLPWRSOMC*/ "Invalid message error" },
{ 0x11, 0x05, /*    WR O  */ "L-ec uncorrectable error" },
{ 0x60, 0x00, /*      S   */ "Lamp failure" },
{ 0x5B, 0x02, /*DTLPWRSOM */ "Log counter at maximum" },
{ 0x5B, 0x00, /*DTLPWRSOM */ "Log exception" },
{ 0x5B, 0x03, /*DTLPWRSOM */ "Log list codes exhausted" },
{ 0x2A, 0x02, /*DTL WRSOMC*/ "Log parameters changed" },
{ 0x21, 0x00, /*DT  WR OM */ "Logical block address out of range" },
{ 0x08, 0x00, /*DTL WRSOMC*/ "Logical unit communication failure" },
{ 0x08, 0x02, /*DTL WRSOMC*/ "Logical unit communication parity error" },
{ 0x08, 0x01, /*DTL WRSOMC*/ "Logical unit communication time-out" },
{ 0x05, 0x00, /*DTL WRSOMC*/ "Logical unit does not respond to selection" },
{ 0x4C, 0x00, /*DTLPWRSOMC*/ "Logical unit failed self-configuration" },
{ 0x3E, 0x00, /*DTLPWRSOMC*/ "Logical unit has not self-configured yet" },
{ 0x04, 0x01, /*DTLPWRSOMC*/ "Logical unit is in process of becoming ready" },
{ 0x04, 0x00, /*DTLPWRSOMC*/ "Logical unit not ready, cause not reportable" },
{ 0x04, 0x04, /*DTL    O  */ "Logical unit not ready, format in progress" },
{ 0x04, 0x02, /*DTLPWRSOMC*/ "Logical unit not ready, initializing command required" },
{ 0x04, 0x03, /*DTLPWRSOMC*/ "Logical unit not ready, manual intervention required" },
{ 0x25, 0x00, /*DTLPWRSOMC*/ "Logical unit not supported" },
{ 0x15, 0x01, /*DTL WRSOM */ "Mechanical positioning error" },
{ 0x53, 0x00, /*DTL WRSOM */ "Media load or eject failed" },
{ 0x3B, 0x0D, /*        M */ "Medium destination element full" },
{ 0x31, 0x00, /*DT  W  O  */ "Medium format corrupted" },
{ 0x3A, 0x00, /*DTL WRSOM */ "Medium not present" },
{ 0x53, 0x02, /*DT  WR OM */ "Medium removal prevented" },
{ 0x3B, 0x0E, /*        M */ "Medium source element empty" },
{ 0x43, 0x00, /*DTLPWRSOMC*/ "Message error" },
{ 0x3F, 0x01, /*DTLPWRSOMC*/ "Microcode has been changed" },
{ 0x1D, 0x00, /*D   W  O  */ "Miscompare during verify operation" },
{ 0x11, 0x0A, /*DT     O  */ "Miscorrected error" },
{ 0x2A, 0x01, /*DTL WRSOMC*/ "Mode parameters changed" },
{ 0x07, 0x00, /*DTL WRSOM */ "Multiple peripheral devices selected" },
{ 0x11, 0x03, /*DT  W SO  */ "Multiple read errors" },
{ 0x00, 0x00, /*DTLPWRSOMC*/ "No additional sense information" },
{ 0x00, 0x15, /*     R    */ "No current audio status to return" },
{ 0x32, 0x00, /*D   W  O  */ "No defect spare location available" },
{ 0x11, 0x09, /* T        */ "No gap found" },
{ 0x01, 0x00, /*D   W  O  */ "No index/sector signal" },
{ 0x06, 0x00, /*D   WR OM */ "No reference position found" },
{ 0x02, 0x00, /*D   WR OM */ "No seek complete" },
{ 0x03, 0x01, /* T        */ "No write current" },
{ 0x28, 0x00, /*DTLPWRSOMC*/ "Not ready to ready transition (medium may have changed)" },
{ 0x5A, 0x01, /*DT  WR OM */ "Operator medium removal request" },
{ 0x5A, 0x00, /*DTLPWRSOM */ "Operator request or state change input (unspecified)" },
{ 0x5A, 0x03, /*DT  W  O  */ "Operator selected write permit" },
{ 0x5A, 0x02, /*DT  W  O  */ "Operator selected write protect" },
{ 0x61, 0x02, /*      S   */ "Out of focus" },
{ 0x4E, 0x00, /*DTLPWRSOMC*/ "Overlapped commands attempted" },
{ 0x2D, 0x00, /* T        */ "Overwrite error on update in place" },
{ 0x3B, 0x05, /*  L       */ "Paper jam" },
{ 0x1A, 0x00, /*DTLPWRSOMC*/ "Parameter list length error" },
{ 0x26, 0x01, /*DTLPWRSOMC*/ "Parameter not supported" },
{ 0x26, 0x02, /*DTLPWRSOMC*/ "Parameter value invalid" },
{ 0x2A, 0x00, /*DTL WRSOMC*/ "Parameters changed" },
{ 0x03, 0x00, /*DTL W SO  */ "Peripheral device write fault" },
{ 0x50, 0x02, /* T        */ "Position error related to timing" },
{ 0x3B, 0x0C, /*      S   */ "Position past beginning of medium" },
{ 0x3B, 0x0B, /*      S   */ "Position past end of medium" },
{ 0x15, 0x02, /*DT  WR O  */ "Positioning error detected by read of medium" },
{ 0x29, 0x00, /*DTLPWRSOMC*/ "Power on, reset, or bus device reset occurred" },
{ 0x42, 0x00, /*D         */ "Power-on or self-test failure (should use 40 nn)" },
{ 0x1C, 0x01, /*D      O  */ "Primary defect list not found" },
{ 0x40, 0x00, /*D         */ "Ram failure (should use 40 nn)" },
{ 0x15, 0x00, /*DTL WRSOM */ "Random positioning error" },
{ 0x3B, 0x0A, /*      S   */ "Read past beginning of medium" },
{ 0x3B, 0x09, /*      S   */ "Read past end of medium" },
{ 0x11, 0x01, /*DT  W SO  */ "Read retries exhausted" },
{ 0x14, 0x01, /*DT  WR O  */ "Record not found" },
{ 0x14, 0x00, /*DTL WRSO  */ "Recorded entity not found" },
{ 0x18, 0x02, /*D   WR O  */ "Recovered data - data auto-reallocated" },
{ 0x18, 0x05, /*D   WR O  */ "Recovered data - recommend reassignment" },
{ 0x17, 0x05, /*D   WR O  */ "Recovered data using previous sector ID" },
{ 0x18, 0x03, /*     R    */ "Recovered data with CIRC" },
{ 0x18, 0x01, /*D   WR O  */ "Recovered data with error correction and retries applied" },
{ 0x18, 0x00, /*DT  WR O  */ "Recovered data with error correction applied" },
{ 0x18, 0x04, /*     R    */ "Recovered data with LEC" },
{ 0x17, 0x03, /*DT  WR O  */ "Recovered data with negative head offset" },
{ 0x17, 0x00, /*DT  WRSO  */ "Recovered data with no error correction applied" },
{ 0x17, 0x02, /*DT  WR O  */ "Recovered data with positive head offset" },
{ 0x17, 0x01, /*DT  WRSO  */ "Recovered data with retries" },
{ 0x17, 0x04, /*    WR O  */ "Recovered data with retries and/or CIRC applied" },
{ 0x17, 0x06, /*D   W  O  */ "Recovered data without ECC - data auto-reallocated" },
{ 0x17, 0x07, /*D   W  O  */ "Recovered data without ECC - recommend reassignment" },
{ 0x1E, 0x00, /*D   W  O  */ "Recovered ID with ECC correction" },
{ 0x3B, 0x08, /* T        */ "Reposition error" },
{ 0x36, 0x00, /*  L       */ "Ribbon, ink, or toner failure" },
{ 0x37, 0x00, /*DTL WRSOMC*/ "Rounded parameter" },
{ 0x5C, 0x00, /*D      O  */ "Rpl status change" },
{ 0x39, 0x00, /*DTL WRSOMC*/ "Saving parameters not supported" },
{ 0x62, 0x00, /*      S   */ "Scan head positioning error" },
{ 0x47, 0x00, /*DTLPWRSOMC*/ "Scsi parity error" },
{ 0x54, 0x00, /*   P      */ "Scsi to host system interface failure" },
{ 0x45, 0x00, /*DTLPWRSOMC*/ "Select or reselect failure" },
{ 0x3B, 0x00, /* TL       */ "Sequential positioning error" },
{ 0x00, 0x03, /* T        */ "Setmark detected" },
{ 0x3B, 0x04, /*  L       */ "Slew failure" },
{ 0x09, 0x03, /*    WR O  */ "Spindle servo failure" },
{ 0x5C, 0x02, /*D      O  */ "Spindles not synchronized" },
{ 0x5C, 0x01, /*D      O  */ "Spindles synchronized" },
{ 0x1B, 0x00, /*DTLPWRSOMC*/ "Synchronous data transfer error" },
{ 0x55, 0x00, /*   P      */ "System resource failure" },
{ 0x33, 0x00, /* T        */ "Tape length error" },
{ 0x3B, 0x03, /*  L       */ "Tape or electronic vertical forms unit not ready" },
{ 0x3B, 0x01, /* T        */ "Tape position error at beginning-of-medium" },
{ 0x3B, 0x02, /* T        */ "Tape position error at end-of-medium" },
{ 0x3F, 0x00, /*DTLPWRSOMC*/ "Target operating conditions have changed" },
{ 0x5B, 0x01, /*DTLPWRSOM */ "Threshold condition met" },
{ 0x26, 0x03, /*DTLPWRSOMC*/ "Threshold parameters not supported" },
{ 0x2C, 0x01, /*      S   */ "Too many windows specified" },
{ 0x09, 0x00, /*DT  WR O  */ "Track following error" },
{ 0x09, 0x01, /*    WR O  */ "Tracking servo failure" },
{ 0x61, 0x01, /*      S   */ "Unable to acquire video" },
{ 0x57, 0x00, /*     R    */ "Unable to recover table-of-contents" },
{ 0x53, 0x01, /* T        */ "Unload tape failure" },
{ 0x11, 0x00, /*DT  WRSO  */ "Unrecovered read error" },
{ 0x11, 0x04, /*D   W  O  */ "Unrecovered read error - auto reallocate failed" },
{ 0x11, 0x0B, /*D   W  O  */ "Unrecovered read error - recommend reassignment" },
{ 0x11, 0x0C, /*D   W  O  */ "Unrecovered read error - recommend rewrite the data" },
{ 0x46, 0x00, /*DTLPWRSOMC*/ "Unsuccessful soft reset" },
{ 0x59, 0x00, /*       O  */ "Updated block read" },
{ 0x61, 0x00, /*      S   */ "Video acquisition error" },
{ 0x50, 0x00, /* T        */ "Write append error" },
{ 0x50, 0x01, /* T        */ "Write append position error" },
{ 0x0C, 0x00, /* T    S   */ "Write error" },
{ 0x0C, 0x02, /*D   W  O  */ "Write error - auto reallocation failed" },
{ 0x0C, 0x01, /*D   W  O  */ "Write error recovered with auto reallocation" },
{ 0x27, 0x00, /*DT  W  O  */ "Write protected" },
/*=================================================================================+
 | SCSI-3 Additions:                                                               |
 |                                                                                 |
 |              D - DIRECT ACCESS DEVICE (SBC)                Device column key    |
 |              .T - SEQUENTIAL ACCESS DEVICE (SSC)              blank = reserved  |
 |              . L - PRINTER DEVICE (SSC)                    not blank = allowed  |
 |              .  P - PROCESSOR DEVICE (SPC)                                      |
 |              .  .W - WRITE ONCE READ MULTIPLE DEVICE (SBC)                      |
 |              .  . R - CD DEVICE (MMC)                                           |
 |              .  .  S - SCANNER DEVICE (SGC)                                     |
 |              .  .  .O - OPTICAL MEMORY DEVICE (SBC)                             |
 |              .  .  . M - MEDIA CHANGER DEVICE (SMC)                             |
 |              .  .  .  C - COMMUNICATION DEVICE (SSC)                            |
 |              .  .  .  .A - STORAGE ARRAY DEVICE (SCC)                           |
 |              .  .  .  . E - ENCLOSURE SERVICES DEVICE (SES)                     |
 |              .  .  .  .                                                         |
 | ASC ASCQ     DTLPWRSOMCAE    Description                                        |
 |---------+-----------------+-----------------------------------------------------|
 */
{ 0x00, 0x16, /*DTLPWRSOMCAE*/ "Operation in progress" },
{ 0x00, 0x17, /*DTL WRSOM AE*/ "Cleaning requested" },
{ 0x04, 0x05, /*          A */ "Logical unit not ready, rebuild in progress" },
{ 0x04, 0x06, /*          A */ "Logical unit not ready, recalculation in progress" },
{ 0x04, 0x07, /*DTLPWRSOMCAE*/ "Logical unit not ready, operation in progress" },
{ 0x04, 0x08, /*     R      */ "Logical unit not ready, long write in progress" },
{ 0x08, 0x03, /*DT  R OM    */ "Logical unit communication CRC error (ULTRA-DMA/32)" },
{ 0x09, 0x04, /*DT  WR O    */ "Head select fault" },
{ 0x0B, 0x00, /*DTLPWRSOMCAE*/ "Warning" },
{ 0x0B, 0x01, /*DTLPWRSOMCAE*/ "Warning - specified temperature exceeded" },
{ 0x0B, 0x02, /*DTLPWRSOMCAE*/ "Warning - enclosure degraded" },
{ 0x0C, 0x03, /*D   W  O    */ "Write error - recommend reassignment" },
{ 0x0C, 0x04, /*DT  W  O    */ "Compression check miscompare error" },
{ 0x0C, 0x05, /*DT  W  O    */ "Data expansion occurred during compression" },
{ 0x0C, 0x06, /*DT  W  O    */ "Block not compressible" },
{ 0x0C, 0x07, /*     R      */ "Write error - recovery needed" },
{ 0x0C, 0x08, /*     R      */ "Write error - recovery failed" },
{ 0x0C, 0x09, /*     R      */ "Write error - loss of streaming" },
{ 0x0C, 0x0A, /*     R      */ "Write error - padding blocks added" },
{ 0x11, 0x0D, /*DT  WR O    */ "De-compression CRC error" },
{ 0x11, 0x0E, /*DT  WR O    */ "Cannot decompress using declared algorithm" },
{ 0x11, 0x0F, /*     R      */ "Error reading UPC/EAN number" },
{ 0x11, 0x10, /*     R      */ "Error reading ISRC number" },
{ 0x11, 0x11, /*     R      */ "Read error - loss of streaming" },
{ 0x14, 0x05, /*DT  W  O    */ "Record not found - recommend reassignment" },
{ 0x14, 0x06, /*DT  W  O    */ "Record not found - data auto-reallocated" },
{ 0x16, 0x01, /*D   W  O    */ "Data sync error - data rewritten" },
{ 0x16, 0x02, /*D   W  O    */ "Data sync error - recommend rewrite" },
{ 0x16, 0x03, /*D   W  O    */ "Data sync error - data auto-reallocated" },
{ 0x16, 0x04, /*D   W  O    */ "Data sync error - recommend reassignment" },
{ 0x17, 0x08, /*D   W  O    */ "Recovered data without ECC - recommend rewrite" },
{ 0x17, 0x09, /*D   W  O    */ "Recovered data without ECC - data rewritten" },
{ 0x18, 0x06, /*D   WR O    */ "Recovered data - recommend rewrite" },
{ 0x18, 0x07, /*D   W  O    */ "Recovered data with ECC - data rewritten" },
{ 0x1F, 0x00, /*D      O    */ "Partial defect list transfer" },
{ 0x26, 0x04, /*DTLPWRSOMCAE*/ "Invalid release of active persistent reservation" },
{ 0x27, 0x01, /*DT  W  O    */ "Hardware write protected" },
{ 0x27, 0x02, /*DT  W  O    */ "Logical unit software write protected" },
{ 0x27, 0x03, /* T          */ "Associated write protect" },
{ 0x27, 0x04, /* T          */ "Persistent write protect" },
{ 0x27, 0x05, /* T          */ "Permanent write protect" },
{ 0x29, 0x01, /*DTLPWRSOMCAE*/ "Power on occurred" },
{ 0x29, 0x02, /*DTLPWRSOMCAE*/ "SCSI bus reset occurred" },
{ 0x29, 0x03, /*DTLPWRSOMCAE*/ "Bus device reset function occurred" },
{ 0x29, 0x04, /*DTLPWRSOMCAE*/ "Device internal reset" },
{ 0x2A, 0x03, /*DTLPWRSOMCAE*/ "Reservations preempted" },
{ 0x2C, 0x03, /*     R      */ "Current program area is not empty" },
{ 0x2C, 0x04, /*     R      */ "Current program area is empty" },
{ 0x30, 0x04, /*DT  WR O    */ "Cannot write medium - unknown format" },
{ 0x30, 0x05, /*DT  WR O    */ "Cannot write medium - incompatible format" },
{ 0x30, 0x06, /*DT  W  O    */ "Cannot format medium - incompatible medium" },
{ 0x30, 0x07, /*DTL WRSOM AE*/ "Cleaning failure" },
{ 0x30, 0x08, /*     R      */ "Cannot write - application code mismatch" },
{ 0x30, 0x09, /*     R      */ "Current session not fixated for append" },
{ 0x34, 0x00, /*DTLPWRSOMCAE*/ "Enclosure failure" },
{ 0x35, 0x00, /*DTLPWRSOMCAE*/ "Enclosure services failure" },
{ 0x35, 0x01, /*DTLPWRSOMCAE*/ "Unsupported enclosure function" },
{ 0x35, 0x02, /*DTLPWRSOMCAE*/ "Enclosure services unavailable" },
{ 0x35, 0x03, /*DTLPWRSOMCAE*/ "Enclosure services transfer failure" },
{ 0x35, 0x04, /*DTLPWRSOMCAE*/ "Enclosure services transfer refused" },
{ 0x3A, 0x01, /*DT  WR OM   */ "Medium not present - tray closed" },
{ 0x3A, 0x02, /*DT  WR OM   */ "Medium not present - tray open" },
{ 0x3B, 0x0F, /*     R      */ "End of medium reached" },
{ 0x3B, 0x11, /*DT  WR OM   */ "Medium magazine not accessible" },
{ 0x3B, 0x12, /*DT  WR OM   */ "Medium magazine removed" },
{ 0x3B, 0x13, /*DT  WR OM   */ "Medium magazine inserted" },
{ 0x3B, 0x14, /*DT  WR OM   */ "Medium magazine locked" },
{ 0x3B, 0x15, /*DT  WR OM   */ "Medium magazine unlocked" },
{ 0x3E, 0x01, /*          A */ "Logical unit failure" },
{ 0x3E, 0x02, /*          A */ "Timeout on logical unit" },
{ 0x55, 0x01, /*D      O    */ "System buffer full" },
{ 0x5D, 0x00, /*DTLPWRSOMCAE*/ "Failure prediction threshold exceeded" },
{ 0x5D, 0xFF, /*DTLPWRSOMCAE*/ "Failure prediction threshold exceeded (false)" },
{ 0x5E, 0x00, /*DTLPWRSO CA */ "Low power condition on" },
{ 0x5E, 0x01, /*DTLPWRSO CA */ "Idle condition activated by timer" },
{ 0x5E, 0x02, /*DTLPWRSO CA */ "Standby condition activated by timer" },
{ 0x5E, 0x03, /*DTLPWRSO CA */ "Idle condition activated by command" },
{ 0x5E, 0x04, /*DTLPWRSO CA */ "Standby condition activated by command" },
{ 0x63, 0x01, /*     R      */ "Packet does not fit in available space" },
{ 0x64, 0x01, /*     R      */ "Invalid packet size" },
{ 0x65, 0x00, /*DTLPWRSOMCAE*/ "Voltage fault" },
{ 0x66, 0x00, /*      S     */ "Automatic document feeder cover up" },
{ 0x66, 0x01, /*      S     */ "Automatic document feeder lift up" },
{ 0x66, 0x02, /*      S     */ "Document jam in automatic document feeder" },
{ 0x66, 0x03, /*      S     */ "Document miss feed automatic in document feeder" },
{ 0x67, 0x00, /*          A */ "Configuration failure" },
{ 0x67, 0x01, /*          A */ "Configuration of incapable logical units failed" },
{ 0x67, 0x02, /*          A */ "Add logical unit failed" },
{ 0x67, 0x03, /*          A */ "Modification of logical unit failed" },
{ 0x67, 0x04, /*          A */ "Exchange of logical unit failed" },
{ 0x67, 0x05, /*          A */ "Remove of logical unit failed" },
{ 0x67, 0x06, /*          A */ "Attachment of logical unit failed" },
{ 0x67, 0x07, /*          A */ "Creation of logical unit failed" },
{ 0x68, 0x00, /*          A */ "Logical unit not configured" },
{ 0x69, 0x00, /*          A */ "Data loss on logical unit" },
{ 0x69, 0x01, /*          A */ "Multiple logical unit failures" },
{ 0x69, 0x02, /*          A */ "Parity/data mismatch" },
{ 0x6A, 0x00, /*          A */ "Informational, refer to log" },
{ 0x6B, 0x00, /*          A */ "State change has occurred" },
{ 0x6B, 0x01, /*          A */ "Redundancy level got better" },
{ 0x6B, 0x02, /*          A */ "Redundancy level got worse" },
{ 0x6C, 0x00, /*          A */ "Rebuild failure occurred" },
{ 0x6D, 0x00, /*          A */ "Recalculate failure occurred" },
{ 0x6E, 0x00, /*          A */ "Command to logical unit failed" },
#if 0
{ 0x70, 0xNN, /* T          */ "Decompression exception short algorithm ID of NN" },
#endif
{ 0x71, 0x00, /* T          */ "Decompression exception long algorithm id" },
{ 0x72, 0x00, /*     R      */ "Session fixation error" },
{ 0x72, 0x01, /*     R      */ "Session fixation error writing lead-in" },
{ 0x72, 0x02, /*     R      */ "Session fixation error writing lead-out" },
{ 0x72, 0x03, /*     R      */ "Session fixation error - incomplete track in session" },
{ 0x72, 0x04, /*     R      */ "Empty or partially written reserved track" },
{ 0x73, 0x00, /*     R      */ "CD control error" },
{ 0x73, 0x01, /*     R      */ "Power calibration area almost full" },
{ 0x73, 0x02, /*     R      */ "Power calibration area is full" },
{ 0x73, 0x03, /*     R      */ "Power calibration area error" },
{ 0x73, 0x04, /*     R      */ "Program memory area update failure" },
{ 0x73, 0x05, /*     R      */ "program memory area is full" }
};
int SenseCodeEntrys = sizeof(SenseCodeTable) / sizeof(struct sense_entry);

/*
 * Function to Find Additional Sense Code/Qualifier Message.
 */
char *
cdbg_SenseMessage (struct all_req_sns_data *sdp)
{
      struct sense_entry *se;
      int entrys = 0;

      for (se = SenseCodeTable; entrys < SenseCodeEntrys; se++, entrys++) {
	if ( (se->sense_code == sdp->asc) &&
	     (se->sense_qualifier == sdp->asq) ) {
	    return (se->sense_message);
	}
      }
      return ((char *) 0);
}

/*
 * EEI Status Code Table.
 */
struct EEI_StatusTable {
	u_int	eei_status;
	caddr_t	status_msg_brief;
	caddr_t	status_msg_full;
} eei_StatusTable[] = {
    {	EEI_NO_STATUS,		"EEI_NO_STATUS",
				"No EEI status available"		},
    {	EEI_INVALID_PARAM,	"EEI_INVALID_PARAM",
				"Invalid software parameter provided"	},
    {	EEI_MALLOC_FAILURE,	"EEI_MALLOC_FAILURE",
				"Memory allocation failed"		},
    {	EEI_CNTBUSY_FAILURE,	"EEI_CNTBUSY_FAILURE",
				"HBA/Controller Busy (retriable)"	},
    {	EEI_GROSSSW_FAILURE,	"EEI_GROSSSW_FAILURE",
				"Gross internal software error"		},
    {	EEI_DEVBUSY_FAILURE,	"EEI_DEVBUSY_FAILURE",
				"Device is busy (retriable)"		},
    {	EEI_DEVBIO_FAILURE,	"EEI_DEVBIO_FAILURE",
				"Device/bus I/O error (retriable)"	},
    {	EEI_DEVSOFT_FAILURE,	"EEI_DEVSOFT_FAILURE",
				"Device medium error (soft error)"	},
    {	EEI_TAPE_POS_LOST,	"EEI_TAPE_POS_LOST",
				"Tape position lost"			},
    {	EEI_DEVREQ_FAILURE,	"EEI_DEVREQ_FAILURE",
				"Device request illegal/unsupported"	},
    {	EEI_DEVSTATE_FAILURE,	"EEI_DEVSTATE_FAILURE",
				"Device inoperable (needs initialize)"	},
    {	EEI_DEVHARD_FAILURE,	"EEI_DEVHARD_FAILURE",
				"Device hardware error (hard error)"	},
    {	EEI_ABORTIO_FAILURE,	"EEI_ABORTIO_FAILURE",
				"Abort/Timed out I/O occured"		},
    {	EEI_CNTRLR_FAILURE,	"EEI_CNTRLR_FAILURE",
				"HBA/Controller failure (retriable)"	},
    {	EEI_CXALLOC_FAILURE,	"EEI_CXALLOC_FAILURE",
				"Shared Device allocation failed"	},
    {	EEI_RESERVE_PENDING,	"EEI_RESERVE_PENDING",
				"Shared Device Re-reserve pending"	},
    {	EEI_DEVPATH_FAILURE,	"EEI_DEVPATH_FAILURE",
				"Device not there (powered off?)"	},
    {	EEI_DEVPATH_RESET,	"EEI_DEVPATH_RESET",
				"Device/bus reset occured"		},
    {	EEI_DEVPATH_CONFLICT,	"EEI_DEVPATH_CONFLICT",
				"Device access denied (path conflict)"	}
};
static int eei_StatusEntrys =
		sizeof(eei_StatusTable) / sizeof(eei_StatusTable[0]);

char *
cdbg_EEIStatus (u_int eei_status, int report_format)
{
	struct EEI_StatusTable *est = eei_StatusTable;
	int entrys;

	for (entrys = 0; entrys < eei_StatusEntrys; est++, entrys++) {
	    if (est->eei_status == eei_status) {
		if (report_format == CDBG_BRIEF) {
		    return (est->status_msg_brief);
		} else {
		    return (est->status_msg_full);
		}
	    }
	}
	return ((report_format) ? "Unknown Status" : "Unknown");
}
#endif /* defined(EEI) */
