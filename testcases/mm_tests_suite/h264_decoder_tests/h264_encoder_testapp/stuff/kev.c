/*
 ***********************************************************************
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
 ***********************************************************************
 */
/*!
 ***********************************************************************
 *  \file
 *      kev.c
 *
 *  \brief
 *    This file contains low-level KevFile I/O routines.
 *
 *  \author
 *      Raghavan Subramaniyan   <raghavan.s@motorola.com>
 *
 *  \version
 *      $Revision: 1.2 $ - $Date: 2004/05/24 23:05:37 $
 *      $Revision: 1.3 $ - $Date: 2005/07/11 23:05:37 $
 *                 Added support for windows
 *
 *
 ***********************************************************************
 */
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "kev.h"
#include <assert.h>
#ifdef _WINDOWS
#  include <windows.h>
#  include <winbase.h>
#endif

#if IO_KEV
#  ifdef __unix__
int createDir(char *dirname)
{
    FILE   *f;
    char    cmd[256];

    // create a dir if it doesn't exist
    f = fopen(dirname, "r");
    if (f == 0)
    {
        strcpy(cmd, "mkdir ");
        strcat(cmd, dirname);
        return system(cmd);
    }
    else
        fclose(f);
    return 0;
}

#  elif defined _WINDOWS
// This is for DOS platform
void UnixToDosFilename(char *filename)
{
    unsigned int i, n = strlen(filename);

    for (i = 0; i < n; i++)
    {
        if (filename[i] == '/')
            filename[i] = '\\';                 //Convert to DOS pathname

    }
    return;
}

int createDir(char *dirname)
{
    UnixToDosFilename(dirname);                 //Convert to DOS pathname

    CreateDirectory(dirname, NULL);

    return 0;
}

#  elif defined __arm
int createDir(char *dirname)
{
    printf("ARM: Assuming directory %s already exists. If not,"
           " create manualy\n", dirname);

    return 0;
}
#  else
// This platform is not supported
int createDir(char *dirname)
{
    printf("File I/O for this platform not supported\n");
    exit(1);

    return 0;
}
#  endif
#  ifndef _WINDOWS
static FILE *OpenFile(char *name, char *mode)
{
    FILE   *fp;

    if ((fp = fopen(name, mode)) == NULL)
    {
        printf("KevOpen error - could not open file %s in %s mode\n",
               name, mode);
        exit(1);
    }
    return fp;
}

