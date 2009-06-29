/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
    @file   signals.h 

    @brief  This file contains array of signal's names.
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Dmitriy Kazachkov           10/06/2004      TLSbo39741   initial version
==================================================================================================*/

/* signal names for numbers */

char   *signames[] = { "!!ERROR - NOT A SIGNAL!!",
        "SIGHUP",       /* 1 // Hangup (POSIX).  */
        "SIGINT",       /* 2 // Interrupt (ANSI).  */
        "SIGQUIT",      /* 3 // Quit (POSIX).  */
        "SIGILL",       /* 4 // Illegal instruction (ANSI).  */
        "SIGTRAP",      /* 5 // Trace trap (POSIX).  */
        "SIGIOT/SIGABRT",       /* 6 // Abort (ANSI).  */
        /* 6 // IOT trap (4.2 BSD).  */
        "SIGBUS",       /* 7 // BUS error (4.2 BSD).  */
        "SIGFPE",       /* 8 // Floating-point exception (ANSI).  */
        "SIGKILL",      /* 9 // Kill, unblockable (POSIX).  */
        "SIGUSR1",      /* 10 // User-defined signal 1 (POSIX).  */
        "SIGSEGV",      /* 11 // Segmentation violation (ANSI).  */
        "SIGUSR2",      /* 12 // User-defined signal 2 (POSIX).  */
        "SIGPIPE",      /* 13 // Broken pipe (POSIX).  */
        "SIGALRM",      /* 14 // Alarm clock (POSIX).  */
        "SIGTERM",      /* 15 // Termination (ANSI).  */
        "SIGSTKFLT",    /* 16 // Stack fault.  */
        "SIGCLD/SIGCHLD",       /* SIGCHLD // Same as SIGCHLD (System V).  */
        /* "SIGCHLD", // 17 // Child status has changed (POSIX).  */
        "SIGCONT",      /* 18 // Continue (POSIX).  */
        "SIGSTOP",      /* 19 // Stop, unblockable (POSIX).  */
        "SIGTSTP",      /* 20 // Keyboard stop (POSIX).  */
        "SIGTTIN",      /* 21 // Background read from tty (POSIX).  */
        "SIGTTOU",      /* 22 // Background write to tty (POSIX).  */
        "SIGURG",       /* 23 // Urgent condition on socket (4.2 BSD).  */
        "SIGXCPU",      /* 24 // CPU limit exceeded (4.2 BSD).  */
        "SIGXFSZ",      /* 25 // File size limit exceeded (4.2 BSD).  */
        "SIGVTALRM",    /* 26 // Virtual alarm clock (4.2 BSD).  */
        "SIGPROF",      /* 27 // Profiling alarm clock (4.2 BSD).  */
        "SIGWINCH",     /* 28 // Window size change (4.3 BSD, Sun).  */
        "SIGPOLL/SIGIO",        /* SIGIO // Pollable event occurred (System V).  */
        /* "SIGIO", // 29 // I/O now possible (4.2 BSD).  */
        "SIGPWR",       /* 30 // Power failure restart (System V).  */
        "SIGSYS/SIGUNUSED"      /* 31 // Bad system call.  */
            /* "SIGUNUSED", // 31 */
};
