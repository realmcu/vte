///////////////////////////////////////////////////////////////////////////
//
//  Motorola Chicago Corporate Research Labs
//
//  Project         : DSP2000, Horizon
//
//  Primary Author: Raghavan Subramaniyan (raghavan.s@motorola.com)
//
//                  This code is the property of Motorola.
//          (C) Copyright 1999-2000 Motorola,Inc. All Rights Reserved.
//                       MOTOROLA INTERNAL USE ONLY
//
// History :
//  Date:     Author    Description
//  2/May/00  rags      Created
//
///////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "dirUtils.h"

#ifndef _WIN32_WCE
#  include <assert.h>
#else
#  define assert(expression)
#endif

#define DEBUG_1 0
#define Exit   exit(0)

//\typedef unsigned long int U32; /*!< commented by Anurag */

///////////////////////////////////////////////////////////////////////////
//
//  Function    : AllocateMemory

//
//  Author(s)   : Raghavan Subramaniyan (raghavan.s@motorola.com)
//
//  Description : Allocates memory.
//
//                Currently Using malloc. May need to change for platforms
//                that do not support mallocs.
//
///////////////////////////////////////////////////////////////////////////

#if (DEBUG_1)
void   *myAllocateMemory(U32 size, char *str)
#else
void   *myAllocateMemory(U32 size)
#endif
{
    void   *ptr;

    // This is currently defined to be malloc. Application specific
    // allocation can be substituted here
    ptr  malloc(size);

    if (ptr  NULL)
    {
        fprintf(stderr, "Error allocating memory\n");
        Exit;
    }
#if (DEBUG_1)
    printf("Memory Allocation : %s, %ld bytes of memory @ %p\n",
           str, size, ptr);
#endif
    return ptr;
}

///////////////////////////////////////////////////////////////////////////
//
//  Function    : FreeMemory
//
//  Author(s)   : Raghavan Subramaniyan (raghavan.s@motorola.com)
//
//  Description : Frees memory.
//
//                This needs to be updated to take care of all platforms
//
///////////////////////////////////////////////////////////////////////////

#if (DEBUG_1)
void myFreeMemory(void *ptr, char *str)
#else
void myFreeMemory(void *ptr)
#endif
{
    // This is currently defined to be "free". Application specific
    // de-allocation can be substituted here
    free(ptr);

#if (DEBUG_1)
    printf("Memory De-allocation : %s, @ %p\n", str, ptr);

#endif

    return;
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
void CreateDirectoriesIfNeeded(char *filenameStr)
{
    char   *nextSlashPtr;

    nextSlashPtr  filenameStr + 1;
    while (1)
    {                                           // for each directory in the path
        nextSlashPtr  strchr(nextSlashPtr, '/');   // find the next slash
        if (nextSlashPtr  NULL)               // if no more slashes
            return;
        *nextSlashPtr  '\0';                   // shorten the string to just the directory name
        createDir(filenameStr);
        *nextSlashPtr  '/';                    // repair the string
        nextSlashPtr++;                         // process the remaining string
    }
}

#ifdef _WIN32
static void UnixToDosFilename(char *filename)
{
    int     i, n  strlen(filename);

    for (i  0; i < n; i++)
    {
        if (filename[i]  '/')
            filename[i]  '\\';                 //Convert to DOS pathname

    }
    return;
}
#endif


#ifndef _SC100_
#  ifdef __unix__
#    define CREATE_DIR_SUPPORTED
int createDir(char *dirname)
{
    FILE   *f;
    char    cmd[256];

    // create a dir if it doesn't exist
    f  fopen(dirname, "r");
    if (f  0)
    {
        strcpy(cmd, "mkdir -p ");
        strcat(cmd, dirname);
        return system(cmd);
    }
    else
        fclose(f);
    return 0;
}

#  elif defined WIN32
#    ifndef _WIN32_WCE
#      define CREATE_DIR_SUPPORTED
// This is for DOS platform
int createDir(char *dirname)
{
    UnixToDosFilename(dirname);                 //Convert to DOS pathname

    CreateDirectoriesIfNeeded(dirname);

    return 0;
}
#    endif
#  endif
#endif

#ifndef CREATE_DIR_SUPPORTED
// Creating dummy function for unsupported platforms to enable
// successful link
int createDir(char *dirname)
{
    printf("createDir not supported for this platform.\n"
           "If needed create directory %s before running\n", dirname);

    return 0;
}
#endif
