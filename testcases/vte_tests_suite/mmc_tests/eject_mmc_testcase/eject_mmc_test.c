/* 
* Copyright (C) 2004, Freescale Semiconductor, Inc. 
* All Rights Reserved THIS SOURCE CODE IS
* CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
* Freescale Semiconductor, Inc. */

/**
@file eject_mmc_test.c

@brief VTE C source MMC/SD device input/eject testcase

Description of the file

@par Portability: arm, gcc, montavista */

/*======================== REVISION HISTORY ==================================

Author (core ID)             Date         CR Number       Description of Changes
-------------------   ----------   ----------   ------------------------------
S.ZAVJALOV/zvjs001c   22/03/2005   tlsbo46706    Initial version
I.Inkina/nknl001      25/07/2005   TLSbo50891   Update open device  
E.Gromazina           14/10/2005    TLSbo56643  Update for the first MMC
=============================================================================*/

#ifdef __cplusplus
extern "C"{ 
#endif

/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "eject_mmc_test.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <termios.h>

/*======================== LOCAL CONSTANTS ==================================*/

/*======================== LOCAL MACROS =====================================*/

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/

/*======================== LOCAL VARIABLES ==================================*/

/*======================== GLOBAL CONSTANTS =================================*/

/*======================== GLOBAL VARIABLES =================================*/

extern int vb_mode;
struct termios stored_settings;

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

void    set_terminal(void);
void    restore_terminal(void);
BOOLEAN kbhit(void);
int     ir_event(char *event, char *slot_number, char *device_name);

/*======================== LOCAL FUNCTIONS ==================================*/

/*======================== GLOBAL FUNCTIONS =================================*/

/*===== VT_eject_mmc_setup =====*/
/**
Description of the function
@brief  assumes the pre-condition of the test case execution

@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
=============================================================================*/

int VT_eject_mmc_setup(void)
{
        set_terminal();
        return TPASS;
}

/*===== VT_tempalte_cleanup =====*/
/**
Description of the function
@brief  assumes the post-condition of the test case execution
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
=============================================================================*/

int VT_eject_mmc_cleanup(void)
{
        restore_terminal();
        return TPASS;
}

/*===== VT_eject_mmc_test =====*/
/**
@brief  Template test scenario X function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
=============================================================================*/

int VT_eject_mmc_test(param_mmc * par)
{
        int     rv1 = 1;

        struct stat MMC_insertion;
        int     key;

        param_mmc *p = (param_mmc *) par;

        /* cheek for no one cards are present */
        rv1 = stat(p->file_name_mmc_1, &MMC_insertion);

        if (rv1 != -1 )
                tst_resm(TINFO, "Please remove all MMC/SD card or press Esc to cancel");
        while (rv1 != -1 )
        {
                if (kbhit())
                {
                        key = getchar();
                        if (key == 27)
                                return TFAIL;
                }
                rv1 = stat(p->file_name_mmc_1, &MMC_insertion);
        }

        if (ir_event("insert", "to first", p->file_name_mmc_1) != TPASS)
                return TFAIL;
        if (ir_event("remove", "from first", p->file_name_mmc_1) != TPASS)
                return TFAIL;
        if (ir_event("insert", "to second", p->file_name_mmc_1) != TPASS)
                return TFAIL;
        if (ir_event("remove", "from second", p->file_name_mmc_1) != TPASS)
                return TFAIL;


        return TPASS;
}

/*===== ir_event =====*/
/**
@brief  Template test scenario X function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
=============================================================================*/

int ir_event(char *event, char *slot_number, char *device_name)
{
        int     d_event;
        int     rv;
        struct stat MMC_insertion;

        if (strcmp(event, "insert") == 0)
                d_event = 0;
        else
                d_event = -1;
        tst_resm(TINFO, "Please %s MMC/SD card %s slot or press Esc to cancel", event, slot_number);
        do
        {
                if (kbhit())
                {
                        if (getchar() == 27)
                                return TFAIL;
                }
                rv = stat(device_name, &MMC_insertion);
        }
        while (rv != d_event);
        return TPASS;
}

/*===== set_terminal =====*/
/**
@brief  Template test scenario X function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
=============================================================================*/

void set_terminal(void)
{
        struct termios new_settings;

        tcgetattr(0, &stored_settings);
        new_settings = stored_settings;
        /* NO ENTER*/
        new_settings.c_lflag &= (~ICANON);
        /* noecho*/
        new_settings.c_lflag &= (~ECHO);
        /* no CTR+C
         new_settings.c_lflag &= (~ISIG);
         BUFF = 1
         new_settings.c_cc[VTIME] = 0;
         new_settings.c_cc[VMIN] = 1;*/
         
        tcsetattr(0, TCSANOW, &new_settings);
        return;
}

/*===== restore_terminal =====*/
/**
@brief  Template test scenario X function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
=============================================================================*/

void restore_terminal(void)
{
        tcsetattr(0, TCSANOW, &stored_settings);
        return;
}

/*===== kbhit =====*/
/**
@brief  Template test scenario X function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
=============================================================================*/

BOOLEAN kbhit(void)
{
        fd_set  rset;
        struct timeval tv;
        int     nSelect;

        FD_ZERO(&rset);
        FD_SET(fileno(stdin), &rset);

        tv.tv_sec = 0;
        tv.tv_usec = SLEEP_TIME;

        nSelect = select(fileno(stdin) + 1, &rset, NULL, NULL, &tv);

        if (nSelect == -1)
                return FALSE;

        return nSelect > 0;
}

#ifdef __cplusplus
} 
#endif
