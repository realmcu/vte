/*====================*/
/**
    @file   hacc_test.c

    @brief  hacc API test
*/
/*======================

        Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          10/08/2004     TLSbo40418   Initial version
S.ZAVJALOV/zvjs001c          01/10/2004     TLSbo40649   Version after inspection
S.ZAVJALOV/zvjs001c          11/10/2004     TLSbo43283   Version for 1.5 release kernel
S.ZAVJALOV/zvjs001c          04/11/2004     TLSbo43890   Chages for virt_to_phys function
S.ZAVJALOV/zvjs001c          04/07/2005     TLSbo51629   Change hacc test strategy
A.URUSOV                     14/09/2005     TLSbo53754   VT_hacc_test_setup warning eliminated,
                                                         data2hash_len type is specified
A.URUSOV                     18/10/2005     TLSbo57061   New test functions runs are added
======================*/

/*======================
Total Tests: 1

Test Executable Name:  hacc_test

Test Strategy:  Examine the HAC module functions
=====================*/

#include <sys/mman.h>
#include <sys/stat.h>

#include "hacc_test.h"

/*====================*/

extern char *TCID;

extern int contiguous_arrays;   /* indication of contiguous array */
extern int verbose_mode;
extern unsigned long data2hash;
extern int data2hash_len;
extern short int stop_flag;

int     fd = 0;
hacc_test_param kernel_arg;

/*====================*/
int VT_hacc_test(void)
{
        int     VT_rv = TFAIL;

        kernel_arg.start_hacc_addr = data2hash;
        kernel_arg.data2hash_len = data2hash_len;
        kernel_arg.verbose_mode = verbose_mode;
        kernel_arg.stop_flag = stop_flag;

        switch (contiguous_arrays)
        {
        case 0:
                if (ioctl(fd, CASE_TEST_HACC_02, &kernel_arg) == 0)
                        VT_rv = TPASS;
                break;
        case 1:
                if (ioctl(fd, CASE_TEST_HACC_01, &kernel_arg) == 0)
                        VT_rv = TPASS;
                break;
        case 2:
                if (ioctl(fd, CASE_TEST_HACC_03, &kernel_arg) == 0)
                        VT_rv = TPASS;
                break;
        case 3:
                if (ioctl(fd, CASE_TEST_HACC_04, &kernel_arg) == 0)
                        VT_rv = TPASS;
                break;
        case 4:
                if (ioctl(fd, CASE_TEST_HACC_05, &kernel_arg) == 0)
                        VT_rv = TPASS;
                break;
#ifdef CONFIG_PM
        case 5:
                if (ioctl(fd, CASE_TEST_HACC_06, &kernel_arg) == 0)
                        VT_rv = TPASS;
                break;
        case 6:
                if (ioctl(fd, CASE_TEST_HACC_07, &kernel_arg) == 0)
                        VT_rv = TPASS;
                break;
#endif                          /* CONFIG_PM */
        default:
                help();
        }
        return VT_rv;
}

/*====================*/
int VT_hacc_test_setup(void)
{
        char    f_name[256] = "/dev/";
        int     VT_rv = TFAIL;

        strcat(f_name, HACC_DEVICE_NAME);

        if ((fd = open(f_name, O_RDWR)) < 0)
        {
                VT_hacc_test_cleanup();
                tst_resm(VT_rv, "Failed open device %s", HACC_DEVICE_NAME);
        }
        else
        {
                VT_rv = TPASS;
                tst_resm(VT_rv, "Device named %s is opened", HACC_DEVICE_NAME);
        }
        return VT_rv;
}

/*====================*/
int VT_hacc_test_cleanup(void)
{
        if (fd > 0)
                close(fd);
        return TPASS;
}

/*====================*/
void help(void)
{
        printf("\nUsage: %s\n", TCID);
        printf("            [-S <start_address>]\n");
        printf("            [-L <lenght_block>]\n");
        printf("            [-C continued_arrays]\n");
        printf("            [-V verbose_mode]\n");
        printf("            [-T contiguous with stop test]\n");
        printf("            [-R software reset of the entire HAC Module test]\n");
        printf("            [-B the HAC Module burst mode set test]\n");
        printf("            [-D the HAC Module burst read nature configure test]\n");
#ifdef CONFIG_PM
        printf("            [-N suspends HAC Module test]\n");
        printf("            [-M resumes HAC Module test]\n");
#endif                          /* CONFIG_PM */
        printf("            [-H this help]\n");
}
