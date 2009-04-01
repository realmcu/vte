/*================================================================================================*/
/**
        @file   testmod_ssi.c

        @brief  SSI DAM test module C-file
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
-------------------------   ------------    ----------  -------------------------------------------
J.Quintero/jqui1c            26/09/2005     xxx         Initial version
S.V-Guilhou/svan01c          04/10/2005     TLSbo55818  Add MXC91331
I.Inkina/nknl001             20/12/2005     TLSbo56432  function descriptions were added
D.Simakov                    06/06/2006     TLSbo67103  No sound on SSI 1 and SSI 2
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/devfs_fs_kernel.h>

#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>

#include <ssi/ssi.h>
#include <ssi/registers.h>
#include <dam/dam.h>
#include <mc13783_legacy/core/mc13783_external.h>
#include <mc13783_legacy/module/mc13783_audio.h>

#ifdef CONFIG_ARCH_MX3
#include <../../arch/arm/mach-mx3/iomux.h>
#include <../../arch/arm/mach-mx3/crm_regs.h>
#endif
#include "testmod_ssi.h"

#define MAX_BUF_SIZE                                    (1024 * 128)

#define TIMESLOTS_2                                     0x3
#define TIMESLOTS_4                                     0x2
#define SAMPLE_RATE_MAX                                 0x9
#define TX_WATERMARK                                    0x4
#define RX_WATERMARK                                    0x6


/* MXC91131 configuration */
#ifdef CONFIG_ARCH_MXC91131
#define DEFAULT_PRESCALER                                  47360000
#define DIVIDE_CLK_RATE_BY_TWO                             1
#define NORMALIZE_CCM_DIVIDER(v)                        ( ((v) * 2) / 5 )
#endif

/* MXC91231 configuration */

#ifdef CONFIG_ARCH_MXC91231

#ifdef CONFIG_MXC_MC13783_PMIC
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
#ifdef CONFIG_ARCH_MXC91321
#define DEFAULT_PRESCALER                                  49887500
#define DIVIDE_CLK_RATE_BY_TWO                             0
#define NORMALIZE_CCM_DIVIDER(v)                           ( ((v) * 2) / 5 )
#endif

#define DEBUG
#ifdef DEBUG
#define DPRINTK(fmt, args...)           printk("%s: " fmt, __FUNCTION__, ## args)
#else                           /* DEBUG */
#define DPRINTK(fmt, args...)
#endif                          /* DEBUG */

struct ssi_priv_data
{
        int     ssi;
        int     size;
        struct wave_config wavconf;
        char    buf[MAX_BUF_SIZE];
};

/* ! This structure is used to store precalculated values to set ccm_clk and prescaler correctly.
 * These values have been calculated based on supported sample rates. */
struct freq_coefficients
{
        /* the desired sample rate */
        int     sample_rate;

        /* Divider to apply to ccm_ssi_clk clock to get our * sample rate. ccm_ssi_clk clock is
         * currently set * to 96Mhz on ZAS platforms and 240Mhz on iMX.31. */
        int     ccm_div;

        /* prescaler value to be applied. Please note that * this value is multiplied by 2 internally
         * when * SSI STCCR register is set (see below) */
        int     prescaler;
};

/* ! SSI major */
static int major_ssi;
static struct class_simple *ssi_class;

spinlock_t ssi_lock = SPIN_LOCK_UNLOCKED;

#ifdef CONFIG_ARCH_MXC91231
int     mxc91231_evb_ecn = 1;
#else                           /* CONFIG_ARCH_MXC91231 */
int     mxc91231_evb_ecn = 0;
#endif                          /* CONFIG_ARCH_MXC91231 */

#ifdef CONFIG_ARCH_MX3
int     mc13783_clock_source = ci_cli_b;
#else                           /* CONFIG_ARCH_MXC91231 */
int     mc13783_clock_source = ci_cli_a;
#endif                          /* CONFIG_ARCH_MXC91231 */

static struct ssi_priv_data ssi1_priv;
static struct ssi_priv_data ssi2_priv;

