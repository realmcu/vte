/*================================================================================================*/
/**
        @file   pmic_test_SU.c

        @brief  Subscribe/Unsubscribe test scenario source file for PMIC
                Protocol driver test application
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
D.Khoroshev/b00313           07/20/2005     TLSbo52700  Initial version
D.Khoroshev/b00313           09/05/2005     TLSbo52700  Rework version
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
#include "pmic_test_SU.h"

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
extern char device_name[32];

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== fn_callback_SU_1 =====*/
/**
@brief  Callback using for subscribing events.

@param  Input:  arg - some argument, it's just displayed as integer value.

@return Nothing.
*/
/*================================================================================================*/
void fn_callback_SU_1(void *arg)
{
        pthread_mutex_lock(&mutex);
        tst_resm(TINFO,"callback fn_callback_SU_1 was called, arg is %d",(int)arg);
        pthread_mutex_unlock(&mutex);
}

/*================================================================================================*/
/*===== fn_callback_SU_2 =====*/
/**
@brief  Callback using for subscribing events.

@param  Input:  arg - some argument, it's just displayed as integer value.

@return Nothing.
*/
/*================================================================================================*/
void fn_callback_SU_2(void *arg)
{
        pthread_mutex_lock(&mutex);
        tst_resm(TINFO,"callback fn_callback_SU_2 was called, arg is %d",(int)arg);
        pthread_mutex_unlock(&mutex);
}

/*================================================================================================*/
/*===== VT_pmic_SU_setup =====*/
/**
@brief  This function opens test module's device file.

@param  Input:   thread_num - number of thread.
        Output:  fd - descriptor of test module's device file.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_SU_setup(int *fd, int thread_num)
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
/*===== VT_pmic_SU_cleanup =====*/
/**
@brief This function closes test module's device file.

@param  Input:  fd - descriptor of test device file.
                thread_num - number of thread.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_SU_cleanup(int fd, int thread_num)
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
/*===== VT_pmic_test_SU =====*/
/**
@brief  Subscribe/Unsubscribe test scenario function

@param  Input:  thread_num - number of thread

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_test_SU(int thread_num)
{
        int fd = -1;
        int event;

        if(VT_pmic_SU_setup(&fd, thread_num) != TPASS)
        {
                VT_pmic_SU_cleanup(fd, thread_num);
                return TFAIL;
        }
#ifdef CONFIG_MXC_PMIC_MC13783
        for(event= EVENT_ADCDONEI; event< EVENT_NB; event++)
#endif
#ifdef CONFIG_MXC_PMIC_MC13892
        for(event= EVENT_ADCDONEI; event< EVENT_NB; event++)
#endif
#ifdef CONFIG_MXC_PMIC_MC9SDZ60
        for(event= 2; event< 10; event++)
#endif
        {
#ifdef CONFIG_MXC_PMIC_MC13783
                if(event==5||event==15||event==17||event==18||event==20||event==23||event==26||event==40)
                    continue;
#endif
#ifdef  CONFIG_MXC_PMIC_MC13892
                if(event==15 ||event==16||event==17||event==18||event==23||event==26||event==28||event==35||event==39||event==41||event==42||event==43)  
                    continue;
#endif
#ifdef CONFIG_MXC_PMIC_SC55112
                if(event==13)
                    continue;
#endif
                printf("EVENT is %d\n",event);
                if (VT_pmic_subscribe(fd, event) != PMIC_SUCCESS)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TWARN,"[%d]Event #%d did not subscribed.",thread_num,event);
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
                tst_resm(TINFO,"SU event over \n");
        }

        if(VT_pmic_SU_cleanup(fd, thread_num) != TPASS)
        {
            return TFAIL;
        }

        return TPASS;
}
