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
 * Module:	dttty.c
 * Author:	Robin T. Miller
 *
 * Description:
 *	Terminal line support functions for generic data test program.
 */
#include "dt.h"
#include <fcntl.h>
#if !defined(_QNX_SOURCE)
#  if !defined(sun)
#    include <sys/ioctl.h>
#  endif /* !defined(sun) */
#  include <sys/file.h>
#  include <sys/param.h>
#endif /* !defined(_QNX_SOURCE) */

/*
 * Define maximum values for VMIN & VTIME field values:
 */
#if defined(_QNX_SOURCE)
#  define VMIN_MAX	65535U
#  define VTIME_MAX	65535U
#else /* !defined(_QNX_SOURCE) */
#  define VMIN_MAX	255U
#  define VTIME_MAX	255U
#endif /* defined(_QNX_SOURCE) */

/*
 * Modification History:
 *
 * December 30th, 2000 by Robin Miller.
 *	Make changes to build using MKS/NuTCracker product.
 *
 * May 8th, 2000 by Robin Miller.
 *	Honor the di_closing flag, to avoid a race condition with the
 * close function being called again while still closing, from the
 * terminate() routine called by the runtime= alarm, or signals.
 *
 * February 17th, 2000 by Robin Miller.
 *	Adding better support for multi-volume tape testing.
 *
 * July 29, 1999 by Robin Miller.
 *	Merge in changes made to compile on FreeBSD.
 *
 * May 27, 1999 by Robin Miller.
 *	Adding support for micro-second delays.
 *
 * December 19, 1995 by Robin Miller
 *      Conditionalize for Linux Operating System.
 *
 * December 7, 1995 by Robin Miller.
 *	Conditionally support speeds of 38400, 57600, 76800, & 115200.
 *
 * December 6, 1995 by Robin Miller.
 *	When setting the VMIN and VTIME, ensure the maximum values are
 *	not exceeded or blindly truncated to an incorrect value. These
 *	fields are currently declared as unsigned char's, so 255 is the
 *	maximum value possible.  [ I've been burnt by this too... ]
 *	Added additional baud rates supported by QNX Operating System.
 *
 * July 24, 1995 by Robin Miller.
 *	When doing modem testing, after waiting for carrier, disable
 *	non-blocking mode, to prevent EWOULDBLOCK on reads & writes.
 *	[ We open'ed w/O_NONBLOCK so we could read modem signals. ]
 *
 * October 29, 1993 by Robin Miller.
 *	Add test in tty_open() to ensure the terminal characteristics
 *	only get saved once when performing loopback to same terminal.
 *	Otherwise test attributes get restored, instead of original.
 *
 * September 7, 1993 by Robin Miller.
 *	Added tty specific test functions & test function dispatch table.
 *
 * September 24, 1992 by Robin Miller.
 *	If parity is being enabled, ensure input parity checking is also
 *	enabled, since these are independent of each other.  This change
 *	will allow parity and framing errors to come back as '\0' in the
 *	input stream, instead of silently being ignored.
 *
 * September 19, 1992 by Robin Miller.
 *	Added flushing of tty input & output queue in save_tty() to
 *	prevent unwarrented data comparison errors during testing. 
 *
 * September 18, 1992 by Robin Miller.
 *	Added save_tty() & restore_tty() routines to save & restore the
 *	terminal line discipline and characteristics.  This must be done
 *	since other programs may presume a certain tty setup.
 *
 * September 11, 1992 by Robin Miller.
 *	Change 3rd parmameter of TCBRK ioctl() to -1 instead of 1, to
 *	inhibit sending of actual break sequence and only drain output.
 *	This caused 'dt' to get intermittent data compare errors (ouch!).
 *
 * September 5, 1992 by Robin Miller.
 *	Initial port to QNX 4.1 Operating System.
 *
 * August 19, 1992 by Robin Miller.
 *	Added modem support functions and modem test setup.
 *
 * August 18, 1992 by Robin Miller.
 *	Added support for ULTRIX terminal driver setup (yuck!).
 *	Also ensure tty line discipline is setup properly.
 *
 * June 5, 1992 by Robin Miller.
 *	Added setting the terminal attributes for 2 stop bits for
 *	baud rates slower than 300 baud.
 *
 * May 25, 1992 by Robin Miller.
 *	Added POSIX compliant tty commands for OSF terminal driver.
 *	Ensure CLOCAL & HUPCL do not get cleared inadvertantly.
 *
 * October 16, 1989 by Robin Miller.
 *	Added setting the ISTRIP terminal characteristic when testing
 *	with even or odd parity.  This is needed for Sun's zs driver
 *	which passes the parity bit through if this isn't set, even
 *	though we have setup for only 7 data bits (SUN's bug).
 *
 * July 25, 1989 by Robin Miller.
 *	Add functions for setting up terminal chacteristics for testing
 *	terminal ports in loopback mode.
 *
 */

