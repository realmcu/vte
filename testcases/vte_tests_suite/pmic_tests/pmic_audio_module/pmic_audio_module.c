/*====================*/
/**
        @file   pmic_audio_module.c

        @brief  PMIC audio dirver API
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
S.Bezrukov/SBAZR1C           31/08/2005     TLSbo52697   Initial version
A.Ozerov/b00320              08/08/2006     TLSbo73745   Review version(in accordance to L26_1_19 release).

====================
Portability: ARM GCC
======================*/

/*======================
                                        INCLUDE FILES
======================*/
#include <linux/device.h>       /* Added on 05/03/06 by RAKESH S JOSHI */
#include <linux/fs.h>

/* Driver specific include files */
#include <linux/pmic_status.h>
#include <linux/pmic_audio.h>
#include <linux/pmic_external.h>     /* For PMIC Protocol driver interface. */

#include <pmic_audio_module.h>

/*======================
                                        GLOBAL VARIABLES
======================*/
static struct class *pmic_audio_class;   /* added on 05/03/06 RAKESH S JOSHI */

/*======================
                                        DEFINES AND MACROS
======================*/
#define CHECK_ERROR(a) \
if( a != PMIC_SUCCESS ) \
{ \
        VT_rv = a; \
        printk(KERN_WARNING "Error in "#a". Error code: %d\n", VT_rv); \
} \

/*======================
                                        FUNCTION PROTOTYPES
======================*/
int VT_pmic_audio_test_output(void)
{
        PMIC_AUDIO_HANDLE hStDAC = 0,
            extStereoIn = 0;
        PMIC_AUDIO_OUTPUT_PORT port,
                port_1;
        PMIC_AUDIO_STEREO_IN_GAIN gain,
                gain_1;
        PMIC_AUDIO_OUTPUT_PGA_GAIN pga_gain,
                pga_gain_1;
        PMIC_AUDIO_MONO_ADDER_MODE ma_mode;
        PMIC_AUDIO_MONO_ADDER_OUTPUT_GAIN ma_gain,
                ma_gain_1;
        PMIC_AUDIO_OUTPUT_CONFIG config,
                config_1;
        int     VT_rv = PMIC_SUCCESS;
        int clockIn, clockFreq;
        int ret_result;

        /* Use the Stereo DAC to play a 44.1 kHz audio stream on the stereo headset. */
        printk("Calling pmic_audio_open(&hStDAC, STEREO_DAC).\n");
        /*CHECK_ERROR(pmic_audio_open(&hStDAC, STEREO_DAC));*/
        ret_result = pmic_audio_open(&hStDAC, STEREO_DAC);


        /* Got Stereo DAC handle, now select data bus and transmission protocol. */
        printk
            ("Calling pmic_audio_set_protocol(hStDAC, AUDIO_DATA_BUS_1, NETWORK_MODE, BUS_MASTER_MODE, USE_2_TIMESLOTS).\n");
        CHECK_ERROR(pmic_audio_set_protocol
                    (hStDAC, AUDIO_DATA_BUS_1, NETWORK_MODE, BUS_MASTER_MODE, USE_2_TIMESLOTS));

        #ifdef CONFIG_MXC_PMIC_SC55112
        clockIn = CLOCK_IN_DEFAULT;
        clockFreq = STDAC_CLI_13MHZ;
        #elif defined( CONFIG_MXC_PMIC_MC13783 )
        clockIn = CLOCK_IN_CLIA;
        clockFreq = STDAC_CLI_33_6MHZ;
        #endif

        /* Now also configure the sampling rate and PMIC clock source. */
        printk
            ("Calling pmic_audio_stdac_set_clock(hStDAC, CLOCK_IN_DEFAULT, STDAC_CLI_33_6MHZ, STDAC_RATE_44_1_KHZ, NO_INVERT).\n");
        CHECK_ERROR(pmic_audio_stdac_set_clock
                    (hStDAC, clockIn, clockFreq, STDAC_RATE_44_1_KHZ, NO_INVERT));

        /* Set, get and clear various audio output section options. */
        for (config = MONO_SPEAKER_INVERT_OUT_ONLY; config <= STEREO_HEADSET_AMP_AUTO_DISABLE;
             config <<= 1)
        {
                printk("Calling pmic_audio_output_set_config(hStDAC, config). config=%d\n", config);
                ret_result = pmic_audio_output_set_config(hStDAC, config);
                if (ret_result == -3)
                {
                printk("PMIC NOT SUPPERTED for config =%d\n", config);
                }else{
                CHECK_ERROR(pmic_audio_output_get_config(hStDAC, &config_1));
                printk("Calling pmic_audio_output_get_config(hStDAC, &config_1). config_1=%d\n",
                       config_1);

                if (config != config_1)
                {
                        printk(KERN_WARNING
                               "Error in output config, mismatch written and read parametrs: written=%d read=%d\n",
                               config, config_1);
                        return PMIC_ERROR;
                }

                CHECK_ERROR(pmic_audio_output_clear_config(hStDAC, config));
                }
        }

        printk("Calling pmic_audio_enable(hStDAC).\n");
        CHECK_ERROR(pmic_audio_enable(hStDAC));

        /* Select and check the audio output ports. */
        /*for (port = MONO_SPEAKER; port <= STEREO_HEADSET_RIGHT; port <<= 1)*/
        for (port = MONO_SPEAKER; port <= STEREO_OUT_RIGHT; port <<= 1)
        {
            if ( port != MONO_CDCOUT){
                 printk("Calling pmic_audio_output_set_port(hStDAC, port). port=%d\n", port);
                /*CHECK_ERROR(pmic_audio_output_set_port(hStDAC, port));*/
                ret_result = pmic_audio_output_set_port(hStDAC, port);
                if (ret_result == -3)
                {
                printk("PMIC NOT SUPPERTED for port =%d\n", port);
                }else{
                    CHECK_ERROR(pmic_audio_output_get_port(hStDAC, &port_1));
                    printk("Calling pmic_audio_output_get_port(hStDAC, &port_1). port_1=%d\n", port_1);

                    if (port != port_1)
                    {
                        printk(KERN_WARNING
                               "Error in output port, mismatch written and read parametrs: written=%d read=%d\n",
                               port, port_1);
                        return PMIC_ERROR;
                     }
                }
            }
        }

        /* Set and check the gain level for the external stereo inputs. */
        printk("Calling pmic_audio_open(&extStereoIn,EXTERNAL_STEREO_IN).\n");
        CHECK_ERROR(pmic_audio_open(&extStereoIn, EXTERNAL_STEREO_IN));

        gain = STEREO_IN_GAIN_0DB;
        printk("Calling pmic_audio_output_set_stereo_in_gain(extStereoIn, gain). gain=%d\n", gain);
        CHECK_ERROR(pmic_audio_output_set_stereo_in_gain(extStereoIn, gain));
        CHECK_ERROR(pmic_audio_output_get_stereo_in_gain(extStereoIn, &gain_1));
        printk("Calling pmic_audio_output_get_stereo_in_gain(extStereoIn, &gain_1). gain_1=%d\n",
               gain_1);

        if (gain != gain_1)
        {
                printk(KERN_WARNING
                       "Error in output stereo_in_gain, mismatch written and read parametrs: written=%d read=%d\n",
                       gain, gain_1);
                return PMIC_ERROR;

        }

        /* Set and check the output PGA gain level. */
        for (pga_gain = OUTPGA_GAIN_MINUS_24DB; pga_gain <= OUTPGA_GAIN_PLUS_6DB; pga_gain++)
        {
                printk("Calling pmic_audio_output_set_pgaGain(hStDAC, pga_gain). pga_gain=%d\n",
                       pga_gain);
                ret_result = pmic_audio_output_set_pgaGain(hStDAC, pga_gain);
                if ( ret_result == -3 ){
                    printk("PMIC NOT SUPPORTED FOR PGA GAIN =%d\n", pga_gain);
                }else{
                    CHECK_ERROR(pmic_audio_output_get_pgaGain(hStDAC, &pga_gain_1));
                    printk
                        ("Calling pmic_audio_output_get_pgaGain(hStDAC, &pga_gain_1). pga_gain_1=%d\n",
                         pga_gain_1);

                    if (pga_gain != pga_gain_1)
                    {
                            printk(KERN_WARNING
                               "Error in output stereo_pgaGain, mismatch written and read parametrs: written=%d read=%d\n",
                               pga_gain, pga_gain_1);
                            return PMIC_ERROR;
                    }
                }
        }

        /* Enable and than disable the output mixer */
        printk("Calling pmic_audio_output_enable_mixer(hStDAC).\n");
        CHECK_ERROR(pmic_audio_output_enable_mixer(hStDAC));

        /* Enable mono adder */
        ma_mode = MONO_ADD_LEFT_RIGHT;
        printk("Calling pmic_audio_output_enable_mono_adder(hStDAC, ma_mode). ma_mode=%d\n",
               ma_mode);
        CHECK_ERROR(pmic_audio_output_enable_mono_adder(hStDAC, ma_mode));

        /* Disable the output mono adder. */
        printk("Calling pmic_audio_output_disable_mono_adder(hStDAC).\n");
        CHECK_ERROR(pmic_audio_output_disable_mono_adder(hStDAC));

        /* Configure and get the mono adder output gain level. */
        for (ma_gain = MONOADD_GAIN_MINUS_6DB; ma_gain <= MONOADD_GAIN_0DB; ma_gain++)
        {
                printk
                    ("Calling pmic_audio_output_set_mono_adder_gain(hStDAC, ma_gain). ma_gain=%d\n",
                     ma_gain);
                ret_result = pmic_audio_output_set_mono_adder_gain(hStDAC, ma_gain);
                if (ret_result == -3){
                    printk("PMIC NOT SUPPORTED ON MONO ADDER GAIN = %d", ma_gain);
                }else{
                    CHECK_ERROR(pmic_audio_output_get_mono_adder_gain(hStDAC, &ma_gain_1));
                    printk
                        ("Calling pmic_audio_output_get_mono_adder_gain(hStDAC, &ma_gain_1). ma_gain_1=%d\n",
                         ma_gain_1);

                    if (ma_gain != ma_gain_1)
                    {
                        printk(KERN_WARNING
                               "Error in output mono_adder_gain, mismatch written and read parametrs: written=%d read=%d\n",
                               ma_gain, ma_gain_1);
                        return PMIC_ERROR;
                    }
                }
        }

        /* Enable and disable the phantom ground circuit that is used to help
           identify the type of headset that has been inserted. */
        printk("Calling pmic_audio_output_enable_phantom_ground(hStDAC).\n");
        ret_result = pmic_audio_output_enable_phantom_ground();
        if (ret_result == -3){
            printk("PMIC NOT SUPPORTED\n");
        }
        printk("Calling pmic_audio_output_disable_phantom_ground(hStDAC).\n");
        ret_result = pmic_audio_output_disable_phantom_ground();
        if (ret_result == -3){
            printk("PMIC NOT SUPPORTED\n");
        }

        printk("Calling pmic_audio_close(hStDAC).\n");
        CHECK_ERROR(pmic_audio_close(hStDAC));
        printk("Calling pmic_audio_close(extStereoIn).\n");
        CHECK_ERROR(pmic_audio_close(extStereoIn));

        return VT_rv;
}

/*====================*/
int VT_pmic_audio_test_input(void)
{
        int     VT_rv = PMIC_SUCCESS;

        PMIC_AUDIO_HANDLE hVCodec;
        PMIC_AUDIO_INPUT_CONFIG config,
                config_1;

        /* Record from a microphone using the Voice CODEC with an 8 kHz sampling rate. */
        printk("Calling pmic_audio_open(&hVCodec, VOICE_CODEC).\n");
        CHECK_ERROR(pmic_audio_open(&hVCodec, VOICE_CODEC));

        /* Got Voice CODEC handle, now select data bus and transmission protocol. */
        printk
            ("Calling pmic_audio_set_protocol(hVCodec, AUDIO_DATA_BUS_1, NETWORK_MODE, BUS_MASTER_MODE, USE_4_TIMESLOTS).\n");
        CHECK_ERROR(pmic_audio_set_protocol
                    (hVCodec, AUDIO_DATA_BUS_1, NETWORK_MODE, BUS_MASTER_MODE, USE_4_TIMESLOTS));

        /* Next select a mono (right channel only) audio input microphone. */
        printk("Calling pmic_audio_vcodec_set_mic(hVCodec, NO_MIC, MIC1_RIGHT_MIC_MONO).\n");
        CHECK_ERROR(pmic_audio_vcodec_set_mic(hVCodec, NO_MIC, MIC1_RIGHT_MIC_MONO));

        /* Also configure microphone bias and gain (mono right channel only). */
        printk("Calling pmic_audio_vcodec_enable_micbias(hVCodec, MIC_BIAS1).\n");
        CHECK_ERROR(pmic_audio_vcodec_enable_micbias(hVCodec, MIC_BIAS1));
        printk
            ("Calling pmic_audio_vcodec_set_record_gain(hVCodec, AMP_OFF, MIC_GAIN_0DB,CURRENT_TO_VOLTAGE, MIC_GAIN_PLUS_3DB).\n");
        CHECK_ERROR(pmic_audio_vcodec_set_record_gain
                    (hVCodec, AMP_OFF, MIC_GAIN_0DB, CURRENT_TO_VOLTAGE, MIC_GAIN_PLUS_3DB));

        /* Finally, turn on the microphone (mono right channel only). */
        printk
            ("Calling pmic_audio_vcodec_set_mic_on_off(hVCodec, MICROPHONE_OFF, MICROPHONE_ON).\n");
        CHECK_ERROR(pmic_audio_vcodec_set_mic_on_off(hVCodec, MICROPHONE_OFF, MICROPHONE_ON));

        /* Ready now to enable the Voice CODEC for recording. */
        printk("Calling pmic_audio_enable(hVCodec).\n");
        CHECK_ERROR(pmic_audio_enable(hVCodec));
        /* Clear/Disable the audio input section options. */
/*        printk("Calling pmic_audio_input_clear_config(hVCodec,config).\n");
        CHECK_ERROR(pmic_audio_input_clear_config(hVCodec, config));
*/
        printk("Calling pmic_audio_close(hVCodec).\n");
        CHECK_ERROR(pmic_audio_close(hVCodec));
        return VT_rv;
}

/*====================*/
int VT_pmic_audio_test_sdac(void)
{
        int     VT_rv = PMIC_SUCCESS;
        int     result_ret;
        PMIC_AUDIO_HANDLE hStDAC;

        PMIC_AUDIO_CLOCK_IN_SOURCE clockIn,
                clockIn_1;
        PMIC_AUDIO_STDAC_SAMPLING_RATE samplingRate,
                samplingRate_1;
        PMIC_AUDIO_STDAC_CLOCK_IN_FREQ clock_in_freq,
                clock_in_freq_1;
        PMIC_AUDIO_CLOCK_INVERT invert,
                invert_1;

        PMIC_AUDIO_STDAC_TIMESLOTS timeslot,
                timeslot_1;
        PMIC_AUDIO_STDAC_CONFIG config,
                config_1;

        /* Use the Stereo DAC to play a 44.1 kHz audio stream on the stereo headset. */
        printk("Calling pmic_audio_open(&hStDAC, STEREO_DAC).\n");
        CHECK_ERROR(pmic_audio_open(&hStDAC, STEREO_DAC));

        /* Got Stereo DAC handle, now select data bus and transmission protocol. */
        printk
            ("Calling pmic_audio_set_protocol(hStDAC, AUDIO_DATA_BUS_1, NETWORK_MODE, BUS_MASTER_MODE, USE_2_TIMESLOTS).\n");
        CHECK_ERROR(pmic_audio_set_protocol
                    (hStDAC, AUDIO_DATA_BUS_1, NETWORK_MODE, BUS_MASTER_MODE, USE_2_TIMESLOTS));

        /* Now also configure the sampling rate and PMIC clock source. */
        /* Try set and get all clockin source */
        samplingRate = STDAC_RATE_44_1_KHZ;

        #ifdef CONFIG_MXC_PMIC_SC55112
        clockIn = CLOCK_IN_DEFAULT;
        clock_in_freq = STDAC_CLI_13MHZ;
        #elif defined( CONFIG_MXC_PMIC_MC13783 )
        clockIn = CLOCK_IN_CLIA;
        clock_in_freq = STDAC_CLI_33_6MHZ;
        #endif

        printk("\nConfiguring the audio data sampling rate...\n");
        for (samplingRate = STDAC_RATE_8_KHZ; samplingRate <= STDAC_RATE_96_KHZ; samplingRate++)
        {
                printk
                    ("Calling pmic_audio_stdac_set_clock(hStDAC, clockIn, samplingRate, clock_in_freq, NO_INVERT). samplingRate=%d\n",
                     samplingRate);
                result_ret = pmic_audio_stdac_set_clock (hStDAC, clockIn, clock_in_freq, samplingRate, NO_INVERT);
                if (result_ret == -3){
                    printk("PMIC NOT SUPPORTED\n");
                }else{
                    CHECK_ERROR(pmic_audio_stdac_get_clock
                                (hStDAC, &clockIn_1, &samplingRate_1, &clock_in_freq_1, &invert_1));
                    printk
                        ("Calling pmic_audio_stdac_get_clock(hStDAC, &clockIn_1, &samplingRate_1, &samplingRate_1, &invert_1). samplingRate_1=%d\n",
                         samplingRate_1);
                }
        }

        /* BEGIN rxtx_timeslot() tests */
        /* Set and get the Stereo DAC primary audio channel timeslot. */
        timeslot = USE_TS0_TS1;

        printk("\nCalling pmic_audio_stdac_set_rxtx_timeslot(hStDAC, timeslot).\n");
        CHECK_ERROR(pmic_audio_stdac_set_rxtx_timeslot(hStDAC, timeslot));
        CHECK_ERROR(pmic_audio_stdac_get_rxtx_timeslot(hStDAC, &timeslot_1));
        printk("Calling pmic_audio_stdac_get_rxtx_timeslot(hStDAC, &timeslot_1). timeslot_1=%d\n",
               timeslot_1);

        if (timeslot != timeslot_1)
        {
                printk(KERN_WARNING
                       "Error in stdac rxtx_timeslot, mismatch written and read parametrs: written=%d read=%d\n",
                       timeslot, timeslot_1);
                return PMIC_ERROR;
        }

        /* BEGIN audio_stdac_set/get/clear_config() tests */
        /* Set/Enable the Stereo DAC options. */
        config = STDAC_MASTER_CLOCK_OUTPUTS;
        printk("Calling pmic_audio_stdac_set_config(hStDAC, config). config=%d\n", config);
        CHECK_ERROR(pmic_audio_stdac_set_config(hStDAC, config));

        /* Get the current Stereo DAC options */
        CHECK_ERROR(pmic_audio_stdac_get_config(hStDAC, &config_1));
        printk("Calling pmic_audio_stdac_get_config(hStDAC, &config_1). config_1=%d\n", config_1);

        if (config != config_1)
        {
                printk(KERN_WARNING
                       "Error in stdac config, mismatch written and read parametrs: written=%d read=%d\n",
                       config, config_1);
                return PMIC_ERROR;
        }

        /* Clear/Disable Stereo DAC options. */
        printk("Calling pmic_audio_stdac_clear_config(hStDAC, STDAC_MASTER_CLOCK_OUTPUTS).\n");
        CHECK_ERROR(pmic_audio_stdac_clear_config(hStDAC, STDAC_MASTER_CLOCK_OUTPUTS));

        printk("Calling pmic_audio_close(hStDAC).\n");
        CHECK_ERROR(pmic_audio_close(hStDAC));

        return VT_rv;
}

/*====================*/
int VT_pmic_audio_test_codec(void)
{
        int     VT_rv = PMIC_SUCCESS;

        PMIC_AUDIO_HANDLE hVCodec;
        PMIC_AUDIO_DATA_BUS busID_set,busID_get;
        PMIC_AUDIO_BUS_PROTOCOL protocol_set,protocol_get;
        PMIC_AUDIO_BUS_MODE masterslave_set,masterslave_get;
        PMIC_AUDIO_NUMSLOTS numslots_set,numslots_get;
        PMIC_AUDIO_CLOCK_IN_SOURCE clockin_set, clockin_get;
        PMIC_AUDIO_VCODEC_CLOCK_IN_FREQ clock_in_freq_set, clock_in_freq_get;
        PMIC_AUDIO_VCODEC_SAMPLING_RATE samplingrate_set, samplingrate_get;
        PMIC_AUDIO_CLOCK_INVERT invert_set, invert_get;
        PMIC_AUDIO_VCODEC_CONFIG config_set, config_get;
        PMIC_AUDIO_VCODEC_TIMESLOT timeslot_set, timeslot_get;
        PMIC_AUDIO_INPUT_PORT leftchannel_set,rightchannel_set,leftchannel_get,rightchannel_get;
        PMIC_AUDIO_INPUT_MIC_STATE leftchannel_state_set, rightchannel_state_set,leftchannel_state_get, rightchannel_state_get;

        printk("Calling pmic_audio_open(&hVCodec, VOICE_CODEC).\n");
        CHECK_ERROR(pmic_audio_open(&hVCodec, VOICE_CODEC));

        /* Got Voice CODEC handle, now select data bus and transmission protocol. */
        printk("Calling pmic_audio_set_protocol.\n");
        for (busID_set=AUDIO_DATA_BUS_1;busID_set<=AUDIO_DATA_BUS_2;busID_set++)
        {
            for (protocol_set=NETWORK_MODE;protocol_set<=I2S_MODE;protocol_set++)
            {
#ifdef CONFIG_MXC_PMIC_SC55112
                /* SC55112 Supports only Network Mode */
                if(protocol_set!=NETWORK_MODE)
                    continue;
#endif
                for (masterslave_set=BUS_MASTER_MODE;masterslave_set<=BUS_SLAVE_MODE;masterslave_set++)
                {
                    numslots_set=USE_4_TIMESLOTS;
                    CHECK_ERROR(pmic_audio_set_protocol(hVCodec, busID_set, protocol_set, masterslave_set, numslots_set));
                    CHECK_ERROR(pmic_audio_get_protocol(hVCodec, &busID_get, &protocol_get, &masterslave_get, &numslots_get));
                    if((busID_set!=busID_get)||(protocol_set!=protocol_get)||(masterslave_set!=masterslave_get)||(numslots_set!=numslots_get))
                    {
                        printk("Error in pmic_audio_set_protocol \n");
                        printk("busID=%d, protocol=%d, masterslave=%d, numslots=%d",busID_set, protocol_set, masterslave_set, numslots_set);
                        return PMIC_ERROR;
                    }
                }
            }
        }

        /* Try set and get all clockin source */
        for(clockin_set=CLOCK_IN_DEFAULT;clockin_set<=CLOCK_IN_CLKIN;clockin_set++)
        {
#ifdef CONFIG_MXC_PMIC_MC13783
            if (clockin_set != CLOCK_IN_CLIA || clockin_set != CLOCK_IN_CLIB)
                continue;
#else
            if (clockin_set != CLOCK_IN_DEFAULT || clockin_set != CLOCK_IN_CLKIN)
                continue;
#endif
            /* Sample rates */
            for (samplingrate_set = VCODEC_RATE_8_KHZ; samplingrate_set<=VCODEC_RATE_16_KHZ; samplingrate_set++)
            {
            /* Invert */
                for (invert_set=NO_INVERT;invert_set<=INVERT_FRAMESYNC;invert_set++)
                {
                    /* Clock in freq. */
                    for(clock_in_freq_set=VCODEC_CLI_13MHZ;clock_in_freq_set<=VCODEC_CLI_33_6MHZ;clock_in_freq_set++)
                    {
#ifdef CONFIG_MXC_PMIC_SC55112
                        if (samplingrate_set == VCODEC_RATE_16_KHZ && clock_in_freq_set==VCODEC_CLI_13MHZ)
                            continue;
#endif
                        printk("Calling pmic_audio_vcodec_set_clock \n");
                        CHECK_ERROR(pmic_audio_vcodec_set_clock(hVCodec,clockin_set,clock_in_freq_set,samplingrate_set,invert_set));
                        CHECK_ERROR(pmic_audio_vcodec_get_clock(hVCodec,&clockin_get,&clock_in_freq_get,&samplingrate_get,&invert_get));
                        if((clockin_set!=clockin_get)||(clock_in_freq_set!=clock_in_freq_get)||(samplingrate_set!=samplingrate_get)||(invert_set!=invert_set))
                        {
                            printk("Error in pmic_audio_vcodec_set_clock \n");
                            printk("clockin=%d,clock_in_freq=%d,samplingrate=%d,invert=%d,",clockin_set,clock_in_freq_set,samplingrate_set,invert_set);
                            return PMIC_ERROR;
                        }
                    }
                }
            }
        }

        /* BEGIN audio_vcodec_set/get/clear_config() tests */
        /* Set/Enable the vcodec options. */
        for (config_set = DITHERING; config_set <= TRISTATE_TS; config_set <<= 1)
        {
#ifdef CONFIG_MXC_PMIC_SC55112
            if (config_set == ANALOG_LOOPBACK)
                continue;
#endif
            printk("ANALOG_LOOPBACK==%d \n\n",ANALOG_LOOPBACK);
            printk("Calling pmic_audio_vcodec_set_config(hVCodec, config)\n");
            CHECK_ERROR(pmic_audio_vcodec_set_config(hVCodec, config_set));

            /* Get the current vcodec options */
            CHECK_ERROR(pmic_audio_vcodec_get_config(hVCodec, &config_get));
            /* Dont make the comparisions, coz the written value is shifted by
 * offset in Driver */
/*            if (config_set != config_get)
            {
                printk(KERN_WARNING "Error in vcodec config, mismatch written and read parametrs: written=%d read=%d\n",config_set, config_get);
                return PMIC_ERROR;
            }*/

            printk("Calling pmic_audio_vcodec_clear_config(hVCodec, config).\n");
            CHECK_ERROR(pmic_audio_vcodec_clear_config(hVCodec, config_set));
        }

        /* Begin Enable/Disable the Voice CODEC bypass audio pathway */
        printk("Calling pmic_audio_vcodec_enable_bypass(hVCodec).\n");
        CHECK_ERROR(pmic_audio_vcodec_enable_bypass(hVCodec));
        printk("Calling pmic_audio_vcodec_disable_bypass(hVCodec).\n");
        CHECK_ERROR(pmic_audio_vcodec_disable_bypass(hVCodec));

        /* Set the Voice CODEC primary audio channel timeslot */
        for (timeslot_set=USE_TS0;timeslot_set<=USE_TS3;timeslot_set++)
        {
#ifdef CONFIG_MXC_PMIC_SC55112
            if (timeslot_set != USE_TS0)
                continue;
#endif
            CHECK_ERROR(pmic_audio_vcodec_set_rxtx_timeslot(hVCodec,timeslot_set));
            CHECK_ERROR(pmic_audio_vcodec_get_rxtx_timeslot(hVCodec,&timeslot_get));

            if(timeslot_set != timeslot_get)
            {
                printk(KERN_WARNING "Error in vcodec primary rxtx timeslot, mismatch written and read parametrs: written=%d read=%d\n",timeslot_set,timeslot_get);
                return PMIC_ERROR;
            }
        }
/* SC55112 Does not support secondary slot*/
#ifdef CONFIG_MXC_PMIC_MC13783
        /* Set the Voice CODEC secondary audio channel timeslot */
        for (timeslot_set=USE_TS0;timeslot_set<=USE_TS3;timeslot_set++)
        {
            if (timeslot_set == USE_TS3)
                continue; /*BOTH  primary and secondary channles are same.*/

            CHECK_ERROR(pmic_audio_vcodec_set_secondary_txslot(hVCodec,timeslot_set));
            CHECK_ERROR(pmic_audio_vcodec_get_secondary_txslot(hVCodec,&timeslot_get));

            if(timeslot_set != timeslot_get)
            {
                printk(KERN_WARNING "Error in vcodec secondary rxtx timeslot, mismatch written and read parametrs: written=%d read=%d\n",timeslot_set,timeslot_get);
                return PMIC_ERROR;
            }
        }

        /* Try to set the Voice CODEC primary and secondary audio channel with same timeslot */
        for (timeslot_set=USE_TS0;timeslot_set<=USE_TS3;timeslot_set++)
        {
            CHECK_ERROR(pmic_audio_vcodec_set_rxtx_timeslot(hVCodec,timeslot_set));
            CHECK_ERROR(pmic_audio_vcodec_get_rxtx_timeslot(hVCodec,&timeslot_get));
            if(timeslot_set != timeslot_get)
            {
                printk(KERN_WARNING "Error in vcodec primary rxtx timeslot, mismatch written and read parametrs: written=%d read=%d\n",timeslot_set,timeslot_get);
                return PMIC_ERROR;
            }
            if( PMIC_PARAMETER_ERROR != (pmic_audio_vcodec_set_secondary_txslot(hVCodec,timeslot_set)))
            {
                printk(KERN_WARNING "Error in vcodec primary and secondary rxtx timeslot, it allows to write the %d timeslot on both primary and secondary. ERROR IN DRIVER \n",timeslot_set);
                return PMIC_ERROR;
            }
        }
#endif

        printk("Calling pmic_audio_close(hVCodec).\n");
        CHECK_ERROR(pmic_audio_close(hVCodec));

        return VT_rv;
}

/*====================*/
int VT_pmic_audio_test_bus(void)
{
        PMIC_AUDIO_HANDLE hStDAC,
                hVCodec;
        PMIC_AUDIO_DATA_BUS busID,
                busID_1;
        PMIC_AUDIO_BUS_PROTOCOL protocol,
                protocol_1;
        PMIC_AUDIO_BUS_MODE masterSlave,
                masterSlave_1;
        PMIC_AUDIO_NUMSLOTS numSlots_1;
        int     VT_rv = PMIC_SUCCESS;

        printk("\nTesting the bus (with Stereo DAC handle)...\n");
        /* Use the Stereo DAC to play a 44.1 kHz audio stream on the stereo headset. */
        printk("Calling pmic_audio_open(&hStDAC, STEREO_DAC).\n");
        CHECK_ERROR(pmic_audio_open(&hStDAC, STEREO_DAC));

        /* Got Stereo DAC handle, now select data bus and transmission protocol. */
        /* Try set all data buses */
        for (busID = AUDIO_DATA_BUS_1; busID <= AUDIO_DATA_BUS_2; busID++)
        {
                printk
                    ("Calling pmic_audio_set_protocol(hStDAC, busID, NETWORK_MODE, BUS_MASTER_MODE, USE_2_TIMESLOTS). busID=%d\n",
                     busID);
                CHECK_ERROR(pmic_audio_set_protocol
                            (hStDAC, busID, NETWORK_MODE, BUS_MASTER_MODE, USE_2_TIMESLOTS));
                CHECK_ERROR(pmic_audio_get_protocol
                            (hStDAC, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1));
                printk
                    ("Calling pmic_audio_get_protocol(hStDAC, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1). busID_1=%d\n",
                     busID_1);

                if (busID != busID_1)
                {
                        printk(KERN_WARNING
                               "Error in audio protocol (Stereo DAC), mismatch written and read parametrs: written=%d read=%d\n",
                               busID, busID_1);
                        VT_rv = PMIC_ERROR;
                }
        }

        /* Try set all protocols */
        for (protocol = NORMAL_MSB_JUSTIFIED_MODE; protocol <= I2S_MODE; protocol++)
        {
                printk
                    ("Calling pmic_audio_set_protocol(hStDAC, AUDIO_DATA_BUS_1, protocol, BUS_MASTER_MODE, USE_2_TIMESLOTS). protocol=%d\n",
                     protocol);
                CHECK_ERROR(pmic_audio_set_protocol
                            (hStDAC, AUDIO_DATA_BUS_1, protocol, BUS_MASTER_MODE, USE_2_TIMESLOTS));
                CHECK_ERROR(pmic_audio_get_protocol
                            (hStDAC, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1));
                printk
                    ("Calling pmic_audio_get_protocol(hStDAC, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1). protocol_1=%d\n",
                     protocol_1);

                if (protocol != protocol_1)
                {
                        printk(KERN_WARNING
                               "Error in audio protocol (Stereo DAC), mismatch written and read parametrs: written=%d read=%d\n",
                               protocol, protocol_1);
                        VT_rv = PMIC_ERROR;
                }
        }

        /* Try set all bus modes */
        masterSlave = BUS_MASTER_MODE;
        printk
            ("Calling pmic_audio_set_protocol(hStDAC, AUDIO_DATA_BUS_1, NETWORK_MODE, masterSlave, USE_2_TIMESLOTS). masterSlave=%d\n",
             masterSlave);
        CHECK_ERROR(pmic_audio_set_protocol
                    (hStDAC, AUDIO_DATA_BUS_1, NETWORK_MODE, masterSlave, USE_2_TIMESLOTS));
        CHECK_ERROR(pmic_audio_get_protocol
                    (hStDAC, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1));
        printk
            ("Calling pmic_audio_get_protocol(hStDAC, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1). masterSlave_1=%d\n",
             masterSlave_1);

        if (masterSlave != masterSlave_1)
        {
                printk(KERN_WARNING
                       "Error in audio protocol (Stereo DAC), mismatch written and read parametrs: written=%d read=%d\n",
                       masterSlave, masterSlave_1);
                VT_rv = PMIC_ERROR;
        }

        printk("Calling pmic_audio_close(hStDAC).\n");
        CHECK_ERROR(pmic_audio_close(hStDAC));

        printk("\nTesting the bus (with Voice codec handle)...\n");

        printk("Calling pmic_audio_open(&hVCodec, VOICE_CODEC).\n");
        CHECK_ERROR(pmic_audio_open(&hVCodec, VOICE_CODEC));

        /* Try set all data buses */
        protocol = NETWORK_MODE;
        for (busID = AUDIO_DATA_BUS_1; busID <= AUDIO_DATA_BUS_2; busID++)
        {
                printk
                    ("Calling pmic_audio_set_protocol(hVCodec, busID, NETWORK_MODE, BUS_MASTER_MODE, USE_4_TIMESLOTS). busID=%d\n",
                     busID);
                CHECK_ERROR(pmic_audio_set_protocol
                            (hVCodec, busID, protocol, BUS_MASTER_MODE, USE_4_TIMESLOTS));
                CHECK_ERROR(pmic_audio_get_protocol
                            (hVCodec, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1));
                printk
                    ("Calling pmic_audio_get_protocol(hVCodec, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1). busID_1=%d\n",
                     busID_1);

                if (busID != busID_1)
                {
                        printk(KERN_WARNING
                               "Error in audio protocol (Voice codec), mismatch written and read parametrs: written=%d read=%d\n",
                               busID, busID_1);
                        VT_rv = PMIC_ERROR;
                }
        }

        /* Try set all protocols */
        protocol = NETWORK_MODE;
        printk
            ("Calling pmic_audio_set_protocol(hVCodec, AUDIO_DATA_BUS_1, protocol, BUS_MASTER_MODE, USE_4_TIMESLOTS). protocol=%d\n",
             protocol);
        CHECK_ERROR(pmic_audio_set_protocol
                    (hVCodec, AUDIO_DATA_BUS_1, protocol, BUS_MASTER_MODE, USE_4_TIMESLOTS));
        CHECK_ERROR(pmic_audio_get_protocol
                    (hVCodec, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1));
        printk
            ("Calling pmic_audio_get_protocol(hVCodec, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1). protocol_1=%d\n",
             protocol_1);

        if (protocol != protocol_1)
        {
                printk(KERN_WARNING
                       "Error in audio protocol (Voice codec), mismatch written and read parametrs: written=%d read=%d\n",
                       protocol, protocol_1);
                VT_rv = PMIC_ERROR;
        }

        for (masterSlave = BUS_MASTER_MODE; masterSlave <= BUS_SLAVE_MODE; masterSlave++)
        {
                printk
                    ("Calling pmic_audio_set_protocol(hVCodec, AUDIO_DATA_BUS_1, NETWORK_MODE, masterSlave, USE_4_TIMESLOTS). masterSlave=%d\n",
                     masterSlave);
                CHECK_ERROR(pmic_audio_set_protocol
                            (hVCodec, AUDIO_DATA_BUS_1, NETWORK_MODE, masterSlave,
                             USE_4_TIMESLOTS));
                CHECK_ERROR(pmic_audio_get_protocol
                            (hVCodec, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1));
                printk
                    ("Calling pmic_audio_get_protocol(hVCodec, &busID_1, &protocol_1, &masterSlave_1, &numSlots_1). masterSlave_1=%d\n",
                     masterSlave_1);

                if (masterSlave != masterSlave_1)
                {
                        printk(KERN_WARNING
                               "Error in audio protocol (Voice codec), mismatch written and read parametrs: written=%d read=%d\n",
                               masterSlave, masterSlave_1);
                        VT_rv = PMIC_ERROR;
                }
        }

        printk("Calling pmic_audio_close(hVCodec).\n");
        CHECK_ERROR(pmic_audio_close(hVCodec));

        return VT_rv;

}

/*====================*/
static int pmic_test_open(struct inode *inode, struct file *filp)
{
        return 0;
}

static ssize_t pmic_test_read(struct file *file, char *buf, size_t count, loff_t * ppos)
{
        return 0;
}

static ssize_t pmic_test_write(struct file *filp, const char *buf, size_t count, loff_t * ppos)
{
        return 0;
}

/*====================*/
static int pmic_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                           unsigned long arg)
{
        switch (cmd)
        {
        case PMIC_AUDIO_TEST_OUTPUT:
                return VT_pmic_audio_test_output();
        case PMIC_AUDIO_TEST_INPUT:
                return VT_pmic_audio_test_input();
        case PMIC_AUDIO_TEST_SDAC:
                return VT_pmic_audio_test_sdac();
        case PMIC_AUDIO_TEST_CODEC:
                return VT_pmic_audio_test_codec();
        case PMIC_AUDIO_TEST_BUS:
                return VT_pmic_audio_test_bus();
        }
        return -EINVAL;
}

