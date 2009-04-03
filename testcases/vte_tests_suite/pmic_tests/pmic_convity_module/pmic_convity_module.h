/*================================================================================================*/
/**
        @file   pmic_convity_module.h

        @brief  CONNECTIVITY driver test header file
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
V.Khalabuda/hlbv001          13/08/2005     TLSbo52695   Initial version
V.Khalabuda/hlbv001          24/11/2005     TLSbo58397   Update for linux-2.6.10-rel-L26_1_14
A.Ozerov/b00320              15/05/2006     TLSbo64237   Code was cast in accordance to coding conventions.
A.Ozerov/b00320              05/07/2006     TLSbo64237   Two defines were added.

====================================================================================================
Portability:    ARM GCC
==================================================================================================*/
#ifndef PMIC_CONVITY_TEST_MODULE_H
#define PMIC_CONVITY_TEST_MODULE_H

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/
static const unsigned int RESET_BUSCTRL = 0x0402a4;
static const unsigned int RESET_ISR     = 0x000000;
static const unsigned int RESET_IMR     = 0xdfffff;
static const unsigned int RESET_PSTAT   = 0x000000;

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define PMIC_CONVITY_DEV "pmic_convity_test"
#define RESET_USBCNTRL_REG_0 0x080060
#define RESET_USBCNTRL_REG_1 0x000006

#ifndef __ALL_TRACES__
#define __ALL_TRACES__
#endif

#define SET_BITS(field, value)        (((value) << regBUSCTRL.field.offset) & \
                                regBUSCTRL.field.mask)

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
/* Test IOCTLS */
typedef enum
{
        PMIC_CONVITY_TEST_MODE,
        PMIC_CONVITY_TEST_RESET,
        PMIC_CONVITY_TEST_CALLBACK,
        PMIC_CONVITY_TEST_USB_SPEED,
        PMIC_CONVITY_TEST_USB_POWER,
        PMIC_CONVITY_TEST_USB_TRANSCEIVER_MODE,
        PMIC_CONVITY_TEST_USB_OTG_DLP_DURATION,
        PMIC_CONVITY_TEST_USB_OTG_CONFIG,
        PMIC_CONVITY_TEST_RS232,
        PMIC_CONVITY_TEST_CEA936,
        PMIC_CONVITY_TEST_OPEN,
        PMIC_CONVITY_TEST_CLOSE
} PMIC_CONVITY_TEST_IOCTL;

typedef enum
{
        COMPARE_EQUAL,
        COMPARE_NOT_EQUAL,
        COMPARE_GREATER_THAN,
        COMPARE_GREATER_THAN_OR_EQUAL,
        COMPARE_LESS_THAN,
        COMPARE_LESS_THAN_OR_EQUAL,
        COMPARE_CUSTOM,
        DONT_CARE
} COMPARE_VALUE;

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef const struct
{
        unsigned char offset;
        unsigned int  mask;
} REGFIELD;

typedef const struct
{
        REGFIELD FSENB;
        REGFIELD USB_SUSPEND;
        REGFIELD USB_PU;
        REGFIELD USBDP_PD;
        REGFIELD USBDM_PD;
        REGFIELD PULLOVR;
        REGFIELD VUSB_EN;
        REGFIELD USB_PS;
        REGFIELD VUSB_MSTR_EN;
        REGFIELD VBUS_PD_ENB;
        REGFIELD CURRLIM;
        REGFIELD SE0_CONN;
        REGFIELD DLP_SRP;
        REGFIELD DLP_DURATION;
        REGFIELD RS232ENB;
        REGFIELD RS232_POL;
} PMIC_REG_BUSCTRL;

