/*====================*/
/**
        @file   dvfs_dptc_test.c

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
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.Khalabuda/b00306           22/11/2006     TLSbo83054  Initial version

====================
Portability: ARM GCC

======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Verification Test Environment Include Files */
#include "dvfs_dptc_test.h"

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
static int      dvfs_dptc_device = -1;  /* dptc device file descriptor */
static int      proc_dptc_device = -1;  /* dptc device file descriptor in proc interface */
static void    *in_buffer;
static int      table_file;             /* for reading and update test */
static const char      *table_file_name = "dptc_table";

/*! Data structure that holds the DVFS/DPTC log entries after formatting them into text lines. */
log_table_s     log_table;
int     dptc_is_active,
        dvfs_is_active;

/*======================
                                        GLOBAL CONSTANTS
======================*/

/*======================
                                        GLOBAL VARIABLES
======================*/
DVFS_TEST_CASES test_case = 0;
extern char     device_name[128];
extern unsigned int     arg_case;

/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/

/*======================
                                        LOCAL FUNCTIONS
======================*/
void    readkey_routine(void *);

/*====================*/
/*= VT_dvfs_dptc_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int     VT_dvfs_dptc_setup(void)
{
        int     rv = TPASS;

        /* open DVFS/DPTC device */
        tst_resm(TINFO, "Open %s test", device_name);
        dvfs_dptc_device = open(device_name, O_RDWR);
        if (dvfs_dptc_device == -1)
        {
                rv = TBROK;
                tst_brkm(TBROK, cleanup, "Can't open %s", device_name);
        }

        /* open proc DPTC */
        tst_resm(TINFO, "Open %s test", DPTC_PROC_DEVICE);
        proc_dptc_device = open(DPTC_PROC_DEVICE, O_RDONLY);
        if (proc_dptc_device == -1)
        {
                rv = TBROK;
                tst_brkm(TBROK, cleanup, "Failed open %s", DPTC_PROC_DEVICE);
        }

        return  rv;
}

/*====================*/
/*= VT_dvfs_dptc_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int     VT_dvfs_dptc_cleanup(void)
{
        int     rv = TPASS;

        if (dvfs_dptc_device != -1)
        {
                if (table_file >= 0)
                {
                        if (close(table_file) < 0)
                        {
                                tst_resm(TWARN, "Can't close %s file", table_file);
                                rv = TWARN;
                        }
                }

                if (dptc_is_active > 0)
                {
                        if (ioctl(dvfs_dptc_device, DPTC_IOCTDISABLE) < 0)
                        {
                                tst_resm(TWARN, "Disable the DPTC module failed");
                                rv = TWARN;
                        }
                }

                /* close DVFS/DPTC device */
                if (close(dvfs_dptc_device) < 0)
                {
                        rv = TBROK;
                        tst_brkm(TBROK, cleanup, "Close %s failed", DPTC_DEVICE);
                }
                else
                        tst_resm(TINFO, "Close %s passed", DPTC_DEVICE);

                /* close proc DPTC */
                if (close(proc_dptc_device) < 0)
                {
                        rv = TBROK;
                        tst_brkm(TBROK, cleanup, "Close %s failed", DPTC_PROC_DEVICE);
                }
                else
                        tst_resm(TINFO, "Close %s passed", DPTC_PROC_DEVICE);
        }

        return  rv;
}

/*====================*/
/*= VT_dvfs_dptc_read_test =*/
/**
@brief  This function reads the DPTC translation table used by the DPTC driver via
        the DPTC daemon and writes it to an output file.
        The function sends a READ_TABLE command to the DPTC daemon and waits
        for the returned data containing the DPTC taranslation table currently used
        by the DPTC driver.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int     VT_dvfs_dptc_read_test(void)
{
        int     rv = TPASS;

        /* allocating memory for holding the DTPC table */
        if (NULL == (in_buffer = malloc(MAX_TABLE_SIZE/2)))
        {
                tst_resm(TFAIL, "Can't allocate %d bytes", MAX_TABLE_SIZE/2);
                rv = TFAIL;
        }

        /* Check if memory was allocated */
        if (in_buffer)
        {
                memset(in_buffer, 0, MAX_TABLE_SIZE/2);

                /* Wait and read message from daemon containing table data */
                if (ioctl(dvfs_dptc_device, PM_IOCGTABLE, in_buffer) >= 0)
                {
                        /* Open output file for writing */
                        if ((table_file = open(table_file_name, O_WRONLY | O_CREAT)) < 0)
                        {
                                /* Unable to open output file */
                                tst_resm(TFAIL, "Error opening file %s", table_file_name);
                                rv = TFAIL;
                        }
                        else
                        {
                                /* Write table data to output file */
                                if (write(table_file, in_buffer, strlen(in_buffer)+1) < 0)
                                {
                                        tst_resm(TFAIL, "Can't write to the '%s'", table_file_name);
                                        rv = TFAIL;
                                }

                                /* Close output file */
                                if (close(table_file) < 0)
                                {
                                        tst_resm(TFAIL, "Can't close %s file", table_file);
                                        rv = TFAIL;
                                }
                        }
                }
                else
                {
                        /* Error receiving table data */
                        tst_resm(TFAIL, "Error receiving message");
                        rv = TFAIL;
                }

                /* Free allocated memory */
                SAFE_DELETE(in_buffer);
        }
        else
        {
                /* Unable to allocate memory for table */
                tst_resm(TFAIL, "Error allocating memory");
                rv = TFAIL;
        }

        tst_resm(TPASS, "Reading from %s passed", device_name);

        return  rv;
}