void KevOpen(char *name, int mode, KevFile * k)
{
    char    cbuff[256], *ptr, *ptrBase;
    char    modeBin[3], modeAsc[2];

    k->mode = mode;

    if (mode == KEV_WRITE)
    {
        strcpy(modeBin, "wb");
        strcpy(modeAsc, "w");
    }
    else
    {
        strcpy(modeBin, "rb");
        strcpy(modeAsc, "r");
    }

    if (mode == KEV_WRITE)
        createDir(name);

    strcpy(cbuff, name);
    ptrBase = cbuff + strlen(cbuff);

    strcpy(ptrBase, "/y");
    if (mode == KEV_WRITE)
        createDir(cbuff);
    ptr = cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->Y_fp = OpenFile(cbuff, modeBin);
    strcpy(ptr, "/descriptor");
    k->Y_desc = OpenFile(cbuff, modeAsc);

    strcpy(ptrBase, "/cb");
    if (mode == KEV_WRITE)
        createDir(cbuff);
    ptr = cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->CB_fp = OpenFile(cbuff, modeBin);
    strcpy(ptr, "/descriptor");
    k->CB_desc = OpenFile(cbuff, modeAsc);

    strcpy(ptrBase, "/cr");
    if (mode == KEV_WRITE)
        createDir(cbuff);
    ptr = cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->CR_fp = OpenFile(cbuff, modeBin);
    strcpy(ptr, "/descriptor");
    k->CR_desc = OpenFile(cbuff, modeAsc);

    // The source KEV files we have do not have a "ts" component,
    // hence open files only for "write" mode
    if (mode == KEV_WRITE)
    {
        strcpy(ptrBase, "/ts");
        createDir(cbuff);
        ptr = cbuff + strlen(cbuff);
        strcat(cbuff, "/data");
        k->TS_fp = OpenFile(cbuff, modeAsc);
        strcpy(ptr, "/descriptor");
        k->TS_desc = OpenFile(cbuff, modeAsc);
    }
    else
    {
        k->TS_fp = NULL;
        k->TS_desc = NULL;
    }

    k->width = k->height = 0;
    k->currentFrame = k->totalFrames = 0;

    if (mode == KEV_READ)
    {
        while (fscanf(k->Y_desc, "(_dimensions%d%d", &k->height, &k->width)
               != EOF)
            fgets(cbuff, 256, k->Y_desc);       // go to next line

        if ((k->height == 0) || (k->width == 0))
        {
            printf("Error: frame dimensions not found in descriptor\n");
            exit(1);
        }
        rewind(k->Y_desc);

        while (fscanf(k->Y_desc, "(_data_sets%ld", &k->totalFrames) != EOF)
            fgets(cbuff, 256, k->Y_desc);       // go to next line

        if (k->totalFrames == 0)
        {
            printf("Error: total frames not found in descriptor\n");
            exit(1);
        }

        //printf("Opened KEV file for read: frames = %ld, size = %d x %d\n",
        //       k->totalFrames, k->width, k->height);

    }
}
#  else
void KevOpen(char *name, int write, KevFile * k)
{
    char    cbuff[256];
    char    mode[3];
    char   *ptr, *ptrBase;

    k->mode = write;
    if (write)
        strcpy(mode, "wb");
    else
        strcpy(mode, "rb");

    if (write)
        createDir(name);
    strcpy(cbuff, name);
    ptrBase = cbuff + strlen(cbuff);
    strcpy(ptrBase, "/y");

    if (write)
        createDir(cbuff);
    ptr = cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->Y_fp = fopen(cbuff, mode);

    if (k->Y_fp == NULL)
    {
        printf("KevOpen error - could not open Y file in %s mode\n", mode);
        exit(1);
    }

    strcpy(ptr, "/descriptor");
    k->Y_desc = fopen(cbuff, mode);
    if (k->Y_desc == NULL)
    {
        printf
            ("KevOpen error - could not open Ydescriptor file in %s mode\n",
             mode);
        exit(1);
    }

    strcpy(ptrBase, "/cb");
    if (write)
        createDir(cbuff);
    ptr = cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->CB_fp = fopen(cbuff, mode);
    if (k->CB_fp == NULL)
    {
        printf("KevOpen error - could not open CB file in %s mode\n",
               mode);
        exit(1);
    }

    strcpy(ptr, "/descriptor");
    k->CB_desc = fopen(cbuff, mode);
    if (k->CB_desc == NULL)
    {
        printf("KevOpen error - could not open CB desc file in %s mode\n",
               mode);
        exit(1);
    }

    strcpy(ptrBase, "/cr");
    if (write)
        createDir(cbuff);
    ptr = cbuff + strlen(cbuff);
    strcat(cbuff, "/data");
    k->CR_fp = fopen(cbuff, mode);
    if (k->CR_fp == NULL)
    {
        printf("KevOpen error - could not open CR file in %s mode\n",
               mode);
        exit(1);
    }

    strcpy(ptr, "/descriptor");
    k->CR_desc = fopen(cbuff, mode);
    if (k->CR_desc == NULL)
    {
        printf("KevOpen error - could not open CR desc file in %s mode\n",
               mode);
        exit(1);
    }

    if (write)
    {
        // Tmp. because orig video sequences do not have ts directory
        createDir(name);
        strcpy(cbuff, name);
        strcat(cbuff, "/ts");
        if (write)
            createDir(cbuff);
        ptr = cbuff + strlen(cbuff);
        strcat(cbuff, "/data");
        k->TS_fp = fopen(cbuff, mode);
        if (k->TS_fp == NULL)
        {
            printf("KevOpen error - could not open ts file in %s mode\n",
                   mode);
            exit(1);
        }
        strcpy(ptr, "/descriptor");
        k->TS_desc = fopen(cbuff, mode);
        if (k->TS_desc == NULL)
        {
            printf
                ("KevOpen error - could not open TS desc file in %s mode\n",
                 mode);
            exit(1);
        }

    }
    k->width = k->height = 0;
    k->currentFrame = k->totalFrames = 0;

    if (k->mode == KEV_READ)
    {
        while (fscanf(k->Y_desc, "(_dimensions%d%d", &k->height, &k->width)
               != EOF)
            fgets(cbuff, 256, k->Y_desc);       // go to next line

        if ((k->height == 0) || (k->width == 0))
        {
            printf("Error: frame dimensions not found in descriptor\n");
            exit(1);
        }
        fseek(k->Y_desc, 0, SEEK_SET);

        while (fscanf(k->Y_desc, "(_data_sets%ld", &k->totalFrames) != EOF)
            fgets(cbuff, 256, k->Y_desc);       // go to next line

        if (k->totalFrames == 0)
        {
            printf("Error: total frames not found in descriptor\n");
            exit(1);
        }

        //printf("Opened KEV file for read: frames = %ld, size = %d x %d\n",
        //       k->totalFrames, k->width, k->height);

    }
}
#  endif
// For READ mode only
static void ReadFrame(FILE *fp, unsigned char *d, int width, int height,
                      int xsize)
{
    int     i;

    for (i = 0; i < height; i++)
    {
        fread(d, sizeof (unsigned char), width, fp);

        d += xsize;
    }

}

