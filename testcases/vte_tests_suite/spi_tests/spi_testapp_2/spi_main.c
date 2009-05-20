/*====================*/
/**
@file   spi_main.c

@brief  LTP Motorola template.
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
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490   SPI test development
I.Inkina/nknl001             24/08/2005     TLsbo53757   SPI test options added
====================
Portability: Indicate if this module is portable to other compilers or platforms.
If not, indicate specific reasons why is it not portable.

======================*/

/*======================
Total Tests: TO BE COMPLETED

Test Name:   TO BE COMPLETED

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
TO BE COMPLETED
======================*/


#ifdef __cplusplus
extern "C"{
#endif

#ifndef bool
#define bool int
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

/*======================
INCLUDE FILES
======================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "spi_test_2.h"

/*======================
LOCAL MACROS
======================*/
#define READ_REGISTER_0 0x00000000
#define READ_REGISTER_1 0x02000000
/*======================
LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/


/*======================
LOCAL CONSTANTS
======================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================
LOCAL VARIABLES
======================*/


/*======================
GLOBAL CONSTANTS
======================*/
static int mod;

int Tflag = 0;
int Mflag = 0;
int Sflag = 0;
int Uflag = 0;

char *Topt,*Mopt,*Sopt,*Uopt;

option_t options[] =
{
        { "T:", &Tflag, &Topt },
        { "M:", &Mflag, &Mopt },
        { "S:", &Sflag, &Sopt },
        { "U:", &Uflag, &Uopt },
        { NULL, NULL, NULL }
};

/*======================
GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(...) */

/* Global Variables */
char *TCID     = "spi_TestApp_2";    /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */



/*======================
GLOBAL FUNCTION PROTOTYPES
======================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

/*======================
LOCAL FUNCTION PROTOTYPES
======================*/
struct s_list *addtree(struct s_list *p, char *adress,  char *value, int count);


/*======================
GLOBAL FUNCTIONS
======================*/

/*====================*/
/*= cleanup =*/
/**
@brief  Performs all one time clean up for this test on successful
completion,  premature exit or  failure. Closes all temporary
files, removes all temporary directories exits the test with
appropriate return code by calling tst_exit(...) function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;

        VT_rv = VT_spi_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

/*====================*/
/*= help =*/
/**
@brief Displays help

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void help (void)
{
        printf("   -M        module : 1 for SPI1 or 2 for SPI2\n");
        printf("   -U        card : 1 for MXC275-30 ADS , 2 for MXC275-30 EVB or 3 for i.300-30 EVB\n");
        printf("   -T        number couple Addres Value <number>\n");
        printf("   -S        string Addres Value, Addres Value, Addres Value ......\n");

}

/*======================
LOCAL FUNCTIONS
======================*/

/*====================*/
/*= setup =*/
/**
@brief  Performs all one time setup for this test. This function is
typically used to capture signals, create temporary dirs
and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*====================*/
void setup(void)
{
        int VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_spi_setup(mod);
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
        /* VTE */

        return;
}



/*====================*/
/*= main =*/
/**
@brief  Entry point to this test-case. It parses all the command line
inputs, calls the global setup and executes the test. It logs
the test status and results appropriately using the LTP API's
On successful completion or premature failure, cleanup(...) func
is called and test exits with an appropriate return code.

@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
        Describe input arguments to this test-case
        -l - Number of iteration
        -v - Prints verbose output
        -V - Prints the version number

@return On failure - Exits by calling cleanup(...).
        On success - exits with 0 exit value.
*/
/*====================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        int module = 1;
        char* msg;
        int card = 2;
        int number_couple=1;

        char *adress;
        char *value;
        struct s_list *root = NULL;
        char lis[80][80];
        int i=0;
        int dl=0, dl_o=0, dl_l=0;
        char *ad=NULL;
        char *ad_l=NULL;

        if ( (msg=parse_opts(argc,argv,options,&help)) != NULL )
                tst_brkm(TBROK, help, "OPTION PARSING ERROR - %s", msg);

        if ( Mflag )
        {
                module = atoi(Mopt);
                tst_resm(TINFO ," module = %d", module);
        }

        mod = module;

        if ( Uflag )
        {
                card = atoi(Uopt);
                tst_resm ( TINFO, " card = %d", card);
        }


        if ( Tflag )
        {
                number_couple = atoi(Topt);
                tst_resm( TINFO, " number_couple = %d\n", number_couple);
        }
        if ( Sflag )
        {
                while(i<  number_couple)
                {
                        ad=strstr(Sopt+dl_o,",");
                        if(ad)
                        {
                                dl=strlen(Sopt)-strlen(ad)-dl_o;
                        }
                        else
                        {
                                dl=strlen(Sopt)-dl_o;
                        }
                        strncpy(lis[i],Sopt+dl_o, dl);
                        dl_o+=dl+1;
                        ad_l=strstr(lis[i],"-");
                        if(ad_l)
                        {
                                dl_l=strlen(lis[i])-strlen(ad_l);
                                adress=(char*)calloc(sizeof(char),dl_l);
                                value=(char*)calloc(sizeof(char),  strlen(ad_l)-1);
                                strncpy(adress,lis[i], dl_l);
                                strncpy(value,lis[i]+dl_l+1, strlen(ad_l)-1);
                                root=addtree(root,adress,value,i);
                                free(adress);
                                free(value);
                        }

                        i++;

                }//end while

        }
        else
        {
                tst_brkm(TBROK, NULL, "Error while parsing command line options: %s", "Missing Adrdress/Value of SPI");
                return TFAIL;
        }

        /* perform global test setup, call setup(...) function. */
        setup();

        /* Print test Assertion using tst_resm(...) function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);


        VT_rv = VT_spi_test_1(module, card, root);

        if(VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);


        cleanup(); /** OR tst_exit(...); */
        /* VTE */

        return VT_rv;

}

struct s_list *addtree(struct s_list *p, char *adress,  char *value, int count)
{

        if(p==NULL)
        {
                p=(struct s_list*) malloc(sizeof(struct s_list));

                p->adress= strdup(adress);
                p->value =strdup(value);
                p->count = count;
                p->next=NULL;
        }
        else
        {
                p->next=addtree( p->next,adress, value, count);
        }

        return p;
}

#ifdef __cplusplus
}
#endif

