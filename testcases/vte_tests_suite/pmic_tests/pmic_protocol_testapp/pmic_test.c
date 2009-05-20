/*====================*/
/**
        @file   pmic_test.c

        @brief  This file contains functions, which executes testcases or some specific options.
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
D.Khoroshev/b00313           07/20/2005     TLSbo52700  Initial version
D.Khoroshev/b00313           09/06/2005     TLSbo52700  Rework version
D.Khoroshev/b00313           07/25/2006     TLSbo64239  Added mc13783 legacy API support

====================
Portability: ARM GCC
======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "pmic_test.h"
#include "pmic_test_common.h"
#include "pmic_test_RW.h"
#include "pmic_test_SU.h"
#include "pmic_test_CA.h"
#include "pmic_test_IP.h"
#include "pmic_test_S_IT_U.h"

/*======================
                                        LOCAL MACROS
======================*/

/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/

/*======================
                                       LOCAL CONSTANTS
======================*/

/*======================
                                       LOCAL VARIABLES
======================*/

/*======================
                                       GLOBAL CONSTANTS
======================*/
extern char device_name[32];

/*======================
                                       GLOBAL VARIABLES
======================*/

/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/

/*======================
                                       LOCAL FUNCTIONS
======================*/

/*====================*/
/*= VT_pmic_exec_opt =*/
/**
@brief  This universal function opens SC55112 protocol test module's virtual device, performs
        specified operation and then closes it.

@param  Input :        params - pointer to opt_params structure,
                        operation - performing operation (PMIC_READ_REG_T, PMIC_WRITE_REG_T,
                                    PMIC_SUBSCRIBE_T, PMIC_UNSUBSCRIBE_T, PMIC_CHECK_SENSOR_T,
                                    PMIC_GET_SENSORS_T)
                        val1 - reg number if operation is PMIC_READ_REG_T or PMIC_WRITE_REG_T,
                                sensor number if operation PMIC_CHECK_SENSOR_T.
                        val2 - writing reg value in case of PMIC_WRITE_REG_T operation, pointer
                                to event_type_notification structure of PMIC_SUBSCRIBE_T,
                                PMIC_UNSUBSCRIBE_T operation or PMIC_GET_SENSORS_T.
         Output:        val2 - return reg value in case of PMIC_READ_REG_T operation,
                             - return sensor state in case of PMIC_CHECK_SENSOR_T operation,
                             - return pointer to structure t_sensor_bits if performing operation
                               is PMIC_GET_SENSORS_T. Memory for the structure should be allocated

@return Returns result of performing operation
        On success - return TPASS
        On failure - return TFAIL
*/
/*====================*/
void *VT_pmic_exec_opt(void *params)
{
        int fd, rv=TFAIL;
        fd = open(PMIC_DEVICE, O_RDWR);
        if (fd < 0)
        {
                tst_resm(TFAIL, "Unable to open %s", PMIC_DEVICE);
                return (void*)TFAIL;
        }

        rv = close(fd);
        if (rv < 0)
        {
                tst_resm(TFAIL, "Unable to close file descriptor %d, returned code %d", fd, rv);
                return (void*)rv;
        }

        return (void*)rv;
}

/*====================*/
/*= VT_pmic_exec_test_case =*/
/**
@brief  exec specified SC55112 test case scenario.

@param  Input:  t_param - test case parameters, contains test case identifier and number of thread.

@return Returns result of execution specified SC55112 test case scenario.
        On success - return TPASS
        On failure - return the error code
*/
/*====================*/
void *VT_pmic_exec_test_case(void *t_param)
{
        int VT_rv = 0;

        switch(((test_param*)t_param)->test_id)
        {
        case PMIC_PROTOCOL_RW:
                VT_rv = VT_pmic_test_RW( ((test_param*)t_param)->thread_num );
                break;
        case PMIC_PROTOCOL_SU:
                VT_rv = VT_pmic_test_SU( ((test_param*)t_param)->thread_num );
                break;
        case PMIC_PROTOCOL_CA:
                VT_rv = VT_pmic_test_CA( ((test_param*)t_param)->thread_num );
                break;
        case PMIC_PROTOCOL_IP:
                VT_rv = VT_pmic_test_IP( ((test_param*)t_param)->thread_num );
                break;
        case PMIC_PROTOCOL_S_IT_U:
                VT_rv = VT_pmic_test_S_IT_U( ((test_param*)t_param)->thread_num );
                break;
        default:
                tst_resm(TFAIL,"VT_pmic_exec_test_case: Unknown operation (%d)",((test_param*)t_param)->test_id);
                VT_rv= PMIC_NOT_SUPPORTED;
        }

        return (void*)VT_rv;
}
