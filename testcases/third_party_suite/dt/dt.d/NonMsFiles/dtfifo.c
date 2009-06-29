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
 * Module:	dtfifo.c
 * Author:	Robin T. Miller
 * Date:	September 15, 1993
 *
 * Description:
 *	Named pipes (FIFO) test functions for the 'dt' program.
 *
 * Modification History:
 *
 * April 28, 1998 by Robin Miller.
 *	For WIN32/NT, or in O_BINARY into open flags to force binary
 *	mode (the default is text mode).
 *
 */
#include "dt.h"
#include <fcntl.h>

/*
 * Declare the FIFO (named pipes) test functions.
 *
 * NOTE:  Since loopback testing is currently done via two processes,
 *	  I've purposely omitted the reopen() function to avoid race
 *	  conditions (writer may reopen() & write before reader, etc).
 */
struct dtfuncs fifo_funcs = {
    /*	tf_open,		tf_close,		tf_initialize,	  */
	fifo_open,		close_file,		initialize,
    /*  tf_start_test,		tf_end_test,				  */
	init_file,		nofunc,
    /*	tf_read_file,		tf_read_data,		tf_cancel_reads,  */
	read_file,		read_data,		nofunc,
    /*	tf_write_file,		tf_write_data,		tf_cancel_writes, */
	write_file,		write_data,		nofunc,
    /*	tf_flush_data,		tf_verify_data,		tf_reopen_file,   */
	flush_file,		verify_data,		reopen_file,
    /*	tf_startup,		tf_cleanup,		tf_validate_opts  */
	nofunc,			nofunc,			nofunc
};

/************************************************************************
 *									*
 * fifo_open() - Open a FIFO (named pipe) for read/write access.	*
 *									*
 * Description:								*
 *	This function does the FIFO specific open processing.		*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		oflags = The device/file open flags.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
fifo_open (struct dinfo *dip, int oflags)
{
	int status;

	/*
	 * For named pipes (FIFO's), we must open with the non-blocking
	 * flag, or else the open() blocks waiting for a reader/writer.
	 */
	if (loopback) {
	    oflags |= O_NONBLOCK;
	}

#if defined(__WIN32__)
	oflags |= O_BINARY;
#endif /* defined(__WIN32__) */
	if ((status = open_file (dip, oflags)) == FAILURE) {
	    return (status);
	}

	/*
	 * For named pipes (FIFO's), we must reset non-blocking flag
	 * or else I/O requests will fail with EAGAIN (EWOULDBLOCK).
	 */
	if (loopback) {
	    int flags;
	    if ( (flags = fcntl (dip->di_fd, F_GETFL)) == FAILURE) {
		report_error("fcntl(F_GETFL)", TRUE);
		exit (exit_status);
	    }
	    flags &= ~O_NONBLOCK;
	    if (fcntl (dip->di_fd, F_SETFL, flags) == FAILURE) {
		report_error("fcntl(F_SETFL)", TRUE);
		exit (exit_status);
	    }
	}

	return (status);
}
