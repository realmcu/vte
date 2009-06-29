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
        @file   pmic_test_S_IT_U.c

        @brief  Interrupt Test scenario C source for PMIC Protocol driver test.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
D.Khoroshev/b00313           12/08/2005     TLSbo56844  Initial version
D.Khoroshev/b00313           02/15/2006     TLSbo59968  Added test module
D.Khoroshev/b00313           06/26/2006     TLSbo59968  Removed timeout. Added checkings for callback
                                                        function calls
D.Khoroshev/b00313           07/25/2006     TLSbo64239  Added mc13783 legacy API support

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
#include "pmic_test_S_IT_U.h"

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
/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
extern sig_atomic_t sig_count;
extern char device_name[32];

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== VT_pmic_S_IT_U_setup =====*/
/**
@brief  Opens device file and allocates test sequence array.

@param  Input:  thread_num - number of thread
        Output: fd - descriptor of device file

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_S_IT_U_setup(int *fd, int thread_num)
{
        *fd = open(device_name, O_RDWR);
        if (*fd < 0)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TFAIL, "[%d]Unable to open %s", thread_num, device_name);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }

        /* This test is manual. Disable timeout */
        sig_count = 0;

        return TPASS;
}

/*================================================================================================*/
/*===== VT_pmic_S_IT_U_cleanup =====*/
/**
@brief  Closes device file and frees memory for array test_sequnce.

@param  Input:  fd - descriptor of device file
                thread_num - number of thread

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_S_IT_U_cleanup(int fd, int thread_num)
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
/*===== VT_pmic_test_S_IT_U =====*/
/**
@brief  This function executes Interrupt test scenario

@param  Input: thread_num - number of thread.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_test_S_IT_U(int thread_num)
{

        int fd = -1;
        int event, i=1, user_val;

        if(VT_pmic_S_IT_U_setup(&fd, thread_num) != TPASS)
        {
                VT_pmic_S_IT_U_cleanup(fd, thread_num);
                return TFAIL;
        }

#ifdef CONFIG_MXC_PMIC_SC55112
        event = EVENT_ONOFFI;
#endif
#ifdef CONFIG_MXC_PMIC_MC13783  
        event = EVENT_ONOFD1I;
#endif
#ifdef CONFIG_MXC_PMIC_MC13892
        event = EVENT_PWRONI;
#endif
#ifdef CONFIG_MXC_PMIC_MC9SDZ60 
        event = EVENT_POWER_KEY;
#endif
        printf("EVENT is %d\n",event);
        if (VT_pmic_subscribe(fd, event) != PMIC_SUCCESS)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TWARN,"[%d]Event #%d did not subscribed.",thread_num,event);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }
        pthread_mutex_lock(&mutex);
        printf("\n\tPress PWR button,(Either on the Atlas Card Or on the Keypad) you should see IT callback info");
        printf("\n\tIf you see the IT callback info then press \"ENTER\" key to continue the test\n");
        pthread_mutex_unlock(&mutex);
        getchar();

        if (VT_pmic_unsubscribe(fd, event) != PMIC_SUCCESS)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TWARN,"[%d]Event #%d did not unsubscribed.",thread_num,event);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }

        /*=============================================================
                Trying to subscribe 2 callback functions
        ===============================================================*/
        pthread_mutex_lock(&mutex);
        printf("Test subscribe/unsubscribe 2 event = %d\n", event);
        pthread_mutex_unlock(&mutex);
        if (VT_pmic_subscribe(fd, event) != PMIC_SUCCESS)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TWARN,"[%d]Event #%d did not subscribed.",thread_num,event);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }
        if (VT_pmic_subscribe(fd, event) != PMIC_SUCCESS)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TWARN,"[%d]Event #%d did not subscribed.",thread_num,event);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }
        pthread_mutex_lock(&mutex);
        printf("\n\tPress PWR button,(Either on the Atlas Card Or on the Keypad) you should see IT callback info");
        printf("\n\tIf you see the IT callback info then press \"ENTER\" key to continue the test\n");
        pthread_mutex_unlock(&mutex);
        getchar();

        if (VT_pmic_unsubscribe(fd, event) != PMIC_SUCCESS)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TWARN,"[%d]Event #%d did not unsubscribed.",thread_num,event);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }
        if (VT_pmic_unsubscribe(fd, event) != PMIC_SUCCESS)
        {
                pthread_mutex_lock(&mutex);
                tst_resm(TWARN,"[%d]Event #%d did not unsubscribed.",thread_num,event);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
        }
        do
        {
                fflush(stdin);
                printf("Did you see the IT callback Y/N ?\n");
                user_val = fgetc(stdin);
                if (user_val == 'Y' || user_val == 'y')
                        i =1;
                else if(user_val == 'N' || user_val == 'n')
                {
                        i=1;
                        VT_pmic_S_IT_U_cleanup(fd, thread_num);
                        return TFAIL;
                }
                else
                        i = 2;
        } while(i==2);

        if(VT_pmic_S_IT_U_cleanup(fd, thread_num) != TPASS)
                return TFAIL;

        return TPASS;
}
