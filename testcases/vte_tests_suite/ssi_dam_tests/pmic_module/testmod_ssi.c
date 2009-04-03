/*================================================================================================*/
/**
        @file   testmod_ssi.c

        @brief  ssi test module
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
J.Quintero/jqui1c            26/09/2005     xxx         Initial version
S.V-Guilhou/svan01c          04/10/2005     TLSbo55818  Add MXC91331
D.Khoroshev/b00313           29/11/2005     TLSbo56844  Add SC55112 support
D.Simakov                    06/06/2006     TLSbo67103  No sound on SSI 1 and SSI 2
D.Kardakov                   11/09/2006     TLSbo71015  Re-written for L26_21 release
D.Karaakov                   02/01/2007     TLSbo87890  Update for MXC91131Evb, i.MX31ADS platforms
                                                        Some problems with
                                                        voice codec testcases was fixed
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/autoconf.h>
#include <linux/delay.h>

/* Common API's Include Files */
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/arch/spba.h>
#include <asm-arm/mach-types.h>
/* Platform Specific Include Files */
#include <ssi/ssi.h>
#include <ssi/registers.h>

//#include "dam.h"
#include <dam/dam.h>
#include <asm/arch/pmic_external.h>
#include <asm/arch/pmic_audio.h>

/* Verification Test Environment Include Files */
#include "testmod_ssi.h"

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
#define TIMESLOTS_2                                        0x3
#define TIMESLOTS_4                                        0x2

#define TX_WATERMARK                                       0x4
#define RX_WATERMARK                                       0x6

#define PMIC_OUTPUT_VOLUME_MAX                             0xf
#define OUTPUT_VOLUME_MAX                                  0x64

/* MXC91131 configuration */
#ifdef CONFIG_ARCH_MXC91131
#define DEFAULT_PRESCALER                                  47360000
#define DIVIDE_CLK_RATE_BY_TWO                             1
#define NORMALIZE_CCM_DIVIDER(v)                        ( ((v) * 2) / 5 )
#endif

/* MXC91231 configuration */
#ifdef CONFIG_MXC_PMIC_MC13783
#define DEFAULT_PRESCALER                                  24000000
#define DIVIDE_CLK_RATE_BY_TWO                             0
#define INIT_CKO_CLOCK()                                   \
do                                                         \
{                                                          \
        volatile unsigned long reg;                        \
                                                           \
        reg = readl(IO_ADDRESS(CRM_AP_BASE_ADDR) + 0x44);  \
        reg = ( (reg & (~(0x00000070))) | 0x00000020 );    \
        writel(reg, IO_ADDRESS(CRM_AP_BASE_ADDR) + 0x44);  \
                                                           \
        reg = readl(IO_ADDRESS(CRM_COM_BASE_ADDR) + 0xc);  \
        reg = ( (reg & (~(0x00060000))) | 0x00020000 );    \
        writel(reg, IO_ADDRESS(CRM_COM_BASE_ADDR) + 0xc);  \
}                                                          \
while(0)

#define NORMALIZE_CCM_DIVIDER(v)                           ( ((v) * 2) / 5 )
#endif

/* MX3 configuration */
#ifdef CONFIG_ARCH_MX3
#define DEFAULT_PRESCALER                                  61000000       /* (47360000 * 3 / 2) */
#define DIVIDE_CLK_RATE_BY_TWO                             1
#define CCM_COSR_OFFSET                                    0x1c
#define INIT_CKO_CLOCK()                                   \
do                                                         \
{                                                          \
        volatile unsigned long reg;                        \
                                                           \
        unsigned int mask = 0x00000fff;                    \
        unsigned int data = 0x00000208;                    \
                                                           \
        mxc_ccm_modify_reg(CCM_COSR_OFFSET, mask, data);   \
        reg = mxc_ccm_get_reg(CCM_COSR_OFFSET);            \
}                                                          \
while(0)

#define NORMALIZE_CCM_DIVIDER(v)                           (v)
#endif

/* MXC91331 configuration */
#ifdef CONFIG_ARCH_MXC91331
#define DEFAULT_PRESCALER                                  49887500
#define DIVIDE_CLK_RATE_BY_TWO                             0
#define NORMALIZE_CCM_DIVIDER(v)                           ( ((v) * 2) / 5 )
#endif

/* DEBUG configuration */
#ifndef NDEBUG
#define DPRINTK(fmt, args...)   printk("%s: " fmt, __FUNCTION__ , ## args)
#else                           /* NDEBUG */
#define DPRINTK(fmt, args...)
#endif                          /* NDEBUG */


/* AUDMUX definition */
//#define DUMP_REGS 1
#ifdef DUMP_REGS
#define ModifyRegister32(a,b,c)    (c = ( ( (c)&(~(a)) ) | (b) ))

#define DAM_VIRT_BASE_ADDR    IO_ADDRESS(AUDMUX_BASE_ADDR)

#ifndef _reg_DAM_PTCR1
#define    _reg_DAM_PTCR1   (*((volatile unsigned long *) \
                            (DAM_VIRT_BASE_ADDR + 0x00)))
#endif

#ifndef _reg_DAM_PDCR1
#define    _reg_DAM_PDCR1  (*((volatile unsigned long *) \
                           (DAM_VIRT_BASE_ADDR + 0x04)))
#endif

#ifndef _reg_DAM_PTCR2
#define    _reg_DAM_PTCR2   (*((volatile unsigned long *) \
                            (DAM_VIRT_BASE_ADDR + 0x08)))
#endif

#ifndef _reg_DAM_PDCR2
#define    _reg_DAM_PDCR2  (*((volatile unsigned long *) \
                           (DAM_VIRT_BASE_ADDR + 0x0C)))
#endif

#ifndef _reg_DAM_PTCR3
#define    _reg_DAM_PTCR3   (*((volatile unsigned long *) \
                            (DAM_VIRT_BASE_ADDR + 0x10)))
#endif

#ifndef _reg_DAM_PDCR3
#define    _reg_DAM_PDCR3  (*((volatile unsigned long *) \
                           (DAM_VIRT_BASE_ADDR + 0x14)))
#endif

#ifndef _reg_DAM_PTCR4
#define    _reg_DAM_PTCR4   (*((volatile unsigned long *) \
                            (DAM_VIRT_BASE_ADDR + 0x18)))
#endif

#ifndef _reg_DAM_PDCR4
#define    _reg_DAM_PDCR4  (*((volatile unsigned long *) \
                           (DAM_VIRT_BASE_ADDR + 0x1C)))
#endif

#ifndef _reg_DAM_PTCR5
#define    _reg_DAM_PTCR5   (*((volatile unsigned long *) \
                            (DAM_VIRT_BASE_ADDR + 0x20)))
#endif

#ifndef _reg_DAM_PDCR5
#define    _reg_DAM_PDCR5  (*((volatile unsigned long *) \
                           (DAM_VIRT_BASE_ADDR + 0x24)))
#endif

#ifndef _reg_DAM_PTCR6
#define    _reg_DAM_PTCR6   (*((volatile unsigned long *) \
                            (DAM_VIRT_BASE_ADDR + 0x28)))
#endif

#ifndef _reg_DAM_PDCR6
#define    _reg_DAM_PDCR6  (*((volatile unsigned long *) \
                           (DAM_VIRT_BASE_ADDR + 0x2C)))
#endif

#ifndef _reg_DAM_PTCR7
#define    _reg_DAM_PTCR7   (*((volatile unsigned long *) \
                            (DAM_VIRT_BASE_ADDR + 0x30)))
#endif

#ifndef _reg_DAM_PDCR7
#define    _reg_DAM_PDCR7  (*((volatile unsigned long *) \
                           (DAM_VIRT_BASE_ADDR + 0x34)))
#endif

#ifndef _reg_DAM_CNMCR
#define    _reg_DAM_CNMCR   (*((volatile unsigned long *) \
                            (DAM_VIRT_BASE_ADDR + 0x38)))