/* ! Structure used to store precalculated values to get correct Bit Clock and Frame Sync when
 * playing sound in MCU master mode. Remember that ccm_div field values have been multiplied by 2 for
 * * * FP accuracy (i.e if we want divide the clock by 10, we must pass value 20 to
 * mxc_set_clocks_div function) On the other hand, prescaler field value is divided by 2. This is
 * because ssi device internally multiplies prescaler by 2. */
 /*
  * bit_clock = sys_clock / [(div2 + 1) x (7 x psr + 1) x (prescaler + 1) x 2] where ssi_clock =
  * system_clock/divider. prescaler = defined below. div2 = 0, psr = 0,
  *
  * frame_sync_clock = (bit_clock) / [(frdc + 1) x word_length] where frdc = 4 word_length = 16
  * (sample bits) */

static struct freq_coefficients freq_values[SAMPLE_RATE_MAX] =
{
        {8000, NORMALIZE_CCM_DIVIDER(10), 94 / 2},
        {11025, NORMALIZE_CCM_DIVIDER(8), 85 / 2},
        {16000, NORMALIZE_CCM_DIVIDER(10), 47 / 2},
        {22050, NORMALIZE_CCM_DIVIDER(2), 170 / 2},
        {24000, NORMALIZE_CCM_DIVIDER(12), 26 / 2},
        {32000, NORMALIZE_CCM_DIVIDER(10), 24 / 2},
        {44100, NORMALIZE_CCM_DIVIDER(2), 85 / 2},
        {48000, NORMALIZE_CCM_DIVIDER(12), 13 / 2},
        {96000, 4, 39},
};


/* ! * This function is used only for iMX.31 architecture. * It programs GPIO to activate AUDMUX
 * ports */
