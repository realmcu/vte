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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "io.h"
#include "kev.h"
#include <writebits.h>
#include <utils.h>

FILE   *traceFile = NULL;
S32     traceIndex = 0;

void OpenInputVideo(IOParams *p)
{
    KevOpen(p->sourceFilename, KEV_READ, &p->kevInput);
    assert((p->kevInput.width & 0xf) == 0);
    assert((p->kevInput.height & 0xf) == 0);
}

void OpenOutputVideo(IOParams *p)
{
    if (p->writeReconstructedFlag)
    {
        KevOpen(p->reconFilename, KEV_WRITE, &p->kevOutput);
        SetDimensions(&p->kevOutput, p->kevInput.width,
                      p->kevInput.height);
    }
}

///////////////////////////////////////////////////////////////////////////
//
//  Function    : CreateDirectoriesIfNeeded
//
//  Author(s)   : Re-used form Horizon
//
//  Description : This function will create if necessary all directories
//                required to open the given file. The file itself is not
//                created or checked.
//
///////////////////////////////////////////////////////////////////////////
void    createDir(char *dirname);
static void CreateDirectoriesIfNeeded(char *filenameStr)
{
    char   *nextSlashPtr;

    nextSlashPtr = filenameStr + 1;
    while (1)
    {                                           // for each directory in the path

        nextSlashPtr = strchr(nextSlashPtr, '/');   // find the next slash

        if (nextSlashPtr == NULL)               // if no more slashes

            return;
        *nextSlashPtr = '\0';                   // shorten the string to just the directory name

        createDir(filenameStr);
        *nextSlashPtr = '/';                    // repair the string

        nextSlashPtr++;                         // process the remaining string

    }
}

void OpenOutputFile(IOParams *p)
{
#if PRINT_ENCODED_FRAME
    char   *FrameNumFilename;
    int     i, len;
#endif

    //!< If we are NOT outputting to an BUFFER (thus writing to a file)
    CreateDirectoriesIfNeeded(p->bitFilename);

    if ((p->outFile = fopen(p->bitFilename, "wb")) == NULL)
    {
        printf("Error opening %s for writing\n", p->bitFilename);
        exit(1);
    }

#if (SUPPORT_TRACEFILE)
    if (!p->traceFlag)
        traceFile = NULL;
    else if ((traceFile = fopen(p->traceFilename, "wb")) == NULL)
    {
        printf("Error opening %s for writing\n", p->traceFilename);
        exit(1);
    }
    else
    {
        fprintf(traceFile,
                "------------------------------------------------------------------------\n"
                "Byte  Type nbits Name                              Bitstring  (Value)\n"
                "------------------------------------------------------------------------\n");
    }
#endif
#if  PRINT_ENCODED_FRAME

    len = strlen(p->bitFilename);
    FrameNumFilename = (char *) malloc(len + 1 + 20);
    strcpy(FrameNumFilename, p->bitFilename);
    for (i = (len - 1); i >= 0; i--)
    {
        if (p->bitFilename[i] == '.')
            break;
    }
    strcpy(&FrameNumFilename[i], ".fnum");

    if ((p->frameNum = fopen(FrameNumFilename, "w")) == NULL)
    {
        printf("Error opening frameNum for writing\n");
        exit(1);
    }

    free(FrameNumFilename);

#endif

}

void IO_ReadVideoFrame(IOParams *p, S32 frameNum, AVCEnc_YCbCrStruct * f)
{
    // Add initial offset because the encoder library always starts count
    // from 0.  For I/O, we have the option of starting from startFrameNum
    frameNum += p->startFrameNum;
    ReadKevFrame(&p->kevInput, frameNum, f->y, f->xSize,
                 f->cb, f->cr, f->cxSize);
}

void IO_WriteOutputVideo(IOParams *p, AVCEnc_YCbCrStruct * f)
{
    if (p->writeReconstructedFlag)
    {
        WriteKevFrame(&p->kevOutput, f->frameNumber, f->y, f->xSize,
                      f->cb, f->cr, f->cxSize);
    }
}

#if (SUPPORT_TRACEFILE)
void PrintByteTrace(int val)
{
    if (traceFile != NULL)
    {
        S32     nBytes, nBits;
        char    s[200];

        nBytes = traceIndex >> 3;
        nBits = traceIndex & 7;
        sprintf(s, "%5ld.%ld [%d] %2d %s", nBytes, nBits, 0, 8, "AnnexB");
        AVCE_PrintString(s, val, 8, val);
        fprintf(traceFile, "%s\n", s);
        traceIndex += 8;
        fflush(traceFile);
    }
}
#endif
/*!
 ***********************************************************************
 * \brief
 *    Writes bits in buffer to output file formatted as a NAL bytestream.
 *    (Annex B). Assumes that the number of bits is a multiple of 8
 *
 * \param *p
 *    pointer to the IOParams structure
 *
 * \param *b
 *    pointer to the BitBuffer structure
 *
 * \param refIdc
 *    Reference indication. Non-zero value means used as reference.
 *    Higher number could indicate higher priority for transport
 ***********************************************************************
 */
void IO_WriteNALUnit(IOParams *p, char *data, long size)
{
    // Write at least one zero byte
    putc(0x00, p->outFile);

    // Write startcode
    putc(0x00, p->outFile);
    putc(0x00, p->outFile);
    putc(0x01, p->outFile);

    fwrite(data, 1, size, p->outFile);

    fflush(p->outFile);
}

void CloseInputVideo(IOParams *p)
{
    KevClose(&p->kevInput);
}

void CloseOutputVideo(IOParams *p)
{
    if (p->writeReconstructedFlag)
        KevClose(&p->kevOutput);
}

void CloseOutputFile(IOParams *p)
{
    fclose(p->outFile);
#if  PRINT_ENCODED_FRAME
    fclose(p->frameNum);
#endif

#if (SUPPORT_TRACEFILE)
    if (traceFile != NULL)
        fclose(traceFile);
#endif

}

void IO_Init(IOParams *p)
{
    OpenInputVideo(p);

    p->imageCount = p->kevInput.totalFrames;

    OpenOutputVideo(p);

    OpenOutputFile(p);
}

void IO_Close(IOParams *p)
{
    CloseInputVideo(p);

    CloseOutputVideo(p);

    CloseOutputFile(p);
}