typedef const struct
{
        REGFIELD ADCDONEI;
        REGFIELD TSI;
        REGFIELD CLK_1HZI;
        REGFIELD WHI;
        REGFIELD WLI;
        REGFIELD TODAI;
        REGFIELD USB4V4I;
        REGFIELD ONOFFI;
        REGFIELD ONOFF2I;
        REGFIELD USB0V8I;
        REGFIELD MOBPORTI;
        REGFIELD PTTI;
        REGFIELD A1I;
        REGFIELD PCI;
        REGFIELD WARMI;
        REGFIELD EOLI;
        REGFIELD CLKI;
        REGFIELD USB2V0I;
        REGFIELD AB_DETI;
        REGFIELD ADCDONE2I;
        REGFIELD SOFT_RESETI;
} PMIC_REG_ISR;

typedef const struct
{
        REGFIELD ADCDONEM;
        REGFIELD TSM;
        REGFIELD CLK_1HZM;
        REGFIELD WHM;
        REGFIELD WLM;
        REGFIELD TODAM;
        REGFIELD USB4V4M;
        REGFIELD ONOFFM;
        REGFIELD ONOFF2M;
        REGFIELD USB0V8M;
        REGFIELD MOBPORTM;
        REGFIELD PTTM;
        REGFIELD A1M;
        REGFIELD PCM;
        REGFIELD WARMM;
        REGFIELD EOLM;
        REGFIELD CLKM;
        REGFIELD USB2V0M;
        REGFIELD AB_DETM;
        REGFIELD ADCDONE2M;
        REGFIELD SOFT_RESETM;
} PMIC_REG_IMR;

typedef const struct
{
        REGFIELD USBDET_4V4;
        REGFIELD OFFONSNS;
        REGFIELD OFFONSNS2;
        REGFIELD USBDET_0V8;
        REGFIELD MOBSENSB;
        REGFIELD PTTSNS;
        REGFIELD A1SNS;
        REGFIELD USBDET_2V0;
        REGFIELD EOL_STAT;
        REGFIELD CLK_STAT;
        REGFIELD SYS_RST;
        REGFIELD WARM_SYS_RST;
        REGFIELD BATT_DET_IN_SNS;
        REGFIELD SOFT_RESETI;
} PMIC_REG_PSTAT;

typedef struct
{
        PMIC_CONVITY_HANDLE        handle;
        PMIC_CONVITY_MODE        mode;
} pmic_covity_test_param;

/*==================================================================================================
    Create a structure that maps to the PMIC's BUSCTRL register
    and which will allow us to read/write each field.
==================================================================================================*/
static PMIC_REG_BUSCTRL regBUSCTRL =
{
    {  0, 0x00001 }, /* FSENB        */
    {  1, 0x00002 }, /* USB_SUSPEND  */
    {  2, 0x00004 }, /* USB_PU       */
    {  3, 0x00008 }, /* USBDP_PD     */
    {  4, 0x00010 }, /* USBDM_PD     */
    {  5, 0x00020 }, /* PULLOVR      */
    {  6, 0x00040 }, /* VUSB_EN      */
    {  7, 0x00080 }, /* USB_PS       */
    {  8, 0x00100 }, /* VUSB_MSTR_EN */
    {  9, 0x00200 }, /* VBUS_PD_ENB  */
    { 10, 0x00400 }, /* CURRLIM      */
    { 11, 0x00800 }, /* SE0_CONN     */
    { 12, 0x01000 }, /* DLP_SRP      */
    { 13, 0x0e000 }, /* DLP_DURATION */
    { 17, 0x20000 }, /* RS232ENB     */
    { 18, 0x40000 }  /* RS232_POL    */
};

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern PMIC_REG_BUSCTRL    regBUSCTRL;
extern PMIC_REG_ISR        regISR;
extern PMIC_REG_IMR        regIMR;
extern PMIC_REG_PSTAT      regPSTAT;

typedef PMIC_STATUS (*const COMPARE_FUNC)(const unsigned int actual, const unsigned int mask, const unsigned int expected);

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* PMIC_CONVITY_TEST_MODULE_H */
