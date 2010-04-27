/***
**Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   sdma_tty_test.c

    @brief  SDMA TTY test
    
====================================================================================================
Revision History:
                Modification     Tracking
Author             Date          Number       Description of Changes
--------------  -----------   ------------    ----------------------------------------------------
S. ZAVJALOV       19/07/2004    TLSbo40259    Initial version
E.Gromazina       31/10/2005    TLSbo56685    Fix bag
D.Kazachkov       26/06/2006    TLSbo71594    Clean OPOST flag in c_oflag 

====================================================================================================
Portability: ARM GCC Montavista

==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <termios.h> 
#include <poll.h>

/* Verification Test Environment Include Files */
#include "sdma_tty_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
int fd = 0;
char *r_buf = 0;
struct termios mxc, old;

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/


/*============================================ ======================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern char *TCID;
extern char *string_param;
extern int num_param;
extern int r_buf_len_param;

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int poll_sdma(int fd,int events);

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_sdma_tty_test_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution. Opens SDMA TTY device, allocates mamory for read buffer.
        
@param  Input:  None
        Output: None
    
@return On success - return TPASS
        On failure - return TFAIL*/
/*================================================================================================*/
int VT_sdma_tty_test_setup(void)
{
        int VT_rv = TFAIL;
        
        if ((fd = open(SDMA_TTY_DEVICE, O_RDWR)) < 0)
        {
                tst_resm(TFAIL,"VT_sdma_tty_test_setup. Cannot open SDMA TTY");
                return VT_rv;
        }
        tcgetattr(fd, &old);
        mxc = old;
        mxc.c_lflag &= ~(ICANON | ECHO | ISIG);
        mxc.c_oflag &= OPOST;
        tcsetattr(fd, TCSANOW, &mxc);
        
        if (! (r_buf = (char *) malloc (r_buf_len_param)))
        {
                tst_resm(TFAIL, "Cannot allocate memory");
                return VT_rv; 
        }
        
        VT_rv = TPASS;
        return VT_rv;
}

/*================================================================================================*/
/*===== VT_sdma_tty_test_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution. Closes SDMA TTY device, frees the memory

@param  Input:  None
        Output: None
    
@return On success - return TPASS
        On failure - return the error code*/
/*================================================================================================*/
int VT_sdma_tty_test_cleanup(void)
{
        if (fd > 0)
        {
                tcsetattr(fd, TCSAFLUSH, &old);
                close (fd);
        }
        if (r_buf != 0) free (r_buf);

        return TPASS;
}

/*================================================================================================*/
/*===== VT_sdma_tty_test =====*/
/**
@brief  Write to SDMA TTY device a few strings, after that, try to read it is back .

@param  Input:  None
        Output: None
    
@return On success - return TPASS
        On failure - return the error code*/
/*================================================================================================*/
int VT_sdma_tty_test (void)
{
        int VT_rv = TFAIL, err, mode, i, j, strl;
        
        /* CALLBACK SETUP */
        mode = 1;
        if ((err = ioctl(fd, TIOCLPBACK, &mode)) <0 )
        {
                tst_resm(TFAIL, "VT_sdma_tty_test. Failed calback setup");
                return VT_rv; 
        }
        
        /* WRITE */
        for(i=0; i < num_param; i++)
        {
                if ((err = write(fd, string_param, strlen(string_param))) < 0)
                {
                        tst_resm(TFAIL,"VT_sdma_tty_test. Failed write");
                        return VT_rv;
                }
                
                while(!(poll_sdma(fd, POLLOUT) & POLLOUT))
                {
                        tst_resm(TINFO,"wait, device is busy");
                        usleep (500);
                }
        }
        
        /* READ & CHECK */
        j = 0; 
        strl = strlen(string_param) - 1;
        do
        {
                if ((err = read(fd, r_buf, r_buf_len_param)) < 0)
                {
                        tst_resm(TFAIL,"VT_sdma_tty_test. Failed read");
                        return VT_rv;
                }
                
                i = 0;
                do
                {
                        if (r_buf[i] != string_param[j])
                        {
                                tst_resm(TFAIL, "VT_sdma_tty_test. Failed, read not equal write data");
                                return VT_rv;
                        }
                        if (j == strl) 
                                j = 0;
                        else
                                j++;
                        i++;
                }while (i != err);
        }while (poll_sdma(fd, POLLIN) != 0);
        
        VT_rv = TPASS;
        return VT_rv;
}


/*================================================================================================*/
/*===== help =====*/
/**
@brief  Displays the program usage

@param  Input:  None
        Output: None

@return None*/
/*================================================================================================*/
void help(void)
{
        printf("====================================================\n");
        printf("  -n x    Number of string to write\n");
        printf("  -s x    String\n");
        printf("  -r x    Read buffer length\n");
        printf("\nUsage: %s -n <num_of_string> -s <string> -r <read_buffer_length>\n\n", TCID);
}

/*================================================================================================*/
/*===== poll_sdma=====*/
/**
@brief  

@param  Input:  file fescriptor, mode 
        Output: None

@return On success - return TPASS
        On failure - return the error code*/
/*================================================================================================*/
int poll_sdma(int fd,int events)
{
        struct pollfd fds;
        int err;
        
        fds.fd = fd;
        fds.events = events;
        fds.revents = 0;
        
        if ((err =poll(&fds,1,0)) > 0 )
                return fds.revents;
        else
                return err;
}

#ifdef __cplusplus
}
#endif

