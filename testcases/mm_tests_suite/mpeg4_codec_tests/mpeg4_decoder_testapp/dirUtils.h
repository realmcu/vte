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
//  2/May/00  rags      Created
//
///////////////////////////////////////////////////////////////////////////

#ifndef DIR_UTILS_H
#define DIR_UTILS_H

//#ifndef BUFFER_INPUT
//#include "debug.h"
//#else
#define DEBUG_1   0
//typedef unsigned char UCHAR; /*!< Commented by Anurag */
typedef unsigned long int U32;
//#endif

#if (DEBUG_1)
#define ALLOCATE_MEMORY(p,n)    (p) = AllocateMemory((n), #p);
#define FREE_MEMORY(p)          FreeMemory(p, #p);
void   *AllocateMemory(U32 size, char *name);
void    FreeMemory(void *ptr, char *name);

#else
#define ALLOCATE_MEMORY(p,n)    (p) = myAllocateMemory(n);
#define FREE_MEMORY(p)      FreeMemory(p);
void   *myAllocateMemory(U32 size);
void    myFreeMemory(void *ptr);

#endif

// Buffer alignment

#ifdef _SC100_
#define ALIGN   16
#elif defined __arm
#define ALIGN   4
#else
#define ALIGN   1
#endif

#define MASK_ALIGN (-ALIGN)


void    CreateDirectoriesIfNeeded(char *filenameStr);
int     createDir(char *dirname);

#ifdef _WIN32
void    UnixToDosFilename(char *filename);

#endif

#endif
