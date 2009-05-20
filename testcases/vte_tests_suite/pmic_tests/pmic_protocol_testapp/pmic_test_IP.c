/*====================*/
/**
        @file   pmic_test_IP.c

        @brief  Illegal parameters test scenario source file
                for SC55112 Protocol driver test application.
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
D.Khoroshev/b00313          07/20/2005     TLSbo52700   Initial version
D.Khoroshev/b00313          09/05/2005     TLSbo52700   Rework version
D.Khoroshev/b00313          01/13/2006     TLSbo59968   Added default data for MC13783
D.Khoroshev/b00313          07/25/2006     TLSbo64239   Added mc13783 legacy API support

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
#include "pmic_test_IP.h"

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
                                   LOCAL FUNCTION PROTOTYPES
======================*/
void fn_callback_IP(void *arg);

/*======================
                                       LOCAL VARIABLES
======================*/
unsigned int TEST_VALUE= 0xFFF000;

#ifdef CONFIG_MXC_PMIC_SC55112

/* Read only Registers */
#define nb_REG_RO 1
unsigned int TEST_REG_RO[nb_REG_RO] =
{
        REG_PSTAT
};

/* Non Applicable registers */
#define nb_REG_NA 41
unsigned int TEST_REG_NA[nb_REG_NA] =
{
        REG_IRQ_TEST,
        REG_TMOD_TRIM_3,
        REG_RTC_TOD,
        REG_RTC_TODA,
        REG_RTC_DAY,
        REG_RTC_DAYA,
        REG_RTC_CAL,
        REG_PWRCTRL,
        REG_ARB_REG,
        REG_RX_STATIC1,
        REG_RX_STATIC2,
        REG_RX_STATIC3,
        REG_RX_STATIC4,
        REG_RX_DYNAMIC,
        REG_TX_DYNAMIC,
        REG_TX_DYNAMIC2,
        REG_TX_STATIC1,
        REG_TX_STATIC2,
        REG_TX_STATIC3,
        REG_TX_STATIC4,
        REG_TX_STATIC5,
        REG_TX_STATIC6,
        REG_TX_STATIC7,
        REG_TX_STATIC8,
        REG_TX_STATIC9,
        REG_TX_STATIC10,
        REG_TX_STATIC11,
        REG_TX_STATIC12,
        REG_TX_STATIC13,
        REG_TX_STATIC14,
        REG_TX_STATIC15,
        REG_TX_STATIC16,
        REG_TX_STATIC17,
        REG_TX_STATIC18,
        REG_TONE_GEN,
        REG_GPS_REG,
        REG_TMODE_TRIM_1,
        REG_TMODE_TRIM_2,
        REG_T_RD_ERROR,
        REG_T_RD_NPCOUNT,
        REG_ADC2_ALL
};


/* Non available events */
#define nb_EVNT_NE 1
unsigned int TEST_EVNT_NA[nb_EVNT_NE] = { 13 };
#endif

#ifdef CONFIG_MXC_PMIC_MC13783
/* Read only Registers */
#define nb_REG_RO 5
unsigned int TEST_REG_RO[nb_REG_RO] =
{
        REG_INTERRUPT_SENSE_0,
        REG_INTERRUPT_SENSE_1,
        REG_POWER_UP_MODE_SENSE,
        REG_SPARE,
        REG_REVISION
};

/* Non Available registers */
#define nb_REG_NA 10
unsigned int TEST_REG_NA[nb_REG_NA] =
{
        REG_CONTROL_SPARE,
        REG_POWER_SPARE,
        REG_AUDIO_SPARE,
        REG_SPARE,
        REG_TRIM_0,
        REG_TEST_0,
        REG_TEST_1,
        REG_TEST_2,
        REG_TEST_3,
        REG_TRIM_1
};

/* Non available events */
#define nb_EVNT_NE 10
unsigned int TEST_EVNT_NA[nb_EVNT_NE] = { 5,15,17,18,20,23,26,40,46,47 };
#endif

#ifdef CONFIG_MXC_PMIC_MC13892
/* Read only Registers */
#define nb_REG_RO 3
unsigned int TEST_REG_RO[nb_REG_RO] =
{
        REG_INT_SENSE0,
        REG_INT_SENSE1,
        REG_PU_MODE_S
};

/* Non Available registers */
#define nb_REG_NA 6
unsigned int TEST_REG_NA[nb_REG_NA] =
{
        REG_TRIM0,
        REG_TEST0,
        REG_TEST1,
        REG_TEST2,
        REG_TEST3,
        REG_TRIM1
};

/* Non available events */
#define nb_EVNT_NE 10
unsigned int TEST_EVNT_NA[nb_EVNT_NE] = { 15,16,17,18,23,26,28,35,39,41,42,43 };
#endif
#ifdef CONFIG_MXC_PMIC_MC9SDZ60
/* Read only Registers */
#define nb_REG_RO 1
unsigned int TEST_REG_RO[nb_REG_RO] =
{
         REG_MCU_VERSION
};

/* Non Available registers */

/* Non available events */
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
                                       LOCAL FUNCTIONS
