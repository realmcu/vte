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
 * Module:	dttape.c
 * Author:	Robin T. Miller
 * Date:	August 20, 1993
 *
 * Description:
 *	This file contains the tape support functions.
 *
 * Modification History:
 *
 * January 15th, 2001 by Robin Miller.
 *	On errors, do not terminate() if the error limit is exceeded.
 * This way the caller can handle the failure (may expect an error :-).
 *
 * February 17th, 2000 by Robin Miller.
 *	For each tape operation, when debug is enabled, also report the
 * file and record numbers.
 *
 * January 22nd, 2000 by Robin Miller.
 *	Added support for Cygwin tape devices for Windows/NT.
 *
 * August 5th, 1999 by Robin Miller.
 *	Added tape commands for SCO UnixWare 7.
 *
 * July 29, 1999 by Robin Miller.
 *	Merge in changes made to compile on FreeBSD.
 *
 * February 25, 1999 by Robin Miller.
 *	Add support for multiple reset conditions.
 *
 * December 21, 1998 by Robin Miller.
 *	Add device information parameter to all tape functions.
 *
 * December 19, 1998 by Robin Miller.
 *	For DUNIX, display extended error information (if enabled).
 *
 * <date unknown>, by Robin Miller.
 *	Add DoWriteFileMark() function for QNX 4.22 Operating System.
 *
 */

#include "dt.h"

#if defined(_QNX_SOURCE)
#  include <sys/qioctl.h>
#elif defined(SCO)
#  include <sys/ioctl.h>
#  include <sys/tape.h>
#elif defined(__WIN32__)
#  include <sys/mtio.h>
#else /* !defined(_QNX_SOURCE) && !defined(SCO) */
#  include <sys/ioctl.h>
#  include <sys/mtio.h>
#endif /* defined(_QNX_SOURCE) */

#if !defined(_QNX_SOURCE) && !defined(SCO)
/************************************************************************
 *									*
 * DoMtOp()	Setup & Do a Magtape Operation.				*
 *									*
 * Inputs:	dip = The device information.				*
 *		cmd = The magtape command to issue.			*
 *		count = The command count (if any).			*
 *		msg = The error message for failures.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
DoMtOp (dinfo_t *dip, short cmd, daddr_t count, caddr_t msgp)
{
    struct mtop Mtop;
    struct mtop *mtop = &Mtop;

    mtop->mt_op = cmd;

    /*
     * Setup the 'mt' command count (if any).
     */
    if ((mtop->mt_count = count) < 0) {
	 mtop->mt_count = 1;
    }

    if (debug_flag) {
	Fprintf(
	"Issuing '%s', count = %d (%#x) [file #%lu, record #%lu]\n",
			msgp,  mtop->mt_count, mtop->mt_count,
		(dip->di_mode == READ_MODE) ?
		 (dip->di_files_read + 1) : (dip->di_files_written + 1),
		(dip->di_mode == READ_MODE) ?
		 dip->di_records_read : dip->di_records_written);
    }
    return (DoIoctl (dip, MTIOCTOP, (caddr_t) mtop, msgp));
}

/************************************************************************
 *									*
 * DoXXXXX()	Setup & Do Specific Magtape Operations.			*
 *									*
 * Description:								*
 *	These functions provide a simplistic interface for issuing	*
 * magtape commands from within the program.  They all take 'count'	*
 * as an argument, except for those which do not take a count.		*
 *									*
 * Inputs:	fd = The file descriptor.				*
 *		count = The command count (if any).			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
DoForwardSpaceFile (dinfo_t *dip, daddr_t count)
{
	short cmd = MTFSF;
	return (DoMtOp (dip, cmd, count, "forward space file"));
}

int
DoBackwardSpaceFile (dinfo_t *dip, daddr_t count)
{
	short cmd = MTBSF;
	return (DoMtOp (dip, cmd, count, "backward space file"));
}

int
DoForwardSpaceRecord (dinfo_t *dip, daddr_t count)
{
	short cmd = MTFSR;
	return (DoMtOp (dip, cmd, count, "forward space record"));
}

int
DoBackwardSpaceRecord (dinfo_t *dip, daddr_t count)
{
	short cmd = MTBSR;
	return (DoMtOp (dip, cmd, count, "backward space record"));
}

int
DoRewindTape (dinfo_t *dip)
{
	short cmd = MTREW;
	return (DoMtOp (dip, cmd, (daddr_t) 0, "rewind tape"));
}

int
DoTapeOffline (dinfo_t *dip)
{
	short cmd = MTOFFL;
	return (DoMtOp (dip, cmd, (daddr_t) 0, "tape offline"));
}

#if !defined(OSFMK)
int
DoRetensionTape (dinfo_t *dip)
{
#if defined(FreeBSD)
	short cmd = MTRETENS;
#else /* !defined(FreeBSD) */
	short cmd = MTRETEN;
