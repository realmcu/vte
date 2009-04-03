/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file erw_mmc_test.c

@brief VTE C source MMC/SD testcase extracting event during read/write process

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
S.ZAVJALOV/zvjs001c   04/04/2005   tlsbo45047   Initial version

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
#include "erw_mmc_test.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <termios.h>
#include <time.h>

/*======================== LOCAL CONSTANTS ==================================*/

/*======================== LOCAL MACROS =====================================*/

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/

/*======================== LOCAL VARIABLES ==================================*/

struct termios stored_settings;
int fd_device = 0;
unsigned char *tmp_buf = NULL;

/*======================== GLOBAL CONSTANTS =================================*/

/*======================== GLOBAL VARIABLES =================================*/

extern int vb_mode;
extern char *device_name;
extern unsigned long bcount;
extern BOOLEAN read_flag;

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

void set_terminal(void);
void restore_terminal(void);
BOOLEAN kbhit(void);

/*======================== LOCAL FUNCTIONS ==================================*/

/*===== set_terminal =====*/
/**
@brief  Template test scenario X function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
void set_terminal(void)
{
    struct termios new_settings;

    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
//NO ENTER
    new_settings.c_lflag &= (~ICANON);
//noecho
    new_settings.c_lflag &= (~ECHO);
//no CTR+C
//    new_settings.c_lflag &= (~ISIG);
//BUFF = 1
//    new_settings.c_cc[VTIME] = 0;
//    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &new_settings);
    return;
}

/*===== restore_terminal =====*/
/**
@brief  Template test scenario X function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
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
*/
BOOLEAN kbhit(void)
{
  fd_set rset;
  struct timeval tv;
  int nSelect;

  FD_ZERO(&rset);
  FD_SET(fileno(stdin), &rset);

  tv.tv_sec = 0;
  tv.tv_usec = SLEEP_TIME;

  nSelect = select(fileno(stdin) + 1, &rset, NULL, NULL, &tv);

  if (nSelect == -1)
    return FALSE;

  return nSelect > 0;
}

/*======================== GLOBAL FUNCTIONS =================================*/

/*===== VT_erw_mmc_setup =====*/
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
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
int VT_erw_mmc_setup()
{
    fd_device = open(device_name, O_RDWR);
    if (fd_device == -1)
    {
        tst_resm(TFAIL, "VT_erw_mmc_setup() Failed open device: %s", device_name);
        return TFAIL;
    }
    srand((unsigned int)time((time_t *)NULL));
    set_terminal();

    tmp_buf = (unsigned char*)malloc(bcount);
    if (tmp_buf == NULL)
    {
        tst_resm(TFAIL, "VT_erw_mmc_setup() Failed allocate memory");
        return TFAIL;
    }

    return TPASS;
}

/*===== VT_tempalte_cleanup =====*/
/**
Description of the function
@brief  assumes the post-condition of the test case execution
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
int VT_erw_mmc_cleanup(void)
{
    if (fd_device > 0)
        if (close(fd_device) == -1)
        {
            tst_resm(TFAIL, "VT_rw_mmc_cleanup() Failed close device");
            return TFAIL;
        }

    if (tmp_buf != NULL)
        free(tmp_buf);

    restore_terminal();

    return TPASS;
}

/*===== VT_erw_mmc_test =====*/
/**
@brief  Template test scenario X function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
int VT_erw_mmc_test()
{
    struct stat MMC_insertion;
    unsigned long ul;
    ssize_t rv;

    if (read_flag)
	tst_resm(TINFO, "Start reading, please remove card, or press Esc to cancel");
    else
    {
        if (vb_mode)
	    tst_resm(TINFO, "Filling pattern");

	for (ul = 0; ul < bcount; ul++)
	    tmp_buf[ul] = (unsigned char) (random() * 0xFF);
	tst_resm(TINFO, "Start writing, please remove card, or press Esc to cancel");
    }

    do{
        if (kbhit())
        {
            if (getchar() == 27)
                return TFAIL;
        }
	switch(read_flag)
	{
	    case FALSE:	rv = write(fd_device, tmp_buf, bcount);
			break;
	    case TRUE:	rv = read(fd_device, tmp_buf, bcount);
			break;
	}

	if (rv != bcount)
	{
	    if(stat(device_name, &MMC_insertion) != -1 )
	    {
		if (read_flag)
		    tst_resm(TFAIL, "Problem with read. Extraction event not detected.");
		else
		    tst_resm(TFAIL, "Problem with write. Extraction event not detected.");
		return TFAIL;
	    }
	    else
	    {
		if (read_flag)
		    tst_resm(TINFO, "Read fails while extracting the MMC card ");
		else
		    tst_resm(TINFO, "Write fails while extracting the MMC card ");
		return TPASS;
	    }
	}
    }while(TRUE);

    return TPASS;
}

#ifdef __cplusplus
}
#endif
