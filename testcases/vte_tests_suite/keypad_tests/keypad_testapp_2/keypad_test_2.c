/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   keypad_test_2.c

        @brief  Threads implementation of keypad test : display and read a keypad key in MAP mode
====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. BECKER / rc023c           29/04/2004     TLSbo38735  MAP mode testing. § Matrix Reloaded §
V. BECKER / rc023c           25/05/2004     TLSbo38735  Change file name
V. BECKER / rc023c           03/06/2004     TLSbo39684  Corrections for End key press 
C.Gagneraud cgag1c           08/11/2004     TLSbo44474  Rewrite.
L.Delaspre/rc149c            16/12/2004     TLSbo45474  Dead pressed key event handle
L.Delaspre/rc149c            22/03/2005     TLSbo48665  Update mapcode
D.Simakov / smkd001c         17/03/2005     TLSbo49806  Change sequence odrer of prompted keys to follow 
A.Ozerov/NONE                10/01/2006     TLSbo61037  Update in accordance with linux-2.6.10-rel-L26_1_15

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <sys/time.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "keypad_test_2.h"

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Keypad ioctl arguments */
kbd_arg keypad_args = { 0, 0 };

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/
char    keyarray[NUMBER_OF_KEYS][8] = 
{
        "Sel", "LEFT", "DOWN", "RIGHT", "UP", "Key2", "End", "Back",
        "Key1", "Send", "Home", "App1", "Vol_up", "App2", "App3", "App4",
        "3", "2", "1", "4", "Voldown", "7", "5", "6",
        "9", "#", "8", "0", "*", "record", "q", "w",
        "a", "s", "d", "e", "f", "r", "t", "y",
        "Tab", "Symbol", "Caps", "z", "x", "c", "v", "g",
        "b", "h", "n", "m", "j", "k", "u", "i",
        "space", "On/Off", ".", "Enter", "l", "bs", "p", "o",
};

char   *mode_string[] = { "RAW", "MAP", };

