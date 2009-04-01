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
#ifndef _IO_H
#define _IO_H

#define IO_KEV                  1
#define  PRINT_ENCODED_FRAME    1
#define MAX_NAME_LENGTH         200

#include <debug.h>
#include "kev.h"
#include <AVC_VideoEncoder.h>

//! Input/Output parameters used by the encoder. An
//! alternate structure can be defined based upon the specific I/O
//!    e.g. if display/capture device is present

#define CTL_CURRENT_VERSION             4

typedef struct
{
    int     version;                    //!< Version of control file
    char    ctlFile[MAX_NAME_LENGTH];   //!< Input control file
    char    SliceGroupConfigFileName[MAX_NAME_LENGTH];  //!< For FMO
    char    sourceFilename[MAX_NAME_LENGTH];    //!< Input video sequence
    char    bitFilename[MAX_NAME_LENGTH];   //!< Ouput compressed video bitstream
    char    traceFilename[MAX_NAME_LENGTH]; //!< Encoder trace filename
    char    reconFilename[MAX_NAME_LENGTH]; //!< Reconstructed video seqience
    char    traceFlag;                  //!< Generate a tracefile [No=0/Yes=1]
    char    writeReconstructedFlag;     //!< Generate reconstructed video [0/1]
    KevFile kevInput;                   //!< Input source KEV file
    KevFile kevOutput;                  //!< Output reconstructed KEV file
    long    startFrameNum;              //!< Start frame num for encoding
    long    endFrameNum;                //!< End frame num for encoding
    long    imageCount;                 //!< Total no. of source frames
    int     outputMode;                 //!< 0:Bitstream, 1:RTP, 2:IFF
    FILE   *outFile;                    //!< Bitstream output file
#if PRINT_ENCODED_FRAME
    FILE   *frameNum;                   //!< File ahs the frame number of the encoded frames
#endif
}
IOParams;

//! Function prototpyes in io.c
void    IO_Init(IOParams *p);

void    IO_ReadVideoFrame(IOParams *p, long frameNum,
                          AVCEnc_YCbCrStruct * f);
void    IO_WriteNALUnit(IOParams *p, char *b, long size);

void    IO_WriteOutputVideo(IOParams *p, AVCEnc_YCbCrStruct * f);
void    IO_Close(IOParams *p);

#if (SUPPORT_TRACEFILE)
void    PrintByteTrace(int val);
#else
#  define PrintByteTrace(val)
#endif
#endif