#endif /* defined(FreeBSD) */
	return (DoMtOp (dip, cmd, (daddr_t) 0, "retension tape"));
}
#endif /* !defined(OSFMK) */

#if defined(__osf__)			/* Really DEC specific. */

int
DoSpaceEndOfData (dinfo_t *dip)
{
	short cmd = MTSEOD;
	return (DoMtOp (dip, cmd, (daddr_t) 0, "space to end of data"));
}

int
DoEraseTape (dinfo_t *dip)
{
	short cmd = MTERASE;
	return (DoMtOp (dip, cmd, (daddr_t) 0, "erase tape"));
}

int
DoTapeOnline (dinfo_t *dip)
{
	short cmd = MTONLINE;
	return (DoMtOp (dip, cmd, (daddr_t) 0, "tape online"));
}

int
DoLoadTape (dinfo_t *dip)
{
	short cmd = MTLOAD;
	return (DoMtOp (dip, cmd, (daddr_t) 0, "load tape"));
}

int
DoUnloadTape (dinfo_t *dip)
{
	short cmd = MTUNLOAD;
	return (DoMtOp (dip, cmd, (daddr_t) 0, "unload tape"));
}

#endif /* defined(__osf__) */

#if defined(sun)

int
DoSpaceEndOfData (dinfo_t *dip)
{
	short cmd = MTEOM;
	return (DoMtOp (dip, cmd, (daddr_t) 0, "space to end of data"));
}

#endif /* defined(sun) */

int
DoWriteFileMark (dinfo_t *dip, daddr_t count)
{
	short cmd = MTWEOF;
	return (DoMtOp (dip, cmd, count, "write file mark"));
}

/************************************************************************
 *									*
 * DoIoctl()	Do An I/O Control Command.				*
 *									*
 * Description:								*
 *	This function issues the specified I/O control command to the	*
 * device driver.							*
 *									*
 * Inputs:	dip = The device information.				*
 *		cmd = The I/O control command.				*
 *		argp = The command argument to pass.			*
 *		msgp = The message to display on errors.		*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
DoIoctl (dinfo_t *dip, int cmd, caddr_t argp, caddr_t msgp)
{
    int status;

#if defined(EEI)
retry:
#endif
    status = ioctl (dip->di_fd, cmd, argp);
#if defined(EEI)
    if ( (status == FAILURE) && (errno == EIO) &&
	 (dip->di_dtype->dt_dtype == DT_TAPE) ) {
	if (eei_resets) {
	    if ( HandleTapeResets(dip) ) {
		goto retry;
	    } else if (dip->di_reset_condition) {
		return (status);	/* Resursive reset condition. */
	    }
	} else if (eei_flag) {
	    if ( !get_eei_status(dip->di_fd, dip->di_mt) ) {
		 print_mtstatus (dip->di_fd, dip->di_mt, TRUE);
	    }
	}
    }
#endif /* defined(EEI) */
    if (status == FAILURE) {
	perror (msgp);
	(void)RecordError();
    }
    return (status);
}

