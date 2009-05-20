/*====================*/
/**
    @file   spi_test_1.c

    @brief  Test scenario C source for spi.
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
Irina Inkina/nknl001         19/05/2005     Tlsbo52848   Fix bug that makes segmantation test
Irina Inkina/nknl001         15/08/2005     Tlsbo53514   Number of the iterations were added

====================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

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
#include <errno.h>
#include <sys/types.h>    /* open()  */
#include <fcntl.h>        /* open()  */
#include <sys/ioctl.h>    /* ioctl() */

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "spi_test_1.h"

#include <asm/ioctl.h>

#define SPI_IOCTL              0x53

/*!
 * IOCTL Command to configure the device
 */
#define SPI_CONFIG           _IOWR(SPI_IOCTL,1, spi_config)

/*!
 * IOCTL Command to get the handle on the device
 */
#define SPI_GET_CUR_HANDLE   _IOWR(SPI_IOCTL,2, void*)

/*!
 * IOCTL Command to write on the device
 */
#define SPI_WRITE            _IOWR(SPI_IOCTL,3, spi_frame)

/*!
 * IOCTL Command to free memory on the device
 */
#define SPI_FREE            _IOWR(SPI_IOCTL,4, int)

/*!
 * IOCTL Command to configure in loopback mode
 */
#define SPI_LOOPBACK_MODE    _IOWR(SPI_IOCTL,5, int)

/*!
 * IOCTL Command to test multi-clients mode with MC13783 chips
 */
#define SPI_MULTI_CLIENT_TEST    _IOWR(SPI_IOCTL,6, int)

/*!
 * IOCTL Command to test long buffer transfer (8 and 16 bytes)
 */
#define SPI_BUFF_TEST    _IOWR(SPI_IOCTL,7, int)

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


/*======================
                                       GLOBAL VARIABLES
======================*/
int fbfd = 0;
static char messageMC13783[] = { 0x20, 0, 0, 0 };

static spi_frame mc13783Frame;
static spi_config theConfig;

/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/


/*======================
                                       LOCAL FUNCTIONS
======================*/
unsigned int write_mc13783_register(char address, unsigned int value);
int setup_step_1(int mod, int card);


/*====================*/
/*= VT_TEMPLATE_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_spi_setup(int mod)
{
    int rv = TFAIL;

    if ( mod == 1 )
    {
        fbfd = open("/dev/spi1", O_RDWR);
    if (!(fbfd < 0))
    {
    rv = TPASS;
    tst_resm(TINFO ,"Open /dev/spi1");
    }
    }
    if ( mod == 2 )
    {
    fbfd = open("/dev/spi2", O_RDWR);
    if (!(fbfd < 0))
    {
    rv = TPASS;
    tst_resm(TINFO ,"Open /dev/spi2");
    }
    }

    return rv;
}


/*====================*/
/*= VT_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_spi_cleanup(void)
{
        int rv = TFAIL;

    if(fbfd) close(fbfd);

        rv = TPASS;

        return rv;
}


/*====================*/
/*= setup_step_1 =*/
/**
@brief  setup of the parametrs

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int setup_step_1(int mod, int card)
{
        int rv = TPASS;


        mc13783Frame.data = messageMC13783;
        mc13783Frame.length = sizeof(messageMC13783);
    theConfig.priority = HIGH;
        theConfig.master_mode = true;
        theConfig.bit_rate = 1000000;
        theConfig.bit_count = 32;
        theConfig.active_high_polarity = true;
        theConfig.active_high_ss_polarity = true;
        theConfig.phase = false;
        theConfig.ss_low_between_bursts = true;

    if (card == 2) {
        /* A+ */
        theConfig.ss_asserted = SS_0;
    } else if (card == 0) {
        /* MXC275-30 ADS */
        theConfig.ss_asserted = SS_2;
    } else {
        /* MXC275-30 EVB */
        theConfig.ss_asserted = SS_3;
    }

    if ( mod == 1 )
    {
        theConfig.module_number = SPI1;
    }

    if ( mod == 2 )
    {
        theConfig.module_number = SPI2;
    }


    return rv;
}


/*====================*/
/*= VT_spi_test_0 =*/
/**
@brief  spi test scenario 0 function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_spi_test_1(int mod, int card, int  iteration)
{
    int rv = TFAIL;
    int ret=TPASS;
    int loop;
    int reg_number;
    unsigned int reg_data;
    unsigned int data_loop;

    void* handle_on_device;


    setup_step_1( mod, card);


    if(ioctl(fbfd,SPI_CONFIG,&theConfig)<0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_CONFIG fails");
    return rv;
    }

    if( ioctl(fbfd,SPI_GET_CUR_HANDLE,&handle_on_device)<0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_GET_CUR_HANDLE fails");
    return rv;
    }

    mc13783Frame.dev_handle = handle_on_device;

    if( ioctl(fbfd,SPI_WRITE,&mc13783Frame)<0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_WRITE fails");
    return rv;
    }

    if(ioctl(fbfd,SPI_FREE,0)<0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_FREE fails");
    return rv;
    }


    VT_spi_cleanup();

    if(VT_spi_setup( mod)!=TPASS){ return rv;}

    setup_step_1( mod, card);


    if(ioctl(fbfd,SPI_LOOPBACK_MODE,1)<0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_LOOPBACK_MODE fails");
    return rv;
    }


    if(ioctl(fbfd,SPI_CONFIG,&theConfig)<0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_CONFIG fails");
    return rv;
    }
    if( ioctl(fbfd,SPI_GET_CUR_HANDLE,&handle_on_device) < 0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_CUR_HANDLE fails");
    return rv;
    }

    mc13783Frame.dev_handle = handle_on_device;

        reg_number = 18;
        data_loop = 0x123456;

    for (loop = 0; loop <  iteration; loop++)
    {
    reg_data = write_mc13783_register(reg_number, data_loop);
    if ( reg_data != data_loop)
    {
    ret = TFAIL;
    tst_resm(TINFO, "Data loop failed");
    break;
    }
    else
    {
//                        tst_resm(TINFO, "Write in loopback in Register %d", reg_number);
     }

    data_loop++;
    }


    if(ioctl(fbfd,SPI_LOOPBACK_MODE,0)<0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_LOOPBACK_MODE fails");
    return rv;
    }

    if(ioctl(fbfd,SPI_FREE,0)<0)
    {
    tst_resm(TFAIL, "ERROR : ioctl SPI_FREE fails");
    return rv;
    }

    rv=ret;
    return rv;
}

/*====================*/
/*= write_mc13783_register =*/
/**
@brief

@param  address, value

@return  - return result
*/
/*====================*/


unsigned int write_mc13783_register(char address, unsigned int value)
{
    int ret;
    unsigned int result;
    messageMC13783[0] = 0 | 0x80 | ((address & 0x3F)<<1);
    messageMC13783[1] = (value & 0x00FF0000)>>16;
    messageMC13783[2] = (value & 0x0000FF00)>>8;
    messageMC13783[3] = (value & 0x000000FF);

    ret = ioctl(fbfd,SPI_WRITE,&mc13783Frame);
    result = (messageMC13783[1]<<16) + (messageMC13783[2]<<8)+ messageMC13783[3];
    return result;
}




#ifdef __cplusplus
}
#endif