int     hw_layout[NUMBER_OF_KEYS] = 
{
        9, 8, 5, 6, 10, 0, 1, 3, 2, 4, 7,
        11, 13, 14, 15, 12, 18, 17, 16,
        20, 19, 22, 23, 21, 26, 24, 29,
        28, 27, 25, 30, 31, 35, 37, 38,
        39, 54, 55, 63, 62, 32, 33, 34,
        36, 47, 49, 52, 53, 60, 61, 40,
        43, 44, 45, 46, 48, 50, 51, 58,
        59, 41, 42, 56, 57
};

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
int     file_desc = 0;

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/
#define KEY_PRESS 1
#define KEY_RELEASE 2

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void   *readkey_function(unsigned int *map_code);
void   *printkey_function(unsigned int *map_code);
int     test_mapcode(unsigned char mapcode);

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== VT_keypad_test2_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_keypad_test2_setup(void)
{
        int     rv = TFAIL;

        /* Open keypad driver */
        file_desc = open(KEYPAD_DRIVER, O_RDONLY);
        sleep(1);
        if (file_desc == -1)
        {
                tst_resm(TFAIL, "ERROR : Open keypad driver fails");
                perror("Error: cannot open /dev/vc/0 device");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_keypad_test2_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_keypad_test2_cleanup(void)
{
        int     rv = TFAIL;
        int     ret = 0;

        /* Close keypad driver */
        ret = close(file_desc);
        if (ret == -1)
        {
                tst_resm(TFAIL, "ERROR : Close keypad driver fails");
                perror("Error: cannot close /dev/vc/0 device");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_keypad_test2 =====*/
/**
@brief  Keypad test with read and display key scan code in MAP mode

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_keypad_test2(void)
{
        int     rv = TFAIL;
        int     res = -EFAULT;
        unsigned int keypad_mode = 0;
        int     key_idx;
        unsigned char mapcode;

        /* Configure keypad matrix to a normal value (8 by 8) */
        tst_resm(TINFO, "Configure keypad matrix to a normal value (8 by 8)");
        keypad_args.arg1 = 8;
        keypad_args.arg2 = 8;
        res = ioctl(file_desc, KDROWCOL, &keypad_args);
        if (res < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl fails for KDROWCOL");
                perror("ioctl");
                return rv;
        }

        /* Set keypad in MAP mode to get the map codes */
        tst_resm(TINFO, "Set keypad driver in MAP mode to get the map codes");
        keypad_args.arg1 = K_XLATE;
        res = ioctl(file_desc, KDSKBMODE, &keypad_args);
        if (res < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl fails for KDSKBMODE\n");
                perror("ioctl");
                return rv;
        }

        keypad_mode = keypad_args.arg1;
        tst_resm(TINFO, "Keypad mode changed OK to %s", mode_string[keypad_mode]);

        /* Setting keypad for non-blocking read. Every time a character is queued from keypad driver, 
        * application reads it */
        keypad_args.arg1 = NONBLOCK;
        res = ioctl(file_desc, KDSBLOCK, &keypad_args);
        if (res < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl fails for KDSBLOCK");
                perror("ioctl");
                return rv;
        }

        /* 
        * Flush keypad queue?
        */
        tst_resm(TINFO, "Flushing keypad queue, please dot not press/release any key.");

        /* 
        * Test each key.
        */
        rv = TPASS;

        for (key_idx = 0; key_idx < NUMBER_OF_KEYS; key_idx++)
        {
                mapcode = hw_layout[key_idx];
                tst_resm(TINFO, "Testing for key pressed: key='%s', code=%d",
                         keyarray[hw_layout[key_idx]], mapcode);
                tst_resm(TINFO, "Please press key '%s' (and do not release it! ;o)",
                         keyarray[hw_layout[key_idx]]);
                res = test_mapcode(mapcode);
                if (res != TPASS)
                        rv = res;
                mapcode += NUMBER_OF_KEYS;
                tst_resm(TINFO, "Testing for key released: key='%s', code=%d",
                         keyarray[hw_layout[key_idx]], mapcode);
                tst_resm(TINFO, "Please release key '%s'", keyarray[hw_layout[key_idx]]);
                res = test_mapcode(mapcode);
                if (res != TPASS)
                        rv = res;
        }

        return rv;
}

/*================================================================================================*/
/*===== test_mapcode =====*/
/**
Description of the function
@brief  read pressed/released keys
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
/*================================================================================================*/
int test_mapcode(unsigned char mapcode)
{
        ssize_t read_retval;
        unsigned char read_mapcode;
        struct timeval tv;
        int     rv = TFAIL;

        /* We should read only one byte corresponding to the specified key * pressed/released */
        read_retval = 0;
        while (read_retval != 1)
        {
                read_retval = read(file_desc, &read_mapcode, sizeof(read_mapcode));
                if (read_retval < 0)
                {
                        switch (errno)
                        {
                                /* Nothing available. Ignore it. */
                        case EAGAIN:
                                break;
                                /* Interrupted. Ignore it. */
                        case EINTR:
                                break;
                                /* "Real" error. */
                        default:
                                tst_resm(TFAIL, "ERROR : read fails.");
                                perror("Keypad read");
                                return rv;
                        }
                }
                else if (read_retval == 0)
                {
                        /* If this append, is it an error? */
                        tst_resm(TFAIL, "ERROR : got EOF!\n");
                        return rv;
                }
                else if (read_retval != 1)
                {
                        /* Ask for one byte, got more. */
                        tst_resm(TFAIL, "ERROR : Got more than one mapcode!");
                        return rv;
                }
        }
        gettimeofday(&tv, NULL);
        tst_resm(TINFO, "Event received [timestamp: %d s %d ms] ", tv.tv_sec % 100,
                 tv.tv_usec / 1000);

        /* Finally check read map code against expected one. */
        if (read_mapcode != mapcode)
        {
                tst_resm(TFAIL, "ERROR : Expecting mapcode %d (0x%02X), got %d (0x%02X)!\n",
                         mapcode, mapcode, read_mapcode, read_mapcode);
                rv = TFAIL;
        }
        else
        {
                tst_resm(TPASS, "%d (0x%02X) is the expecded mapcode.\n",
                         read_mapcode, read_mapcode);
                rv = TPASS;
        }

        return rv;
}