#endif

#ifndef _reg_DAM_PTCR
#define    _reg_DAM_PTCR(a)   (*((volatile unsigned long *) \
                              (DAM_VIRT_BASE_ADDR + a*8)))
#endif

#ifndef _reg_DAM_PDCR
#define    _reg_DAM_PDCR(a)   (*((volatile unsigned long *) \
                              (DAM_VIRT_BASE_ADDR + 4 + a*8)))
#endif
#endif
/*==================================================================================================
                                  GLOBAL STATIC DEFINITIONS
==================================================================================================*/

// The following SSI bit clock configuration settings are based upon
//  16 bits/word and 4 words/frame. These values are used to configure
//  the USB PLL and the SSI clock generator in order to produce the
//  required bit clock and frame sync signals when the SSI is in master
//  mode. These values are not used when the SSI is operating in slave
//  mode.
//
static const struct {
    unsigned int samplingRate_Hz;    /*< Sampling rate (Hz)           */
    struct {
        unsigned long target_ccm_ssi_clk_Hz;    /*< SSI master clock freq (Hz)   */
        unsigned char useDiv2;    /*< Enable/disable Div2 divider  */
        unsigned char usePSR;    /*< Enable/disable /8 divider    */
        unsigned char prescalerModulus;    /*< Prescaler modulus value      */
        unsigned long targetBitClock_Hz;    /*< SSI master clock target (Hz) */
    } clockParam;
} ssiClockSetup[] = {
    {
        8000,        /* Clock settings for 8 kHz sampling rate.   */
        {
            12288000,    /* Ideal input clock = 1.2288 MHz    */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                12,    /* Use /12 prescaler modulus         */
                512000,    /* SSI master clock target = 512 kHz */
        }
    }, {
        11025,        /* Clock settings for 11.025 kHz sampling rate. */
        {
            11289600,    /* Ideal input clock = 11.2896 MHz   */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                8,    /* Use /8 prescaler modulus          */
                705600,    /* SSI master clock target = 705.6 kHz */
        }
    }, {
        12000,        /* Clock settings for 12 kHz sampling rate.  */
        {
            12288000,    /* Ideal input clock = 12.288 MHz    */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                8,    /* Use /8 prescaler modulus          */
                768000    /* SSI master clock target = 768 kHz */
        }
    }, {
        16000,        /* Clock settings for 16 kHz sampling rate.  */
        {
            12288000,    /* Ideal input clock = 12.288 MHz    */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                6,    /* Use /6 prescaler modulus          */
                1024000,    /* SSI master clock target = 1.024 MHz */
        }
    }, {
        22050,        /* Clock settings for 22.05 kHz sampling rate. */
        {
            11289600,    /* Ideal input clock = 11.2896 MHz   */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                4,    /* Use /4 prescaler modulus          */
                1411200,    /* SSI master clock target = 1.4112 MHz */
        }
    }, {
        24000,        /* Clock settings for 24 kHz sampling rate.  */
        {
            12288000,    /* Ideal input clock = 12.288 MHz    */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                4,    /* Use /4 prescaler modulus          */
                1536000    /* SSI master clock target = 1.536 MHz */
        }
    }, {
        32000,        /* Clock settings for 32 kHz sampling rate.  */
        {
            12288000,    /* Ideal input clock = 12.288 MHz    */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                3,    /* Use /3 prescaler modulus          */
                2048000,    /* SSI master clock target = 2.048 MHz */
        }
    }, {
        44100,        /* Clock settings for 44.1 kHz sampling rate. */
        {
            11289600,    /* Ideal input clock = 11.2896 MHz   */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                2,    /* Use /2 prescaler modulus          */
                2822400    /* SSI master clock target = 2.8224 MHz */
        }
    }, {
        48000,        /* Clock settings for 48 kHz sampling rate.  */
        {
            12288000,    /* Ideal input clock = 12.288 MHz    */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                2,    /* Use /2 prescaler modulus          */
                3072000    /* SSI master clock target = 3.072 MHz */
        }
    }, {
        64000,        /* Clock settings for 64 kHz sampling rate.  */
        {
            12288000,    /* Ideal input clock = 12.288 MHz    */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                3,    /* Use /3 prescaler modulus          */
                4096000    /* SSI master clock target = 4.096 MHz */
        }
    }, {
        96000,        /* Clock settings for 96 kHz sampling rate.  */
        {
            12288000,    /* Ideal input clock = 12.288 MHz    */
                false,    /* Disable Div2 divider              */
                false,    /* Disable /8 divider                */
                2,    /* Use /2 prescaler modulus          */
                6144000    /* SSI master clock target = 6.144 MHz */
        }
},};

/*! Define the size of the SSI master clock setup table. @see ssiClockSetup */
static const unsigned int nSSIClockSetup = sizeof(ssiClockSetup) /
                                           sizeof(ssiClockSetup[0]);

static const unsigned int REG_FULLMASK = 0xffffff;

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

static struct class *ssi_class;    /* added on 05/01/06 Bunloeur Sean */

/*==================================================================================================
                                     LOCAL VARIABLES
==================================================================================================*/
/* buffers used to transfer data from kernel space */
static char ssi1_buf[MAX_CHUNK_SIZE];
static char ssi2_buf[MAX_CHUNK_SIZE];

/* ! SSI major */
static int major_ssi;

spinlock_t ssi_lock = SPIN_LOCK_UNLOCKED;
int num_zero = 0;
#ifdef  CONFIG_MACH_MXC27530EVB
int     mxc27530evb_ecn = 1;
#else
int     mxc27530evb_ecn = 0;
#endif

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
#ifdef DUMP_REGS
void pmic_audio_dump_registers(void);
void dump_AUDMUX_reg(void);
void dump_SSI_registers(ssi_mod module);
#endif
/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

struct ssi_priv_data
{
    int ssi;
    int size;
    char *buf;
    struct wave_config wavconf;
};

static struct ssi_priv_data ssi1_priv =
{
        0,
        0,
        0
};

static struct ssi_priv_data ssi2_priv =
{
        0,
        0,
        0
};

volatile unsigned long getreg_value(unsigned int offset, unsigned int ssi)
{
        volatile unsigned long reg = 0;
    unsigned int base_addr = 0;
    base_addr = (ssi == SSI1) ? IO_ADDRESS(SSI1_BASE_ADDR) :
    IO_ADDRESS(SSI2_BASE_ADDR);
    reg = __raw_readl(base_addr + offset);
    return reg;
}

#ifdef DUMP_REGS
/*================================================================================================*/
/*===== pmic_audio_dump_registers =====*/
/**
 * @brief Provide a hexadecimal dump of all PMIC audio registers (DEBUG only)
 *
 * This function is intended strictly for debugging purposes only and will
 * print the current values of the following PMIC registers:
 *
 * - AUD_CODEC
 * - ST_DAC
 * - AUDIO_RX_0
 * - AUDIO_RX_1
 * - AUDIO_TX
 * - AUDIO_SSI_NW
 *
 * The register fields will not be decoded.
 *
 * Note that we don't dump any of the arbitration bits because we cannot
 * access the true arbitration bit settings when reading the registers
 * from the secondary SPI bus.
 *
 * Also note that we must not call this function with interrupts disabled,
 * for example, while holding a spinlock, because calls to pmic_read_reg()
 * eventually end up in the SPI driver which will want to perform a
 * schedule() operation. If schedule() is called with interrupts disabled,
 * then you will see messages like the following:
 *
 * BUG: scheduling while atomic: ...
 *
 */
