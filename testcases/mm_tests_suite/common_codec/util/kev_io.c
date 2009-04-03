/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file kev_io.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  21/02/2006   TLSbo61035   Initial version
=============================================================================*/ 

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>         
#include <string.h>
#include <errno.h>


/*=============================================================================
        bla-bla-bla.kev - folder
        y, cb, cr - subfolders
        data - binary file
        descriptor - text file    
        Assuming that the data is 4:2:0 format 
 =============================================================================*/
struct hKevFile
{               
        FILE * mpDataStream[3];
        FILE * mpDescStream[3];
        int    mWriteKev;
        size_t mTotalFrames;
        size_t mCurrentFrame;
        int    mWidth;
        int    mHeight;
};


/*==================================================================================*/
/*==================================================================================*/
static int _MakeDir( const char * pDirname )
{
        int oldMask = umask( 0 );
        int ret = mkdir( pDirname, 0777 );
        umask( oldMask );
        return ret == 0 || errno == EEXIST;
}


/*==================================================================================*/
/*==================================================================================*/
static void _ReadOneFrame( FILE * in, unsigned char * pFrm, int width, int height, int xSz )
{
        assert( in && pFrm );
        int i; 
        for( i = 0; i < height; ++i )
        {
                fread( pFrm, 1, width, in ); 
                pFrm += xSz;
        }
}


/*==================================================================================*/
/*==================================================================================*/
static void _WriteOneFrame( FILE * out, unsigned char * pFrm, int width, int height, int xSz )
{
        assert( out && pFrm );
        int i;        
        for( i = 0; i < height; ++i )
        {
                fwrite( pFrm, 1, width, out );                
                pFrm += xSz;
        }
}


/*==================================================================================*/
/*==================================================================================*/
static void _WriteDescriptor( FILE * out, int width, int height, size_t nFrames, 
                              const char * typeStr )
{
    /* Overwrite any previous description. */
    fseek( out, 0, SEEK_SET );

    fprintf( out, "(_data_sets %lu)\n(_channels 1)\n"
                  "(_dimensions %d %d)\n(_data_type %s)\n",
             (unsigned long)nFrames, (int)height, (int)width, typeStr );

    fflush( out );
}


/*==================================================================================*/
/*==================================================================================*/
struct hKevFile * Kev_Open( const char * fname, int write )
{
        assert( fname );

        char path[MAX_STR_LEN];
        const char * subDirs[] = {"y", "cb", "cr" };
        int i;

        /* Make the dirs. */
        if( write )
        {                                        
                if( !_MakeDir( fname ) )
                        return NULL;
                
                sprintf( path, "%s/y", fname );
                if( !_MakeDir( path ) )
                        return NULL;

                sprintf( path, "%s/cb", fname );
                if( !_MakeDir( path ) )
                        return NULL;

                sprintf( path, "%s/cr", fname );
                if( !_MakeDir( path ) )
                        return NULL;
        }

        /* Allocate the hander. */
        struct hKevFile * pKev = (struct hKevFile*)malloc( sizeof(struct hKevFile) );
        if( !pKev )
                return NULL;
        memset( pKev, 0, sizeof(*pKev) );
        
        /* Init the kev & open the streams. */
        do 
        {        
                const char * binMode = write ? "wb" : "rb";
                const char * txtMode = write ? "wt"  : "rt";
                pKev->mWriteKev = write;
                pKev->mWidth = pKev->mHeight = 0;
                pKev->mTotalFrames = pKev->mCurrentFrame = 0;               
                for( i = 0; i < 3; ++i )
                {
                        /* Open the data file. */
                        sprintf( path, "%s/%s/data", fname, subDirs[i] );
                        pKev->mpDataStream[i] = fopen( path, binMode );
                        if( !pKev->mpDataStream[i] )
                                break; // error
                        
                        /* Open the desc file. */
                        sprintf( path, "%s/%s/descriptor", fname, subDirs[i] );
                        pKev->mpDescStream[i] = fopen( path, txtMode );
                        if( !pKev->mpDescStream[i] )
                                break; // error
                        
                }

                /* Read the descriptor. */
                if( !write )
                {
                        while( !feof( pKev->mpDescStream[0] ) )
                        {
                                fgets( path, MAX_STR_LEN, pKev->mpDescStream[0] );
                                if( strstr(path, "(_data_sets") )
                                {
                                        sscanf( path, "(_data_sets%lu", (unsigned long*)&pKev->mTotalFrames );                                
                                }
                                else if( strstr(path, "(_dimensions") )
                                {
                                        sscanf( path, "(_dimensions%d%d", &pKev->mWidth, &pKev->mHeight );                                                                                                
                                }
                        }
                        fseek( pKev->mpDescStream[0], 0, SEEK_SET );
                }
                
                return pKev;

        } while( 0 );

        /* Error. Cleanup. */
        for( i = 0; i < 3; ++i )
        {
                if( pKev->mpDataStream[i] )
                        fclose( pKev->mpDataStream[i] );
                if( pKev->mpDescStream[i] )                
                        fclose( pKev->mpDescStream[i] );
        }
        free( pKev );
        return 0;
}


