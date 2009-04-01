/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file ssi_dam_test.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Tony THOMASSIN/RB595C 29/07/2004   TLSbo41151   SSI/DAM test development
I.Inkina/nknl001      29/06/2005   TLSbo50735   code was improved
I.Inkina/nknl001      26/07/2005   TLSbo52849   add ask user
D.Simakov/smkd001c    26/07/2005   TLSbo53189   Mc13783 board configuration bit values was updated.
V.Halabuda/hlbv001    05/08/2005   TLSbo53363   update for linux-2.6.10-rel-1.12.arm
I.Inkina/nknl001      22/09/2005   TLSbo55818   update for VTE_1.13
I.Inkina/nknl001      14/10/2005   TLSbo56432   Updated for check of driver feature
D.Khoroshev/b00313    12/01/2005   TLSbo56844   Add SC55112 support
D.Simakov             05/06/2006   TLSbo67103   Re-written 
D.Kardakov            11/09/2006   TLSbo71015   update for L26_21 release
D.Karaakov            02/01/2007   TLSbo87890  	Update for MXC91131Evb, i.MX31ADS platforms
                                                Some problems with
                                                voice codec testcases was fixed
=============================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files. */
#include <errno.h>
#include <sys/types.h> 
#include <sys/stat.h>  
#include <sys/ioctl.h> 
#include <fcntl.h>     
#include <unistd.h>    
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>
#include <assert.h>
#include <asm/arch/pmic_audio.h>
#include <asm-arm/mach-types.h>
/* Harness Specific Include Files. */
#include "test.h"

/* Other Include Files. */
#include "ssi_dam_test.h"

/*==================================================================================================
                                        CONSTANTS
==================================================================================================*/

#define SSI1_DEV_NAME  "/dev/mxc_ssi1"
#define SSI2_DEV_NAME  "/dev/mxc_ssi2"
#define MAX_CHUNK_SIZE (1024 * 128)


/*==================================================================================================
                                   FUNCTION PROTOTYPES
==================================================================================================*/

int ReadWAVEHeader ( void );
int Confugure      ( void );
int PlaySample     ( void );



/*==================================================================================================
                                       VARIABLES
==================================================================================================*/

static struct wave_config   gWaveConfig;
static FILE               * gpSndStream  = NULL;
static int                  gSsiFileDesc = -1;  

/*==================================================================================================
                                       FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/
int VT_ssi_dam_setup( void )
{              
        switch( gTestappConfig.mTestCase )
        {
        case 0:
                gWaveConfig.dac_codec = STEREO_DAC;
                gWaveConfig.dac_bus = AUDIO_DATA_BUS_2;
                gWaveConfig.ssi = 0; 
                gpSndStream = fopen( gTestappConfig.mSndName, "rb" );                
                gSsiFileDesc = open( SSI1_DEV_NAME, O_RDWR );
                break;

        case 1:
                gWaveConfig.dac_codec = CODEC;
                gWaveConfig.dac_bus = AUDIO_DATA_BUS_1;
                gWaveConfig.ssi = 0; 
                gpSndStream = fopen( gTestappConfig.mSndName, "rb" );
                gSsiFileDesc = open( SSI1_DEV_NAME, O_RDWR );
                break;

        case 2:                 
                gWaveConfig.dac_codec = STEREO_DAC; 
                gWaveConfig.dac_bus = AUDIO_DATA_BUS_2;
                gWaveConfig.ssi = 1; 
                gpSndStream = fopen( gTestappConfig.mSndName, "rb" );
                gSsiFileDesc = open( SSI2_DEV_NAME, O_RDWR );
                break;

        case 3: 
                gWaveConfig.dac_codec = CODEC;
                gWaveConfig.dac_bus = AUDIO_DATA_BUS_1;
                gWaveConfig.ssi = 1; 
                gpSndStream = fopen( gTestappConfig.mSndName, "rb" );
                gSsiFileDesc = open( SSI2_DEV_NAME, O_RDWR );
                break;

        default:
                tst_brkm( TCONF, (void(*)())VT_ssi_dam_cleanup,
                        "%s : Unknown test case", __FUNCTION__ );  
        }

        if( !gpSndStream )
                tst_brkm( TBROK, (void(*)())VT_ssi_dam_cleanup,
                          "%s : Can't open sound file %s", __FUNCTION__, gTestappConfig.mSndName );  
        
        if( gSsiFileDesc < 0 )
                tst_resm( TWARN, "%s : Can't open SSI device, error: %s", strerror(errno) );        

        if( !ReadWAVEHeader() )
                return TFAIL;
        if( !Confugure() )
                return TFAIL;

        return TPASS; 
}


/*================================================================================================*/
/*================================================================================================*/
int VT_ssi_dam_cleanup( void )
{       
        if( gpSndStream )
        {
                fclose( gpSndStream );
                gpSndStream = 0;
        }
        if( gSsiFileDesc > -1 )
        {
                close( gSsiFileDesc );
                gSsiFileDesc = -1;
                printf("Codec closing.....\n"); 
                sleep(2);
        }
        return TPASS; 
}