======================*/
/*====================*/
/*= VT_pmic_IP_setup =*/
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
int VT_pmic_IP_setup(int *fd, int thread_num)
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
/*= VT_pmic_IP_cleanup =*/
/**
@brief This function closes test module's device file and frees memory for test sequence array.

@param  Input:  fd - descriptor of test device file.
                test_sequence - pointer to array of test sequence.
                thread_num - number of thread.

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_pmic_IP_cleanup(int fd, int thread_num)
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
/*= VT_pmic_test_IP =*/
/**
@brief  SC55112 test scenario which trying to perform read/write, subscribe/unsubscribe with
        incorrect parametres.

@param  Input:  thread_num - number of thread.

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_pmic_test_IP(int thread_num)
{
        int fd, i;
        unsigned int TEST_VALUE_READ, INITIAL_READ;

        if(VT_pmic_IP_setup(&fd, thread_num) != TPASS)
        {
                VT_pmic_IP_cleanup(fd, thread_num);
                return TFAIL;
        }

        pthread_mutex_lock(&mutex);
        tst_resm(TINFO, "\nWRITING INTO READ ONLY REGISTER\n");
        pthread_mutex_unlock(&mutex);
        /* Writing into Read Only register */
        for(i=0; i<nb_REG_RO; i++)
        {
                /* Read the Values from the Register, before writing to the register. */
                if (VT_pmic_read(fd, TEST_REG_RO[i], &INITIAL_READ) != TPASS)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TFAIL, "FAILED TO READ");
                        pthread_mutex_unlock(&mutex);
                }
                /* Write some Values to the Register. */
                if (VT_pmic_write(fd, TEST_REG_RO[i], TEST_VALUE) == TPASS)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TINFO, "Try to write to read only reg %d", TEST_REG_RO[i]);
                        pthread_mutex_unlock(&mutex);
                }
                else
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TFAIL, "FAILED TO WRITE\n");
                        pthread_mutex_unlock(&mutex);
                        return TFAIL;
                }
                /* Read The Registers after write. */
                if (VT_pmic_read(fd, TEST_REG_RO[i], &TEST_VALUE_READ) != TPASS)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TFAIL, "Failed to read");
                        pthread_mutex_unlock(&mutex);
                }
                /* Now compare both the read Values. They should be same */
                if (TEST_VALUE_READ != INITIAL_READ)
                {
                            pthread_mutex_lock(&mutex);
                        tst_resm(TFAIL, "The read val is not same\n");
                        pthread_mutex_unlock(&mutex);
                        return TFAIL;
                }
        }

        pthread_mutex_lock(&mutex);
        tst_resm(TINFO, "\nTRYING ON NON AVAILABLE REGISTERS\n");
        pthread_mutex_unlock(&mutex);

#if defined(CONFIG_MXC_PMIC_MC13783) || defined(CONFIG_MXC_PMIC_MC13892)
        /* writing into non avaible register */
        for(i=0; i<nb_REG_NA; i++)
        {
                /* Read the Values from the Register, before writing to the register. */
                if (VT_pmic_read(fd, TEST_REG_NA[i], &INITIAL_READ) != TPASS)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TFAIL, "Failed to read");
                        pthread_mutex_unlock(&mutex);
                }
                /* Write some Values to the Register. */
                if (VT_pmic_write(fd, TEST_REG_NA[i], TEST_VALUE) == TPASS)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TINFO, "Try to write to non available reg %d", TEST_REG_NA[i]);
                        pthread_mutex_unlock(&mutex);
                }
                else
                {
                        tst_resm(TFAIL, "FAILED TO WRITE\n");
                        return TFAIL;
                }
                /* Read The Registers after write. */
                if (VT_pmic_read(fd, TEST_REG_NA[i], &TEST_VALUE_READ) != TPASS)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TFAIL, "Failed to read in NA");
                        pthread_mutex_unlock(&mutex);
                }
                /* Now compare both the read Values. They should be same */
                if (TEST_VALUE_READ != INITIAL_READ)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TFAIL, "The read val is not same\n");
                        return TFAIL;
                        pthread_mutex_unlock(&mutex);
                }
        }

        pthread_mutex_lock(&mutex);
        tst_resm(TINFO, "\nTrying to subscribe/unsubscribe for  NON existing events\n");
        pthread_mutex_unlock(&mutex);
#endif

#if defined(CONFIG_MXC_PMIC_MC13783) || defined(CONFIG_MXC_PMIC_MC13892)
        /* subscribing non existing event */
        for(i=0; i<nb_EVNT_NE; i++)
        {
                if (ioctl(fd, PMIC_SUBSCRIBE, &TEST_EVNT_NA[i]) != TPASS )
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TINFO, "Try to subscribe non existing event %d", TEST_EVNT_NA[i]);
                        pthread_mutex_unlock(&mutex);
                }
                else
                {
                        tst_resm(TFAIL, "got unexpected success, DRIVER FAILS\n");
                        return TFAIL;
                }

                /* unsubscribing not subscribed event */
                if (ioctl(fd, PMIC_UNSUBSCRIBE, &TEST_EVNT_NA[i]) != TPASS)
                {
                        pthread_mutex_lock(&mutex);
                        tst_resm(TINFO, "Try to unsubscribe not subscribed event %d", TEST_EVNT_NA[i]);
                        pthread_mutex_unlock(&mutex);
                }
                else
                {
                        tst_resm(TFAIL, "got unexpected success\n");
                        return TFAIL;
                }

        }
#endif
        if(VT_pmic_IP_cleanup(fd, thread_num) != TPASS)
        {
                return TFAIL;
        }
        return TPASS;
}
