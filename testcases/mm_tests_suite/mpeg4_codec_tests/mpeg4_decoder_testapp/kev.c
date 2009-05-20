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
// 23/Aug/04  Debashis  Assumption: first frame number is 0
//
///////////////////////////////////////////////////////////////////////////
// This file contains functions to work with KEV files
///////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "kev.h"
#include "dirUtils.h"
// #include "timing.h"

// copied from timing.h
#define TimerStartModule
#define TimerEndModule(time)

#ifndef _WIN32_WCE
#  include <assert.h>
#else
#  define assert(expression)
#endif

#define Exit   exit(0)
#define PrintErr printf
#define Print1   printf

#if (USE_KEV)
static FILE *OpenFile(char *name, char *mode)
{
    FILE   *fp;

#ifndef NDEBUG
    printf ("Opening file %s ...", name);
#endif

    if ((fp  fopen(name, mode))  NULL)
    {
        PrintErr("KevOpen error - could not open file %s in %s mode\n",
                 name, mode);
        Exit;
    }
#ifndef NDEBUG
    else
        printf ("Successfull\n");
#endif

    return fp;
}

void KevOpen(char *name, Int mode, KevFile *k)
{
    char    *ptr, *ptrBase;
    char    modeBin[3], modeAsc[2];
    char    cbuff[256];

    k->mode  mode;

    if (mode  KEV_WRITE)
    {
        strcpy(modeBin, "wb");
        strcpy(modeAsc, "w");
    }
    else
    {
        strcpy(modeBin, "rb");
        strcpy(modeAsc, "r");
    }

    if (mode  KEV_WRITE)
        createDir(name);

    strcpy(cbuff, name);
    ptrBase  cbuff + strlen(cbuff);

    strcpy(ptrBase, "/y");
    if (mode  KEV_WRITE)
        createDir(cbuff);
    ptr  cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->Y_fp  OpenFile(cbuff, modeBin);
    // printf ("cbuff %s ptrBase %s ptr %x %s \n", cbuff, ptrBase, ptr, ptr);
    // strcpy(ptr, "/descriptor");
    sprintf (ptr, "%s", "/descriptor");
    // printf ("cbuff %s ptrBase %s ptr %x %s \n", cbuff, ptrBase, ptr, ptr);
    k->Y_desc  OpenFile(cbuff, modeAsc);

    strcpy(ptrBase, "/cb");

    if (mode  KEV_WRITE)
        createDir(cbuff);
    ptr  cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->CB_fp  OpenFile(cbuff, modeBin);
    // strcpy(ptr, "/descriptor");
    // sprintf (ptr, "%s", "/descriptor");
    *ptr  '\0';
    strcat (cbuff, "/descriptor");
    k->CB_desc  OpenFile(cbuff, modeAsc);

    strcpy(ptrBase, "/cr");
    if (mode  KEV_WRITE)
        createDir(cbuff);
    ptr  cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->CR_fp  OpenFile(cbuff, modeBin);
    // strcpy(ptr, "/descriptor");
    sprintf (ptr, "%s", "/descriptor");
    k->CR_desc  OpenFile(cbuff, modeAsc);

    // The source KEV files we have do not have a "ts" component,
    // hence open files only for "write" mode
    if (mode  KEV_WRITE)
    {
        strcpy(ptrBase, "/ts");
        createDir(cbuff);
        ptr  cbuff + strlen(cbuff);
        strcat(cbuff, "/data");
        k->TS_fp  OpenFile(cbuff, modeAsc);
        // strcpy(ptr, "/descriptor");
        // sprintf (ptr, "%s", "/descriptor");
        *ptr  '\0';
        strcat (cbuff, "/descriptor");
        k->TS_desc  OpenFile(cbuff, modeAsc);
    }
    else
    {
        strcpy(ptrBase, "/ts/data");
        k->TS_fp  OpenFile(cbuff, modeAsc);
        strcpy(ptrBase, "/ts/descriptor");
        k->TS_desc  OpenFile(cbuff, modeAsc);
    }

    k->width  k->height  0;
    k->currentFrame  k->totalFrames  0;

    if (mode  KEV_READ)
    {
        while (fscanf(k->Y_desc, "(_dimensions%d%d", &k->height, &k->width)
               ! EOF)
            fgets(cbuff, 256, k->Y_desc);       // go to next line

        if ((k->height  0) || (k->width  0))
        {
            PrintErr("Error: frame dimensions not found in descriptor\n");
            Exit;
        }
        fseek(k->Y_desc, 0, SEEK_SET);

        while (fscanf(k->Y_desc, "(_data_sets%ld", &k->totalFrames) ! EOF)
            fgets(cbuff, 256, k->Y_desc);       // go to next line

        if (k->totalFrames  0)
        {
            PrintErr("Error: total frames not found in descriptor\n");
            Exit;
        }

        Print1("Opened KEV file for read: frames  %ld, size  %d x %d\n",
               k->totalFrames, k->width, k->height);
    }
}

