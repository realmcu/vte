/*====================*/
/**
        @file   signal_test_01.c

        @brief  Simplistic test to verify the signal system function calls.                           */
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
CTU                          05/06/1989                  Initial version
DJK                          11/22/1993                  Rewrite for AIX version 4.1
DJK                          02/07/1994                  Move to "prod" directory
VHM                          06/04/2001                  Port to work in linux
D.KHOROSHEV                  09/05/2005     TLSbo56682   sigpause( ) was replaced with sigsuspend( ).
                                                         LTP integration.
A.Ozerov/b00320              11/12/2006     TLSbo84161   Minor changes.

====================
Portability: ARM GCC
======================*/

/*======================
                                        INCLUDE FILES
======================*/
#include "signal_test_01.h"
#include "signals.h"

/*======================
                                        LOCAL MACROS
======================*/
/* Macro for specifying signal masks */
#define MASK(sig)  (1 << ((sig) - 1))
#define STACKSIZE  20000
#define MAX_MSGS   8192
#define error(a) { tst_resm(TBROK,"(line: %d) %s",__LINE__, a); return errno; }

/*======================
                                        LOCAL VARIABLES
======================*/
/* Define an alternative stack for processing signals */
char    stackarray[STACKSIZE];

#ifdef _LINUX_
stack_t stack = {
        ss_sp:stackarray + STACKSIZE,
        /* stack pointer */
        ss_flags:0,
        /* SS_ONSTACK, // flags */
        ss_size:STACKSIZE/* size */
};
stack_t *oldstack;

/* SIGMAX is defined in the AIX headers to be 63 - 64 seems to work in linux??
   however, signals 32, 33, and 34 are not valid */
# define SIGMAX 64

#else                           /* ! _LINUX */

struct sigstack stack = {
        stackarray + STACKSIZE, /* signal stack pointer */
        (_ONSTACK & ~_OLDSTYLE) /* current status */
};
#endif


/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/
/* Function prototypes */
void    handler(int);   /* signal catching function */
void    init_sig_vec(void);     /* setup signal handler for signals */
void    reset_valid_sig(void);  /* reset valid_sig array */
void    sys_error(const char *, int);   /* system error message function */

/*======================
                                        LOCAL VARIABLES
======================*/
/* Define an array for verifying received signals */
int     valid_sig[SIGMAX + 1];

/*======================
                                        GLOBAL VARIABLES
======================*/
extern char *TCID;

/*======================
                                        LOCAL FUNCTIONS
======================*/
/*====================*/
/*= VT_RM_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code*/
/*====================*/
int VT_RM_setup(void)
{
        return TPASS;
}

/*====================*/
/*= VT_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param

@return On success - return TPASS
        On failure - return the error code*/
/*====================*/
int VT_RM_cleanup(void)
{
        return TPASS;
}

/*====================*/
/*= VT_RM_signal_test =*/
/**
@brief  Main test function of simplistic test to verify the signal system function calls.
        Algorithm:      o  Setup a signal-catching function for every possible
                            signal
                        o  Send signals to the process and verify that they
                            were received by the signal-catching function
                        o  Block a few signals by changing the process signal
                            mask.  Send signals to the process and verify that
                            they indeed were blocked
                        o  Add additional signals to the process signal mask.
                            Verify that they were blocked too
                        o  Change the process signal mask to unblock one
                            signal and suspend execution of the process until
                            the signal is received.  Verify that the unblocked
                            signal is received
        System calls: The following system calls are tested:
                        sigstack () - Sets signal stack context
                        sigsetmask () - Sets the current signal mask
                        sigblock () - Sets the current signal mask
                        sigpause () - Automically changes the set of blocked
                                    signals and waits for a signal
                        sigvec () - Specify the action to take upon delivery
                                    of a signal.
                        kill () - Sends a signal to a process.

@param

@return On success - return 0.
        On failure - return the error code*/
