/*****************************************************************************
 *
 * Motorola Inc.
 * (c) Copyright 2005 Motorola, Inc.
 * ALL RIGHTS RESERVED.
 *
 *****************************************************************************
 * File Name: aacplus_main.h
 *
 * Description: header file for aacplus_main.c .
 *
 ****************************** Change History********************************
 * 
 *    DD/MM/YYYY     Code Ver     Description                   Author
 *   -----------     --------     -----------                   ------
 *    28/06/2005      01          File  Created                 Webber Wang
 *
 ******************************************************************************/

#ifndef _AACPLUS_MISC_H_
#define _AACPLUS_MISC_H_

#include "aacplus_dec_interface.h"

#ifndef SFract_C
typedef long sfract_C;
typedef long fract_C;
#define SFract_C                  sfract_C
#endif

#define CALC_STOP_BAND  /* copy from FFR_sbrdeclib/src/sbrdecsettings.h */
#define BS_BUF_SIZE               AACPD_INPUT_BUFFER_SIZE
#define MAX_ENC_BUF_SIZE          400*BS_BUF_SIZE*24

#define LONG_BOUNDARY             4

#if defined(MOT_AAC) && !defined(AAC_ONLY)

#ifndef FRAME_INFO
#define MAX_ENVELOPES           5
#define MAX_NOISE_ENVELOPES     2

#ifdef PARAMETRICSTEREO
#define NO_SERIAL_ALLPASS_LINKS 3   /* this is defined in ps_dec.h */
#endif

typedef struct FRAME_INFO
{
        unsigned char frameClass;               /*!< Select grid type */
        unsigned char nEnvelopes;               /*!< Number of envelopes */
        unsigned char borders[MAX_ENVELOPES+1]; /*!< Envelope borders (in SBR-timeslots, e.g. mp3PRO: 0..11) */
        unsigned char freqRes[MAX_ENVELOPES];   /*!< Frequency resolution for each envelope (0=low, 1=high) */
        char          tranEnv;                  /*!< Transient envelope, -1 if none */
        unsigned char nNoiseEnvelopes;          /*!< Number of noise envelopes */
        unsigned char bordersNoise[MAX_NOISE_ENVELOPES+1];/*!< borders of noise envelopes */
} FRAME_INFO;
#endif

#ifndef SBR_HEADER_DATA
typedef enum
{
        SBR_NOT_INITIALIZED,
                UPSAMPLING,
                SBR_ACTIVE
} SBR_SYNC_STATE;

typedef enum
{
        UNDEFINED_CHANNEL_MODE,
                MONO,
                STEREO,
                LC_STEREO,
#ifdef PARAMETRICSTEREO
                PS_STEREO
#endif
} CHANNEL_MODE;

#define MAX_NUM_LIMITERS                12
#define MAX_FREQ_COEFFS                 48
#define MAX_NOISE_COEFFS                 5

typedef struct
{
        unsigned char nSfb[2];           /*!< Number of SBR-bands for low and high freq-resolution */
        unsigned char nNfb;              /*!< Actual number of noise bands to read from the bitstream*/
        unsigned char numMaster;         /*!< Number of SBR-bands in v_k_master */
        unsigned char lowSubband;        /*!< QMF-band where SBR frequency range starts */
        unsigned char highSubband;       /*!< QMF-band where SBR frequency range ends */
        unsigned char limiterBandTable[MAX_NUM_LIMITERS+1]; /*!< Limiter band table. */
        unsigned char noLimiterBands;    /*!< Number of limiter bands. */
        unsigned char nInvfBands;        /*!< Number of bands for inverse filtering */
        unsigned char *freqBandTable[2]; /*!< Pointers to freqBandTableLo and freqBandTableHi */
        unsigned char freqBandTableLo[MAX_FREQ_COEFFS/2+1];
        /*!< Mapping of SBR bands to QMF bands for low frequency resolution */
        unsigned char freqBandTableHi[MAX_FREQ_COEFFS+1];
        /*!< Mapping of SBR bands to QMF bands for high frequency resolution */
        unsigned char freqBandTableNoise[MAX_NOISE_COEFFS+1];
        /*!< Mapping of SBR noise bands to QMF bands */
        unsigned char v_k_master[MAX_FREQ_COEFFS+1];
        /*!< Master BandTable which freqBandTable is derived from */
} FREQ_BAND_DATA;

typedef FREQ_BAND_DATA *HANDLE_FREQ_BAND_DATA;