/*================================================================================================*/
/*================================================================================================*/
int VT_ssi_dam_test( void )
{              
        PlaySample();
        return TPASS; 
}


/*================================================================================================*/
/*================================================================================================*/
int ReadWAVEHeader( void )
{
        unsigned short header[29];
        
        tst_resm( TINFO, "SSI[%d] Read WAVE header", gWaveConfig.ssi );
                
        if( gpSndStream )
                fread(header, sizeof(header[0]), 29, gpSndStream);        
        else
        {                
                tst_resm( TWARN, "The WAVE file %s was not opened", gTestappConfig.mSndName );
                return FALSE;
        }
        gWaveConfig.num_channels = header[11];
        gWaveConfig.sample_rate = (((long) (header[12])) & 0x0000FFFF) |
                ((((long) ((header[13])) << 16)) & 0xFFFF0000);
        gWaveConfig.bits_per_sample = header[17];
        gWaveConfig.sample_size = (((long) (header[27])) & 0x0000FFFF) |
                ((((long) ((header[28])) << 16)) & 0xFFFF0000);
        
        tst_resm( TINFO, "SSI[%d] Sample size     : %d", gWaveConfig.ssi, (int)gWaveConfig.sample_size     );
        tst_resm( TINFO, "SSI[%d] Sample rate     : %d", gWaveConfig.ssi, (int)gWaveConfig.sample_rate     );
        tst_resm( TINFO, "SSI[%d] Bits per sample : %d", gWaveConfig.ssi, (int)gWaveConfig.bits_per_sample );
        tst_resm( TINFO, "SSI[%d] Num channels    : %d", gWaveConfig.ssi, (int)gWaveConfig.num_channels    );        
        
        return TRUE;
}


/*================================================================================================*/
/*================================================================================================*/
int Confugure( void )
{    
        gWaveConfig.mix_enabled = 0;
        gWaveConfig.master_slave = BUS_MASTER_MODE;
#ifdef CONFIG_MXC_PMIC_SC55112
        gWaveConfig.pmic_clock_source = CLOCK_IN_CLKIN;
#else
        if (machine_is_mx31ads()) {
        gWaveConfig.pmic_clock_source = CLOCK_IN_CLIB;
        } else {
        gWaveConfig.pmic_clock_source = CLOCK_IN_CLIA;
        }
#endif
        if( ioctl(gSsiFileDesc, IOCTL_SSI_CONFIGURE_AUDMUX, &gWaveConfig) )        
        {
                tst_resm( TWARN, "Error Configure DAM for SSI[%d], error: %s", gWaveConfig.ssi, strerror(errno) );
                return FALSE;
        }
        
        if( ioctl(gSsiFileDesc, IOCTL_SSI_CONFIGURE_SSI, &gWaveConfig) )        
        {
                tst_resm( TWARN, "Error Configure SSI[%s], error: %s", gWaveConfig.ssi, strerror(errno) );
                return FALSE;
        }
                
        if( ioctl(gSsiFileDesc, IOCTL_SSI_CONFIGURE_PMIC, &gWaveConfig) )        
        {
                tst_resm( TWARN, "Error Configure PMIC for SSI[%d], error: %s", gWaveConfig.ssi, strerror(errno) );
                return FALSE;
        }
        
        return TRUE;
}


/*================================================================================================*/
/*================================================================================================*/
int PlaySample( void )
{        
        int     size;
        int     count = 0;
        int     bytesToRead;
        int     offset = 0;
        int     bytesWritten;
        static char buf[MAX_CHUNK_SIZE];

        size = gWaveConfig.sample_size;
        tst_resm( TINFO, "SSI[%d] Sample size : %d", gWaveConfig.ssi, size );        
        tst_resm( TINFO, "SSI[%d] Transferring...", gWaveConfig.ssi );
        
        while( size > 0 )
        {
                bytesToRead = (size > MAX_CHUNK_SIZE) ? MAX_CHUNK_SIZE : size;                        
                memset( buf, 0, MAX_CHUNK_SIZE );
                count = fread(buf, 1, bytesToRead, gpSndStream);

                offset = 0;
                bytesWritten = 0;
                const int chunkSz = gTestappConfig.mWriteChunkSz;

                while( count > offset )
                {
                        bytesWritten = write( gSsiFileDesc, buf + offset, chunkSz );
                        if( bytesWritten < 0 )
                        {                                    
                                tst_resm( TWARN, "Error write SSI[%d] %s", gWaveConfig.ssi, strerror(errno) );
                                return FALSE;
                        }                        
                        printf( "SSI[%d] Total bytes %d, bytes remainning %d    \r", gWaveConfig.ssi, count, count - offset );
                        fflush( stdout );
                        offset += bytesWritten;                                
                }
                size -= count;
        }
        printf("\r                                                                           \r");
        tst_resm( TINFO, "End transfer" );
        return TRUE;
}
