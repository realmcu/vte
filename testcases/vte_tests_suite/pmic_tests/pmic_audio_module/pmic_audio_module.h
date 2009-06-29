/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
        @file   pmic_audio_module.h

        @brief  Header file for PMIC audio driver test
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
S.Bezrukov/SBAZR1C           31/08/2005     TLSbo52697   Initial version
A.Ozerov/b00320              08/08/2006     TLSbo73745   Review version(in accordance to L26_1_19 release).

====================================================================================================
Portability: ARM GCC
==================================================================================================*/    
#ifndef PMIC_AUDIO_TEST_MODULE_H
#define PMIC_AUDIO_TEST_MODULE_H

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                                            INCLUDE FILES
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define PMIC_AUDIO_DEV "pmic_audio"

#define TPASS    0    /* Test passed flag */
#define TFAIL    1    /* Test failed flag */
#define TBROK    2    /* Test broken flag */
#define TWARN    4    /* Test warning flag */
#define TRETR    8    /* Test retire flag */
#define TINFO    16   /* Test information flag */
#define TCONF    32   /* Test not appropriate for configuration flag */

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
enum PMIC_AUDIO_TEST_IOCTL
{
        PMIC_AUDIO_TEST_OUTPUT            = 0,
        PMIC_AUDIO_TEST_INPUT             = 1,
        PMIC_AUDIO_TEST_SDAC              = 2,
        PMIC_AUDIO_TEST_CODEC             = 3,
        PMIC_AUDIO_TEST_BUS               = 4
};

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_pmic_audio_test_output(void);
int VT_pmic_audio_test_input(void);
int VT_pmic_audio_test_sdac(void);
int VT_pmic_audio_test_codec(void);
int VT_pmic_audio_test_bus(void);

#ifdef __cplusplus
}
#endif

#endif /* PMIC_AUDIO_TEST_MODULE_H */
