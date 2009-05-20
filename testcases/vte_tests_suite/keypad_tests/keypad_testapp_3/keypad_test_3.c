/*====================*/
/**
        @file   keypad_test_3.c

        @brief  Test scenario C source template.
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D. SIMAKOV                   04/05/2004     TLSbo39737  Draft version
L. DELASPRE / rc149c         16/08/2004     TLSbo40891  VTE 1.4 integration
D. SIMAKOV / smkd001c        6/08/2004      TLSbo41742  Bug fix
C. Gagneraud                 08/11/2004     TLSbo44474   Warning fixup.
L.Delaspre/rc149c            22/03/2005     TLSbo48665   Update mapcode
A.Ozerov/NONE                10/01/2006     TLSbo61037   Update in accordance with linux-2.6.10-rel-L26_1_15

====================
Portability: ARM GCC
======================*/
/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "keypad_test_3.h"

/*======================
                                        LOCAL MACROS
======================*/

/*======================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/
/* Keypad ioctl arguments */
kbd_arg keypad_args = {0,0};

/*======================
                                        LOCAL CONSTANTS
======================*/
#define K_RAWMODE  0x00 /* Configure keypad in raw mode */
#define K_MAPMODE  0x01 /* Configure keypad in map mode */
#define KDGKBMODE  0x4B44 /* Gets current Keypad mode */
#define KDSKBMODE  0x4B45 /* Sets current Keypad mode */
#define KDSKBMAXCO 0x4B73 /* Sets keypad matrix configuration */

#define NUMBER_OF_KEYS    65  /* total count of the keys */
#define LOW_KEYS          32  /* */
#define HIGH_KEYS         16  /* */
#define STOP_SCANCODE      6  /* scan code for exit */

#define MAX_SIM_KEYS 3

char keyarray[NUMBER_OF_KEYS][8] =
{
        "Sel", "LEFT","DOWN","RIGHT","UP", "Key2", "End", "Back",
        "Key1", "Send", "Home", "App1", "Vol_up", "App2", "App3", "App4",
        "3", "2", "1", "4", "Vol_down", "7", "5", "6",
        "9", "#", "8", "0", "*", "record", "q", "w",
        "a", "s", "d", "e", "f", "r", "t", "y",
        "Tab", "Symbol", "Caps", "z", "x", "c", "v", "g",
        "b", "h", "n", "m", "j", "k", "u", "i",
        "space", "On/Off", ".", "Enter", "l", "bs", "p", "o",
};

char *mode_string[] = {"RAW","MAP" };

/*======================
                                        LOCAL VARIABLES
======================*/
int            driver_file = -1;           /* keypad's driver file name */
unsigned short scan_buffer[MAX_SIM_KEYS];  /**/
int            is_finished = FALSE;        /**/
int            num_keys_pressed = 0;

/*======================
                                        GLOBAL CONSTANTS
======================*/

/*======================
                                        GLOBAL VARIABLES
======================*/

/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/
void process_keypad(void);

/*======================
                                        LOCAL FUNCTIONS
======================*/
/*====================*/
/*= VT_keypad_test_3_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_keypad_test_3_setup(void)
{
        int rv = TFAIL;

        /* open keypad driver */
        driver_file = open( KEYPAD_DRIVER, O_RDONLY );
        sleep( 1 );
        if( driver_file == -1 )
        {
                tst_resm( TFAIL, "ERROR : Open keypad driver fails" );
                perror( "Cannot open /dev/vc/0 device" );
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*====================*/
/*= VT_keypad_test_3_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_keypad_test_3_cleanup(void)
{
        int rv = TFAIL;
        int ret = 0;

        /* close keypad driver */
        ret = close( driver_file );
        if( ret == -1 )
        {
                tst_resm( TFAIL, "ERROR : Close keypad driver fails" );
                perror( "Cannot close /dev/vc/0 device" );
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*====================*/
/*= VT_keypad_test_3 =*/
/**
@brief  keypad test scenario 3 function

@param  None

@return On success - return TPASS
        On failure - return the error code*/
/*====================*/
int VT_keypad_test_3(void)
{
        int rv = TFAIL;
        int res = -EFAULT;
        unsigned int keypad_mode = 0;

        /* Configure keypad matrix to a normal value (8 by 8) */
        printf("Configure keypad matrix to a normal value (8 by 8)\n");
        keypad_args.arg1 = 8;
        keypad_args.arg2 = 8;
        res = ioctl( driver_file, KDROWCOL, &keypad_args );
        if( res < 0 )
        {
                tst_resm( TFAIL, "ERROR : ioctl fails for KDROWCOL" );
                perror( "ioctl" );
                return rv;
        }

        /* Set keypad in RAW mode to get the map codes */
        keypad_args.arg1 = K_RAW;
        res = ioctl( driver_file, KDSKBMODE, &keypad_args );
        if (res < 0)
        {
                tst_resm( TFAIL, "ERROR : ioctl fails for KDSKBMODE" );
                perror( "ioctl" );
            return rv;
        }

        keypad_mode = keypad_args.arg1;
        printf( "Keypad mode changed OK to %s\n", mode_string[keypad_mode] );

        /* Setting keypad for non-blocking read. Every time a character
        is queued from keypad driver, application reads it */
        keypad_args.arg1 = BLOCK;
        res = ioctl( driver_file, KDSBLOCK, &keypad_args );
        if( res < 0 )
        {
                tst_resm( TFAIL, "ERROR : ioctl fails for KDSBLOCK" );
                perror( "ioctl" );
            return rv;
        }

        printf( "press %d keys\n", MAX_SIM_KEYS );
        process_keypad();

        rv = TPASS;

        return rv;
}

/*====================*/
/*= process_keypad =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
void process_keypad(void)
{
        int rb;
        int fail_timeout_count = 0;
        int i;

        while( !is_finished )
        {
                if( driver_file != -1 )
                {
                        if( fail_timeout_count > 10 )
                        {
                                is_finished = TRUE;
                                continue;
                        }

                        rb = read( driver_file, scan_buffer, MAX_SIM_KEYS * sizeof(unsigned short) );

                        for( i = 0; i < MAX_SIM_KEYS-1; ++i )
                        {
                                printf( "%s, ", keyarray[ scan_buffer[i] & 0x3f ] );
                                if( (scan_buffer[i] & 0x3f) == STOP_SCANCODE )
                                is_finished = TRUE;
                        }
                        printf( "%s\n", keyarray[ scan_buffer[MAX_SIM_KEYS-1] & 0x3f ] );
                        if( (scan_buffer[MAX_SIM_KEYS-1] & 0x3f) == STOP_SCANCODE )
                        is_finished = TRUE;

                        if( rb < 0 && errno != EAGAIN )
                        {
                                tst_resm( TFAIL, "ERROR : read scan codes from queue failed" );
                                perror( "read fails" );
                                ++fail_timeout_count;
                        }
                }
        }
}
