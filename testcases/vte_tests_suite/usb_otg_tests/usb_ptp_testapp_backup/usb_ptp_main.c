/*====================*/
/**
        @file usb_HID_main.c

        @brief main file for USB-HID driver test
*/
/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "usb_ptp_test.h"

/*======================
                                    GLOBAL VARIABLES
======================*/
char   *TCID = "usb_ptp_testapp";

int     fd;     /* PMIC test device descriptor */
int     TST_TOTAL = 0;  /* total number of tests in this file. */
int     vflag = 0;      /* verbose flag */
char    device_name[128];
/*======================
                                GLOBAL FUNCTION PROTOTYPES
======================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);
void    help(void);

/*======================
                                    GLOBAL FUNCTIONS
======================*/
/*====================*/
/*= cleanup =*/
/**
@brief This function performs all one time clean up for this test on successful completion,
        premature exit or failure. Closes all temporary files, removes all temporary directories exits
        the test with appropriate return code by calling tst_exit() function.

@param None.

@return None.
*/
/*====================*/
void cleanup(void)
{
        int     rv = TFAIL;

        rv = VT_usb_ptp_test_cleanup();
        if (rv != TPASS)
        {
                tst_resm(TWARN, "usb_ptp_test_cleanup() Failed : error code = %d", rv);
        }

        tst_exit();
}

/*====================*/
/*= setup =*/
/**
@brief Performs all one time setup for this test. This function is typically used to capture
        signals, create temporary dirs and temporary files that may be used in the course of this test.

@param None.

@return None.
*/
/*====================*/
void setup(void)
{
        int     rv = TFAIL;

        rv = VT_usb_ptp_test_setup();
        if (rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "setup() Failed : error code = %d", rv);
        }
}

/*====================*/
/*= main =*/
/**
@brief Entry point to this test-case. It parses all the command line inputs, calls the global
        setup and executes the test. It logs the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func is called and test exits with an
        appropriate return code.

@param Input : argc - number of command line parameters.
        Output: **argv - pointer to the array of the command line parameters.

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*====================*/
int main(int argc, char **argv)
{
        int     rv = TFAIL;
              int     tflag = 0,
            vflg = 0,
            d_num,
            opt = 0;

        char   *msg = 0,
    *d_copt=0,
            *topt = 0;



        option_t options[] =
        {
    {"T:", &tflag, &topt},
                {"v", &vflg, NULL},
                {NULL, NULL, NULL}      /* NULL required to end array */
        };

        if ((msg = parse_opts(argc, argv, options, &help)) != NULL)
        {
                tst_resm(TWARN, "%s test didn't work as expected. Parse options error: %s", TCID,
                         msg);
                help();
                return rv;
        }


  if (vflg != 0)
        {
                vflag = vflg;
        }

        if (tflag == 0)
        {
                help();
                cleanup();
                return rv;
        }

        opt = atoi(topt);
        if (opt < 0 || opt > 4)
        {
                help();
                cleanup();
                return rv;
        }



        setup();

        rv = VT_usb_ptp_test(opt);

        cleanup();

        return rv;
}

/*====================*/
/*= help =*/
/**
@brief  Print help information.

@param  None.

@return None.
*/
/*====================*/
void help(void)
{
        printf("\n===============\n");
        printf("USB-ev driver option\n");
  printf("  -D x    Device name\n");
  printf("  -T  0  device infomation\n");
  printf("  -T  1  storage information\n");
  printf("  -T  2  file information\n");
  printf("  -T  3  get file\n");
  printf("  -T  4  get file thumbnail\n");
  printf("   -T  5  send a file\n");
  printf("  -T  6 set file protection\n");
  printf("   -T  7 delete file\n");
  printf("   -T  8 format storage\n");
        printf("\n===============\n");
}