S32 GetTimeStamp(KevFile *k, S32 frameNum)
{
    S32     i, n  0;

    if ((k->mode  KEV_WRITE) || (k->TS_fp  NULL))
        return frameNum;

    if (frameNum > k->totalFrames)
        frameNum  k->totalFrames;

    for (i  0; i < frameNum; i++)
        fscanf(k->TS_fp, "%ld", &n);

    fseek(k->TS_fp, 0, SEEK_SET);

    return n;
}

// For READ mode only
static void ReadFrame(FILE *fp, UCHAR *d, Int width, Int height, Int xsize)
{
    Int     i;

    TimerStartModule;

    for (i  0; i < height; i++)
    {
        fread(d, sizeof (UCHAR), width, fp);
        d + xsize;
    }

    TimerEndModule(t_read_total);
}

// For READ mode only
void ReadKevFrame(KevFile *k, S32 frameNum, UCHAR *y, Int xsize,
                  UCHAR *cb, UCHAR *cr, Int cxsize)
{
    S32     size;

    assert(k->mode  KEV_READ);

    if (k->mode ! KEV_READ)
    {
        PrintErr("Error: Wrong mode for reading from KEV file\n");
        Exit;
    }

    if ((frameNum < 0) || (frameNum > k->totalFrames))
    {
        PrintErr("Invalid frame number %ld\n", frameNum);
        Exit;
    }

    size  k->width * k->height;
    fseek(k->Y_fp, size * frameNum, SEEK_SET);
    ReadFrame(k->Y_fp, y, k->width, k->height, xsize);

    size >> 2;
    fseek(k->CB_fp, size * frameNum, SEEK_SET);
    ReadFrame(k->CB_fp, cb, k->width >> 1, k->height >> 1, cxsize);

    fseek(k->CR_fp, size * frameNum, SEEK_SET);
    ReadFrame(k->CR_fp, cr, k->width >> 1, k->height >> 1, cxsize);

    if (feof(k->Y_fp) || feof(k->CB_fp) || feof(k->CR_fp))
    {
        PrintErr("End of file encountered when reading frame %ld\n",
                 frameNum);
        Exit;
    }
}

// For WRITE mode only
void SetDimensions(KevFile *k, Int width, Int height)
{
    assert(k->mode  KEV_WRITE);

    if (k->mode  KEV_WRITE)
    {
        k->width  width;
        k->height  height;
    }
}

static void WriteFrame(FILE *fp, UCHAR *d, Int width, Int height,
                       Int xsize)
{
    Int     i;

    for (i  0; i < height; i++)
    {
        fwrite(d, sizeof (UCHAR), width, fp);
        d + xsize;
    }
}

static void WriteDescriptor(FILE *fp, Int width, Int height, S32 nFrames,
                            char *typeStr)
{
    // Overwrite any previous description
    fseek(fp, 0, SEEK_SET);

    fprintf(fp, "(_data_sets %ld)\n(_channels 1)\n"
            "(_dimensions %d %d)\n(_data_type %s)\n",
            nFrames, height, width, typeStr);
}

void WriteKevFrame(KevFile *k, S32 frameNum, UCHAR *y, Int xsize,
                   UCHAR *cb, UCHAR *cr, Int cxsize)
{
    assert(k->mode  KEV_WRITE);

    if (k->mode  KEV_READ)
    {
        PrintErr("Error: Trying to write to a read-only KEV file\n");
        Exit;
    }

    k->totalFrames++;

    WriteFrame(k->Y_fp, y, k->width, k->height, xsize);
    WriteDescriptor(k->Y_desc, k->width, k->height, k->totalFrames,
                    "unsigned_1");

    WriteFrame(k->CB_fp, cb, k->width >> 1, k->height >> 1, cxsize);
    WriteDescriptor(k->CB_desc, k->width >> 1, k->height >> 1,
                    k->totalFrames, "unsigned_1");

    WriteFrame(k->CR_fp, cr, k->width >> 1, k->height >> 1, cxsize);
    WriteDescriptor(k->CR_desc, k->width >> 1, k->height >> 1,
                    k->totalFrames, "unsigned_1");

    fprintf(k->TS_fp, "%ld\n", frameNum);
    WriteDescriptor(k->TS_desc, 1, 1, k->totalFrames, "ascii");
}

static void CloseFile(FILE *fp)
{
    if (fp ! NULL)
        fclose(fp);
}

void KevClose(KevFile *k)
{
    CloseFile(k->Y_fp);
    CloseFile(k->Y_desc);
    CloseFile(k->CB_fp);
    CloseFile(k->CB_desc);
    CloseFile(k->CR_fp);
    CloseFile(k->CR_desc);
    CloseFile(k->TS_fp);
    CloseFile(k->TS_desc);
}
#endif