/*====================*/
int VT_RM_signal_test(void)
{
        pid_t   pid = getpid(); /* Process ID (of this process) */
        sigset_t mask, newmaskset, oldmaskset;

        /* Print out program header */
        tst_resm(TINFO, "%s: IPC Signals TestSuite program", TCID);

        /*
        * Establish signal handler for each signal & reset "valid signals"
        * array, and setup alternative stack for processing signals
        */
        init_sig_vec();
        reset_valid_sig();
#ifdef _LINUX_
        /* sigstack function is obsolete, use sigaltstack instead */
        if (sigaltstack(&stack, NULL) < 0)
                error("sigaltstack failed");
#else
        if (sigstack(&stack, (struct sigstack *) 0) < 0)
                error("sigstack failed");
#endif
        /*
        * Send SIGILL, SIGALRM & SIGIOT signals to this process:
        *
        * First indicate which signals the signal handler should expect
        * by setting the corresponding valid_sig[] array fields.
        *
        * Then send the signals to this process.
        *
        * And finally verify that the signals were caught by the signal
        * handler by checking to see if the corresponding valid_sig[] array
        * fields were reset.
        */
        tst_resm(TINFO, "\tSend SIGILL, SIGALRM, SIGIOT signals to process");
        valid_sig[SIGILL] = 1;
        valid_sig[SIGALRM] = 1;
        valid_sig[SIGIOT] = 1;

        kill(pid, SIGILL);
        kill(pid, SIGALRM);
        kill(pid, SIGIOT);

        if (valid_sig[SIGILL])
                error("failed to receive SIGILL signal!")
        if (valid_sig[SIGALRM])
                error("failed to receive SIGALRM signal!")
        if (valid_sig[SIGIOT])
                error("failed to receive SIGIOT signal!")

        /*
        * Block SIGILL, SIGALRM & SIGIOT signals:
        *
        * First create the process signal mask by ORing together the
        * signal values.
        *
        * Then change the process signal mask with sigsetmask ().
        *
        * Verify that the desired signals are blocked from interrupting the
        * process, by sending both blocked and unblocked signals to the
        * process. Only the unblocked signals should interrupt the process.
        */
        tst_resm(TINFO, "\tBlock SIGILL, SIGALRM, SIGIOT signals, "
                 "and resend signals + others");
        sigemptyset(&mask);
        sigaddset(&mask, SIGILL);
        sigaddset(&mask, SIGALRM);
        sigaddset(&mask, SIGIOT);
#ifdef _LINUX_
        sigprocmask(SIG_SETMASK, &mask, (sigset_t *)NULL);
#else
        if (sigprocmask(SIG_SETMASK, mask, (sigset_t *)NULL) < 0)
                error("sigprocmask failed");
#endif
        valid_sig[SIGFPE] = 1;
        valid_sig[SIGTERM] = 1;
        valid_sig[SIGINT] = 1;
        valid_sig[SIGILL] = 1;
        valid_sig[SIGALRM] = 1;
        valid_sig[SIGIOT] = 1;


        kill(pid, SIGILL);
        kill(pid, SIGALRM);
        kill(pid, SIGIOT);
        kill(pid, SIGFPE);
        kill(pid, SIGTERM);
        kill(pid, SIGINT);

        if (valid_sig[SIGFPE])
                error("failed to receive SIGFPE signal!");
        if (valid_sig[SIGTERM])
                error("failed to receive SIGTERM signal!");
        if (valid_sig[SIGINT])
                error("failed to receive SIGINT signal!");

        /*
        * Block additional SIGFPE, SIGTERM & SIGINT signals:
        *
        * Create a signal mask containing the additional signals to block.
        *
        * Change the process signal mask to block the additional signals
        * with the sigblock () function.
        *
        * Verify that all of the desired signals are now blocked from
        * interrupting the process.  None of the specified signals should
        * interrupt the process until the process signal mask is changed.
        */
        tst_resm(TINFO, "\tBlock rest of signals");
        sigemptyset(&newmaskset);
        sigaddset(&newmaskset, SIGFPE);
        sigaddset(&newmaskset, SIGTERM);
        sigaddset(&newmaskset, SIGINT);
        if (sigprocmask(SIG_BLOCK, &newmaskset, &oldmaskset) < 0)
        {
                error("sigprocmask failed");
        }
        if (memcmp(&mask, &oldmaskset, sizeof(sigset_t)))
                error("value returned by sigprocmask() does not match the "
                      "old signal mask");
        valid_sig[SIGFPE] = 1;
        valid_sig[SIGTERM] = 1;
        valid_sig[SIGINT] = 1;

        kill(pid, SIGFPE);
        kill(pid, SIGTERM);
        kill(pid, SIGINT);

        if (!valid_sig[SIGFPE])
                error("SIGFPE signal didn't blocked!");
        if (!valid_sig[SIGTERM])
                error("SIGTERM signal didn't blocked!");
        if (!valid_sig[SIGINT])
                error("SIGINT signal didn't blocked!");

        /* Wait two seconds just to make sure that none of the specified signals interrupt the * *
        * process (They should all be blocked). */
        sleep(2);

        /* Change the process signal mask: Now specifiy a new process signal mask to allow the * *
        * SIGINT signal to interrupt the process.  Thus by using sigpause (), force the process to *
        * * suspend execution until delivery of an unblocked signal (SIGINT in this case). * *
        * Additionally, verify that the SIGINT signal was received. */
        sigprocmask(SIG_BLOCK, &oldmaskset, (sigset_t *)NULL);
        valid_sig[SIGINT] = 1;

        tst_resm(TINFO, "\tChange signal mask & wait until signal interrupts process");

        sigemptyset(&newmaskset);
        sigaddset(&newmaskset, SIGINT);
        kill(pid, SIGINT);
        sigprocmask(SIG_UNBLOCK, &newmaskset, &oldmaskset);

        sigsuspend(&newmaskset);
        sigprocmask(SIG_UNBLOCK, &newmaskset, (sigset_t *)NULL);

        if (valid_sig[SIGINT])
                error("failed to receive SIGINT signal!")
        else
                tst_resm(TINFO, "SIGINT received");

        /* Program completed successfully -- exit */
        tst_resm(TINFO, "successful!");

        return 0;
}

