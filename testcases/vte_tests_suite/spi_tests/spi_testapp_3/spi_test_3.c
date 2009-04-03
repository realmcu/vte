/*================================================================================================*/
/**
    @file   spi_test_2.c

    @brief  Test scenario C source for spi.
*/
/*==================================================================================================

        Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490   SPI test development 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

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

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <sys/types.h>    /* open()  */
#include <fcntl.h>        /* open()  */
#include <sys/ioctl.h>    /* ioctl() */

#include <errno.h>    /* ioctl() */
#include <string.h>    /* ioctl() */

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "spi_test_3.h"

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

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

#define BUFF_TEST_MAX_SIZE 32

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
int fbfd = 0;

char *messageMC13783;
spi_frame mc13783Frame;
spi_config theConfig;

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_TEMPLATE_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_setup(void)
{
    int rv = TPASS;

    return rv;
}


/*================================================================================================*/
/*===== VT_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_cleanup(void)
{
    int rv = TFAIL;
    
    close(fbfd);

    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*===== VT_spi_test_3 =====*/
/**
@brief  spi test scenario 0 function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_test_3(int card, int test, int module, int loopback, int nbBytes, char *message)
{
        int rv = TFAIL;
        char *file;
        unsigned int value;
        int fbfd = 0;
        int ret, result, i;
        void* handle_on_device;
        
        if (test == 1) {
                goto _spi_multi_client_test;
        } else {
                goto _spi_buff_test;
        }
        
        /****** Tst SPI Multi Client */
_spi_multi_client_test:	
        printf("Multiclient Test\n");
        
        file = "/dev/spi2";
        fbfd = open(file, O_RDWR);
        if (fbfd < 0) {
                printf("Error: cannot open spi2 device.\n");
                return rv;
        }
        
        ioctl(fbfd,SPI_MULTI_CLIENT_TEST,NULL);        
        
        close(fbfd);
        rv = TPASS;
        return rv;
        
        /****** Tst SPI Large Buffer */
_spi_buff_test:
        printf("Large buffer Test\n");
        
        if (module== 1) { 
                theConfig.module_number = SPI1;
                file = "/dev/spi1";
        } else {
                theConfig.module_number = SPI2;
                file = "/dev/spi2";
        }
        fbfd = open(file, O_RDWR);
        if (fbfd < 0) {
                printf("Error: cannot open %s device.\n",(char *)file);
                return rv;
        }
        
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
        
        if (nbBytes > BUFF_TEST_MAX_SIZE) {
                printf("Too many bytes\n");
                return rv;
        }
        
        if (!strncmp((char *)message, "0x",2)) {
                memcpy(message, (char *)message+2, nbBytes*2);
        }
        
        if ((messageMC13783 = malloc(nbBytes)) == NULL) {
                printf("malloc has failed\n");
                return rv;
        }
        
        mc13783Frame.data = messageMC13783;
        mc13783Frame.length = nbBytes;
        
        theConfig.priority = HIGH;
        theConfig.master_mode = true;
        theConfig.bit_rate = 4000000;
        theConfig.bit_count = nbBytes*8;
        theConfig.active_high_polarity = true;
        theConfig.active_high_ss_polarity = true;
        theConfig.phase = false;
        theConfig.ss_low_between_bursts = true;
        
        
        ioctl(fbfd,SPI_CONFIG,&theConfig);
        
        ioctl(fbfd,SPI_GET_CUR_HANDLE,&handle_on_device);
        
        mc13783Frame.dev_handle = handle_on_device;
        
        if (loopback != 0)
        {
                ioctl(fbfd,SPI_LOOPBACK_MODE,1);
                printf("Loopback mode activated \n");
        }
        else
        {
                printf("No loopback mode \n");
        }
        
        i = 0;
        while (i < (nbBytes)) {
                sscanf((char *)message+2*i, "%8X", &value);
                messageMC13783[i+0] = (value & 0xFF000000)>>24;
                messageMC13783[i+1] = (value & 0x00FF0000)>>16;
                messageMC13783[i+2] = (value & 0x0000FF00)>>8;
                messageMC13783[i+3] = (value & 0xFF);
                i += 4;
        }
        
        ret = ioctl(fbfd,SPI_WRITE,&mc13783Frame);
        
        printf("Received data  : 0x");
        i = 0;
        while (i < nbBytes) {
                result = (messageMC13783[i]<<24) + (messageMC13783[i+1]<<16) + 
                        (messageMC13783[i+2]<<8) + messageMC13783[i+3];
                printf("%8X", result);
                i += 4;
        }
        printf("\n\n");
        
        ioctl(fbfd,SPI_LOOPBACK_MODE,0);
        ioctl(fbfd,SPI_FREE,0);
        
        free(messageMC13783);
        close(fbfd);
        rv = TPASS;
        return rv;
}

#ifdef __cplusplus
}
#endif