#if defined(__WIN32__) && !defined(__NUTC__)
extern int tcgetattr(int , struct termios *);
extern int tcsetattr(int , int , const struct termios *);
extern int tcsendbreak(int , int );
extern int tcdrain(int );
extern int tcflush(int , int );
extern int tcflow(int , int );
#endif /* defined(__WIN32__) && !defined(__NUTC__) */

/*
 * Table to Map Baudrate to tty Speed Codes.
 */
struct tty_baud_rate baud_rate_tbl[] = {
    {	0,	B0	},
    {	50,	B50	},
    {	75,	B75	},
    {	110,	B110	},
    {	134,	B134	},
    {	150,	B150	},
    {	200,	B200	},
    {	300,	B300	},
    {	600,	B600	},
    {	1200,	B1200	},
    {	1800,	B1800	},
    {	2400,	B2400	},
    {	4800,	B4800	},
    {	9600,	B9600	},
    {	19200,	B19200	},
#if defined(B38400)
    {	38400,	B38400	},
#endif /* defined(B38400) */
#if defined(B57600)
    {	57600,	B57600	},
#endif /* defined(B57600) */
#if defined(B76800)
    {	76800,	B76800	},
#endif /* defined(B76800) */
#if defined(B115200)
    {	115200,	B115200 },
#endif /* defined(B115200) */
    {	0,	0	}
};
int num_baud_rates = (sizeof(baud_rate_tbl) / sizeof(baud_rate_tbl[0])) - 1;

char *parity_str = "none";
char *flow_str = "xon_xoff";
char *speed_str = "9600";
speed_t baud_rate_code = B9600;		/* Default baud rate is 9600.	*/
unsigned parity_code = 0;		/* Default parity = none.	*/
unsigned data_bits_code = CS8;		/* Default data bits = 8.	*/

bool tty_saved;				/* tty characteristics saved.	*/
int saved_ldisc;			/* For saving line discipline.	*/
struct termios saved_tmodes;		/* For saving terminal modes.	*/

/*
 * Declare the terminal test functions.
 */
struct dtfuncs tty_funcs = {
    /*	tf_open,		tf_close,		tf_initialize,	  */
	tty_open,		tty_close,		initialize,
    /*  tf_start_test,		tf_end_test,				  */
	nofunc,			nofunc,
    /*	tf_read_file,		tf_read_data,		tf_cancel_reads,  */
	read_file,		read_data,		nofunc,
    /*	tf_write_file,		tf_write_data,		tf_cancel_writes, */
	write_file,		write_data,		nofunc,
    /*	tf_flush_data,		tf_verify_data,		tf_reopen_file,   */
	tty_flush_data,		verify_data,		tty_reopen,
    /*	tf_startup,		tf_cleanup,		tf_validate_opts  */
	nofunc,			nofunc,			validate_opts
};

