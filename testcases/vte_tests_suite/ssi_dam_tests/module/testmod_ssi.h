/*================================================================================================*/
/**
        @file   testmod_ssi.c

        @brief  SSI DAM test module header file.
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
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

#ifndef __TESTMOD_SSI_H__
#define __TESTMOD_SSI_H__

#define SSI_IOCTL                                       0x54

#define IOCTL_SSI_CONFIGURE_SSI                         0x0
#define IOCTL_SSI_GET_CONFIGURATION                     0x1
#define IOCTL_SSI_CONFIGURE_AUDMUX                      0x2
#define IOCTL_SSI_CONFIGURE_PMIC                        0x3
#define IOCTL_SSI_CONFIGURE_PMIC_MIX                    0x4

#define STEREO_DAC                                      0x0
#define CODEC                                           0x1

#define MC13783_MASTER                                  0x1
#define MC13783_SLAVE                                   0x2
#define PMIC_MASTER                                     MC13783_MASTER
#define PMIC_SLAVE                                      MC13783_SLAVE


/**/
#ifdef CONFIG_ARCH_MXC91231

#define INIT_CKO_CLOCK()                                        \
        do{                                                     \
        volatile unsigned long reg;                             \
        \
        reg = readl(IO_ADDRESS(CRM_AP_BASE_ADDR) + 0x44);       \
        reg = ( (reg & (~(0x00000070))) | 0x00000020 );         \
        writel(reg, IO_ADDRESS(CRM_AP_BASE_ADDR) + 0x44);       \
        \
        reg = readl(IO_ADDRESS(CRM_COM_BASE_ADDR) + 0xc);       \
        reg = ( (reg & (~(0x00060000))) | 0x00020000 );         \
        writel(reg, IO_ADDRESS(CRM_COM_BASE_ADDR) + 0xc);       \
}while(0)

/*!
 * This macro is used to normalize master clock (ccm/crm)
 * divider value, as USBPLL runs at different frequency
 * on each platform. In MXC91231 case, we need to multiply
 * the base divider (which depends on frequency to applied
 * to audio to be played) by 2/5.
 */
#define NORMALIZE_CCM_DIVIDER(v)                        ( ((v) * 2) / 5 )

#endif /* CONFIG_ARCH_MXC91231 */

#ifdef CONFIG_ARCH_MX3
#define CCM_COSR_OFFSET                                 (MXC_CCM_BASE + 0x1c)

#define INIT_CKO_CLOCK()                                \
do{                                                     \
volatile unsigned long reg;                             \
                                                        \
unsigned int mask = 0x00000fff;                         \
unsigned int data = 0x00000208;                         \
                                                        \
mxc_ccm_modify_reg(CCM_COSR_OFFSET, mask, data);        \
reg = mxc_ccm_get_reg(CCM_COSR_OFFSET);                 \
gpio_audio_port_active(4);                              \
gpio_audio_port_active(5);                              \
}while(0)

/*!
 * This macro is used to normalize master clock (ccm/crm)
 * divider value, as USBPLL runs at different frequency
 * on each platform. On MX31 case, there is no need to
 * normalize it as MX31 dividers are the base for the
 * other boards.
 */
#define NORMALIZE_CCM_DIVIDER(v)                        (v)

#endif /* CONFIG_ARCH_MX3 */

#if defined(CONFIG_ARCH_MXC91331) || defined(CONFIG_ARCH_MXC91321)
#define NORMALIZE_CCM_DIVIDER(v)                        ( ((v) * 2) / 5 )
#define INIT_CKO_CLOCK()
#endif /* CONFIG_ARCH_MXC91331 && CONFIG_ARCH_MXC91321 */

#ifdef CONFIG_ARCH_MX27
#define NORMALIZE_CCM_DIVIDER(v)                        ( ((v) * 2) / 5 )
#define INIT_CKO_CLOCK()  \
do{ \
  mxc_ccm_modify_reg(0x28,0x1f,0x02); \
  mxc_ccm_modify_reg(0x18,0x03c00000,0x02000000);\
  mxc_clks_enable(SSI1_BAUD);\
  mxc_clks_enable(SSI2_BAUD);\
  gpio_ssi_active(0);\
  gpio_ssi_active(1);\
  mc13783_audio_output_bias_conf(1, 1);\
}while(0)
#endif /* CONFIG_ARCH_MX27 */

#define DEVICE_NAME_SSI1                                "/dev/mxc_ssi1"
#define DEVICE_NAME_SSI2                                "/dev/mxc_ssi2"

/*!
 * Wave file configuration.
 */
struct wave_config{
        /*!
         * SSI
         */
        unsigned short ssi;

        /*!
         * Number of channels (1:MONO, 2:STEREO)
         */
        unsigned short num_channels;

        /*!
         * Sample rate
         */
        unsigned long sample_rate;

        /*!
         * Bits per sample (16-bits mode supported)
         */
        unsigned short bits_per_sample;

        /*!
         * Sample size
         */
        unsigned long sample_size;

        /*!
         * Requested mixing mode
         */
         unsigned long mix_enabled;

        /*!
         * MC13783 device to use
         */
         unsigned long dac_codec;

        /*!
          * ssi fifo to be used
          */
         int ssi_fifo;

        /*!
          * clock provider
          */
         int master_clock;

};

#endif /* __TESTMOD_SSI_H__ */