// For READ mode only
int ReadKevFrame(KevFile * k, long int frameNum, unsigned char *y,
                 int xsize, unsigned char *cb, unsigned char *cr,
                 int cxsize)
{
    int     size;

    assert(k->mode == KEV_READ);

    if (k->mode != KEV_READ)
    {
        printf("Error: Wrong mode for reading from KEV file\n");
        exit(1);
    }

    if ((frameNum < 0) || (frameNum >= k->totalFrames))
    {
        printf("Invalid frame number %ld\n", frameNum);
        //exit(1);
        return 0;
    }

    size = k->width * k->height;
    fseek(k->Y_fp, size * frameNum, SEEK_SET);
    ReadFrame(k->Y_fp, y, k->width, k->height, xsize);

    size >>= 2;
    fseek(k->CB_fp, size * frameNum, SEEK_SET);
    ReadFrame(k->CB_fp, cb, k->width >> 1, k->height >> 1, cxsize);

    fseek(k->CR_fp, size * frameNum, SEEK_SET);
    ReadFrame(k->CR_fp, cr, k->width >> 1, k->height >> 1, cxsize);

    if (feof(k->Y_fp) || feof(k->CB_fp) || feof(k->CR_fp))
    {
        printf("End of file encountered when reading frame %ld\n",
               frameNum);
        //     exit(1);
        return (0);
    }
    return (1);
}

// For WRITE mode only
void SetDimensions(KevFile * k, int width, int height)
{
    assert(k->mode == KEV_WRITE);

    if (k->mode == KEV_WRITE)
    {
        k->width = width;
        k->height = height;
    }
}

static void WriteFrame(FILE *fp, unsigned char *d, int width, int height,
                       int xsize)
{
    int     i;

    for (i = 0; i < height; i++)
    {
        fwrite(d, sizeof (unsigned char), width, fp);

        d += xsize;
    }
}

static void WriteDescriptor(FILE *fp, int width, int height,
                            long int nFrames, char *typeStr)
{
    // Overwrite any previous description
    rewind(fp);

    fprintf(fp, "(_data_sets %ld)\n(_channels 1)\n"
            "(_dimensions %d %d)\n(_data_type %s)\n",
            nFrames, height, width, typeStr);

    fflush(fp);
}

void WriteKevFrame(KevFile * k, long int frameNum, unsigned char *y,
                   int xsize, unsigned char *cb, unsigned char *cr,
                   int cxsize)
{
    assert(k->mode == KEV_WRITE);

    if (k->mode == KEV_READ)
    {
        printf("Error: Trying to write to a read-only KEV file\n");
        exit(1);
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
    if (fp != NULL)
        fclose(fp);
}

void KevClose(KevFile * k)
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