/************************************************************************
 *									*
 * tty_open() - Open a terminal device for read/write access.		*
 *									*
 * Description:								*
 *	This function does the terminal device open processing.		*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		oflags = The device/file open flags.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
tty_open (struct dinfo *dip, int oflags)
{
    int status;

    if ((status = open_file (dip, oflags)) == FAILURE) {
	return (status);
    }

#if !defined(_QNX_SOURCE) && !defined(sun) && !defined(__MSDOS__) && !defined(__WIN32__)
    if (!debug_flag && !loopback) {
	if ((status = ioctl (dip->di_fd, TIOCEXCL, 0)) == FAILURE) {
	    report_error ("TIOCEXCL", FALSE);
	    return (status);
	}
    }
#endif /* !defined(_QNX_SOURCE) */

    /*
     * Setup & optionally flush the input queue.
     */
    if (dip->di_mode == READ_MODE) {
	if ((status = save_tty (dip->di_fd)) < 0) return (status);
	if ((status = flush_tty (dip->di_fd)) < 0) return (status);
    } else if (!loopback) {
	/*
	 * If looping to ourselves, setup is already done above.
	 * [ Presumes input file opened before the output file. ]
	 */
	if ((status = save_tty (dip->di_fd)) < 0) return (status);
	if ((status = setup_tty (dip->di_fd, FALSE)) < 0) return (status);
    }
    verify_flag = FALSE;
    if ((dip->di_ftype == OUTPUT_FILE) && !loopback && (sdelay_count == 0)) {
	sdelay_count = 1;	/* Delay before write on tty. */
    }
    return (status);
}

/************************************************************************
 *									*
 * tty_close() - Close a terminal device file descriptor.		*
 *									*
 * Description:								*
 *	This function does the terminal device close processing.	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
tty_close (struct dinfo *dip)
{
    if (dip->di_closing || dip->di_fd == NoFd) {
	return (SUCCESS);		/* Closing or not open. */
    }
    if (tty_saved == TRUE) {
	(void) restore_tty (dip->di_fd);
    }
    return (close_file (dip));
}

/************************************************************************
 *									*
 * tty_reopen() - Reopen Terminal Devices.				*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *		oflags = The device/file open flags.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCESS / FAILURE.			*
 *									*
 ************************************************************************/
/*ARGSUSED*/
int
tty_reopen (struct dinfo *dip, int oflags)
{
    int status = SUCCESS;

    /*
     * For terminal devices, we don't close the device since this
     * resets characteristics which must then be reset.
     */
    if (edelay_count) {			/* Optional end delay. 	*/
	mySleep (edelay_count);
    }

#ifdef notdef
    if ((status = reopen_file (dip, oflags)) == SUCCESS) {
	(void) setup_tty (&dip->di_fd, FALSE);
    }
#else
    end_of_file = FALSE;
#endif /* notdef */

    return (status);
}

/************************************************************************
 *									*
 * tty_flush_data() - Wait for tty output queue to empty (drain).	*
 *									*
 * Inputs:	dip = The device information pointer.			*
 *									*
 * Return Value:							*
 *		Returns 0 / -1 = SUCESS / FAILURE.			*
 *									*
 ************************************************************************/
int
tty_flush_data (struct dinfo *dip)
{
	return (drain_tty (dip->di_fd));
}

/************************************************************************
 *									*
 * drain_tty() - Wait for tty output data to drain.			*
 *									*
 * Description:								*
 *	This function was added to drain the terminal ports' output	*
 * buffer.  This is needed to properly synchronize with the process	*
 * reading the data.  If the writer exits before the reader reads the	*
 * data with software carrier enabled, the output data gets flushed	*
 * (discarded) by terminal driver.					*
 *									*
 * Inputs:	fd = The terminal file descriptor to drain.		*
 *									*
 * Return Value:							*
 *		Returns SUCCESS / FAILURE.				*
 *									*
 ************************************************************************/