typedef struct SBR_HEADER_DATA
{
        SBR_SYNC_STATE syncState;          /*!< The current initialization status of the header */
        unsigned char frameErrorFlag;      /*!< Current frame data valid ? */
        unsigned char prevFrameErrorFlag;  /*!< Previous frame data valid ? */
        unsigned char numberTimeSlots;     /*!< Number of time slots */
        unsigned char timeStep;            /*!< Time resolution of SBR in QMF-slots */
        unsigned short codecFrameSize;     /*!< Core coder frame size in samples */
        unsigned short outSampleRate;      /*!< Output sampling frequency */
        
        /* Changes in these variables causes concealment */
        CHANNEL_MODE  channelMode;         /*!< Mono, pseudo-stereo or stereo */
        unsigned char ampResolution;       /*!< Amplitude resolution of envelope values (0: 1.5dB, 1: 3dB) */
        
        /* Changes in these variables causes a reset of the decoder */
        unsigned char startFreq;           /*!< Index for SBR start frequency */
        unsigned char stopFreq;            /*!< Index for SBR highest frequency */
        unsigned char xover_band;          /*!< Start index in #v_k_master[] used for dynamic crossover frequency */
        unsigned char freqScale;           /*!< 0: linear scale,  1-3 logarithmic scales */
        unsigned char alterScale;          /*!< Flag for coarser frequency resolution */
        unsigned char noise_bands;         /*!< Noise bands per octave, read from bitstream*/
        
        /* don't require reset */
        unsigned char limiterBands;        /*!< Index for number of limiter bands per octave */
        unsigned char limiterGains;        /*!< Index to select gain limit */
        unsigned char interpolFreq;        /*!< Select gain calculation method (1: per QMF channel, 0: per SBR band) */
        unsigned char smoothingLength;     /*!< Smoothing of gains over time (0: on  1: off) */
        
        HANDLE_FREQ_BAND_DATA hFreqBandData;  /*!< Pointer to struct #FREQ_BAND_DATA */
} SBR_HEADER_DATA;
#endif

#ifndef BITSTREAM_WATCH_ELEMENT
typedef struct
{
        int id;         /*!< Identification number for each element    */
        int bits;       /*!< Number of bits in bitstream               */
        char text[30];  /*!< Name to be printed on occurance           */
        int debugLevel; /*!< Messages are shown if the debugLevel fits */
        int min;        /*!< Minimum value that was read               */
        int max;        /*!< Maximum value that was read               */
        int cnt;        /*!< Occurance counter for this element        */
        int cntnew;     /*!< Number of different values read           */
        char interpretation[8][30];
} BITSTREAM_WATCH_ELEMENT;
#endif

#define SBR_TABLE_SIZE_PART_1     45

#ifndef CALC_STOP_BAND
#define SBR_TABLE_SIZE_PART_2     3
#else
#define SBR_TABLE_SIZE_PART_2     0
#endif  /* end CALC_STOP_BAND */

#ifdef HIGH_QUALITY_SBR
#define SBR_TABLE_SIZE_PART_3     4
#else
#define SBR_TABLE_SIZE_PART_3     0
#endif /* end HIGH_QUALITY_SBR */

#ifdef PARAMETRICSTEREO
#define SBR_TABLE_SIZE_PART_4     24
#else
#define SBR_TABLE_SIZE_PART_4     0
#endif  /* end PARAMETRICSTEREO */

#ifdef RECURSIVE_DCT
#define SBR_TABLE_SIZE_PART_5     13
#else
#define SBR_TABLE_SIZE_PART_5     0
#endif /* end RECURSIVE_DCT */

#define SBRD_TABLE_SIZE  (SBR_TABLE_SIZE_PART_1 + \
                          SBR_TABLE_SIZE_PART_2 + \
                          SBR_TABLE_SIZE_PART_3 + \
                          SBR_TABLE_SIZE_PART_4 + \
                          SBR_TABLE_SIZE_PART_5)
#else
#define SBRD_TABLE_SIZE           0
#endif

#define AACD_TABLE_SIZE           58

#define TABLE_SIZE       (AACD_TABLE_SIZE+SBRD_TABLE_SIZE)

extern const unsigned char sbr_start_freq_16[];
extern const unsigned char sbr_start_freq_22[];
extern const unsigned char sbr_start_freq_32[];
extern const unsigned char sbr_start_freq_44[];
extern const unsigned char sbr_start_freq_24[];
extern const unsigned char sbr_start_freq_48[];
#ifndef CALC_STOP_BAND
extern const unsigned char sbr_stop_freq_48[];
extern const unsigned char sbr_stop_freq_44[];
extern const unsigned char sbr_stop_freq_32[];
#endif
extern const struct FRAME_INFO sbr_frame_info1_16;
extern const struct FRAME_INFO sbr_frame_info2_16;
extern const struct FRAME_INFO sbr_frame_info4_16;
extern const struct SBR_HEADER_DATA sbr_defaultHeader;

extern const char sbr_huffBook_EnvLevel10T[][2];
extern const char sbr_huffBook_EnvLevel10F[][2];
extern const char sbr_huffBook_EnvBalance10T[][2];
extern const char sbr_huffBook_EnvBalance10F[][2];
extern const char sbr_huffBook_EnvLevel11T[][2];
extern const char sbr_huffBook_EnvLevel11F[][2];
extern const char sbr_huffBook_EnvBalance11T[][2];
extern const char sbr_huffBook_EnvBalance11F[][2];
extern const char sbr_huffBook_NoiseLevel11T[][2];
extern const char sbr_huffBook_NoiseBalance11T[][2];
extern const char sbr_limGains_e[];
extern const int  pHybridResolution[];
extern const SFract_C sbr_limGains_m[];
extern const SFract_C sbr_limiterBandsPerOctaveDiv4[];
extern const SFract_C sbr_smoothFilter[];
extern const SFract_C sbr_invIntTable[];
extern const SFract_C sbr_randomPhase[][2];
extern const SFract_C sbr_qmf_64[];