/*====================*/
/*= init_sig_vec =*/
/**
@brief  Initialize the signal vector for ALL possible signals
        (as defined in /usr/include/sys/signal.h) except for
        the following signals which cannot be caught or ignored:
            o  SIGKILL (9)
            o  SIGSTOP (17)
            o  SIGCONT (19)

@param

@return None.*/
/*====================*/
void init_sig_vec(void)
{
        char    errmsg[256];
        int     i;

#ifdef _LINUX_
        static struct sigaction invec;

        for (i = 1; i <= SIGMAX; i++)
        {
                if ((i == SIGKILL) || (i == SIGSTOP) || ((i >= 32) && (i <= 34)))
                        continue;
                invec.sa_handler = handler;
                /* sigaction.sa_mask = 0; */
                invec.sa_flags = 0;

                if (sigaction(i, &invec, (struct sigaction *) 0))
                {
                        printf(errmsg, "init_sig_vec: sigaction failed on signal (%d)", i);
                        perror(errmsg);
                        sys_error(errmsg, __LINE__);
                }
        }
#else
        static struct sigvec invec;

        for (i = 1; i <= SIGMAX; i++)
        {

                /* Cannot catch or ignore the following signals */
# ifdef _IA64   /* SIGWAITING NOT supported, RESERVED */
                if ((i == SIGKILL) || (i == SIGSTOP) || (i == SIGCONT) || (i == SIGWAITING))
                        continue;
# else
                if ((i == SIGKILL) || (i == SIGSTOP) || (i == SIGCONT))
                        continue;
# endif

                invec.sv_handler = handler;
                /* invec.sv_mask = SA_NOMASK; */
# if defined  _IA64
                invec.sv_flags = 1;
# else
                invec.sv_onstack = 1;
# endif
                if (sigvec(i, &invec, (struct sigvec *) 0))
                {
                        sprintf(errmsg, "init_sig_vec: sigvec failed on signal (%d)", i);
                        perror(errmsg);
                        sys_error(errmsg, __LINE__);
                }
        }
#endif                          /* ifdef _LINUX_ */
}

/*====================*/
/*= handler =*/
/**
@brief  Signal catching function.  As specified in init_sig_vec()
        this function is automatically called each time a signal
        is received by the process.
        Once receiving the signal, verify that the corresponding
        signal was expected.
@param  Input:   sig - signal identifier.
        Output:  None.

@return None.*/
/*====================*/
void handler(int sig)
{
        /* Check to insure that expected signal was received */
        char   *sign = "unknown signal";

        if (sig < 32)
                sign = signames[sig];
        if (valid_sig[sig])
        {
                valid_sig[sig] = 0;

                printf("\treceived signal: (%s)\n", sign);
        }
        else
        {
                printf("\tunexpected signal (%d,%s)\n", sig, sign);
        }
}

/*====================*/
/*= reset_valid_sig =*/
/**
@brief  Reset the valid "signal" array.

@param  Input:   None.
        Output:  None.

@return None.*/
/*====================*/
void reset_valid_sig(void)
{
        int     i;

        for (i = 0; i < (SIGMAX + 1); i++)
                valid_sig[i] = 0;
}

/*====================*/
/*= sys_error  =*/
/**
@brief Creates system error message and calls error ()

@param  Input:   msg  - error message string.
                line - line number.
        Output:  None.

@return None.*/
/*====================*/
void sys_error(const char *msg, int line)
{
        tst_resm(TBROK, "ERROR [line %d] %s: %s", line, msg, strerror(errno));
        exit(errno);
}
