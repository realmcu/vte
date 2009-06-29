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
/**
@file lock_mmc_test.c

@brief VTE C source MMC/SD device read/write testcase with locking switch

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
#include "lock_mmc_test.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <time.h>

/*======================== LOCAL CONSTANTS ==================================*/

/*======================== LOCAL MACROS =====================================*/

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/

/*======================== LOCAL VARIABLES ==================================*/

int fd_device = 0;
unsigned char *patten_buf = NULL, *read_buf = NULL;

/*======================== GLOBAL CONSTANTS =================================*/

/*======================== GLOBAL VARIABLES =================================*/

extern int vb_mode;
extern char *device_name;

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

/*======================== LOCAL FUNCTIONS ==================================*/

/*======================== GLOBAL FUNCTIONS =================================*/

/*===== VT_lock_mmc_setup =====*/
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
int VT_lock_mmc_setup()
{
    fd_device = open(device_name, O_RDWR);
    if (fd_device == -1)
    {
        tst_resm(TFAIL, "VT_lock_mmc_setup() Failed open device: %s", device_name);
        return TFAIL;
    }
    srand((unsigned int)time((time_t *)NULL));

    patten_buf = (unsigned char*)malloc(WRITE_BLOCK);
    if (patten_buf == NULL)
    {
        tst_resm(TFAIL, "VT_lock_mmc_test() Failed allocate memory");
        return TFAIL;
    }

    read_buf = (unsigned char*)malloc(READ_BLOCK);
    if (read_buf == NULL)
    {
        tst_resm(TFAIL, "VT_lock_mmc_test() Failed allocate memory");
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
int VT_lock_mmc_cleanup(void)
{
    if (patten_buf != NULL)
        free(patten_buf);
    if (read_buf != NULL)
        free(read_buf);

    if (fd_device > 0)
        if (close(fd_device) == -1)
        {
            tst_resm(TFAIL, "VT_rw_mmc_cleanup() Failed close device");
            return TFAIL;
        }
    return TPASS;
}

/*===== VT_lock_mmc_test =====*/
/**
@brief  Template test scenario X function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
int VT_lock_mmc_test()
{
    ssize_t bytes_count;
    unsigned long ul;

    if (vb_mode)
        tst_resm(TINFO, "Read from %s", device_name);

    if (lseek(fd_device, 0, SEEK_SET) != 0)
    {
        tst_resm(TFAIL, "VT_lock_mmc_test() Failed repositions the offset of the file descriptor");
        return TFAIL;
    }

    bytes_count = read(fd_device, read_buf, READ_BLOCK);
    if (bytes_count != READ_BLOCK)
    {
	tst_resm(TFAIL, "VT_lock_mmc_test() Failed read from device");
        return TFAIL;
    }

   if (vb_mode)
        tst_resm(TINFO, "Fill pattern buffer");
    for (ul = 0; ul < WRITE_BLOCK; ul++)
        patten_buf[ul] = (unsigned char) (random() * 0xFF);

    if (lseek(fd_device, 0, SEEK_SET) != 0)
    {
        tst_resm(TFAIL, "VT_lock_mmc_test() Failed repositions the offset of the file descriptor");
        return TFAIL;
    }

    bytes_count = write(fd_device, patten_buf, WRITE_BLOCK);
    if (bytes_count == WRITE_BLOCK)
    {
	tst_resm(TFAIL, "VT_lock_mmc_test() It is possible to write on device");
        return TFAIL;
    }
            
    return TPASS;
}

#ifdef __cplusplus
}
#endif
