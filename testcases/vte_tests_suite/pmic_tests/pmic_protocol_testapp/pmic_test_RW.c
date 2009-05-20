/*====================*/
/**
        @file   pmic_test_RW.c

        @brief  Read/Write test scenario source file for SC55112 Protocol driver test application.
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
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev/b00313           07/20/2005     TLSbo52700   Initital version
D.Khoroshev/b00313           09/05/2005     TLSbo52700   Rework version
D.Khoroshev/b00313           01/13/2006     TLSbo59968   Added default data for MC13783
D.Khoroshev/b00313           02/15/2006     TLSbo59968   Added test module
D.Khoroshev/b00313           07/25/2006     TLSbo64239   Added mc13783 legacy API support

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
#include "pmic_test_common.h"
#include "pmic_test_RW.h"

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
#define nb_reg_tested 2
#define nb_value 5
unsigned int TEST_VALUE_RW[nb_value] = { 0x000000, 0xFFFFFF, 0x555555, 0xAAAAAA, 0x000000 };

#ifdef CONFIG_MXC_PMIC_SC55112
unsigned int TEST_REGISTER[nb_reg_tested] =
{
        REG_ADC1,
        REG_ADC1,
};
#elif CONFIG_MXC_PMIC_MC13783
unsigned int TEST_REGISTER[nb_reg_tested] =
{
        REG_MEMORY_A,
        REG_MEMORY_B,
};
#elif CONFIG_MXC_PMIC_MC13892
unsigned int TEST_REGISTER[nb_reg_tested] =
{
    REG_MEM_A,
    REG_MEM_B,
};
#endif

/*======================
                                       GLOBAL CONSTANTS
======================*/

/*======================
                                       GLOBAL VARIABLES
======================*/
extern char *ifile_name;
extern char device_name[32];

/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/

/*======================
                                       LOCAL FUNCTIONS
======================*/

/*====================*/
/*= VT_pmic_RW_setup =*/
/**
@brief  This function opens test module's device file, and allocates memory for test sequence array.

@param  Input:   thread_num - number of thread.
        Output:  fd - descriptor of test module's device file.
                 nb_param - size of test_sequence.
                 test_sequence - pointer to array of test sequence.

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_pmic_RW_setup(int *fd, int thread_num)
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

/*====================*/
/*= VT_pmic_RW_cleanup =*/
/**
@brief This function closes test module's device file and frees memory for test sequence array.

@param  Input:  fd - descriptor of test device file.
                test_sequence - pointer to array of test sequence.
                thread_num - number of thread.

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_pmic_RW_cleanup(int fd, int thread_num)
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

/*====================*/
/*= VT_pmic_test_RW =*/
/**
@brief  This function writes set of registers with same defined values, read these registers
        and compares resturned value with initial one.

@param  Input:  thread_num - number of thread, which calls this function

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_pmic_test_RW(int thread_num)
{
    int fd, i, j;
    unsigned int valueW=0, valueR=0, reg=0, valueDef =0;

    if(VT_pmic_RW_setup(&fd, thread_num) != TPASS)
    {
        VT_pmic_RW_cleanup(fd, thread_num);
            return TFAIL;
    }
#if defined(CONFIG_MXC_PMIC_SC55112)|| defined(CONFIG_MXC_PMIC_MC13783) || defined(CONFIG_MXC_PMIC_MC13892)
    reg=TEST_REGISTER[i];
    for(j=0; j<nb_value; j++)
    {
        valueW=TEST_VALUE_RW[j];
        valueR=0;

        if (VT_pmic_write(fd, reg, valueW) != TPASS)
            return TFAIL;
        else
        {
            if (VT_pmic_read(fd, reg, &valueR) != TPASS)
                return TFAIL;
            if (valueR!=valueW)
            {
                pthread_mutex_lock(&mutex);
                tst_resm(TFAIL, "Test ERROR Read/Write res1=0x%X val=0x%X\n", valueW ,valueR);
                pthread_mutex_unlock(&mutex);
                return TFAIL;
            }
        }
    }
#endif
#ifdef CONFIG_MXC_PMIC_MC9SDZ60
    reg =  REG_MCU_ALARM_MINS;
    valueR=0;

    tst_resm(TINFO,"start pmic protocol testcases for mc9sdz60 \n");
    /*save default value*/
    if (VT_pmic_read(fd,reg,&valueR) != TPASS)
    {
        return TFAIL;
    }
    valueDef=valueR;

    /*write value to reg*/
    valueW=16;
    if (VT_pmic_write(fd, reg, valueW) != TPASS)
        return TFAIL;
    else
    {
        if (VT_pmic_read(fd, reg, &valueR) != TPASS)
            return TFAIL;
        if (valueR != valueW)
        {
            pthread_mutex_lock(&mutex);
            tst_resm(TFAIL, "Test ERROR Read/Write res1=0x%X val=0x%X\n", valueW ,valueR);
            pthread_mutex_unlock(&mutex);
            return TFAIL;
        }
    }
    tst_resm(TINFO,"comparation is ok \n");
    /*restore default value to reg*/
    valueW=valueDef;
    if (VT_pmic_write(fd, reg, valueW) != TPASS)
        return TFAIL;
    tst_resm(TINFO,"over \n");
#endif
        if(VT_pmic_RW_cleanup(fd, thread_num) != TPASS)
                return TFAIL;

        return TPASS;
}
