/*====================

        @file   rng_test_module.c

        @brief  rng API

======================*/
/*
* Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
*/

/*
* The code contained herein is licensed under the GNU General Public
* License. You may obtain a copy of the GNU General Public License
* Version 2 or later at the following locations:
*
* http://www.opensource.org/licenses/gpl-licensisr_locke.html
* http://www.gnu.org/copyleft/gpl.html
*/

/*====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Rakesh S Joshi               29/08/2006     TLSbo74375   Initial version
A.Ozerov/b00320              11/12/2006     TLSbo84161   Minor changes.
Rakesh S Joshi               23/01/2007     TLSbo87892   Removed UCOREGISTER_TWICE case


====================
Portability:  ARM GCC
======================

======================
Total Tests: 1

Test Strategy:  Examine the RNG module functions
=====================

======================
                                        INCLUDE FILES
======================*/

#include "rng_test.h"
#include "rng_test_module.h"

/*====================*/

extern char *TCID;
extern int rng_testcase;
extern unsigned long argument;
int     fd = 0;
static uint8_t *seed_val;
rng_random_seed add_struct,
        get_struct;
int     i;
int     rng_getrandom_nonblk(void);
int     rng_getrandom_blk(void);

/*====================*/
int VT_rng_test(void)
{
        seed_val = malloc(argument * sizeof(uint8_t));
        if (NULL == seed_val)
                return TFAIL;

        switch (rng_testcase)
        {
                /* Random Number Configurations */
                /* Configure Pool Value To Zero and Register. With Pool = 0, registration should fail
                */
        case CASE_TEST_RNG_UCOREGISTER_POOL:
                if (RNG_FAILURE == ioctl(fd, CASE_TEST_RNG_UCOREGISTER_POOL))
                {
                        tst_resm(TFAIL, "TEST FAILED ");
                        return TFAIL;
                }
                break;
                /* Set the Blocking Mode and Callback Mode Bits, and try to register. The
                * registration should fail. */
        case CASE_TEST_RNG_UCOREGISTER_FLAGS:
                if (RNG_FAILURE == ioctl(fd, CASE_TEST_RNG_UCOREGISTER_FLAGS))
                {
                        tst_resm(TFAIL, "TEST FAILED");
                        return TFAIL;
                }
                break;
                /* RNG In blocking mode. Will get the Random Numbers. */
        case CASE_TEST_RNG_GETRANDOM_BLK:
                rng_getrandom_blk();
                printf("RANDOM NUMBERS RECEIVED ARE:\n");
                for (i = 0; i < get_struct.rng_no; i++)
                        printf("\n The Random No are:%d \n", *(seed_val + i));
                break;
                /* RNG In Non-Blocking mode without callback function. Will get the Random Numbers. */
        case CASE_TEST_RNG_GETRANDOM_NONBLK:
                rng_getrandom_nonblk();
                printf("RANDOM NUMBERS RECEIVED ARE:\n");
                for (i = 0; i < get_struct.rng_no; i++)
                        printf("\n The Random No are:%d \n", *(seed_val + i));
                break;
                /* RNG In Non-Blocking mode with Callback function. Will get the Random Numbers. */
        case CASE_TEST_RNG_GETRANDOM_NONBLK_CALLBACK:
                get_struct.rng_no = argument;
                get_struct.rng_data = seed_val;
                if (RNG_FAILURE == ioctl(fd, CASE_TEST_RNG_GETRANDOM_NONBLK_CALLBACK, &get_struct))
                {
                        tst_resm(TFAIL, "Failed to get the Random data");
                        return TFAIL;
                }
                seed_val = get_struct.rng_data;
                printf("RANDOM NUMBERS RECEIVED ARE:\n");
                for (i = 0; i < get_struct.rng_no; i++)
                        printf("\n The Random No are:%d \n", *(seed_val + i));
                break;
                /* RNG In Blocking Mode. Will add the n number of seed values. So */
        case CASE_TEST_RNG_ADDENTROPY_BLK:
                rng_getrandom_blk();    /* Get the Random Number, and use them as a new seed value */
                add_struct.rng_no = argument;
                add_struct.rng_data = seed_val; /* The seed_val will be updated with the generated
                                                * random numbers. */
                printf("The SEED VALs are:\n");
                for (i = 0; i < argument; i++)
                        printf("Seed %d is: %d\n", (i + 1), *(seed_val + i));
                if (RNG_FAILURE == ioctl(fd, CASE_TEST_RNG_ADDENTROPY_BLK, &add_struct))
                {
                        tst_resm(TFAIL, "Failed to add entropy\n");
                        return TFAIL;
                }
                rng_getrandom_blk();    /* Get the Random Number after the seed values have been
                                        * added */
                printf("RANDOM NUMBERS RECEIVED ARE:\n");
                for (i = 0; i < get_struct.rng_no; i++)
                        printf("\n The Random No are:%d \n", *(seed_val + i));
                break;

        case CASE_TEST_RNG_ADDENTROPY_NONBLK:
                rng_getrandom_nonblk(); /* Get the Random Number, and use them as a new seed value */
                add_struct.rng_no = argument;
                add_struct.rng_data = seed_val; /* The seed_val will be updated with the generated
                                                * random numbers. */
                printf("The SEED VALs are:\n");
                for (i = 0; i < argument; i++)
                        printf("Seed %d is: %d\n", (i + 1), *(seed_val + i));
                if (RNG_FAILURE == ioctl(fd, CASE_TEST_RNG_ADDENTROPY_BLK, &add_struct))
                {
                        tst_resm(TFAIL, "Failed to add entropy\n");
                        return TFAIL;
                }
                rng_getrandom_nonblk(); /* Get the Random Number after the seed values have been
                                        * added */
                printf("RANDOM NUMBERS RECEIVED ARE:\n");
                for (i = 0; i < get_struct.rng_no; i++)
                        printf("\n The Random No are:%d \n", *(seed_val + i));
                break;
        default:
                printf("Invalid Argument\n");
                break;

        }       /* End of Switch */
        return TPASS;
}

/*====================*/
int rng_getrandom_nonblk(void)
{
        get_struct.rng_no = argument;
        get_struct.rng_data = seed_val;
        if (RNG_FAILURE == ioctl(fd, CASE_TEST_RNG_GETRANDOM_NONBLK, &get_struct))
        {
                tst_resm(TFAIL, "Failed to get the Random data");
                return TFAIL;
        }
        seed_val = get_struct.rng_data;
        return 0;
}

/*====================*/
int rng_getrandom_blk(void)
{
        get_struct.rng_no = argument;
        get_struct.rng_data = seed_val;
        if (RNG_FAILURE == ioctl(fd, CASE_TEST_RNG_GETRANDOM_BLK, &get_struct))
        {
                tst_resm(TFAIL, "Failed to get the Random data");
                return TFAIL;
        }
        seed_val = get_struct.rng_data;
        return 0;
}

/*====================*/
int VT_rng_test_setup(void)
{
        char    f_name[256] = "/dev/";

        strcat(f_name,RNG_DEVICE_NAME);

        if ((fd = open(f_name, O_RDWR)) < 0)
        {
                tst_resm(TFAIL, "VT_rng_test_setup() Failed open device");
                return TFAIL;
        }
        return TPASS;
}

/*====================*/
int VT_rng_test_cleanup(void)
{
        // free(seed_val);
        if (fd > 0)
                close(fd);
        return TPASS;
}

/*====================*/
void help(void)
{
        printf("  -T name Testcase name\n");
        printf("  -A x    Testcase additional arguments\n");
}