#ifdef CONFIG_ARCH_MX3
void gpio_audio_port_active(int port)
{
        switch (port)
        {
        case 4:
                iomux_config_mux(MX31_PIN_SCK4, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
                iomux_config_mux(MX31_PIN_SRXD4, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
                iomux_config_mux(MX31_PIN_STXD4, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
                iomux_config_mux(MX31_PIN_SFS4, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
                break;

        case 5:
                iomux_config_mux(MX31_PIN_SCK5, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
                iomux_config_mux(MX31_PIN_SRXD5, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
                iomux_config_mux(MX31_PIN_STXD5, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
                iomux_config_mux(MX31_PIN_SFS5, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
                break;

        default:
                break;
        }
}
#endif                          /* CONFIG_ARCH_MX3 */

/* ! This function returns the optimal values to get a desired sample rate (among supported ones)
 * Values stored in freq_coefficients array are valid if the following conditions are honored: 1 -
 * DIV2 bit in SSI STCCR register (SSI Transmit and Receive Clock Control Registers) is set to 0. 2 -
 * * * PSR bit in SSI STCCR is set to 0. 3 - DC bits in SSI STCCR are set to 0. 4 - WL bits in SSI
 * STCCR are set to 16. */
inline struct freq_coefficients *get_ccm_divider(int sample_rate)
{
        int     i;

        for (i = 0; i < SAMPLE_RATE_MAX; i++)
        {
                if (freq_values[i].sample_rate == sample_rate)
                {
                        return &freq_values[i];
                }
        }

        return NULL;
}

inline int normalize_speed_for_mc13783(struct wave_config *waveconf)
{
        switch (waveconf->sample_rate)
        {
        case 8000:
                return sr_8k;
        case 11025:
                return sr_11025;
        case 16000:
                return sr_16k;
        case 22050:
                return sr_22050;
        case 24000:
                return sr_24k;
        case 32000:
                return sr_32k;
        case 44100:
                return sr_44100;
        case 48000:
                return sr_48k;
        case 64000:
                return sr_64k;
        case 96000:
                return sr_96k;
        default:
                return sr_16k;
        }

}

inline void disable_codec(void)
{
        mc13783_audio_output_pga(false, rs_codec);
        mc13783_audio_output_set_volume(rs_codec, 0x0);
        mc13783_audio_codec_clk_en(0);
        mc13783_audio_codec_enable(0);
}

inline void disable_stereodac(void)
{
        mc13783_audio_output_pga(false, rs_st_dac);
        mc13783_audio_output_set_volume(rs_st_dac, 0x0);
        mc13783_audio_st_dac_clk_en(0);
        mc13783_audio_st_dac_enable(0);
}


void halt_hardware(struct wave_config *wavconf)
{
        /* disable codec and stereodac */
        disable_codec();
        disable_stereodac();

        /* disable ssi (fifos, interrupts, ...) */
        ssi_transmit_enable(wavconf->ssi, false);
        ssi_tx_fifo_enable(wavconf->ssi, ssi_fifo_0, false);
        ssi_tx_fifo_enable(wavconf->ssi, ssi_fifo_1, false);

        ssi_receive_enable(wavconf->ssi, false);
        ssi_rx_fifo_enable(wavconf->ssi, ssi_fifo_0, false);
        ssi_rx_fifo_enable(wavconf->ssi, ssi_fifo_1, false);

        ssi_interrupt_disable(wavconf->ssi, ssi_tx_fifo_0_empty);
        // ssi_interrupt_disable(wavconf->ssi, ssi_transmitter_underrun_0);
        ssi_enable(wavconf->ssi, false);

}

void configure_stereodac(struct wave_config *wavconf)
{
        int     speed = 0;
        int     master;

        mc13783_audio_output_mixer_input(true, rs_st_dac);
        mc13783_audio_output_pga(true, rs_st_dac);
        mc13783_audio_output_set_volume(rs_st_dac, 0xd);

#ifdef CONFIG_ARCH_MX27
        mc13783_audio_st_dac_select_ssi(sb_ssi_0);
#else
        mc13783_audio_st_dac_select_ssi((wavconf->ssi == SSI1) ? sb_ssi_0 : sb_ssi_1);
#endif

        mc13783_audio_st_dac_select_cli(mc13783_clock_source);

        master = (wavconf->master_clock == MC13783_SLAVE) ? ms_slave : ms_master;
        mc13783_audio_st_dac_master_slave(master);

        mc13783_audio_st_dac_bit_clock_invert(0);
        mc13783_audio_st_dac_frame_sync_invert(0);
        mc13783_audio_st_dac_protocol(bp_network);
        mc13783_audio_st_dac_conf_pll(0x4);

        speed = normalize_speed_for_mc13783(wavconf);
        mc13783_audio_st_dac_set_sample_rate(speed);

        DPRINTK("sample rate = %d\n", (int) wavconf->sample_rate);
        DPRINTK("speed = %d\n", (int) speed);

        mc13783_audio_st_dac_clk_en(1);
        mc13783_audio_st_dac_enable(1);
}

void configure_codec(struct wave_config *wavconf)
{
        int     speed = 0;
        int     master;

        mc13783_audio_output_mixer_input(true, rs_codec);
        mc13783_audio_output_pga(true, rs_codec);
        mc13783_audio_output_set_volume(rs_codec, 0xd);

#ifdef CONFIG_ARCH_MX27
        mc13783_audio_codec_select_ssi(sb_ssi_0);
#else
        mc13783_audio_codec_select_ssi((wavconf->ssi == SSI1) ? sb_ssi_0 : sb_ssi_1);
#endif

        mc13783_audio_codec_select_cli(mc13783_clock_source);

        master = (wavconf->master_clock == MC13783_SLAVE) ? ms_slave : ms_master;
        mc13783_audio_codec_master_slave(master);

        mc13783_audio_codec_bit_clock_invert(0);
        mc13783_audio_codec_frame_sync_invert(0);
        mc13783_audio_codec_protocol(bp_network);
        mc13783_audio_codec_conf_pll(0x4);

        speed = normalize_speed_for_mc13783(wavconf);
        mc13783_audio_codec_set_sample_rate(speed);

        DPRINTK("sample rate = %d\n", (int) wavconf->sample_rate);
        DPRINTK("speed = %d\n", (int) speed);

        mc13783_audio_codec_clk_en(1);
        mc13783_audio_codec_enable(1);
}

void configure_dam_ssi1(void)
{
        dam_reset_register(port_1);
        dam_reset_register(port_4);

        dam_select_mode(port_1, normal_mode);
        dam_select_mode(port_4, normal_mode);

        dam_set_synchronous(port_1, true);
        dam_set_synchronous(port_4, true);

        dam_select_RxD_source(port_4, port_1);
        dam_select_RxD_source(port_1, port_4);

        dam_select_TxFS_direction(port_4, signal_out);
        dam_select_TxFS_source(port_4, false, port_1);

        if (mxc91231_evb_ecn == 1)
        {
                DPRINTK("Applying workaround on DAM for MXC91231\n");
                dam_reset_register(port_7);
                dam_select_mode(port_7, normal_mode);

                dam_set_synchronous(port_7, false);
                dam_select_RxClk_source(port_7, false, port_1);
                dam_select_RxClk_direction(port_7, signal_out);
        }
        else
        {
                dam_select_TxClk_direction(port_4, signal_out);
                dam_select_TxClk_source(port_4, false, port_1);
        }

}

void configure_dam_ssi2(void)
{
        dam_reset_register(port_2);
        dam_reset_register(port_5);

        dam_set_synchronous(port_2, true);

        dam_select_RxD_source(port_2, port_5);
        dam_select_RxD_source(port_5, port_2);

        dam_select_TxFS_direction(port_5, signal_out);
        dam_select_TxFS_source(port_5, false, port_2);

        if (mxc91231_evb_ecn == 1)
        {
                dam_set_synchronous(port_5, false);
                dam_select_RxClk_source(port_5, false, port_2);
                dam_select_RxClk_direction(port_5, signal_out);
        }
        else
        {
                dam_set_synchronous(port_5, true);
                dam_select_TxClk_direction(port_5, signal_out);
                dam_select_TxClk_source(port_5, false, port_2);
        }
}

void configure_dam(struct wave_config *wavconf)
{
        unsigned int source_port;
        unsigned int target_port;

        if (wavconf->ssi == SSI1)
        {
                DPRINTK("SSI1. source port = 1, target port = 4\n");
                source_port = port_1;
                target_port = port_4;
        }
        else
        {
                DPRINTK("SSI2. source port = 2, target port = 5\n");
                source_port = port_2;
                target_port = port_5;
        }

        dam_reset_register(source_port);
        dam_reset_register(target_port);

        dam_select_mode(source_port, normal_mode);
        dam_select_mode(target_port, normal_mode);

        dam_set_synchronous(source_port, true);
        dam_set_synchronous(target_port, true);

        dam_select_RxD_source(target_port, source_port);
        dam_select_RxD_source(source_port, target_port);

        dam_select_TxFS_direction(target_port, signal_out);
        dam_select_TxFS_source(target_port, false, source_port);

        if ((mxc91231_evb_ecn == 1) && (wavconf->ssi == SSI1))
        {
                DPRINTK("Applying workaround on DAM for MXC91231\n");
                dam_reset_register(port_7);
                dam_select_mode(port_7, normal_mode);

                dam_set_synchronous(port_7, false);
                dam_select_RxClk_source(port_7, false, source_port);
                dam_select_RxClk_direction(port_7, signal_out);
        }
        else
        {
                dam_select_TxClk_direction(target_port, signal_out);
                dam_select_TxClk_source(target_port, false, source_port);
        }
}

void configure_dam_mc13783_master(struct wave_config *device)
{
        int     source_port;
        int     target_port;

        DPRINTK("Set MC13783 as clock provider\n");

        if (device->ssi == SSI1)
        {
                DPRINTK("SSI1. source port = 1, target port = 4\n");
                source_port = port_1;
                target_port = port_4;
        }
        else
        {

#ifdef CONFIG_ARCH_MX27
                DPRINTK("SSI2. source port = 2, target port = 4\n");
                source_port = port_2;
                target_port = port_4;
#else
                DPRINTK("SSI2. source port = 2, target port = 5\n");
                source_port = port_2;
                target_port = port_5;
#endif
        }

        /*
         * source_port = (device->ssi == SSI1) ? port_1 : port_2; DPRINTK("Using internal port %d\n",
         * source_port + 1);
         *
         * target_port = (device->dac_codec == STEREO_DAC) ? port_4 : port_5; DPRINTK("Using
         * external port %d\n", target_port + 1); */

        dam_reset_register(source_port);
        dam_reset_register(target_port);

        dam_select_mode(source_port, normal_mode);
        dam_select_mode(target_port, normal_mode);

        dam_set_synchronous(source_port, true);
        dam_set_synchronous(target_port, true);

        dam_select_RxD_source(source_port, target_port);
        dam_select_RxD_source(target_port, source_port);

        dam_select_TxFS_direction(source_port, signal_out);
        dam_select_TxFS_source(source_port, false, target_port);

        dam_select_TxClk_direction(source_port, signal_out);
        dam_select_TxClk_source(source_port, false, target_port);

        dam_select_RxFS_direction(source_port, signal_out);
        dam_select_RxFS_source(source_port, false, target_port);

        dam_select_RxClk_direction(source_port, signal_out);
        dam_select_RxClk_source(source_port, false, target_port);

#ifdef CONFIG_ARCH_MX27
        dam_switch_Tx_Rx(target_port, true);
#else
        writel(0x0031010, IO_ADDRESS(AUDMUX_BASE_ADDR) + 0x38);
#endif
}

/* ! This function applies precalculated values for each supported audio frequency. This settings are
 * applied only if MCU master mode was chosen. */
void set_audio_clocks(struct wave_config *device)
{
        struct freq_coefficients *ccm_div;
        int     ssi_clock;
        int     divider;
        int     ssi;

        ssi = device->ssi;

        /* We never use the divider by 2 implemented in SSI */
        ssi_tx_clock_divide_by_two(ssi, 0);

        /* Set prescaler range (a fixed divide-by-eight prescaler in series with the variable
         * prescaler) to 0 as we don't need it. */
        ssi_tx_clock_prescaler(ssi, 0);

        /* Currently, only supported sample length is 16 bits */
        ssi_tx_word_length(ssi, ssi_16_bits);

        if (device->master_clock == MC13783_MASTER)
        {

                /* set direction of clocks ("externally" means that clocks come from MC13783 to MCU) */
                ssi_tx_frame_direction(ssi, ssi_tx_rx_externally);
                ssi_tx_clock_direction(ssi, ssi_tx_rx_externally);

                /* Frame Rate Divider Control. In Normal mode, this ratio determines the word
                 * transfer rate. In Network mode, this ration sets the number of words per frame. */
                ssi_tx_frame_rate(ssi, 4);
        }
        else
        {
                ssi_tx_frame_rate(ssi, 4);

                ccm_div = get_ccm_divider(device->sample_rate);
                ssi_clock = (ssi == SSI1) ? SSI1_BAUD : SSI2_BAUD;

                /* disable ssi clock */
                mxc_clks_disable(ssi_clock);

                /* set ccm_clock divider */
                divider = (device->num_channels == 1) ? (ccm_div->ccm_div * 2) : ccm_div->ccm_div;

                /* set the clock divider for this audio session */
                mxc_set_clocks_div(ssi_clock, divider);

                /* enable clock */
                mxc_clks_enable(ssi_clock);

                /* set prescaler value */
                ssi_tx_prescaler_modulus(ssi, (unsigned char) ccm_div->prescaler);

                DPRINTK("USBPLL = %d\n", (int) mxc_pll_clock(USBPLL));
                DPRINTK("SSI1 clock = %d\n", (int) mxc_get_clocks(ssi_clock));
                DPRINTK("prescaler value = %d\n", ccm_div->prescaler);

                /* set direction of clocks ("internally" means that clocks go from MCU to MC13783) */
                ssi_tx_frame_direction(ssi, ssi_tx_rx_internally);
                ssi_tx_clock_direction(ssi, ssi_tx_rx_internally);
        }
}

void configure_ssi_tx(struct wave_config *wavconf)
{
        int     mod = wavconf->ssi;

        ssi_enable(mod, false);

        ssi_interrupt_disable(mod, 0xffffffff);
        ssi_system_clock(mod, false);
        ssi_clock_idle_state(mod, true);

        ssi_i2s_mode(mod, i2s_normal);
        ssi_synchronous_mode(mod, true);

        ssi_network_mode(mod, true);
        ssi_tx_fifo_enable(mod, ssi_fifo_0, true);

        ssi_tx_early_frame_sync(mod, ssi_frame_sync_one_bit_before);
        ssi_tx_frame_sync_length(mod, ssi_frame_sync_one_bit);
        ssi_tx_frame_sync_active(mod, ssi_frame_sync_active_high);
        ssi_tx_clock_polarity(mod, ssi_clock_on_rising_edge);
        ssi_tx_shift_direction(mod, ssi_msb_first);

        ssi_tx_bit0(mod, true);

        ssi_tx_fifo_empty_watermark(mod, ssi_fifo_0, (unsigned char) TX_WATERMARK);
        ssi_tx_fifo_empty_watermark(mod, ssi_fifo_1, (unsigned char) TX_WATERMARK);

        ssi_rx_fifo_full_watermark(mod, ssi_fifo_0, (unsigned char) RX_WATERMARK);
        ssi_rx_fifo_full_watermark(mod, ssi_fifo_1, (unsigned char) RX_WATERMARK);

        ssi_interrupt_enable(mod, ssi_tx_fifo_0_empty);
        ssi_enable(mod, true);
        ssi_transmit_enable(mod, false);

        if (wavconf->num_channels == 2)
        {
                ssi_tx_mask_time_slot(mod, 0xfffffffc);
                ssi_rx_mask_time_slot(mod, 0xfffffffc);
        }
        else
        {
                ssi_tx_mask_time_slot(mod, 0xfffffffe);
                ssi_rx_mask_time_slot(mod, 0xfffffffe);
        }

        /* set BCL and FS clocks if MCU master mode */
        set_audio_clocks(wavconf);
}

int write_fifo(struct ssi_priv_data *priv)
{
        unsigned long ssi_sfcsr_addr;
        unsigned long reg;
        int     i,
                left,
                right;
        int     data_size;

        i = 0;
        data_size = priv->size;
        ssi_transmit_enable(priv->ssi, true);

        ssi_sfcsr_addr = (priv->ssi == SSI1) ? IO_ADDRESS(SSI1_BASE_ADDR) :
            IO_ADDRESS(SSI2_BASE_ADDR);

        while (data_size >= 4)
        {
                reg = readl(ssi_sfcsr_addr + MXC_SSISFCSR);
                if ((reg & 0xF00) <= 0x600)
                {
                        left = (unsigned int) (((priv->buf[i + 1] << 8) & 0xFF00) |
                                               (priv->buf[i] & 0x00FF));
                        right = (unsigned int) (((priv->buf[i + 3] << 8) & 0xFF00) |
                                                (priv->buf[i + 2] & 0x00FF));
                        ssi_set_data(priv->ssi, ssi_fifo_0, left);
                        ssi_set_data(priv->ssi, ssi_fifo_0, right);
                        data_size -= 4;
                        i += 4;
                }
        }

        ssi_transmit_enable(priv->ssi, false);
        return priv->size;
}


/* ! Interrupt service routine registered to handle the individual general purpose interrupts or
 * muxed. Interrupts are cleared in ISR before scheduling tasklet @param irq the interrupt number
 * @param dev_id driver private data @param regs holds a snapshot of the processor's context before
 * the processor entered the interrupt code @return The function returns \b IRQ_RETVAL(1) if
 * interrupt was handled, returns \b IRQ_RETVAL(0) if the interrupt was not handled.  \b IRQ_RETVAL
 * is defined in \b include/linux/interrupt.h. */
static irqreturn_t ssi1_interrupt_handler(int irq, void *dev_id, struct pt_regs *regs)
{
        unsigned long stat_reg = 0;

        /* Disable SSI interrupts */
        ssi_interrupt_disable(SSI1, ssi_tx_fifo_0_empty);
        ssi_interrupt_disable(SSI1, ssi_tx_interrupt_enable);

        stat_reg = readl(IO_ADDRESS(SSI1_BASE_ADDR) + MXC_SSI1SISR);
        if ((stat_reg & 0x00000001) == 0)
        {
                printk(KERN_WARNING "unhandled SSI interrupt. Val = 0x%x\n",
                       (unsigned int) stat_reg);
                return IRQ_RETVAL(0);
        }

        printk(KERN_WARNING "SSI Tx FIFO empty interrupt.\n");
        return IRQ_RETVAL(1);
}

/* ! Interrupt service routine registered to handle the individual general purpose interrupts or
 * muxed. Interrupts are cleared in ISR before scheduling tasklet @param irq the interrupt number
 * @param dev_id driver private data @param regs holds a snapshot of the processor's context before
 * the processor entered the interrupt code @return The function returns \b IRQ_RETVAL(1) if
 * interrupt was handled, returns \b IRQ_RETVAL(0) if the interrupt was not handled.  \b IRQ_RETVAL
 * is defined in \b include/linux/interrupt.h. */
static irqreturn_t ssi2_interrupt_handler(int irq, void *dev_id, struct pt_regs *regs)
{
        return IRQ_RETVAL(1);
}

/* ! This function implements IOCTL controls on a SSI device. @param inode pointer on the node @param
 * file pointer on the file @param cmd the command @param arg the parameter @return This function
 * returns 0 if successful. */
static int testmod_ssi_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                             unsigned long arg)
{
        struct wave_config *wavconf;
        struct ssi_priv_data *priv;
        int     res = 0;

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
                break;

        case IOCTL_SSI_GET_CONFIGURATION:
                DPRINTK("IOCTL_SSI_GET_CONFIGURATION called\n");
                res = copy_to_user((void *) arg, (void *) wavconf, sizeof(struct wave_config));
                if (res)
                {
                        return -EFAULT;
                }

                break;

        case IOCTL_SSI_CONFIGURE_AUDMUX:
                DPRINTK("IOCTL_SSI_CONFIGURE_AUDMUX called\n");

                if (wavconf->master_clock == MC13783_MASTER)
                {
                        configure_dam_mc13783_master(wavconf);
                        // configure_dam_ssi1();
                }
                else
                {
                        configure_dam_mc13783_master(wavconf);
                        // configure_dam_ssi2();
                }

                break;

        case IOCTL_SSI_CONFIGURE_PMIC:
                DPRINTK("IOCTL_SSI_CONFIGURE_PMIC called\n");

                mc13783_audio_set_autodetect(1);
                /* Register 36 bits 2,3 */
                mc13783_audio_output_bias_conf(1, 1);

                /* Register 36 bits 9, 10, 11 */
                mc13783_audio_output_set_conf(od_headset, os_mixer_source);

                /* Register 37 bit 16 */
                if (wavconf->num_channels == 1)
                {
                        mc13783_audio_output_config_mixer(ac_mono_opposit);
                }
                else
                {
                        mc13783_audio_output_config_mixer(ac_stereo_opposit);
                }

                if (wavconf->dac_codec == STEREO_DAC)
                {
                        // if(wavconf->ssi == SSI1){
                        disable_codec();
                        configure_stereodac(wavconf);

                        mc13783_audio_st_dac_network_nb_of_timeslots(TIMESLOTS_4);      /* 4
                                                                                         * timeslots */
                        mc13783_audio_st_dac_conf_network_mode(t_mode_0);
                        mc13783_audio_st_dac_conf_network_secondary(t_mode_1, mm_no_mixing);
                        mc13783_audio_st_dac_output_mixing_gain(mm_no_mixing);
                }
                else
                {
                        if ((wavconf->sample_rate != 16000) && (wavconf->sample_rate != 8000))
                        {
                                printk("sample rate not supported by CODEC\n");
                                return -1;
                        }

                        disable_stereodac();
                        configure_codec(wavconf);

                        mc13783_audio_codec_conf_network_mode(t_mode_0);
                        mc13783_audio_codec_conf_network_secondary(t_mode_1, t_mode_2,
                                                                   mm_no_mixing);
                        mc13783_audio_codec_output_mixing_gain(mm_no_mixing);
                }
                mc13783_audio_set_phantom_ground(0);
                /* Register 38 bit 1, 9 */
                mc13783_audio_input_device(1, id_headset);

                /* Register 38 bits 14,15,16,17,18 - 19,20,21,22,23 */
                mc13783_audio_input_set_gain(0xa, 0xa);
                msleep(20);
                break;

        default:
                DPRINTK("Bad IOCTL called\n");
                return -EINVAL;
        }

        return 0;
}

/* ! This function implements the open method on a SSI device. @param inode pointer on the node
 * @param file pointer on the file @return This function returns 0. */
static int testmod_ssi_open(struct inode *inode, struct file *file)
{
        unsigned int minor = MINOR(inode->i_rdev);
        struct ssi_priv_data *priv;

        DPRINTK("minor = %d\n", minor);

        priv = (minor == 0) ? &ssi1_priv : &ssi2_priv;

        priv->ssi = minor;
        priv->size = 0;
        memset(priv->buf, 0, MAX_BUF_SIZE);;

        file->private_data = (void *) priv;
        ssi_tx_flush_fifo(minor);
        return 0;
}

/* ! This function implements the release method on a SSI device. @param inode pointer on the node
 * @param file pointer on the file @return This function returns 0. */
static int testmod_ssi_free(struct inode *inode, struct file *file)
{
        // unsigned int minor = MINOR(inode->i_rdev);
        struct ssi_priv_data *priv;

        priv = (struct ssi_priv_data *) file->private_data;
        halt_hardware(&priv->wavconf);
        file->private_data = NULL;

        return 0;
}

static ssize_t testmod_ssi_write(struct file *file, const char *buf, size_t bytes, loff_t * off)
{
        struct ssi_priv_data *priv;
        unsigned int minor;
        int     res = 0;

        minor = MINOR(file->f_dentry->d_inode->i_rdev);
        priv = (struct ssi_priv_data *) file->private_data;

        if (bytes == 0)
        {
                return -EINVAL;
        }

        if (bytes > MAX_BUF_SIZE)
        {
                priv->size = MAX_BUF_SIZE;
        }
        else
        {
                priv->size = bytes;
        }

        res = copy_from_user(priv->buf, buf, priv->size);
        if (res)
        {
                return -EFAULT;
        }

        return write_fifo(priv);
}

static ssize_t testmod_ssi_read(struct file *file, char *buf, size_t bytes, loff_t * off)
{
        struct ssi_priv_data *priv;
        unsigned int minor;

        minor = MINOR(file->f_dentry->d_inode->i_rdev);
        priv = (struct ssi_priv_data *) file->private_data;

        DPRINTK("minor = %d\n", minor);

        return 0;
}

/* ! This structure defines file operations for a SSI device. */
static struct file_operations ssi_fops = {
        .owner = THIS_MODULE,
        .write = testmod_ssi_write,
        .read = testmod_ssi_read,
        .ioctl = testmod_ssi_ioctl,
        .open = testmod_ssi_open,
        .release = testmod_ssi_free,
};

/* ! This function implements the init function of the SSI device. This function is called when the
 * module is loaded. @return This function returns 0. */
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

        devfs_mk_dir("mxc_ssi");


        ssi_class = class_simple_create(THIS_MODULE, "mxc_ssi");
        if (IS_ERR(ssi_class))
        {
                goto err_out;
        }

        if (IS_ERR(class_simple_device_add(ssi_class, MKDEV(major_ssi, 0), NULL, "mxc_ssi%u", 1)))
        {
                goto err_out;
        }

        if (IS_ERR(class_simple_device_add(ssi_class, MKDEV(major_ssi, 1), NULL, "mxc_ssi%u", 2)))
        {
                goto err_out;
        }

        printk("ssi : creating devfs entry for ssi1 \n");
        devfs_mk_cdev(MKDEV(major_ssi, 0), S_IFCHR | S_IRUGO | S_IWUSR, "mxc_ssi/1");

        printk("ssi : creating devfs entry for ssi2 \n");
        devfs_mk_cdev(MKDEV(major_ssi, 1), S_IFCHR | S_IRUGO | S_IWUSR, "mxc_ssi/2");

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

        /* Activate CKO clock in order to have a 26 mhz clock on MC13783 CLI signal */
        INIT_CKO_CLOCK();

        return 0;

err_out:
        printk(KERN_ERR "ssi : error creating ssi test module class.\n");
        class_simple_device_remove(MKDEV(major_ssi, 0));
        class_simple_device_remove(MKDEV(major_ssi, 1));
        class_simple_destroy(ssi_class);
        unregister_chrdev(major_ssi, "ssi");
        return -1;
}

/* ! This function implements the exit function of the SPI device. This function is called when the
 * module is unloaded. */
static void __exit testmod_ssi_exit(void)
{
        free_irq(INT_SSI1, NULL);
        free_irq(INT_SSI2, NULL);

        class_simple_device_remove(MKDEV(major_ssi, 0));
        class_simple_device_remove(MKDEV(major_ssi, 1));
        class_simple_destroy(ssi_class);

        unregister_chrdev(major_ssi, "ssi");
        devfs_remove("mxc_ssi");

        printk(KERN_DEBUG "ssi : successfully unloaded\n");
}

module_init(testmod_ssi_init);
module_exit(testmod_ssi_exit);

MODULE_DESCRIPTION("SSI test device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