/*==================================================================================*/
/*==================================================================================*/
void Kev_Close( struct hKevFile * pKev )
{
        assert( pKev );
                
        
        int i;
        for( i = 0; i < 3; ++i )
        {
                _WriteDescriptor ( pKev->mpDescStream[i], pKev->mWidth, pKev->mHeight, 
                                   pKev->mTotalFrames, "unsigned_1" );
        
                if( pKev->mpDataStream[i] )
                {                        
                        fclose( pKev->mpDataStream[i] );
                        pKev->mpDataStream[i] = 0;                
                }
                
                if( pKev->mpDescStream[i] )
                {                
                        fclose( pKev->mpDescStream[i] );
                        pKev->mpDescStream[i] = 0;
                }                
        }
        memset( pKev, 0, sizeof(*pKev) );
        free( pKev );
}


/*==================================================================================*/
/*==================================================================================*/
int Kev_Eof( struct hKevFile * pKev )
{
        assert(pKev);
        return pKev->mTotalFrames == pKev->mCurrentFrame;
}


/*==================================================================================*/
/*==================================================================================*/
int Kev_GetNumFrames( struct hKevFile * pKev )
{
        assert(pKev);
        return pKev->mTotalFrames;
}

/*==================================================================================*/
/*==================================================================================*/
int Kev_Width( struct hKevFile * pKev )
{
        assert(pKev);
        return pKev->mWidth;
}


/*==================================================================================*/
/*==================================================================================*/
int Kev_Height( struct hKevFile * pKev )
{
        assert(pKev);
        return pKev->mHeight;
}

/*==================================================================================*/
/*==================================================================================*/
void Kev_SetDims( struct hKevFile * pKev, int width, int height )
{
        assert( pKev );
        pKev->mWidth = width;
        pKev->mHeight = height;
}


/*==================================================================================*/
/*==================================================================================*/
int  Kev_ReadFrame( struct hKevFile * pKev, 
                    size_t            frameNo, 
                    unsigned char   * pY, 
                    size_t            xSz, 
                    unsigned char   * pCb, 
                    unsigned char   * pCr,
                    size_t            cxSz )
{
        assert( pKev && pY && pCr && pCb );
        assert( pKev->mpDataStream[0] && pKev->mpDataStream[1] && pKev->mpDataStream[2] );
        //assert( pKev->mWriteKev && "KEV WAS OPEN READ ONLY MODE" );

        int sz;
                
        if( pKev->mWriteKev )
                return 0;

        /* If the frameNo is invalid. */
        if( frameNo >= pKev->mTotalFrames )
                return 0;
                
        sz = pKev->mWidth * pKev->mHeight;
        fseek( pKev->mpDataStream[0], sz * frameNo, SEEK_SET );
        _ReadOneFrame( pKev->mpDataStream[0], pY, pKev->mWidth, pKev->mHeight, xSz );
        
        sz >>= 2;
        fseek( pKev->mpDataStream[1], sz * frameNo, SEEK_SET );
        _ReadOneFrame( pKev->mpDataStream[1], pCb, pKev->mWidth >> 1, pKev->mHeight >> 1, cxSz );
        
        fseek( pKev->mpDataStream[2], sz * frameNo, SEEK_SET );
        _ReadOneFrame( pKev->mpDataStream[2], pCr, pKev->mWidth >> 1, pKev->mHeight >> 1, cxSz );
        
        ++pKev->mCurrentFrame;

        if( feof(pKev->mpDataStream[0]) || feof(pKev->mpDataStream[1]) || feof(pKev->mpDataStream[2]) )
        {        
                return 0;
        }

        return 1;
}


/*==================================================================================*/
/*==================================================================================*/
int   Kev_WriteFrame( struct hKevFile * pKev, 
                      size_t            frameNo,
                      unsigned char   * pY,
                      int               xSz, 
                      unsigned char   * pCb, 
                      unsigned char   * pCr,
                      int               cxSz )
{
        assert( pKev && pY && pCb && pCr );
        assert( pKev->mpDataStream[0] && pKev->mpDataStream[1] && pKev->mpDataStream[2] );
        assert( pKev->mpDescStream[0] && pKev->mpDescStream[1] && pKev->mpDescStream[2] );
        
        if( !pKev->mWriteKev )
                return 0;
        
        ++pKev->mTotalFrames;
        
        /* Write Y. */
        _WriteOneFrame   ( pKev->mpDataStream[0], pY, pKev->mWidth, pKev->mHeight, xSz );
        //_WriteDescriptor ( pKev->mpDescStream[0], pKev->mWidth, pKev->mHeight, 
        //                   pKev->mTotalFrames, "unsigned_1" );
        
        /* Write CB. */
        _WriteOneFrame   ( pKev->mpDataStream[1], pCb, pKev->mWidth >> 1, pKev->mHeight >> 1, cxSz );
        //_WriteDescriptor ( pKev->mpDescStream[1], pKev->mWidth >> 1, pKev->mHeight >> 1, 
        //                   pKev->mTotalFrames, "unsigned_1" );

        /* Write CR. */
        _WriteOneFrame   ( pKev->mpDataStream[2], pCr, pKev->mWidth >> 1, pKev->mHeight >> 1, cxSz );
        //_WriteDescriptor ( pKev->mpDescStream[2], pKev->mWidth >> 1, pKev->mHeight >> 1, 
        //                   pKev->mTotalFrames, "unsigned_1" );                
                           
        return 1;                           
}

