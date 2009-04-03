/*
 *======================================================================
 *
 *                         Motorola Labs
 *         Multimedia Communications Research Lab (MCRL)
 *
 *              H.264/MPEG-4 Part 10 AVC Video Codec
 *
 *          This code is the property of MCRL, Motorola.
 *  (C) Copyright 2002-03, MCRL Motorola Labs. All Rrights Reserved.
 *
 *       M O T O R O L A  I N T E R N A L   U S E   O N L Y
 *
 *======================================================================
 */

#ifndef _KEV_H
#define _KEV_H

#include <stdio.h>
#include <debug.h>

#define KEV_READ        0
#define KEV_WRITE       1

typedef struct
{
    // I'm assuming that the data is 4:2:0 format
    FILE   *Y_fp;
    FILE   *CB_fp;
    FILE   *CR_fp;
    FILE   *TS_fp;
    FILE   *Y_desc;
    FILE   *CB_desc;
    FILE   *CR_desc;
    FILE   *TS_desc;
    int     mode;
    int     width, height;
    long int totalFrames;
    long int currentFrame;
}
KevFile;

void    KevOpen(char *name, int write, KevFile *k);
void    SetDimensions(KevFile *k, int width, int height);
int     ReadKevFrame(KevFile *k, long int frameNum, unsigned char *y,
                     int xsize, unsigned char *cb, unsigned char *cr,
                     int cxsize);
void    WriteKevFrame(KevFile *k, long int frameNum, unsigned char *y,
                      int xsize, unsigned char *cb, unsigned char *cr,
                      int cxsize);
void    KevClose(KevFile *k);

#endif
