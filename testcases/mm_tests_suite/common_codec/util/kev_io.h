/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file kev_io.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  21/02/2006   TLSbo61035   Initial version
=============================================================================*/

#ifndef __KEV_IO_H__
#define __KEV_IO_H__

struct hKevFile;

struct hKevFile * Kev_Open         ( const char * fname, int write );

void              Kev_Close        ( struct hKevFile * pKev );

int               Kev_Eof          ( struct hKevFile * pKev );

int               Kev_GetNumFrames ( struct hKevFile * pKev );

int               Kev_Width        ( struct hKevFile * pKev );

int               Kev_Height       ( struct hKevFile * pKev );

void              Kev_SetDims      ( struct hKevFile * pKev, int width, int height );

int               Kev_ReadFrame    ( struct hKevFile * pKev,
                                     size_t            frameNum,
                                     unsigned char   * pY,
                                     size_t            xSz,
                                     unsigned char   * pCb,
                                     unsigned char   * pCr,
                                     size_t            cxSz );

int               Kev_WriteFrame   ( struct hKevFile * pKev,
                                     size_t            frameNum,
                                     unsigned char   * pY,
                                     int               xSz,
                                     unsigned char   * pCb,
                                     unsigned char   * pCr,
                                     int               cxSz );


#endif //__KEV_IO_H__
