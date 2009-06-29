/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
@file   spi_test_2.c

@brief  Test scenario C source for spi.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490   SPI test development 
I.Inkina/nknl001             24/08/2005     TLsbo53757   SPI test improved
====================================================================================================

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
#include "spi_test_2.h"
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

static char messageMC13783[] = { 0x20, 0, 0, 0 };
static spi_frame mc13783Frame;
static spi_config theConfig;

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
void* set_param( int mod, int card );


/*================================================================================================*/
/*===== VT_TEMPLATE_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_setup(int mod)
{
        int rv = TPASS;
        char device[9];
        
        sprintf(device, "%s%d","/dev/spi", mod);
        /* Open first time */
        tst_resm(TINFO, "  Opening device %s...", device);
        fbfd = open(device 	, O_RDWR);
        if (fbfd < 0)
        {
                tst_resm(TWARN, "  => Oups! Error opening device");
                rv=TFAIL;  
        }
        
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
        
        if(fbfd) close(fbfd);
        
        rv = TPASS;
        
        return rv;
}


/*================================================================================================*/
/*===== VT_spi_test_0 =====*/
/**
@brief  spi test scenario 0 function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_test_1( int mod, int card , struct s_list *root)
{
        int rv = TPASS;
        int ret=0;
        struct s_list *root_n;
        int reg_number;
        unsigned int reg_data;
        
        for ( root_n = root;  root_n!=NULL ; root_n = root_n->next)
        {
                
                reg_number=atoi(root_n->adress);
                sscanf(root_n->value,"%x",&reg_data);
                tst_resm(TINFO, "");
                tst_resm(TINFO, "Testing SPI  for couple %d:  adress = %d value = 0x%x", root_n->count, reg_number, reg_data);
                
                set_param(mod, card);
                
                ret+=mc13783_commands(reg_number, reg_data);
                if(ret)
                {
                        tst_resm(TFAIL, "MC13783 commands failed for couple %d", root_n->count);
                        rv=TFAIL;
                }
        }
        return rv;
}

void* set_param( int mod, int card )
{
        
        mc13783Frame.data = messageMC13783;
        mc13783Frame.length = sizeof(messageMC13783);
        
        if ( mod == 1 )
        {
                theConfig.module_number = SPI1;
        }
        else if ( mod == 2 )
        {
                theConfig.module_number = SPI2;
        }
        
        switch(card)
        {
        case 1:
                /* MXC275-30 ADS */
                theConfig.ss_asserted = SS_2;
                break;
        case 2:
                /* MXC275-30 EVB */
                theConfig.ss_asserted = SS_3;
                break;
        default:
                /* A+ */
                theConfig.ss_asserted = SS_0;
                break;
                
        }
        theConfig.priority = HIGH;
        theConfig.master_mode = true;
        theConfig.bit_rate = 1000000;
        theConfig.bit_count = 32;
        theConfig.active_high_polarity = true;
        theConfig.active_high_ss_polarity = true;
        theConfig.phase = false;
        theConfig.ss_low_between_bursts = true;
        
        return NULL;
}

int mc13783_commands (int reg_number,unsigned int reg_data)
{
        int rv=TPASS;
        int ret=0;   
        unsigned int reg_data_new=0, reg_data_old=0 ;
        void* handle;
        
        ret=ioctl(fbfd,SPI_CONFIG,&theConfig);
        if(ret<0)
        {
                tst_resm(TFAIL, "ioctl SPI_CONFIG failed");
                rv=TFAIL;
                return rv;	   
        }
        
        ret=ioctl(fbfd,SPI_GET_CUR_HANDLE,&handle);
        if(ret<0)
        {
                tst_resm(TFAIL, "ioctl SPI_GET_CUR_HANDLE failed");
                rv=TFAIL;
                return rv;	
        }
        
        mc13783Frame.dev_handle = handle;
        reg_data_old = read_mc13783_register(reg_number);
        tst_resm(TINFO, "+======================================+");
        tst_resm(TINFO, "|     Register %d = 0x%x              | ", reg_number, reg_data_old);
        tst_resm(TINFO, "+============+============+============+");
        tst_resm(TINFO, "Read operation performed");
        
        ret=write_mc13783_register(reg_number, reg_data);
        if(ret)
        {
                tst_resm(TFAIL, "Error Write operation ");
                rv=TFAIL;
                return rv;	
        }
        
        tst_resm(TINFO, "Write operation performed");
        
        reg_data_new = read_mc13783_register(reg_number);
        tst_resm(TINFO, "+======================================+");
        tst_resm(TINFO, "|Register %d: reg_data_new =0x%x  reg_data =0x%x | ", reg_number,reg_data_new,reg_data);
        tst_resm(TINFO, "+============+============+============+");
        if(reg_data != reg_data_new)
        {
                tst_resm(TFAIL, "=>Written value !=read value");
                rv=TFAIL;
        }
        else
        {
                tst_resm(TPASS, "=>Write/Read operation performed");
        }
        
        ret = ioctl(fbfd,SPI_FREE,0);
        if(ret<0)
        {
                tst_resm(TFAIL, "Error ioctl SPI_FREE ");
                rv=TFAIL;
                return rv;	   
        }
        return rv;
}

unsigned int read_mc13783_register (char address)
{
        int ret;
        int rv=TPASS;	
        unsigned int result=0;
        
        messageMC13783[0] = 0 | ((address & 0x3F)<<1);
        messageMC13783[1] = 0;
        messageMC13783[2] = 0;
        messageMC13783[3] = 0;
        
        ret = ioctl(fbfd,SPI_WRITE,&mc13783Frame);
        if(ret<0)
        {
                tst_resm(TFAIL, "ioctl SPI_Write failed");
                rv=TFAIL;
                return rv;	   
        }
        result = (messageMC13783[1]<<16) + (messageMC13783[2]<<8)+ messageMC13783[3];
        
        return result;
}

int write_mc13783_register(char address,unsigned int value)
{
        int ret;
        int rv=TPASS;
        
        messageMC13783[0] = 0 | 0x80 | ((address & 0x3F)<<1);
        messageMC13783[1] = (value & 0x00FF0000)>>16;
        messageMC13783[2] = (value & 0x0000FF00)>>8;
        messageMC13783[3] = (value & 0x000000FF);
        
        ret = ioctl(fbfd,SPI_WRITE,&mc13783Frame);
        if(ret<0)
        {
                tst_resm(TFAIL, "ioctl SPI_Write failed");
                rv=TFAIL;
        }
        
        return rv;
}

#ifdef __cplusplus
}
#endif

