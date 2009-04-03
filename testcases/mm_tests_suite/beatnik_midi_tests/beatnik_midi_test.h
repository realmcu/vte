/* 
 * Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved THIS SOURCE CODE IS
 * CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
 * Freescale Semiconductor, Inc. */

/**
@file beatnik_midi_test.h

@brief VTE C header template

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  11/09/2005   TLSbo53248   Initial version 
=============================================================================*/

#ifndef __BEATNIK_MIDI_TEST_H__
#define __BEATNIK_MIDI_TEST_H__


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Config's parser. */
#include "stuff/cfg_parser.h"


/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/
#if !defined(TRUE)
#define TRUE 1
#endif
#if !defined(FALSE)
#define FALSE 0
#endif

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define DEFAULT_ITERATIONS 10

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/** Different test cases in the single application */
typedef enum
{
        NOMINAL_FUNCTIONALITY = 0,
        ROBUSTNESS,
        RELOCATABILITY,
        RE_ENTRANCE,
        PRE_EMPTION,
        ENDURANCE,
        LOAD
} eBeatnikMidiTestCases;


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        int     mTestCase;
        int     mNumIter;
        const char *mConfigFilename;
        int     mOutputBan;
        int     mVerbose;
} sTestappConfig;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern sTestappConfig gTestappConfig;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int     VT_beatnik_midi_setup(void);
int     VT_beatnik_midi_cleanup(void);
int     VT_beatnik_midi_test(void);



#endif                          // __BEATNIK_MIDI_TEST_H__