#ifdef HIGH_QUALITY_SBR
extern const SFract_C sbr_t_cos_L32[];
extern const SFract_C sbr_t_sin_L32[];
extern const SFract_C sbr_t_cos_L32_ds[];
extern const SFract_C sbr_t_sin_L32_ds[];
#endif /* end HIGH_QUALITY_SBR */

#ifdef PARAMETRICSTEREO
extern const fract_C aRevLinkDecaySer[];
extern const char aRevLinkDelaySer[];
extern const char groupBorders[];
extern const char groupShift[];
extern const char bins2groupMap[];
extern const char aHybridToBin[];
extern const char aQmfToBin[];
#ifdef USE_IIR_FILTER
extern const SFract_C aaRealSubQmfCoeffsB[][NO_IIR_B_COEFFS];
extern const SFract_C aaRealSubQmfCoeffsA[][NO_IIR_A_COEFFS];
extern const SFract_C aaImagSubQmfCoeffsB[][NO_IIR_B_COEFFS];
extern const SFract_C aaImagSubQmfCoeffsA[][NO_IIR_A_COEFFS];
extern const SFract_C aaRealQmfCoeffsB[][NO_IIR_B_COEFFS];
extern const SFract_C aaRealQmfCoeffsA[][NO_IIR_A_COEFFS];
extern const SFract_C aaImagQmfCoeffsB[][NO_IIR_B_COEFFS];
extern const SFract_C aaImagQmfCoeffsA[][NO_IIR_A_COEFFS];
#else
extern const SFract_C aFractDelayPhaseFactorReQmf[];
extern const SFract_C aFractDelayPhaseFactorImQmf[];
extern const SFract_C aFractDelayPhaseFactorReSubQmf[];
extern const SFract_C aFractDelayPhaseFactorImSubQmf[];
extern const SFract_C aaFractDelayPhaseFactorSerReQmf[][NO_SERIAL_ALLPASS_LINKS];
extern const SFract_C aaFractDelayPhaseFactorSerImQmf[][NO_SERIAL_ALLPASS_LINKS];
extern const SFract_C aaFractDelayPhaseFactorSerReSubQmf[][NO_SERIAL_ALLPASS_LINKS];
extern const SFract_C aaFractDelayPhaseFactorSerImSubQmf[][NO_SERIAL_ALLPASS_LINKS];
#endif /* end USE_IIR_FILTER */
extern const SFract_C scaleFactors[];
extern const SFract_C scaleFactorsFine[];
extern const SFract_C alphas[];
extern const char aBookPsIidTimeDecode[][2];
extern const char aBookPsIidFreqDecode[][2];
extern const char aBookPsIccTimeDecode[][2];
extern const char aBookPsIccFreqDecode[][2];
extern const char aBookPsIidFineTimeDecode[][2];
extern const char aBookPsIidFineFreqDecode[][2];
#endif  /* end PARAMETRICSTEREO */

extern const SFract_C p2_6[];
extern const SFract_C p8_13[];
extern const SFract_C fftTwiddleTable[];
extern const SFract_C sin_twiddle_L32_III[];
extern const SFract_C sin_twiddle_L64_II[];
extern const SFract_C sin_twiddle_L32_II[];
extern const SFract_C cos_twiddle_L32_IV[];
extern const SFract_C sin_twiddle_L32_IV[];
extern const SFract_C alt_sin_twiddle_L32_IV[];
extern const SFract_C cos_twiddle_L64_IV[];
extern const SFract_C sin_twiddle_L64_IV[];
extern const SFract_C alt_sin_twiddle_L64_IV[];
extern const SFract_C trigData[];
extern const SFract_C logDualisTable[];
extern const SFract_C invTable[];
extern const SFract_C sqrtTable[];

#ifdef RECURSIVE_DCT
extern const SFract_C sbr_cos_twiddle_L04[];
extern const SFract_C sbr_cos_twiddle_L08[];
extern const SFract_C sbr_cos_twiddle_L16[];
extern const SFract_C sbr_cos_twiddle_L32[];
extern const SFract_C sbr_sin_twiddle_L04[];
extern const SFract_C sbr_sin_twiddle_L08[];
extern const SFract_C sbr_sin_twiddle_L16[];
extern const SFract_C sbr_sin_twiddle_L32[];
extern const SFract_C sbr_alt_sin_twiddle_L04[];
extern const SFract_C sbr_alt_sin_twiddle_L08[];
extern const SFract_C sbr_alt_sin_twiddle_L16[];
extern const SFract_C sbr_alt_sin_twiddle_L32[];
extern dct4Twiddle dct4TwiddleTable[];
#endif /* end RECURSIVE_DCT */

#if defined(MOT_AAC) && !defined(AAC_ONLY)
extern const BITSTREAM_WATCH_ELEMENT *bitele;
#endif

#endif /*_AACPLUS_MISC_H_ */