/*====================*/
/*= VT_dvfs_dptc_write_test =*/
/**
@brief  This function reads a new DPTC translation table from a file and
        updates the DPTC driver to the new table through the DPTC daemon.
        The function gets from the user the file name of the new translation table
        opens and reads it and updates the driver by using the send_table function.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int     VT_dvfs_dptc_write_test(void)
{
        int     rv = TPASS;
        int     count = 0;

        /* allocating memory for holding the DTPC table */
        if (NULL == (in_buffer = malloc(MAX_TABLE_SIZE)))
        {
                tst_resm(TFAIL, "Can't allocate %d bytes", MAX_TABLE_SIZE);
                rv = TFAIL;
        }

        /* Check if memory was allocated */
        if (in_buffer)
        {
                memset(in_buffer, 0, MAX_TABLE_SIZE);

                /* reading from file and wrinitg to the DPTC */
                if ((table_file = open(table_file_name, O_RDONLY)) < 0)
                {
                        tst_resm(TFAIL, "Can't open %s file", table_file);
                        rv = TFAIL;
                }

                /* reading DPTC table from file */
                count = read(table_file, in_buffer, strlen(in_buffer)+1);
                if (count <= MAX_TABLE_SIZE )
                {
                        if (ioctl(dvfs_dptc_device, PM_IOCSTABLE, (void*)in_buffer) < 0)
                        {
                                tst_resm(TFAIL, "Writing to the %s failed", device_name);
                                rv = TFAIL;
                        }
                        else
                                tst_resm(TINFO, "Writing to the %s passed", device_name);
                }
                else if (count < 0)        /* I/O error */
                {
                        tst_resm(TFAIL, "Can't read from %s", table_file_name);
                        rv = TFAIL;
                }
                else        /* invalid DPTC table */
                {
                        tst_resm(TFAIL, "Invalid DPTC table");
                        rv = TFAIL;
                }

                SAFE_DELETE(in_buffer);
        }
        else
        {
                /* Unable to allocate memory for table */
                tst_resm(TFAIL, "Error allocating memory");
                rv = TFAIL;
        }

        return  rv;
}

