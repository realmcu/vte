///////////////////////////////////////////////////////////////////////////
//
//  Motorola Chicago Corporate Research Labs
//
//  Project         : Visual Communications
//
//  Primary Author: Raghavan Subramaniyan (raghavan.s@motorola.com)
//
//                  This code is the property of Motorola.
//          (C) Copyright 1999-2002 Motorola,Inc. All Rights Reserved.
//                       MOTOROLA INTERNAL USE ONLY
//
// History :
//  Date:     Author    Description
//  1/Feb/02  rags      Created
//
///////////////////////////////////////////////////////////////////////////

#ifndef KEV_H
#define KEV_H

#include <stdio.h>

// #include "mpeg4types.h"
// #include "debug.h"
// #include "global.h"

// #ifndef BUFFER_INPUT
#define USE_KEV         1
// #else

// #if (SUPPORT_KEV_OUT)
// #define USE_KEV         1
// #else
// #define USE_KEV         0
// #endif

// #endif

#define KEV_READ        0
#define KEV_WRITE       1

// To compile the code I have copied few typedefs from the mpeg4types.h
typedef int             Int;
typedef unsigned char   UCHAR;
typedef signed long int S32;
typedef int             UINT;
typedef unsigned short int U16;

typedef struct
{
    // It is assumed that the data is 4:2:0 format
    FILE   *Y_fp;
    FILE   *CB_fp;
    FILE   *CR_fp;
    FILE   *TS_fp;
    FILE   *Y_desc;
    FILE   *CB_desc;
    FILE   *CR_desc;
    FILE   *TS_desc;
    Int     mode;
    Int     width, height;
    S32     totalFrames;
    S32     currentFrame;
}
KevFile;

void    KevOpen(char *name, Int write, KevFile *k);
void    SetDimensions(KevFile *k, Int width, Int height);
void    ReadKevFrame(KevFile *k, S32 frameNum, UCHAR *y, Int xsize,
                     UCHAR *cb, UCHAR *cr, Int cxsize);
void    WriteKevFrame(KevFile *k, S32 frameNum, UCHAR *y, Int xsize,
                      UCHAR *cb, UCHAR *cr, Int cxsize);
S32     GetTimeStamp(KevFile *k, S32 frameNum);
void    KevClose(KevFile *k);

#endif