int
drain_tty (int fd)
{
	int status;

#if !defined(_QNX_SOURCE) && !defined(__MSDOS__) && !defined(__WIN32__)
	/*
	 * If debug is enabled, display the characters not flushed
	 * in the output queue yet.
	 */
	if (debug_flag) {
	    int outq_size;

	    if (ioctl (fd, TIOCOUTQ, &outq_size) < 0) {
		report_error ("TIOCOUTQ", TRUE);
	    } else {
		Fprintf ("Characters remaining in output queue = %d\n",
								outq_size);
	    }
	}
#endif /* !defined(_QNX_SOURCE) */
	/*
	 * Wait for the output queue to drain.
	 */
	if (debug_flag) {
	    Fprintf ("Waiting for output queue to drain...\n");
	}
#if defined(ultrix)
	if ((status = ioctl (fd, TCSBRK, -1)) < 0) {
	    if (debug_flag) report_error ("TCSBRK", TRUE);
#else /* !defined(ultrix) */
	if ((status = tcdrain (fd)) < 0) {
	    report_error ("tcdrain()", TRUE);
#endif /* defined(ultrix) */
	    /*
	     * Since some drivers don't support this ioctl, we must
	     * get and set the terminal characteristics to flush the
	     * output queue (isn't this fun?).
	     */
	    status = setup_tty (fd, FALSE);
	}
	if (debug_flag && (status == SUCCESS) ) {
	    Fprintf ("Output queue finished draining...\n");
	}
	return (status);
}

/************************************************************************
 *									*
 * flush_tty() - Flush the tty input buffer queue.			*
 *									*
 * Description:								*
 *	This function flushs the tty input buffer queue before starting	*
 * the test.  Ordinarily this isn't necessary, since the driver flushes	*
 * unread characters after closing, but the timer associated with this	*
 * action (on output) and the setting of CLOCAL, may result in unwanted	*
 * characters waiting to be read. In any case, to avoid race conditions	*
 * we'll flush input here to avoid unwarrented data compare failures.	*
 *									*
 * Logic Changed:							*
 *	We only flush the input queue if flushing has been disabled,	*
 * since both the read & write queues are flushed in save_tty().	*
 *									*
 * Inputs:	fd = The terminal file descriptor to flush.		*
 *									*
 * Return Value:							*
 *		void							*
 *									*
 ************************************************************************/
int
flush_tty (int fd)
{
	int status;

	if (!flush_flag && debug_flag) {  /* Flush/display input queue. */
	    ssize_t count;
	    unsigned char buff;

	    if ((status = setup_tty (fd, TRUE)) < 0) {
		return (status);
	    }
	    /*
	     * Read & display any characters in the typeahead buffer.
	     */
	    while ( (count = read (fd, &buff, (size_t) 1)) > (ssize_t) 0) {
		Fprintf ("Flushing: %c (%d.) (%#x)\n", buff, buff, buff);
	    }
	} else if (!flush_flag) {
	    /*
	     * This could be dangerous if the writer starts first.
	     */
	    if (tcflush (fd, TCIFLUSH) < 0) {	/* Flush input queue. */
		report_error ("tcflush(TCIFLUSH)", TRUE);
	    }
	}
	status = setup_tty (fd, FALSE);
	return (status);
}

/************************************************************************
 *									*
 * save_tty() - Save The Current Terminal Characteristics.		*
 *									*
 * Inputs:	fd = The terminal file descriptor.			*
 *									*
 * Return Value:							*
 *		Returns SUCCESS / FAILURE.				*
 *									*
 ************************************************************************/
int
save_tty (int fd)
{
	int status;

	if (debug_flag) {
	    Fprintf ("Saving current terminal characteristics, fd = %d...\n", fd);
	}

#if !defined(_QNX_SOURCE) && !defined(sun) && !defined(__MSDOS__) && !defined(__WIN32__)
	/*
	 * Save the line discipline.
	 */
	if ((status = ioctl (fd, TIOCGETD, &saved_ldisc)) < 0) {
	    report_error ("TIOCGETD", FALSE);
	    return (status);
	}
#endif /* !defined(_QNX_SOURCE) */

	/*
	 * Save the terminal characteristics.
	 */
#if defined(sun)
	if ((status = ioctl (fd, TCGETS, &saved_tmodes)) < 0) {
	    report_error ("TCGETS", FALSE);
	}
#elif defined(ultrix)
	if ((status = ioctl (fd, TCGETP, &saved_tmodes)) < 0) {
	    report_error ("TCGETP", FALSE);
	}
#elif defined(_OSF_SOURCE)
	if ((status = ioctl (fd, TIOCGETA, &saved_tmodes)) < 0) {
	    report_error ("TIOCGETA", FALSE);
	}
#elif defined(_QNX_SOURCE) || defined(__MSDOS__) || defined(__WIN32__)
	if ((status = tcgetattr (fd, &saved_tmodes)) < 0) {
	    report_error ("tcgetattr()", FALSE);
	}
#endif /* defined(sun) */
	if (status == SUCCESS) tty_saved = TRUE;
	/*
	 * Since this function is called prior to starting the test,
	 * we'll flush the input & output queues to discard any junk.
	 */
	if (flush_flag) {		/* Allow user to control. */
	    if (tcflush (fd, TCIOFLUSH) < 0) {
		report_error ("tcflush(TCIOFLUSH)", FALSE);
	    }
	}
	return (status);
}

/************************************************************************
 *									*
 * restore_tty() - Restore Saved Terminal Characteristics.		*
 *									*
 * Inputs:	fd = The terminal file descriptor.			*
 *									*
 * Return Value:							*
 *		Returns SUCCESS / FAILURE / WARNING (never saved)	*
 *									*
 ************************************************************************/
int
restore_tty (int fd)
{
	int status;

	if (tty_saved == FALSE) return (WARNING);

	if (debug_flag) {
	    Fprintf ("Restoring saved terminal characteristics, fd = %d...\n", fd);
	}

	/*
	 * Restore the saved terminal characteristics.
	 */
#if defined(sun)
	if ((status = ioctl (fd, TCSETSW, &saved_tmodes)) < 0) {
	    report_error ("TCSETSW", TRUE);
	}
#elif defined(ultrix)
	if ((status = ioctl (fd, TCSANOW, &saved_tmodes)) < 0) {
	    report_error ("TCSANOW", TRUE);
	}
#elif defined(_OSF_SOURCE)
	if ((status = ioctl (fd, TIOCSETA, &saved_tmodes)) < 0) {
	    report_error ("TIOCSETA", TRUE);
	}
#elif defined(_QNX_SOURCE) || defined(__MSDOS__) || defined(__WIN32__)
	if ((status = tcsetattr (fd, TCSANOW, &saved_tmodes)) < 0) {
	    report_error ("tcsetattr()", TRUE);
	}
#endif /* defined(sun) */

#if !defined(_QNX_SOURCE) && !defined(sun) && !defined(__MSDOS__) && !defined(__WIN32__)
	/*
	 * Restore the saved line discipline.
	 */
	if (ioctl (fd, TIOCSETD, &saved_ldisc) < 0) {
		report_error ("TIOCSETD", TRUE);
	}
#endif /* !defined(_QNX_SOURCE) */
	return (status);
}

/************************************************************************
 *									*
 * setup_tty() - Setup terminal characteristics.			*
 *									*
 * Inputs:	fd = The terminal file descriptor.			*
 *									*
 * Return Value:							*
 *		Returns SUCCESS / FAILURE.				*
 *									*
 ************************************************************************/
int
setup_tty (int fd, int flushing)
{
	int status = SUCCESS;
	struct termios tm;		/* For terminal characteristics	*/
#if !defined(_QNX_SOURCE) && !defined(sun) && !defined(__MSDOS__) && !defined(__WIN32__)
	int ldisc;			/* For line discipline.		*/
#endif

	if (debug_flag && !flushing) {
	    Fprintf ("Setting up test terminal characteristics, fd = %d...\n", fd);
	}

#if defined(sun)
	/*
	 * If software carrier was detected, then enable it.
	 */
	switch (softcar_opt) {
	    static int on = 1, off = 0;

	    case ON:
		if (ioctl (fd, TIOCSSOFTCAR, &on) < 0) {
		    report_error ("TIOCSOFTCAR", FALSE);
		}
		break;

	    case OFF:
		if (ioctl (fd, TIOCSSOFTCAR, &off) < 0) {
		    report_error ("TIOCSOFTCAR", FALSE);
		}
		break;
	}
#endif /* defined(sun) */

#if !defined(_QNX_SOURCE) && !defined(sun) && !defined(__MSDOS__) && !defined(__WIN32__) && !defined(SCO)
	/*
	 * Ensure the correct line discipline is setup.
	 */
	if ((status = ioctl (fd, TIOCGETD, &ldisc)) < 0) {
	    report_error ("TIOCGETD", FALSE);
	    return (status);
	}
#if defined(ultrix)
	if (ldisc != TERMIODISC) {
	    ldisc = TERMIODISC;
	    if ((status = ioctl (fd, TIOCSETD, &ldisc)) < 0) {
		report_error ("TIOCSETD", FALSE);
		return (status);
	    }
	}
#elif defined(__linux__)
	if (ldisc != N_TTY) {
	    ldisc = N_TTY;
	    if ((status = ioctl (fd, TIOCSETD, &ldisc)) < 0) {
		report_error ("TIOCSETD", FALSE);
	        return (status);
	    }
	}
#else /* !defined(ultrix) */
	if (ldisc != TTYDISC) {
	    ldisc = TTYDISC;
	    if ((status = ioctl (fd, TIOCSETD, &ldisc)) < 0) {
		report_error ("TIOCSETD", FALSE);
		return (status);
	    }
	}
#endif /* defined(ultrix) */
#endif /* !defined(_QNX_SOURCE) */

	/*
	 * For terminals, get and set the terminal characteristics.
	 */
#if defined(sun)
	if ((status = ioctl (fd, TCGETS, &tm)) < 0) {
	    report_error ("TCGETS", FALSE);
	    return (status);
	}
#elif defined(ultrix)
	if ((status = ioctl (fd, TCGETP, &tm)) < 0) {
	    report_error ("TCGETP", FALSE);
	    return (status);
	}
#elif defined(_OSF_SOURCE)
	if ((status = ioctl (fd, TIOCGETA, &tm)) < 0) {
	    report_error ("TIOCGETA", FALSE);
	    return (status);
	}
#elif defined(_QNX_SOURCE) || defined(__MSDOS__) || defined(__WIN32__)
	if ((status = tcgetattr (fd, &tm) < 0)) {
	    report_error ("tcgetattr()", FALSE);
	    return (status);
	}
#endif /* defined(sun) */
	tm.c_cflag = tm.c_iflag = tm.c_oflag = tm.c_lflag = 0;
	if (modem_flag) {
	    tm.c_cflag = HUPCL;		/* Hangup on last close. */
	} else {
	    tm.c_cflag = CLOCAL;	/* Ignore modem signals. */
	}
#if defined(sun) || defined(__linux__) || defined(SCO)
	tm.c_cflag |= (baud_rate_code | data_bits_code | parity_code | CREAD);
#else /* !defined(sun) */
	tm.c_cflag |= (data_bits_code | parity_code | CREAD);
#if defined(ultrix)
	tm.c_cflag |= ( (baud_rate_code << 16) | baud_rate_code);
#else /* !defined(ultrix) */
	tm.c_ispeed = tm.c_ospeed = baud_rate_code;
#endif /* defined(ultrix) */
#endif /* defined(sun) */

	/*
	 * Send two stop bits for slower speeds (is this right?).
	 */
	if (baud_rate_code < B300) {
	    tm.c_cflag |= CSTOPB;
	}

	/*
	 * Set VMIN & VTIME values, checking for maximum values.
	 */
	if (flushing) {
	    tm.c_cc[VMIN] = 0;
	} else if (tty_minflag) {
	    tm.c_cc[VMIN] = (tty_minimum > VMIN_MAX) ? VMIN_MAX
						     : tty_minimum;
	} else {
	    tm.c_cc[VMIN] = (block_size > VMIN_MAX) ? VMIN_MAX
						    : block_size;
	}

	if (flushing) {
	    tm.c_cc[VTIME] = 1;
	} else {
	    tm.c_cc[VTIME] = (tty_timeout > VTIME_MAX) ? VTIME_MAX
						       : tty_timeout;
	}

	/*
	 * Set the desired flow control.
	 */
#if defined(sun) || defined(_OSF_SOURCE) || defined(__linux__)
	if (flow_type == CTS_RTS) {
	    tm.c_cflag |= CRTSCTS;		/* CTS/RTS flow control. */
	} else if (flow_type == XON_XOFF) {
	    tm.c_iflag |= (IXON | IXOFF);	/* XON/XOFF Flow control. */
	}
#elif defined(_QNX_SOURCE)
	tm.c_lflag |= IEXTEN;			/* QNX POSIX extensions. */
	if (flow_type == CTS_RTS) {
	    tm.c_lflag |= (IHFLOW | OHFLOW);	/* CTS/RTS flow control. */
	} else if (flow_type == XON_XOFF) {
	    tm.c_iflag |= (IXON | IXOFF);	/* XON/XOFF Flow control. */
	}
#else /* Ultrix, POSIX, and all others (I hope)... */
	if (flow_type == XON_XOFF) {
	    tm.c_iflag |= (IXON | IXOFF);	/* XON/XOFF Flow control. */
	}

#endif /* defined(sun) || defined(_OSF_SOURCE) */

	/*
	 * If 7 bit data, or parity is being enabled, enable stripping
	 * of the eight data bit, otherwise the driver passes it back.
	 * We don't test generation of correct parity, driver does this.
	 */
	if ( (data_bits_code == CS7) || (parity_code & PARENB) ) {
	    tm.c_iflag |= ISTRIP;		/* Strip the 8th data bit */
	    /*
	     * For parity, enable input parity checking.
	     */
	    if (parity_code & PARENB) {
		tm.c_iflag |= INPCK;		/* Check input parity.	*/
	    }
	}

	/*
	 * Setup the terminal characteristics after output is done.
	 */
#if defined(sun)
	if ((status = ioctl (fd, TCSETSW, &tm)) < 0) {
	    report_error ("TCSETSW", FALSE);
	    return (status);
	}
#elif defined(ultrix)
	if ((status = ioctl (fd, TCSADRAIN, &tm)) < 0) {
	    report_error ("TCSADRAIN", FALSE);
	    return (status);
	}
#elif defined(_OSF_SOURCE)
	if ((status = ioctl (fd, TIOCSETAW, &tm)) < 0) {
	    report_error ("TIOCSETAW", FALSE);
	    return (status);
	}
#elif defined(_QNX_SOURCE) || defined(__MSDOS__) || defined(__WIN32__)
	if ((status = tcsetattr (fd, TCSADRAIN, &tm)) < 0) {
	    report_error ("tcsetattr()", FALSE);
	    return (status);
	}
#endif /* defined(sun) */

	/*
	 * For testing modem lines, wait for modem to be ready.
	 */
	if (modem_flag) {
#if defined(_OSF_SOURCE) || defined(ultrix)
	    if (debug_flag) (void) ShowModemSignals (fd);
#if 0
	    status = WaitForCarrier (fd);
	    if (debug_flag) (void) ShowModemSignals (fd);
	    status = (status == TRUE) ? SUCCESS : FAILURE;
	    if (status == SUCCESS) {
		status = SetBlockingMode (fd);
#endif
#endif /* defined(_OSF_SOURCE) || defined(ultrix) */
	    /*
	     * Don't wait for modem signals, simply reset non-blocking
	     * mode, and let the first read or write system call block.
	     * [ NOTE: I've changed this logic for direct lines, since
	     *   I'm seeing the writer causing CTS/DTS/Carrier to set.
	     *   Plus, this code should work on all operating systems. ]
	     */
	    status = SetBlockingMode (fd);
	}
	return (status);
}

/************************************************************************
 *									*
 * setup_baud_rate() - Setup the baud rate code.			*
 *									*
 * Description:								*
 *	This function takes a specified baud rate (i.e. 9600) and maps	*
 * it to the baud rate code we must specify to the Unix terminal driver	*
 * to set that baud rate.						*
 *									*
 * Inputs:	baud = Pointer to baud rate string.			*
 *									*
 * Return Value:							*
 *		Returns SUCCESS / FAILURE = Valid Speed/Invalid Speed.	*
 *									*
 ************************************************************************/
int
setup_baud_rate (u_int32 baud)
{
    int i;
    struct tty_baud_rate *tsp;

    for (tsp = baud_rate_tbl, i = 0; i < num_baud_rates; i++, tsp++) {
	if (baud == tsp->usr_speed) {
	    baud_rate_code = tsp->tty_speed;	/* Save baud rate code. */
	    return (SUCCESS);			/* Return success status. */
	}
    }
    fprintf (stderr, "Baud rate '%d' is invalid, valid entrys are:\n", baud);
    for (tsp = baud_rate_tbl, i = 0; i < num_baud_rates; i++, tsp++) {
	if ( (i % 6) == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%10d", tsp->usr_speed);
    }
    fprintf (stderr, "\n");
    return (FAILURE);
}

int
SetBlockingMode (int fd)
{
	int flags;

	if ( (flags = fcntl (fd, F_GETFL)) == FAILURE) {
		perror ("fcntl(F_GETFL)");
		return (FAILURE);
	}
	/*
	 * BEWARE: O_NDELAY & O_NONBLOCK are _not_ the same value anymore.
	 * [ NOTE: O_NONBLOCK is defined by POSIX (O_NDELAY is _not_. ]
	 */
	flags &= ~(O_NONBLOCK);
	if ( (fcntl (fd, F_SETFL, flags)) == FAILURE) {
		perror ("fcntl(F_SETFL)");
		return (FAILURE);
	}
	return (SUCCESS);
}

#if defined(_OSF_SOURCE) || defined(ultrix)

/*
 * To the best of my knowledge, this is Digital Unix (OSF/Ultrix) specific:
 */


unsigned int
GetModemSignals (int fd)
{
	unsigned int msigs;

	if (ioctl(fd, TIOCMGET, &msigs) < 0) {
		perror("TIOCMGET");
		return(FAILURE);
	}
	return (msigs);
}

int
SetModemSignals (int fd, int msigs)
{
	if (ioctl (fd, TIOCMBIS, &msigs) < 0) {
		perror("TIOCMBIS");
		return(FAILURE);
	}
	return (SUCCESS);
}

int
HangupModem (int fd)
{
	int status;
	unsigned int delay = 3;

	if ((status = ioctl(fd, TIOCCDTR, 0)) < 0) perror("TIOCCDTR");
	sleep(delay);
	if ((status = ioctl(fd, TIOCSDTR, 0)) < 0) perror("TIOCSDTR");
	sleep(1);
	return (status);
}

#define P(fmtstr)		fprintf (stderr, fmtstr)
#define P1(fmtstr,arg)		fprintf (stderr, fmtstr, arg)

int
ShowModemSignals (int fd)
{
	unsigned int msigs;

	if ( (msigs = GetModemSignals(fd)) == FAILURE) {
		return(FAILURE);
	}
	P ("--------------------------------------------------\r\n");
	P1("Modem Signals Set: 0x%x\r\n", msigs);
	if (msigs & TIOCM_LE) {
	  P1("   0x%x = TIOCM_LE = Line Enable.\r\n", TIOCM_LE);
	}
	if (msigs & TIOCM_DTR) {
	  P1("   0x%x = TIOCM_DTR = Data Terminal Ready.\r\n", TIOCM_DTR);
	}
	if (msigs & TIOCM_RTS) {
	  P1("   0x%x = TIOCM_RTS = Request To Send.\r\n", TIOCM_RTS);
	}
	if (msigs & TIOCM_ST) {
	  P1("   0x%x = TIOCM_ST = Secondary Transmit.\r\n", TIOCM_ST);
	}
	if (msigs & TIOCM_SR) {
	  P1("   0x%x = TIOCM_SR = Secondary Receive.\r\n", TIOCM_SR);
	}
	if (msigs & TIOCM_CTS) {
	  P1("   0x%x = TIOCM_CTS = Clear To Send.\r\n", TIOCM_CTS);
	}
	if (msigs & TIOCM_CAR) {
	  P1("   0x%x = TIOCM_CAR = Carrier Detect.\r\n", TIOCM_CAR);
	}
	if (msigs & TIOCM_RNG) {
	  P1("   0x%x = TIOCM_RNG = Ring Indicator.\r\n", TIOCM_RNG);
	}
	if (msigs & TIOCM_DSR) {
	  P1("   0x%x = TIOCM_DSR = Data Set Ready.\r\n", TIOCM_DSR);
	}
	P ("--------------------------------------------------\r\n");
	return(SUCCESS);
}

int
WaitForCarrier (int fd)
{
	unsigned int msigs;
	unsigned int delay = 1;

	if (debug_flag) {
	    Fprintf ("Waiting for carrier or DSR signals...\n");
	}
	do {
	    if ( (msigs = GetModemSignals(fd)) == FAILURE) {
		return(FALSE);
	    }
	    sleep (delay);
	} while ( (msigs & (TIOCM_CAR|TIOCM_DSR)) == 0);
	return (TRUE);
}

#endif /* defined(_OSF_SOURCE) || defined(ultrix) */