#elif defined(_QNX_SOURCE)
/************************************************************************
 *									*
 * DoXXXXX()	Setup & Do Specific Magtape Operations.			*
 *									*
 * Description:								*
 *	These functions provide a simplistic interface for issuing	*
 * magtape commands from within the program.  They all take 'count'	*
 * as an argument, except for those which do not take a count.		*
 *									*
 * Inputs:	dip = The device information.				*
 *		count = The command count (if any).			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
DoForwardSpaceFile (dinfo_t *dip, daddr_t count)
{
	int cmd = T_SEEK_FM;
	return (DoIoctl (dip, cmd, count, "forward space file"));
}

int
DoForwardSpaceRecord (dinfo_t *dip, daddr_t count)
{
	int cmd = T_SKIP_FWD_A_BLOCK;
	return (DoIoctl (dip, cmd, count, "forward space record"));
}

int
DoBackwardSpaceRecord (dinfo_t *dip, daddr_t count)
{
	int cmd = T_SKIP_BWD_A_BLOCK;
	return (DoIoctl (dip, cmd, count, "backward space record"));
}

int
DoRewindTape (dinfo_t *dip)
{
	int cmd = T_BOT;
	return (DoIoctl (dip, cmd, 0, "rewind tape"));
}

int
DoRetensionTape (dinfo_t *dip)
{
	int cmd = T_RETENSION;
	return (DoIoctl (dip, cmd, 0, "retension tape"));
}

int
DoSpaceEndOfData (dinfo_t *dip)
{
	int cmd = T_SEEK_EOD;
	return (DoIoctl (dip, cmd, 0, "space to end of data"));
}

int
DoEraseTape (dinfo_t *dip)
{
	int cmd = T_ERASE;
	return (DoIoctl (dip, cmd, 0, "erase tape"));
}

int
DoWriteFileMark (dinfo_t *dip, daddr_t count)
{
	int cmd = T_WRITE_FM;
	return (DoIoctl (dip, cmd, count, "write file mark"));
}

int
DoIoctl (dinfo_t *dip, int cmd, int count, caddr_t msgp)
{
	QIC02_MSG_STRUCT qic02ms;
	int status;

	if (debug_flag) {
	    Fprintf ("Issuing '%s', count = %d (%#x)\n",
					msgp,  count, count);
	}
	bzero((char *)&qic02ms, sizeof(qic02ms));
	qic02ms.Header.Command = (unsigned short)cmd;

	while (count--) {
	    status = qnx_ioctl (dip->di_fd,
				QCTL_RAW_CMD,
				&qic02ms, sizeof (QIC02_HEADER_STRUCT),
				&qic02ms, sizeof (QIC02_MSG_STRUCT));
	    if (status == FAILURE) {
		perror (msgp);
		if (RecordError() >= error_limit) {
		    return (FAILURE);
		}
	    }
	}
	return (status);
}

#elif defined(SCO)

int
DoForwardSpaceFile (dinfo_t *dip, daddr_t count)
{
	int cmd = T_SFF;
	return (DoIoctl (dip, cmd, count, "forward space file"));
}

int
DoBackwardSpaceFile (dinfo_t *dip, daddr_t count)
{
	int cmd = T_SFB;
	return (DoIoctl (dip, cmd, count, "backward space file"));
}

int
DoForwardSpaceRecord (dinfo_t *dip, daddr_t count)
{
	int cmd = T_SBF;
	return (DoIoctl (dip, cmd, count, "forward space record"));
}

int
DoBackwardSpaceRecord (dinfo_t *dip, daddr_t count)
{
	int cmd = T_SBB;
	return (DoIoctl (dip, cmd, count, "backward space record"));
}

int
DoRewindTape (dinfo_t *dip)
{
	int cmd = T_RWD;
	return (DoIoctl (dip, cmd, 0, "rewind tape"));
}

int
DoRetensionTape (dinfo_t *dip)
{
	int cmd = T_RETENSION;
	return (DoIoctl (dip, cmd, 0, "retension tape"));
}

int
DoSpaceEndOfData (dinfo_t *dip)
{
	int cmd = T_EOD;
	return (DoIoctl (dip, cmd, 0, "space to end of data"));
}

int
DoEraseTape (dinfo_t *dip)
{
	int cmd = T_ERASE;
	return (DoIoctl (dip, cmd, 0, "erase tape"));
}

int
DoWriteFileMark (dinfo_t *dip, daddr_t count)
{
	int cmd = T_WRFILEM;
	return (DoIoctl (dip, cmd, count, "write file mark"));
}

int
DoIoctl (dinfo_t *dip, int cmd, int count, caddr_t msgp)
{
	int status;

	if (debug_flag) {
	    Fprintf ("Issuing '%s', count = %d (%#x)\n",
					msgp,  count, count);
	}
	status = ioctl (dip->di_fd, cmd, count);
	if (status == FAILURE) {
	    perror (msgp);
	    (void)RecordError();
	}
	return (status);
}

#endif /* defined(SCO) */