/*====================*/
/*= VT_dvfs_dptc_test =*/
/**
@brief  Template test scenario X function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int     VT_dvfs_dptc_test(void)
{
        int     rv = TPASS;
        int     count = -1;

        switch (test_case)
        {
        case INIT_TEST:
                tst_resm(TINFO, "INIT test");

                /* enable device */
                tst_resm(TINFO, "Enable DPTC module command test");
                if (ioctl(dvfs_dptc_device, DPTC_IOCTENABLE) < 0)
                {
                        tst_resm(TFAIL, "Enable DPTC %s failed", device_name);
                        rv = TFAIL;
                }
                else
                        tst_resm(TINFO, "Enable DPTC %s succeeded", device_name);

                if (!strcmp(device_name, DVFS_DEVICE) || !strcmp(device_name, DVFS_DPTC_DEVICE))
                {
                        tst_resm(TINFO, "Enable DVFS module command test");
                        if (ioctl(dvfs_dptc_device, DVFS_IOCTENABLE) < 0)
                        {
                                tst_resm(TFAIL, "Enable DVFS %s failed", device_name);
                                rv = TFAIL;
                        }
                        else
                                tst_resm(TPASS, "Enable DVFS %s succeeded", device_name);
                }

                /* should be active */
                tst_resm(TINFO, "Get current DPTC module state command test");
                dptc_is_active = ioctl(dvfs_dptc_device, DPTC_IOCGSTATE);
                if (dptc_is_active < 0)
                {
                        tst_resm(TFAIL, "DPTC IOCTL activate %s failed", device_name);
                        rv = TFAIL;
                }
                else if (dptc_is_active == 0)        /* not active */
                {
                        tst_resm(TFAIL, "DPTC IOCTL should be acive %s after enabling", device_name);
                        rv = TFAIL;
                }
                else
                        tst_resm(TPASS, "DPTC IOCTL is active %s after enabling", device_name);

                if (!strcmp(device_name, DVFS_DEVICE) || !strcmp(device_name, DVFS_DPTC_DEVICE))
                {
                        tst_resm(TINFO, "Get current DVFS module state command test");
                        dvfs_is_active = ioctl(dvfs_dptc_device, DVFS_IOCGSTATE);
                        if (dvfs_is_active < 0)
                        {
                                tst_resm(TFAIL, "DVFS IOCTL activate %s failed", device_name);
                                rv = TFAIL;
                        }
                        else if (dvfs_is_active == 0)        /* not active */
                        {
                                tst_resm(TFAIL, "DVFS IOCTL should be acive %s after enabling", device_name);
                                rv = TFAIL;
                        }
                        else
                                tst_resm(TPASS, "DVFS IOCTL is active %s after enabling", device_name);

                        /* disable DVFS/DPTC */
                        tst_resm(TINFO, "Disable DVFS module command test");
                        if (ioctl(dvfs_dptc_device, DVFS_IOCTDISABLE) < 0)
                        {
                                tst_resm(TFAIL, "DVFS IOCTL disable module %s failed", device_name);
                                rv = TFAIL;
                        }
                        else
                        {
                                tst_resm(TPASS, "DVFS IOCTL disable module %s passed", device_name);
                                dptc_is_active = 0;
                        }
                }

                tst_resm(TINFO, "Disable DPTC module command test");
                if (ioctl(dvfs_dptc_device, DPTC_IOCTDISABLE) < 0)
                {
                        tst_resm(TFAIL, "DPTC IOCTL disable module %s failed", device_name);
                        rv = TFAIL;
                }
                else
                {
                        tst_resm(TPASS, "DPTC IOCTL disable module %s passed", device_name);
                        dptc_is_active = 0;
                }

                break;
        case RW_TEST:
                /* Read DPTC driver translation table command test */
                tst_resm(TINFO, "READING test");
                rv = VT_dvfs_dptc_read_test();

                /* Update DPTC driver translation table command test */
                tst_resm(TINFO, "UPDATING test");
                rv = VT_dvfs_dptc_write_test();

                break;
        case RC_TEST:
                tst_resm(TINFO, "REFERENCE CIRCUIT test");

                /* enable dptc reference circuit */
                tst_resm(TINFO, "Enable DPTC reference circuit command test");
                if (ioctl(dvfs_dptc_device, DPTC_IOCSENABLERC, arg_case) < 0)
                {
                        tst_resm(TFAIL, "DPTC IOCTL enable reference circuit %s failed", device_name);
                        rv = TFAIL;
                }
                else
                        tst_resm(TINFO, "DPTC IOCTL enable reference circuit %s succeeded", device_name);

                /* disable dptc reference circuit */
                tst_resm(TINFO, "Disable DPTC reference circuit command test");
                if (ioctl(dvfs_dptc_device, DPTC_IOCSDISABLERC, ~ arg_case) < 0)
                {
                        tst_resm(TFAIL, "DPTC IOCTL disable reference circuit %s failed", device_name);
                        rv = TFAIL;
                }
                else
                        tst_resm(TINFO, "DPTC IOCTL disable reference circuit %s passed", device_name);

                break;
        case WP_TEST:
                tst_resm(TINFO, "WORKING POINT test");

                /* sets working point according to parameter */
                tst_resm(TINFO, "Sets working point according to parameter command test");
                if (ioctl(dvfs_dptc_device, DPTC_IOCSWP, arg_case) < 0)
                {
                        tst_resm(TFAIL, "DPTC IOCTL set working point on %s failed", device_name);
                        rv = TFAIL;
                }
                else
                        tst_resm(TINFO, "DPTC IOCTL set working point on %s succeeded", device_name);

                break;
        case FREQ_TEST:
                tst_resm(TINFO, "FREQUENCY test");

                /* get current frequency command */
                tst_resm(TINFO, "Get current frequency command test");
                count = ioctl(dvfs_dptc_device, PM_IOCGFREQ);
                if (count < 0)
                {
                        tst_resm(TFAIL, "PM IOCTL ARM frequency on %s failed", device_name);
                        rv = TFAIL;
                }
                else
                        tst_resm(TINFO, "PM IOCTL ARM frequency is %dMHz on %s succeeded", count/1000000, device_name);

                break;
        case SW_TEST:
                tst_resm(TINFO, "SW general purpose bits test");

                /* Set SW gp values */
                tst_resm(TINFO, "Set SW general purpose bits command test");
                count = ioctl(dvfs_dptc_device, DVFS_IOCSSWGP, arg_case);
                if (count < 0)
                {
                        tst_resm(TFAIL, "DVFS IOCTL set SW gp values on %s failed", device_name);
                        rv = TFAIL;
                }
                else
                        tst_resm(TINFO, "DVFS IOCTL set SW gp values on %s succeeded", device_name);

                break;
        case WFI_TEST:
                tst_resm(TINFO, "Wait-For-Interrupt state test");

                /* Set wait-for-interrupt state */
                tst_resm(TINFO, "Set wait-for-interrupt state command test");
                count = ioctl(dvfs_dptc_device, DVFS_IOCSWFI, arg_case);
                if (count < 0)
                {
                        tst_resm(TFAIL, "DVFS IOCTL set wfi state on %s failed", device_name);
                        rv = TFAIL;
                }
                else
                        tst_resm(TINFO, "DVFS IOCTL set wfi state on %s succeeded", device_name);

                break;
        default:
                rv = TBROK;
                tst_brkm(TBROK, cleanup, "Bad test number choice");

                break;
        }

        return  rv;
}
