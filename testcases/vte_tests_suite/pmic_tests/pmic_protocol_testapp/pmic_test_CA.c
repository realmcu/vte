/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   pmic_test_CA.c

        @brief  Concurrent Access test scenario source file for SC55112 Protocol dirver test appliaction.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev/b00313           07/20/2005     TLSbo52700   Initial version
D.Khoroshev/b00313           09/05/2005     TLSbo52700   Rework version
D.Khoroshev/b00313           01/13/2006     TLSbo59968   Added default data for MC13783
D.Khoroshev/b00313           07/25/2006     TLSbo64239   Added mc13783 legacy API support

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "pmic_test_common.h"
#include "pmic_test_CA.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/
#define nb_value 8

#ifdef CONFIG_MXC_PMIC_SC55112
/* Here just to make as common between SC55112 and MC13783, REG_ADC1 is
 * selected twice */
unsigned int TEST_VALUE_CA[nb_value][2] =
{
        {pmic_read, REG_ADC1},
        {pmic_write,  REG_ADC1},
        {pmic_subscribe, 2},
        {pmic_read, REG_ADC1},
        {pmic_write,  REG_ADC1},
        {pmic_subscribe,   0},
        {pmic_unsubscribe, 0},
        {pmic_unsubscribe, 2}
};
#endif

#ifdef  CONFIG_MXC_PMIC_MC13783 
unsigned int TEST_VALUE_CA[nb_value][2] =
{
        {pmic_read, REG_MEMORY_A},
        {pmic_write,  REG_MEMORY_A},
        {pmic_subscribe,   2},
        {pmic_read, REG_MEMORY_B},
        {pmic_write,  REG_MEMORY_B},
        {pmic_subscribe,   0},
        {pmic_unsubscribe, 0},
        {pmic_unsubscribe, 2}
};
#endif

#ifdef CONFIG_MXC_PMIC_MC13892 
unsigned int TEST_VALUE_CA[nb_value][2] =
{
        {pmic_read, REG_MEM_A},
        {pmic_write,  REG_MEM_A},
        {pmic_subscribe,   2},
        {pmic_read, REG_MEM_B},
        {pmic_write,  REG_MEM_B},
        {pmic_subscribe,   0},
        {pmic_unsubscribe, 0},
        {pmic_unsubscribe, 2}
};
#endif
#ifdef CONFIG_MXC_PMIC_MC9SDZ60
unsigned int TEST_VALUE_CA[nb_value][2] =
{
        {pmic_read, REG_MAX8660_V3_TARGET_VOLT_1},
        {pmic_write,  REG_MAX8660_V3_TARGET_VOLT_1},
        {pmic_subscribe,   2},
        {pmic_read, REG_MAX8660_V3_TARGET_VOLT_1},
        {pmic_write,  REG_MAX8660_V3_TARGET_VOLT_1},
        {pmic_subscribe,   0},
        {pmic_unsubscribe, 0},
        {pmic_unsubscribe, 2}
};
#endif

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
extern char *ifile_name;
extern char device_name[32];

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_pmic_CA_setup =====*/
/**
@brief  Opens device file and sets up test sequence used by VT_pmic_opt. Get this sequence from file
        specified by key -F or using standart test sequence.

@param  Input:  thread_num - number of thread
        Output: fd - descriptor of device file
                nb_param - size of test_sequence.
                test_sequence - pointer to array of test sequence.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_CA_setup (int *fd, int thread_num)
{
        *fd = open(device_name, O_RDWR);
        if (*fd < 0)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TFAIL, "[%d]Unable to open %s", thread_num, device_name);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }

        return TPASS;
}

/*================================================================================================*/
/*===== VT_pmic_CA_cleanup =====*/
/**
@brief  Closes device file and frees memory for array test_sequnce if it was reserved by
        function VT_pmic_read_opt_params()

@param  Input:  fd - descriptor of device file
                test_sequence - pointer to array of test sequence
                thread_num - number of thread

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_CA_cleanup(int fd, int thread_num)
{
        if (close(fd) < 0)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TFAIL, "[%d]Unable to close file descriptor %d", thread_num, fd);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }

        return TPASS;
}

/*================================================================================================*/
/*===== VT_pmic_test_CA =====*/
/**
@brief  This function trying to perform read/write, subscribe/unsubscribe operations with some
        predefined registers and events.
        Test application should be started with key -N <number_of_threads>.

@param  Input:  thread_num - number of thread

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_test_CA(int thread_num)
{
    int fd, i;
        unsigned int valueR=0;

        if(VT_pmic_CA_setup(&fd, thread_num) != TPASS)
        {
                VT_pmic_CA_cleanup(fd, thread_num);
                return TFAIL;
        }
        for(i=0; i<nb_value; i++)
        {
                switch(TEST_VALUE_CA[i][0])
                {
                        case pmic_read:
                                if (VT_pmic_read(fd, TEST_VALUE_CA[i][1], &valueR) != TPASS)
                                {
                                        pthread_mutex_lock(&mutex);
                                        tst_resm(TWARN,"Failed to Read the reg:%d\n",TEST_VALUE_CA[i][1]);
                                        tst_resm(TWARN,"Test Failed while exiting\n");
                                        pthread_mutex_unlock(&mutex);
                                        return TFAIL;
                                }
                                break;
                        case pmic_write:
                                if (VT_pmic_write(fd, TEST_VALUE_CA[i][1], valueR) != TPASS)
                                {
                                        pthread_mutex_lock(&mutex);
                                        tst_resm(TWARN,"Failed to write to the reg:%d\n",TEST_VALUE_CA[i][1]);
                                        tst_resm(TWARN,"Test Failed while exiting\n");
                                        pthread_mutex_unlock(&mutex);
                                        return TFAIL;
                                }
                                break;
                        case pmic_subscribe:
                                if (VT_pmic_subscribe(fd, TEST_VALUE_CA[i][1]) != PMIC_SUCCESS)
                                {
                                        pthread_mutex_lock(&mutex);
                                        tst_resm(TWARN,"Event #%d did not subscribed.",TEST_VALUE_CA[i][1]);
                                        tst_resm(TWARN,"Test Failed while exiting\n");
                                        pthread_mutex_unlock(&mutex);
                                        return TFAIL;
                               }
                                break;
                        case pmic_unsubscribe:
                                if (VT_pmic_unsubscribe(fd, TEST_VALUE_CA[i][1]) != PMIC_SUCCESS)
                                {
                                        pthread_mutex_lock(&mutex);
                                        tst_resm(TWARN,"Event #%d did not unsubscribed.",TEST_VALUE_CA[i][1]);
                                        tst_resm(TWARN,"Test Failed while exiting\n");
                                        pthread_mutex_unlock(&mutex);
                                        return TFAIL;
                                }
                                break;
                        default:
                                tst_resm(TFAIL, "Unsupported operation");
                    return TFAIL;
                                break;
                }
        }
        if(VT_pmic_CA_cleanup(fd, thread_num) != TPASS)
        {
                return TFAIL;
        }
        return TPASS;
}
