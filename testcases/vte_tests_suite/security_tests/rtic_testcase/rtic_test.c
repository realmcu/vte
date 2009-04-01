/*================================================================================================*/
/**

    @file   rtic_test.c

    @brief  rtic API test
*/
/*==================================================================================================

        Copyright 2004, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version
I.Inkina\nknl001             30/08/2005     TLSbo53743   The pointer size was checked
A.URUSOV                     13/09/2005     TLSbo55076   Fix compilation issue and warnings
A.URUSOV                     27/10/2005     TLSbo57063   New tests is added
A.URUSOV                     01/11/2005     TLSbo57063   Compile under L26.1.14
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  rtic_test

Test Strategy: Examine the RTIC module functions
=================================================================================================*/
#include <sys/mman.h>
#include <sys/stat.h>

#include "rtic_test.h"
/*================================================================================================*/

extern char *TCID;
extern char *data_file_path;
extern rtic_test_param arg;
extern int runtime_mode;

int fd = 0;

/*================================================================================================*/
int VT_rtic_test (void)
{
        int data_file;
        int tmp, ret_Vl = TFAIL;
        struct stat filestat;
        char *file_pointer;
        unsigned long *rtic_data;

        if (data_file_path != 0)
        {
                if ((data_file = open(data_file_path, O_RDONLY)) < 0)
                {
                        tst_resm(TFAIL, "VT_rtic_test(): Failed open data file");
                        return TFAIL;
                }
                else
                {
                        fstat(data_file, &filestat);
                        file_pointer = mmap(0, filestat.st_size, PROT_READ, MAP_SHARED, data_file, 0);
                        arg.data_length = (unsigned long)filestat.st_size / 4;
                        if ((tmp = ((int)arg.data_length % 4)) != 0 )
                        {
                                arg.data_length = arg.data_length + 4 - tmp;
                        }
                        if (!(rtic_data = (unsigned long *)calloc(arg.data_length, sizeof(unsigned long))))
                        {
                                tst_resm(TFAIL, "VT_rtic_test(): Failed allocate memory");
                                close(data_file);
                                return TFAIL;
                        }
                        memcpy(rtic_data, file_pointer, filestat.st_size);
                }
                close(data_file);
        }

        arg.data = rtic_data;

        if ((ioctl(fd, runtime_mode, &arg)) == 0)
        {
                ret_Vl = TPASS;

                switch (runtime_mode)
                {
                case CASE_TEST_RTIC_ONETIME:
                        tst_resm(TPASS, "The Onetime Hashing is successful finished");
                break;
                case CASE_TEST_RTIC_RUNTIME:
                        tst_resm(TPASS, "The Runtime Hashing is successful finished");
                break;
                case CASE_TEST_RTIC_GET_CONTROL:
                        tst_resm(TPASS, "The Hash control register of the RTIC is readed");
                break;

                case CASE_TEST_RTIC_GET_FAULTADDRESS:
                        tst_resm(TPASS, "The Fault address register of the RTIC is readed");
                break;
                }
        }
        else
        {
                ret_Vl = TFAIL;
                switch (runtime_mode)
                {
                case CASE_TEST_RTIC_ONETIME:
                        tst_resm(TFAIL, "The Onetime Hashing is not successful finished");
                break;
                case CASE_TEST_RTIC_RUNTIME:
                        tst_resm(TFAIL, "The Runtime Hashing is not successful finished");
                break;
                case CASE_TEST_RTIC_GET_CONTROL:
                        tst_resm(TFAIL, "The Hash control register of the RTIC is not readed");
                break;

                case CASE_TEST_RTIC_GET_FAULTADDRESS:
                        tst_resm(TFAIL, "The Fault address register of the RTIC is not readed");
                break;
                }
        }
        /*free(rtic_data); */
        return ret_Vl;
}
/*================================================================================================*/
int VT_rtic_test_setup(void)
{
        char f_name[256]="/dev/";

        strcat(f_name, RTIC_DEVICE_NAME);

        if ((fd = open(f_name, O_RDWR)) < 0)
        {
                VT_rtic_test_cleanup();
                tst_resm(TFAIL, "VT_rtic_test_setup() Failed open device");
                return TFAIL;
        }
        else
        {
                tst_resm(TPASS, "VT_rtic_test_setup(): Device is opened");
        }

        return TPASS;
}
/*================================================================================================*/
int VT_rtic_test_cleanup(void)
{
        if (fd > 0) close (fd);
        return TPASS;
}
/*================================================================================================*/
void help(void)
{
        printf("  -M  x   Hashing type (RUNTIME or ONTIME)\n");
        printf("  -F  x   File which to be hashed\n");
        printf("  -R  x   Interrupt enabled (0) or disabled (1)\n");
        printf("  -B  x   Name of memory block, possible values A1|A2|B1...D2\n");
        printf("  -v      Verbose mode\n");
        printf("  -C      Reads the control register of the RTIC\n");
        printf("  -D      Reads the fault address register of the RTIC\n");
}