/*================================================================================================*/
void pmic_audio_dump_registers(void)
{
    unsigned int reg_value = 0;

    /* Dump the AUD_CODEC (Voice CODEC) register. */
    if (pmic_read_reg(REG_AUDIO_CODEC, &reg_value, REG_FULLMASK)
        == PMIC_SUCCESS) {
        DPRINTK("Audio Codec = 0x%x\n", reg_value);
    } else {
        DPRINTK("Failed to read audio codec\n");
    }

    /* Dump the ST DAC (Stereo DAC) register. */
    if (pmic_read_reg
        (REG_AUDIO_STEREO_DAC, &reg_value,
         REG_FULLMASK) == PMIC_SUCCESS) {
        DPRINTK("Stereo DAC = 0x%x\n", reg_value);
    } else {
        DPRINTK("Failed to read Stereo DAC\n");
    }

    /* Dump the SSI NW register. */
    if (pmic_read_reg
        (REG_AUDIO_SSI_NETWORK, &reg_value,
         REG_FULLMASK) == PMIC_SUCCESS) {
        DPRINTK("SSI Network = 0x%x\n", reg_value);
    } else {
        DPRINTK("Failed to read SSI network\n");
    }

    /* Dump the Audio RX 0 register. */
    if (pmic_read_reg(REG_AUDIO_RX_0, &reg_value, REG_FULLMASK)
        == PMIC_SUCCESS) {
        DPRINTK("Audio RX 0 = 0x%x\n", reg_value);
    } else {
        DPRINTK("Failed to read audio RX 0\n");
    }

    /* Dump the Audio RX 1 register. */
    if (pmic_read_reg(REG_AUDIO_RX_1, &reg_value, REG_FULLMASK)
        == PMIC_SUCCESS) {
        DPRINTK("Audio RX 1 = 0x%x\n", reg_value);
    } else {
        DPRINTK("Failed to read audio RX 1\n");
    }
    /* Dump the Audio TX register. */
    if (pmic_read_reg(REG_AUDIO_TX, &reg_value, REG_FULLMASK) ==
        PMIC_SUCCESS) {
        DPRINTK("Audio Tx = 0x%x\n", reg_value);
    } else {
        DPRINTK("Failed to read audio TX\n");
    }

}
/*================================================================================================*/
/*===== dump_AUDMUX_reg =====*/
/**
@brief  This function dumps the content of the registers of the AUDMUX module.

@param  nothing

@return Nothing
*/
/*================================================================================================*/
void dump_AUDMUX_reg(void)
{
    DPRINTK("Registers AUDMUX:\n");
    DPRINTK("_reg_DAM_PTCR1 = 0x%x\n", (int)_reg_DAM_PTCR1);
    DPRINTK("_reg_DAM_PDCR1 = 0x%x\n", (int)_reg_DAM_PDCR1);

    DPRINTK("_reg_DAM_PTCR2 = 0x%x\n", (int)_reg_DAM_PTCR2);
    DPRINTK("_reg_DAM_PDCR2 = 0x%x\n", (int)_reg_DAM_PDCR2);

    DPRINTK("_reg_DAM_PTCR3 = 0x%x\n", (int)_reg_DAM_PTCR3);
    DPRINTK("_reg_DAM_PDCR3 = 0x%x\n", (int)_reg_DAM_PDCR3);

    DPRINTK("_reg_DAM_PTCR4 = 0x%x\n", (int)_reg_DAM_PTCR4);
    DPRINTK("_reg_DAM_PDCR4 = 0x%x\n", (int)_reg_DAM_PDCR4);

    DPRINTK("_reg_DAM_PTCR5 = 0x%x\n", (int)_reg_DAM_PTCR5);
    DPRINTK("_reg_DAM_PDCR5 = 0x%x\n", (int)_reg_DAM_PDCR5);

    DPRINTK("_reg_DAM_PTCR6 = 0x%x\n", (int)_reg_DAM_PTCR6);
    DPRINTK("_reg_DAM_PDCR6 = 0x%x\n", (int)_reg_DAM_PDCR6);

    DPRINTK("_reg_DAM_PTCR7 = 0x%x\n", (int)_reg_DAM_PTCR7);
    DPRINTK("_reg_DAM_PDCR7 = 0x%x\n", (int)_reg_DAM_PDCR7);

    DPRINTK("_reg_DAM_CNMCR = 0x%x\n", (int)_reg_DAM_CNMCR);

}
/*================================================================================================*/
/*===== dump_SSI_registers =====*/
/**
@brief  This function dumps the content of the registers of the SSI module.

@param  module the module number

@return Nothing
*/
/*================================================================================================*/
void dump_SSI_registers(ssi_mod module)
{
        printk("- SCR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SCR, module));
        printk("- SISR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SISR, module));
        printk("- SIER = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SIER, module));
        printk("- STCR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1STCR, module));
        printk("- SRCR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SRCR, module));
        printk("- STCCR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1STCCR, module));
        printk("- SRCCR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SRCCR, module));
        printk("- SFCSR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SFCSR, module));
        printk("- STR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1STR, module));
        printk("- SOR = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SOR, module));
        printk("- SACNT = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SACNT, module));
        printk("- SACADD = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SACADD, module));
        printk("- SACDAT = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SACDAT, module));
        printk("- SATAG = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SATAG, module));
        printk("- STMSK = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1STMSK, module));
        printk("- SRMSK = 0x%8X \n", (unsigned int)getreg_value(MXC_SSI1SRMSK, module));
}
#endif
/*================================================================================================*/
/*=====mxc_config_pll====*/
/*!
 * This function configures the USB PLL to act as the clock source for the
 * SSI's internal clock generator.
 *
 * For the sake of simplicity, we have fixed both the bits/word and words/frame
 * values at 16 and 4, respectively. The SSI can be configured in many other
 * operating modes but fixing these two parameters greatly simplifies the
 * configuration of the SSI and the PMIC.
 *
 * @param        ssi_index [in] the SSI module (SSI1 or SSI2) being configured
 * @param        ssiClockSettingIndex [in] the index into ssiClockSetup[]
 * @param        samplingRate_Hz [in] sampling rate of the audio stream
 *
 */
 /*================================================================================================*/
static void mxc_config_pll(const int ssi_index,
               const int ssiClockSettingIndex,
               const unsigned int samplingRate_Hz)
{
    static const int bitsPerWord = 16;
    static const int wordsPerFrame = 4;

    unsigned long ssiTargetBitClock_Hz = bitsPerWord * wordsPerFrame * samplingRate_Hz;

    unsigned long usbPLLfreq_Hz = 0;
    unsigned long usbPLLDivider = 0;
    unsigned char usbPLLDividerFraction = 0;
    enum mxc_clocks pllClockSelect;
    unsigned long ssiActualBitClock_Hz;

    /*! Identify which SSI clock we want to activate. */
    if (ssi_index == SSI1) {
        pllClockSelect = SSI1_BAUD;
    } else if (ssi_index == SSI2) {
        pllClockSelect = SSI2_BAUD;
    } else {
        /*! Invalid SSI index. */
        DPRINTK("!!! ERROR: attempting to use invalid SSI%d !!!\n",
            ssi_index + 1);
        return;
    }

    /*! Configure and enable the SSI internal clock source using
     *  the USB PLL as the master clock signal.
     *
     *  To calculate the closest USB PLL divider value:
     *
     *  1. Divide the USB PLL frequency by the desired SSI clock input
     *     frequency (scaling by 10 to include one decimal place of
     *     precision).
     *  2. If the last digit is 0, 1, or 2, then round down to zero.
     *     For example, 162 would become 160.
     *  3. If the last digit is 3, 4, 5, 6, or 7, then round it to 5
     *     (since the USB PLL divider only supports fractional division
     *     by 0.5). For example, 166 would become 165.
     *  4. If the last digit is 8 or 9, then round up to the next
     *     multiple of 10. For example, 168 would become 170.
     *  5. Finally, convert the PLL divider to the correct register
     *     value.
     */
    usbPLLfreq_Hz = mxc_pll_clock(USBPLL);
    usbPLLDivider = (usbPLLfreq_Hz * 10) /
        ssiClockSetup[ssiClockSettingIndex].clockParam.
        target_ccm_ssi_clk_Hz;
    usbPLLDividerFraction = (unsigned char)(usbPLLDivider % 10);
    usbPLLDivider = usbPLLDivider / 10 * 2;
    if (usbPLLDividerFraction > 7) {
        /*! Add 1.0 to the PLL divider. Note that the first bit is reserved
         *  for specifying a 0.5 fractional divider so we actually need to
         *  increment the second bit in the value of usbPLLDivider.
         */
        usbPLLDivider += 2;
    } else if (usbPLLDividerFraction > 2) {
        /*! Add 0.5 to the PLL divider. Note that the first bit is reserved
         *  for specifying a 0.5 fractional divider so we just add one to set
         *  the bit.
         */
        usbPLLDivider++;
    }

    /*! Configure the USB PLL to be the SSI internal clock source. */
    mxc_set_clocks_pll(pllClockSelect, USBPLL);
    mxc_set_clocks_div(pllClockSelect, usbPLLDivider);

    /*! Enable the PLL to start generating an SSI bit clock. */
    mxc_clks_enable(pllClockSelect);

    DPRINTK("INFO: USB PLL frequency = %lu Hz\n", usbPLLfreq_Hz);
    DPRINTK("INFO: SSI%d PLL divider  = %ld\n", ssi_index + 1,
        usbPLLDivider);
    DPRINTK("INFO: SSI%d ccm_ssi_clk  = %lu Hz\n", ssi_index + 1,
        mxc_get_clocks(pllClockSelect));

    DPRINTK("INFO: SSI%d Prescaler Modulus = %d\n", ssi_index + 1,
        ssiClockSetup[ssiClockSettingIndex].clockParam.
        prescalerModulus);

    ssiActualBitClock_Hz = mxc_get_clocks(pllClockSelect) /
        (ssiClockSetup[ssiClockSettingIndex].clockParam.useDiv2 ? 2 : 1) /
        (ssiClockSetup[ssiClockSettingIndex].clockParam.usePSR ? 8 : 1) /
        ssiClockSetup[ssiClockSettingIndex].clockParam.prescalerModulus / 2;

    DPRINTK("INFO: Bit clock actual = %lu Hz\n", ssiActualBitClock_Hz);
    DPRINTK("INFO: Bit clock target = %lu Hz\n", ssiTargetBitClock_Hz);
    DPRINTK("INFO: Bit clock error  = %ld Hz\n",
        (long)(ssiActualBitClock_Hz - ssiTargetBitClock_Hz));
}
/*================================================================================================*/
/*=====configure_ssi_tx====*/
/*!
 * This function configures the SSI bus (including the correct clock settings
 * for either master or slave mode).
 *
 * @param        drv_inst [in] info about the current transfer
 *
 */
 /*================================================================================================*/
void configure_ssi_tx(const struct wave_config * const waveconf)
{
    int mod = waveconf->ssi;
    int i;

    /*! Disable the SSI transmitter and receiver. */
    ssi_transmit_enable(mod, false);
    ssi_receive_enable(mod, false);

    /*! Next disable the SSI so that we can reconfigure the items that
     *  can only be changed when the SSI is disabled (see the SSI chapter
     *  of the MXC Reference Manual for complete details).
     *
     *  Note that this will also flush all data from both the transmit and
     *  receive FIFOs.
     */
    ssi_enable(mod, false);

    /*! Disable all SSI interrupts (specifically the DMA interrupts). */
    ssi_interrupt_disable(mod, 0xFFFFFFFF);

    /*! Disable the SSI internal clock source. */
    mxc_clks_disable((mod == SSI1) ? SSI1_BAUD : SSI2_BAUD);

    /*! Configure for synchronous 4-wire network mode. */
    ssi_system_clock(mod, false);
    ssi_i2s_mode(mod, i2s_normal);
    ssi_network_mode(mod, true);
    ssi_two_channel_mode(mod, true);
    ssi_synchronous_mode(mod, true);
    //ssi_two_channel_mode(mod, false);
    ssi_clock_idle_state(mod, false);

    ssi_tx_fifo_enable(mod, ssi_fifo_0, true);
    ssi_tx_fifo_enable(mod, ssi_fifo_1, true);
    ssi_rx_fifo_enable(mod, ssi_fifo_0, false);

    /*! Configure full and empty FIFO watermark levels. */
    ssi_rx_fifo_full_watermark(mod, ssi_fifo_0, RX_WATERMARK);
    ssi_rx_fifo_full_watermark(mod, ssi_fifo_1, RX_WATERMARK);
    ssi_tx_fifo_empty_watermark(mod, ssi_fifo_0, TX_WATERMARK);
    ssi_tx_fifo_empty_watermark(mod, ssi_fifo_1, TX_WATERMARK);

    /*! SSI transmitter configuration for audio playback. This is also the
     *  receiver configuration for audio recording since we are using
     *  synchronous mode. Therefore, there is no need to separately specify
     *  any SSI receiver settings.
     */
    ssi_tx_early_frame_sync(mod, ssi_frame_sync_one_bit_before);
    ssi_tx_frame_sync_length(mod, ssi_frame_sync_one_bit);
    ssi_tx_frame_sync_active(mod, ssi_frame_sync_active_high);
    ssi_tx_clock_polarity(mod, ssi_clock_on_rising_edge);
    ssi_tx_shift_direction(mod, ssi_msb_first);
    ssi_tx_bit0(mod, true);
    ssi_tx_word_length(mod, ssi_16_bits);
    ssi_tx_frame_rate(mod, (unsigned char)4);

    /*! Configure the SSI for either master or slave clocking mode. */
    if (waveconf->master_slave == BUS_SLAVE_MODE) {
        DPRINTK("INFO: Configuring SSI%d as bus clock master\n",
            mod + 1);

        /*! Search the SSI clock configuration table for the appropriate
         *  clock settings to be used based upon the desired sampling rate.
         */
        for (i = 0; i < nSSIClockSetup; i++) {
            if (waveconf->sample_rate ==
                (unsigned long) ssiClockSetup[i].samplingRate_Hz) {
                DPRINTK
                    ("INFO: Using sampling rate of %d Hz (table index %d) "
                     "for SSI%d\n", (int) waveconf->sample_rate,
                     i, mod + 1);
                break;
            }
        }

        if (i >= nSSIClockSetup) {
            /*! An unsupported sampling rate was given. Just leave the SSI
             *  disabled.
             */
            DPRINTK
                ("INFO: Invalid sampling rate %d given for SSI%d\n",
                 (int) waveconf->sample_rate, mod + 1);
            return;
        }

        /*! Configure the SSI clock dividers to support the desired
         *  sampling rate.
         */
        ssi_tx_clock_divide_by_two(mod,
                       ssiClockSetup[i].clockParam.useDiv2);
        ssi_tx_clock_prescaler(mod,
                       ssiClockSetup[i].clockParam.usePSR);
        ssi_tx_prescaler_modulus(mod,
                     ssiClockSetup[i].clockParam.
                     prescalerModulus);

        /*! Configure the SSI to generate bit clock and frame sync
         *  using the internal clock source (SSI in master mode).
         */
        ssi_tx_frame_direction(mod, ssi_tx_rx_internally);
        ssi_tx_clock_direction(mod, ssi_tx_rx_internally);

        mxc_config_pll(mod, i, waveconf->sample_rate);
    } else {
        DPRINTK("INFO: Configuring SSI%d as bus clock slave\n",
            mod + 1);

        /*! Configure the SSI to use externally generated bitclock
         *  and frame sync signals (SSI in slave mode).
         */
        ssi_tx_frame_direction(mod, ssi_tx_rx_externally);
        ssi_tx_clock_direction(mod, ssi_tx_rx_externally);
        mxc_clks_enable((mod == SSI1) ? SSI1_BAUD : SSI2_BAUD);
    }
    ssi_interrupt_enable(mod, ssi_tx_fifo_0_empty);

    /*! We can reenable the SSI now and complete the configuration process
     *  by setting the Tx and Rx timeslot masks.
     *
     *  Note that the SSI has to be reenabled first before the Tx and Rx
     *  mask registers can be successfully updated.
     *
     *  Also note that we do not reenable the SSI transmitter and/or receiver
     *  until we actually have data to be sent/received (which occurs later
     *  when the audio application actually initiates an I/O operation).
     */
    ssi_enable(mod, true);
    if(waveconf->num_channels == 2)
    {
        ssi_tx_mask_time_slot(mod, 0xfffffffc);
        ssi_rx_mask_time_slot(mod, 0xfffffffc);
    }
    else
    {
        ssi_tx_mask_time_slot(mod, 0xfffffffe);
        ssi_rx_mask_time_slot(mod, 0xfffffffe);
    }
}
/*================================================================================================*/
/*=====normalize_speed_for_pmic====*/
/**
@brief  This function returns normalized sample rate speed for wavconf.

@param  wavconf  pointer to wave_config ctructure

@return This function returns normalized sample rate speed for wavconf.
*/
/*================================================================================================*/
inline int normalize_speed_for_pmic(struct wave_config *waveconf)
{
        if (waveconf->dac_codec == STEREO_DAC )
    {
        switch (waveconf->sample_rate)
            {
            case 8000:
                    return STDAC_RATE_8_KHZ;
            case 11025:
                    return STDAC_RATE_11_025_KHZ;
            case 12000:
                    return STDAC_RATE_12_KHZ;
            case 16000:
                    return STDAC_RATE_16_KHZ;
            case 22050:
                    return STDAC_RATE_22_050_KHZ;
            case 24000:
                    return STDAC_RATE_24_KHZ;
            case 32000:
                    return STDAC_RATE_32_KHZ;
            case 44100:
                    return STDAC_RATE_44_1_KHZ;
            case 48000:
                    return STDAC_RATE_48_KHZ;
            case 64000:
                    return STDAC_RATE_64_KHZ;
            case 96000:
                    return STDAC_RATE_96_KHZ;
            default:
                    return STDAC_RATE_16_KHZ;
            }
    }
    else
    {
        switch (waveconf->sample_rate)
        {
        case 8000:
            return VCODEC_RATE_8_KHZ;
        case 16000:
            return VCODEC_RATE_16_KHZ;
        default:
            return VCODEC_RATE_8_KHZ;
        }
    }
}

/*================================================================================================*/
/*=====halt_hardware====*/
/**
@brief  This function halts hardware, disables voice CODEC and stereo DAC.

@param  none

@return Nothing
*/
/*================================================================================================*/
void halt_hardware(struct wave_config *wavconf)
{
        /* disable codec and stereodac */
        pmic_audio_reset_all();
        DPRINTK("Number FIFO data level nullings  =  %d \n", num_zero);
        num_zero = 0;
        /* disable ssi (fifos, interrupts, ...) */
        ssi_transmit_enable(wavconf->ssi, false);
        ssi_tx_fifo_enable(wavconf->ssi, ssi_fifo_0, false);
        ssi_tx_fifo_enable(wavconf->ssi, ssi_fifo_1, false);

        ssi_receive_enable(wavconf->ssi, false);
        ssi_rx_fifo_enable(wavconf->ssi, ssi_fifo_0, false);
        ssi_rx_fifo_enable(wavconf->ssi, ssi_fifo_1, false);

        ssi_enable(wavconf->ssi, false);
}

#ifdef CONFIG_ARCH_MXC91331
/*================================================================================================*/
/*=====set_mxc91331_clocks====*/
/**
@brief  This function configures clock(for MXC91331).

@param  mask   mask
        data   data
        offset offset

@return Nothing
*/
/*================================================================================================*/
void set_mxc91331_clocks(unsigned int mask, unsigned int data, unsigned int offset)
{
        volatile unsigned long reg = 0;
        unsigned int base_addr = 0;
        unsigned long flags = 0;

        spin_lock_irqsave(&ssi_lock, flags);
        base_addr = IO_ADDRESS(CRM_MCU_BASE_ADDR);
        reg = readl(base_addr + offset);
        reg = (reg & (~mask)) | data;
        writel(reg, base_addr + offset);
        spin_unlock_irqrestore(&ssi_lock, flags);
}
#endif

/*================================================================================================*/
/*=====internal_init====*/
/**
@brief  This function initializes hardware.

@param  none

@return Nothing
*/
/*================================================================================================*/
void internal_init(void)
{
#if defined(CONFIG_ARCH_MXC91231) || defined(CONFIG_ARCH_MX3)

        /* assign USBPLL to be used by SSI1 */
        mxc_set_clocks_pll(SSI1_BAUD, USBPLL);

        /* set divider to 2 */
        mxc_set_clocks_div(SSI1_BAUD, 2);

        /* enable clock */
        mxc_clks_enable(SSI1_BAUD);

        /* assign USBPLL to be used by SSI2 */
        mxc_set_clocks_pll(SSI2_BAUD, USBPLL);

        /* set divider to 2 */
        mxc_set_clocks_div(SSI2_BAUD, 2);

        /* enable clock */
        mxc_clks_enable(SSI2_BAUD);

        /* Activate CKO clock in order to have a 26 mhz clock on MC13783 CLI signal */
        INIT_CKO_CLOCK();
#endif
#ifdef CONFIG_ARCH_MXC91331
        /*ModifyRegister(0x18000000, 0x00000000, _reg_MCU_MCR);*/
        /*ModifyRegister(0x0003FFFF, 0x00000C06, _reg_MCU_MPDR1);*/

        /*MCR register*/
        set_mxc91331_clocks(0x18000000, 0x00000000, 0x0);

        /*MPDR1 register*/
        set_mxc91331_clocks(0x0003FFFF, 0x00000C06, 0x8);
#endif
}
/*================================================================================================*/
/*=====configure_stereodac====*/
/**
@brief  This function configures stereo DAC.

@param  wavconf pointer to wave_config structure

@return Nothing
*/
/*================================================================================================*/
void configure_stereodac(PMIC_AUDIO_HANDLE hinst, struct wave_config *wavconf)
{
    int speed         = 0;
    int err           = 0;
    int output_level  = 50;
    int leveldb = (output_level * PMIC_OUTPUT_VOLUME_MAX) / OUTPUT_VOLUME_MAX;
    PMIC_AUDIO_STDAC_CLOCK_IN_FREQ freq;
#ifdef CONFIG_MXC_PMIC_SC55112
    freq = STDAC_CLI_33_6MHZ;
#else
    freq = STDAC_CLI_26MHZ;
#endif
    if ( (err = pmic_audio_output_enable_phantom_ground(hinst)) < 0)
            DPRINTK("Error in pmic_audio_output_enable_phantom_ground: err = %d\n", err);

    if ( (err = pmic_audio_antipop_enable(ANTI_POP_RAMP_SLOW)) < 0)
            DPRINTK("antipop error: %d", err);
#ifndef CONFIG_MXC_PMIC_SC55112
    if ( (err = pmic_audio_output_enable_mixer(hinst)) < 0)
            DPRINTK("Error in pmic_audio_output_enable_mixer: err = %d\n", err);

    if ( (err = pmic_audio_stdac_enable_mixer(hinst, USE_TS0_TS1, STDAC_NO_MIX, STDAC_MIX_OUT_0DB)) < 0)
            DPRINTK("Error in pmic_audio_stdac_enable_mixer: err = %d\n", err);
#endif
    if ( (err = pmic_audio_set_protocol(hinst, wavconf->dac_bus,
                                        NETWORK_MODE,wavconf->master_slave,
                                        USE_4_TIMESLOTS)) < 0)
            DPRINTK("Error in pmic_audio_set_protocol: err = %d\n", err);
    msleep(30);

    if ( (err = pmic_audio_stdac_set_rxtx_timeslot(hinst, USE_TS0_TS1)) < 0)
            DPRINTK("Error in pmic_audio_stdac_set_rxtx_timeslot: err = %d\n", err);

    speed = normalize_speed_for_pmic(wavconf);
    if ( (err = pmic_audio_stdac_set_clock(hinst, wavconf->pmic_clock_source,
                                           freq, speed, NO_INVERT)) < 0)
            DPRINTK("Error in pmic_audio_stdac_set_clock: err = %d\n", err);
    msleep(30);

    if ( (err = pmic_audio_stdac_set_config (hinst, STDAC_MASTER_CLOCK_OUTPUTS)) < 0)
        DPRINTK("Error in pmic_audio_stdac_set_config: err = %d\n", err);

    if (machine_is_mx31ads())
            pmic_audio_output_enable_mono_adder(hinst,STEREO_OPPOSITE_PHASE);

    if ( (err = pmic_audio_output_set_port(hinst, STEREO_HEADSET_LEFT | STEREO_HEADSET_RIGHT)) < 0)
                    DPRINTK("Error in pmic_audio_output_set_port 1. Returned code %d\n", err);


    if ( (err = pmic_audio_output_set_pgaGain(hinst, leveldb)) < 0)
            DPRINTK("Error in pmic_audio_output_set_pgaGain: err = %d\n", err);

    DPRINTK("(stdac) sample rate = %d\n", (int) wavconf->sample_rate);
    DPRINTK("        speed = %d\n", (int) speed);

}
/*================================================================================================*/
/*=====configure_codec====*/
/**
@brief  This function configures voice CODEC.

@param  wavconf pointer to wave_config structure

@return Nothing
*/
/*================================================================================================*/
void configure_codec(PMIC_AUDIO_HANDLE hinst, struct wave_config *wavconf)
{
    int speed          = 0;
    int err            = 0;
    int output_level   = 50;
    int leveldb = (output_level * PMIC_OUTPUT_VOLUME_MAX) / OUTPUT_VOLUME_MAX;
    PMIC_AUDIO_VCODEC_CLOCK_IN_FREQ freq;

    freq = VCODEC_CLI_33_6MHZ;

    if ( (err = pmic_audio_antipop_enable(ANTI_POP_RAMP_SLOW)) < 0)
            DPRINTK("Error in pmic_audio_antipop_enable: err = %d\n", err);

    if ( (err = pmic_audio_output_enable_phantom_ground(hinst)) < 0)
            DPRINTK("Error in pmic_audio_output_enable_phantom_ground: err = %d\n", err);

    if ( (err = pmic_audio_vcodec_set_rxtx_timeslot(hinst, USE_TS0)) < 0)
            DPRINTK("Error in pmic_audio_vcodec_set_rxtx_timeslot: err = %d\n", err);
#ifndef CONFIG_MXC_PMIC_SC55112
    if ( (err = pmic_audio_vcodec_enable_mixer(hinst, USE_TS0, VCODEC_NO_MIX, VCODEC_MIX_OUT_0DB)) < 0)
            DPRINTK("Error in pmic_audio_vcodec_enable_mixer: err = %d\n", err);
#endif
    if ( (err = pmic_audio_set_protocol(hinst, wavconf->dac_bus,
                                        NETWORK_MODE, wavconf->master_slave,
                                        USE_4_TIMESLOTS)) < 0)
            DPRINTK("Error in pmic_audio_set_protocol: err = %d\n", err);
    msleep (20);

    speed = normalize_speed_for_pmic(wavconf);
    if ( (err = pmic_audio_vcodec_set_clock(hinst, wavconf->pmic_clock_source,
                                            freq, speed, NO_INVERT)) < 0)
            DPRINTK("Error in pmic_audio_vcodec_set_clock: err = %d\n", err);
    msleep (20);

    if ( (err = pmic_audio_vcodec_set_config (hinst, VCODEC_MASTER_CLOCK_OUTPUTS)) < 0)
            DPRINTK("Error in pmic_audio_stdac_set_config: err = %d\n", err);

    if ( (err = pmic_audio_output_enable_mixer(hinst)) < 0)
            DPRINTK("Error in pmic_audio_output_enable_mixer: err = %d\n", err);
    
    if ( (err = pmic_audio_output_set_port(hinst, STEREO_HEADSET_LEFT | STEREO_HEADSET_RIGHT)) < 0)
            DPRINTK("Error in pmic_audio_output_set_port: err = %d\n", err);

    if ( (err = pmic_audio_output_set_pgaGain(hinst, leveldb)) < 0)
            DPRINTK("Error in pmic_audio_output_set_pgaGain: err = %d\n", err);

    DPRINTK("(codec) sample rate = %d\n", (int) wavconf->sample_rate);
    DPRINTK("        speed = %d\n", (int) speed);
}

/*================================================================================================*/
/*=====configure_dam====*/
/**
@brief  This function configures DAM.

@param  none

@return Nothing
*/
/*================================================================================================*/
void configure_dam(struct wave_config *wavconf)
{
    unsigned int source_port;
    unsigned int target_port;
    unsigned int change;
    dam_reset_register(port_1);
    dam_reset_register(port_2);
    dam_reset_register(port_4);
    dam_reset_register(port_5);
    dam_reset_register(port_6);
    dam_reset_register(port_7);

    if (wavconf->dac_bus == AUDIO_DATA_BUS_1)
    {

        target_port = port_4;
        if (wavconf->ssi == SSI1)
        {
            source_port = port_1;
            DPRINTK("Call configure_dam for AUDIO_DATA_BUS_1:\n");
        }
        else
        {
            source_port = port_2;
            DPRINTK("Call configure_dam for AUDIO_DATA_BUS_1:\n");
        }
    }
    else /*wavconf->dac_bus == AUDIO_DATA_BUS_2*/
    {
        target_port = port_5;
        if (wavconf->ssi == SSI1)
        {
            source_port = port_1;
            DPRINTK("Call configure_dam for AUDIO_DATA_BUS_2:\n");
        }
        else
        {
            source_port = port_2;
            DPRINTK("Call configure_dam for AUDIO_DATA_BUS_2:\n");
        }

    }

    /*When PMIC is master ( PMIC generates the FS  and BClk clocks ) clock direction through AUDMUX is next:     target_port --> source_port */
    /*When PMIC is slave  (  SSI source of clocks )  clock direction through AUDMUX is next:    source_port --> target_port */

    if ( wavconf->master_slave == BUS_SLAVE_MODE )
    {
            change = source_port;
            source_port = target_port;
            target_port = change;
    }

    dam_reset_register(source_port);
    dam_reset_register(target_port);
    dam_select_mode(source_port, normal_mode);
    dam_select_mode(target_port, normal_mode);
    dam_set_synchronous(source_port, true);
    dam_set_synchronous(target_port, true);
    dam_select_RxD_source(target_port, source_port);
    dam_select_RxD_source(source_port, target_port);

    dam_select_TxFS_direction(source_port, signal_out);
    dam_select_TxFS_source(source_port, false, target_port);

    dam_select_TxClk_direction(source_port, signal_out);
    dam_select_TxClk_source(source_port, false, target_port);

    dam_select_RxFS_direction(source_port, signal_out);
    dam_select_RxFS_source(source_port, false, target_port);

    dam_select_RxClk_direction(source_port, signal_out);
    dam_select_RxClk_source(source_port, false, target_port);

    dam_select_TxClk_direction(target_port, signal_in);
    dam_select_TxFS_direction(target_port, signal_in);
    dam_select_RxFS_direction(target_port, signal_in);
    dam_select_RxClk_direction(target_port, signal_in);

    writel(0x0031010, IO_ADDRESS(AUDMUX_BASE_ADDR) + 0x38);
}
/*=====wirte_fifo====*/
/**
@brief  This function writes performs transmiting of input data

@param  priv pointer to struct ssi_priv_data

@return none
*/
/*================================================================================================*/
int write_fifo(struct ssi_priv_data *priv)
{
        unsigned long ssi_sfcsr_addr;
        unsigned long reg;

        int     i,
                left,
                right;
        int     data_size,
                counter = 0,
                rv = priv->size;
        i = 0;
        data_size = priv->size;
        ssi_sfcsr_addr = (priv->ssi == SSI1) ? IO_ADDRESS(SSI1_BASE_ADDR) : IO_ADDRESS(SSI2_BASE_ADDR);
        reg = readl(ssi_sfcsr_addr + MXC_SSISFCSR);
        ssi_transmit_enable(priv->ssi, false);
        while (((reg & 0xF00) >> 8) != 8)
        {
                left = (unsigned int) (((priv->buf[i + 1] << 8) & 0xFF00) |
                                        (priv->buf[i] & 0x00FF));
                right = (unsigned int) (((priv->buf[i + 3] << 8) & 0xFF00) |
                                        (priv->buf[i + 2] & 0x00FF));
                ssi_set_data(priv->ssi, ssi_fifo_0, left);
                ssi_set_data(priv->ssi, ssi_fifo_1, right);
                data_size -= 4;
                i += 4;
                reg = readl(ssi_sfcsr_addr + MXC_SSISFCSR);
        }
        ssi_transmit_enable(priv->ssi, true);
        //DPRINTK("ssi: transmit enable\n");
        while (data_size >= 4)
        {
                //DPRINTK(" data_size: %d\n",data_size);
                reg = readl(ssi_sfcsr_addr + MXC_SSISFCSR);

                if (((reg & 0xF00) >> 8) == 0 )
                        num_zero++;
                //DPRINTK("ssi:readl. reg: %lX \n", reg);
                
                ++ counter;

                if ( ( ((reg & 0xF00) >> 8) <= 7 ) && ( ((reg & 0xF000000) >> 24) <= 7))
                {
                        left = (unsigned int) (((priv->buf[i + 1] << 8) & 0xFF00) |
                                               (priv->buf[i] & 0x00FF));
                        right = (unsigned int) (((priv->buf[i + 3] << 8) & 0xFF00) |
                                                (priv->buf[i + 2] & 0x00FF));
                        //DPRINTK("left:%d, right:%d\n", left, right);
                        ssi_set_data(priv->ssi, ssi_fifo_0, left);
                        //DPRINTK("ssi:ssi_set_data fifo_0\n");
                        ssi_set_data(priv->ssi, ssi_fifo_1, right);
                        //DPRINTK("ssi:ssi_set_data fifo_1\n");
                        data_size -= 4;
                        i += 4;
                        counter = 0;
                }

                if(counter > 10000)
                {
                        DPRINTK("============================<><><><>++++++++++++++++++++\n");
                        rv = -EAGAIN;
                        break;
                }
        }

        ssi_transmit_enable(priv->ssi, false);
        //DPRINTK("ssi:disable transmit: data_size = %d\n", data_size);
        //DPRINTK("!!!!!!!!!!!!!!!!!!!!!!!!!! OKOKOKOK rv %d\n", rv);
        return rv;
}

/*================================================================================================*/
/*=====ssi1_interrupt_handler====*/
/**
@brief  Interrupt service routine registered to handle the individual general purpose interrupts or
        muxed. Interrupts are cleared in ISR before scheduling tasklet

@param  irq the interrupt number
        dev_id  driver private data
        regs    holds a snapshot of the processor's context before the processor entered the
                 interrupt code
@return The function returns \b IRQ_RETVAL(1) if interrupt was handled, returns \b IRQ_RETVAL(0)
        if the interrupt was not handled.  \b IRQ_RETVAL is defined in \b include/linux/interrupt.h.
*/
/*================================================================================================*/
static irqreturn_t ssi1_interrupt_handler(int irq, void *dev_id, struct pt_regs *regs)
{
        int     handled = 0;

        return IRQ_RETVAL(handled);
}

/*================================================================================================*/
/*=====ssi2_interrupt_handler====*/
/**
@brief  Interrupt service routine registered to handle the individual general purpose interrupts or
        muxed. Interrupts are cleared in ISR before scheduling tasklet

@param  irq the interrupt number
        dev_id  driver private data
        regs    holds a snapshot of the processor's context before the processor entered the
                interrupt code
@return The function returns \b IRQ_RETVAL(1) if interrupt was handled, returns \b IRQ_RETVAL(0)
        if the interrupt was not handled.  \b IRQ_RETVAL is defined in \b include/linux/interrupt.h.
*/
/*================================================================================================*/
static irqreturn_t ssi2_interrupt_handler(int irq, void *dev_id, struct pt_regs *regs)
{
        int     handled = 0;

        return IRQ_RETVAL(handled);
}

/*================================================================================================*/
/*=====ssi_ioctl====*/
/**
@brief  This function implements IOCTL controls on a SSI device.

@param  param inode pointer on the node
        file  pointer on the file
        cmd   the command
        arg   the parameter

@return This function returns 0 if successful.
*/
/*================================================================================================*/
static int ssi_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
        struct wave_config *wavconf;
        struct ssi_priv_data *priv;
        int    res = 0;
        PMIC_AUDIO_HANDLE hinst;
        PMIC_STATUS err;

        priv = (struct ssi_priv_data *) file->private_data;
        wavconf = (struct wave_conf *) &priv->wavconf;

        memset(wavconf, 0, sizeof(struct wave_config));
        res = copy_from_user((void *) wavconf, (void *) arg, sizeof(struct wave_config));
        if (res)
        {
                return -EFAULT;
        }

        switch (cmd)
        {
        case IOCTL_SSI_CONFIGURE_SSI:
                DPRINTK("IOCTL_SSI_CONFIGURE_SSI called\n");
                DPRINTK("wav configuration for SSI%d:\n", (int) (wavconf->ssi + 1));
                DPRINTK("sample_rate: %d\n", (int) wavconf->sample_rate);
                DPRINTK("bits per sample: %d\n", (int) wavconf->bits_per_sample);
                DPRINTK("number of channels: %d\n", (int) wavconf->num_channels);

                configure_ssi_tx(wavconf);
                #ifdef DUMP_REGS
                DPRINTK("DUMP SSI1 registers. \n");
                dump_SSI_registers(SSI1);
                DPRINTK("DUMP SSI2 registers. \n");
                dump_SSI_registers(SSI2);
                #endif
                printk("CONFIGURED SSI...\n");

                break;

        case IOCTL_SSI_GET_CONFIGURATION:
                DPRINTK("IOCTL_SSI_GET_CONFIGURATION called\n");
                res = copy_to_user((void *) arg, (void *) wavconf, sizeof(struct wave_config));
                if (res)
                {
                        return -EFAULT;
                }
                printk("GET CONFIGURETION...\n");
                break;

        case IOCTL_SSI_CONFIGURE_AUDMUX:
                DPRINTK("IOCTL_SSI_CONFIGURE_AUDMUX called\n");

                if (wavconf->ssi == SSI1)
                {
                    configure_dam (wavconf);
                    printk("CONFIGURED AUSMUX for SSI1...\n");
                    #ifdef DUMP_REGS
                    dump_AUDMUX_reg();
                    #endif
                }
                else
                {
                    configure_dam (wavconf);
                    printk("CONFIGURED AUSMUX for SSI2...\n");
                    #ifdef DUMP_REGS
                    dump_AUDMUX_reg();
                    #endif
                }
                break;

        case IOCTL_SSI_CONFIGURE_PMIC:
                if ((err = pmic_audio_reset_all()) != PMIC_SUCCESS)
                    DPRINTK("Error pmic_audio_reset_all called!!!!\n");
                else
                    DPRINTK("Rest PMIC!!!!\n");
                msleep(30);
                DPRINTK("IOCTL_SSI_CONFIGURE_PMIC called\n");
                if (wavconf->dac_codec == STEREO_DAC)
                {
                    if((err = pmic_audio_open(&hinst, STEREO_DAC)) < 0)
                            DPRINTK("Error opening STEREO_DAC. Returned code %d\n", err);

                    pmic_audio_reset(hinst);
                }
                else
                {
                    if((err = pmic_audio_open(&hinst, VOICE_CODEC)) < 0)
                            DPRINTK("Error opening VOICE_CODEC. Returned code %d\n", err);

                    pmic_audio_reset(hinst);
                }

                if (wavconf->dac_codec == STEREO_DAC)
                {
                    pmic_audio_disable(hinst);
                    configure_stereodac(hinst, wavconf);

                        //msleep (30);
                        //pmic_write_reg (REG_AUDIO_RX_0, 0x63ee07, REG_FULLMASK);
                        //msleep (30);
                        //pmic_write_reg (REG_AUDIO_RX_1, 0xd273, REG_FULLMASK);
                        //msleep (30);
                        //pmic_write_reg (REG_AUDIO_SSI_NETWORK, 0x12060, REG_FULLMASK);
                        //msleep (30);
                        //pmic_write_reg (REG_AUDIO_STEREO_DAC, 0xe1a21, REG_FULLMASK);

                    pmic_audio_enable(hinst);
                    #ifdef DUMP_REGS
                    pmic_audio_dump_registers();
                    #endif
                }
                else
                {
                    if ((wavconf->sample_rate != 16000) && (wavconf->sample_rate != 8000))
                    {
                            DPRINTK("sample rate not supported by CODEC\n");
                                return -1;
                    }

                    pmic_audio_disable(hinst);
                    configure_codec(hinst, wavconf);
                    pmic_audio_enable(hinst);
                    #ifdef DUMP_REGS
                    pmic_audio_dump_registers();
                    #endif
                }

                DPRINTK("CONFIGURED PMIC...\n");
                break;

        default:
                DPRINTK("Bad IOCTL called\n");
                return -EINVAL;
        }

        return 0;
}

/*================================================================================================*/
/*====ssi_open====*/
/**
@brief  This function implements the open method on a SSI device.

@param  param inode pointer on the node
        file  pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
static int ssi_open(struct inode *inode, struct file *file)
{
        unsigned int minor = MINOR(inode->i_rdev);
        struct ssi_priv_data *priv;

        DPRINTK("minor = %d\n", minor);

        priv = (minor == 0) ? &ssi1_priv : &ssi2_priv;
        priv->buf = (minor == 0) ? &ssi1_buf[0] : &ssi2_buf[0];

        priv->ssi = minor;
        priv->size = 0;

        file->private_data = (void *) priv;

        return 0;
}

/*================================================================================================*/
/*=====ssi_free====*/
/**
@brief  This function implements the release method on a SSI device.

@param  param inode pointer on the node
        file  pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
static int ssi_free(struct inode *inode, struct file *file)
{
        /* unsigned int minor = MINOR(inode->i_rdev); */
        struct ssi_priv_data *priv;

        priv = (struct ssi_priv_data *) file->private_data;
        halt_hardware(&priv->wavconf);
        file->private_data = NULL;

        return 0;
}

/*================================================================================================*/
/*=====ssi_write====*/
/**
@brief  This function implements the write method on a SSI device.

@param  param inode pointer on the node
        file  pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
static ssize_t ssi_write(struct file *file, const char *buf, size_t bytes, loff_t * off)
{
        struct ssi_priv_data *priv;
        unsigned int minor;
        int     res = 0;

        minor = MINOR(file->f_dentry->d_inode->i_rdev);
        priv = (struct ssi_priv_data *) file->private_data;

        res = copy_from_user(priv->buf, (void *) buf, bytes);
        if (res)
        {
                printk("write: fault error when copying user data\n");
                return -EFAULT;
        }

        priv->size = bytes;
        return write_fifo(priv);
}

/*================================================================================================*/
/*=====ssi_write====*/
/**
@brief  This function implements the read method on a SSI device.

@param  param inode pointer on the node
        file  pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
static ssize_t ssi_read(struct file *file, char *buf, size_t bytes, loff_t * off)
{
        struct ssi_priv_data *priv;
        unsigned int minor;

        minor = MINOR(file->f_dentry->d_inode->i_rdev);
        priv = (struct ssi_priv_data *) file->private_data;

        DPRINTK("minor = %d\n", minor);

        return 0;
}

/* ! This structure defines file operations for a SSI device. */
static struct file_operations ssi_fops =
{
        /* ! the owner */
        .owner = THIS_MODULE,

        .read = ssi_read,

        .write = ssi_write,

        /* ! the ioctl operations */
        .ioctl = ssi_ioctl,

        /* ! the open operation */
        .open = ssi_open,

        /* ! the release operation */
        .release = ssi_free,
};

/*================================================================================================*/
/*=====testmod_ssi_init====*/
/**
@brief   This function implements the init function of the SSI device. This function is called when the
         module is loaded.

@param

@return This function returns 0.
*/
/*================================================================================================*/
static int __init testmod_ssi_init(void)
{
        int     res = 0;

        printk(KERN_DEBUG "ssi : ssi_init(void) \n");

        major_ssi = register_chrdev(0, "ssi", &ssi_fops);
        if (major_ssi < 0)
        {
                printk(KERN_WARNING "Unable to get a major for ssi");
                return major_ssi;
        }

        //devfs_mk_dir("mxc_ssi");


        ssi_class = class_create(THIS_MODULE, "mxc_ssi");
        if (IS_ERR(ssi_class))
        {
                goto err_out;
        }

        if (IS_ERR(class_device_create(ssi_class, NULL, MKDEV(major_ssi, 0), NULL, "mxc_ssi%u", 1)))
        {
                goto err_out;
        }

        if (IS_ERR(class_device_create(ssi_class, NULL, MKDEV(major_ssi, 1), NULL, "mxc_ssi%u", 2)))
        {
                goto err_out;
        }

        printk("ssi : creating devfs entry for ssi1 \n");
        //---devfs_mk_cdev(MKDEV(major_ssi, 0), S_IFCHR | S_IRUGO | S_IWUSR, "mxc_ssi/1");

        printk("ssi : creating devfs entry for ssi2 \n");
        //---devfs_mk_cdev(MKDEV(major_ssi, 1), S_IFCHR | S_IRUGO | S_IWUSR, "mxc_ssi/2");

        res = request_irq(INT_SSI1, ssi1_interrupt_handler, 0, "testmod_ssi1", NULL);
        if (res != 0)
        {
                return -1;
        }

        res = request_irq(INT_SSI2, ssi2_interrupt_handler, 0, "testmod_ssi2", NULL);
        if (res != 0)
        {
                return -1;
        }
        return 0;

err_out:
        printk(KERN_ERR "ssi : error creating ssi test module class.\n");
        class_device_destroy(ssi_class, MKDEV(major_ssi, 0));
        class_device_destroy(ssi_class, MKDEV(major_ssi, 1));
        class_destroy(ssi_class);
        unregister_chrdev(major_ssi, "ssi");
        return -1;
}

/*================================================================================================*/
/*=====testmod_ssi_exit====*/
/**
@brief  This function implements the init function of the SSI device. This function is called when the
        module is unloading.

@param  none

@return This function returns 0.
*/
/*================================================================================================*/
static void __exit testmod_ssi_exit(void)
{
        free_irq(INT_SSI1, NULL);
        free_irq(INT_SSI2, NULL);

        class_device_destroy(ssi_class, MKDEV(major_ssi, 0));
        class_device_destroy(ssi_class, MKDEV(major_ssi, 1));
        class_destroy(ssi_class);

        unregister_chrdev(major_ssi, "ssi");
        //---devfs_remove("mxc_ssi");

        printk(KERN_DEBUG "ssi : successfully unloaded\n");
}

/*================================================================================================*/

module_init(testmod_ssi_init);
module_exit(testmod_ssi_exit);

MODULE_DESCRIPTION("SSI test device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