/*====================*/
static int pmic_test_release(struct inode *inode, struct file *filp)
{
        return 0;
}

/*======================*/
static struct file_operations pmic_test_fops =
{
        owner:THIS_MODULE,
        open:pmic_test_open,
        release:pmic_test_release,
        read:pmic_test_read,
        write:pmic_test_write,
        ioctl:pmic_test_ioctl
};

/*====================*/
static int __init pmic_test_init(void)
{
        int     res;

        printk("PMIC Audio Test: creating virtual device\n");
        res = register_chrdev(236, PMIC_AUDIO_DEV, &pmic_test_fops);

        if (res < 0)
        {
                printk(KERN_WARNING "PMIC Audio  Test: unable to register the device\n");
                return res;
        }

        pmic_audio_class = class_create(THIS_MODULE, PMIC_AUDIO_DEV);
        if (IS_ERR(pmic_audio_class))
        {
                printk(KERN_ALERT "class simple created failed\n");
                goto err_out;
        }

        if (IS_ERR(class_device_create(pmic_audio_class,NULL, MKDEV(236, 0), NULL, PMIC_AUDIO_DEV)))
        {
                printk(KERN_ALERT "class simple add failed\n");
                goto err_out;
        }

        //---devfs_mk_cdev(MKDEV(236, 0), S_IFCHR | S_IRUGO | S_IWUGO, PMIC_AUDIO_DEV);
        return 0;

        err_out:
        printk(KERN_ERR "PMIC_AUDIO : error creating audio test module class.\n");
        class_device_destroy(pmic_audio_class, MKDEV(236, 0));
        class_destroy(pmic_audio_class);
        unregister_chrdev(236, PMIC_AUDIO_DEV);
        return -1;
}

/*====================*/
static void __exit pmic_test_exit(void)
{
        unregister_chrdev(236, PMIC_AUDIO_DEV);
        //---devfs_remove(PMIC_AUDIO_DEV);
        class_device_destroy(pmic_audio_class, MKDEV(236, 0));
        class_destroy(pmic_audio_class);
        printk("PMIC Audio Test: removing virtual device\n");
}

/*====================*/

module_init(pmic_test_init);
module_exit(pmic_test_exit);

MODULE_DESCRIPTION("Test Module for PMIC Audio driver");
MODULE_LICENSE("GPL");
