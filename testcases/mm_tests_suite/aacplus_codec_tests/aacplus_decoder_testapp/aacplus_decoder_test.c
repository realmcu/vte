/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file aacplus_decoder_test.c

@brief VTE C header template

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  06/09/2005   TLSbo53247   Initial version
D.Simakov / smkd001c  12/09/2005   TLSBo53247   Memory leaks and bit-matching were fixed
=============================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "aacplus_decoder_test.h"

#include <pthread.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>

/* AAC+ Decoder API. */
#include <aacplus_dec_interface.h>

#include "stuff/aacplus_misc.h"
#include "stuff/aacplus_aux.h"
#include "stuff/aacplus_portio.h"
#include "stuff/mem_stat.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define MAX_THREADS 4
#define SAFE_DELETE(p) {if(p){Util_FreeMem(p);p=NULL;}}
#define NA "n/a"
#define M(m){printf("<<<--- %s --->>>\n",m);fflush(stdout);}


/**********************************************************************
* Macro name:  CALL_AACPLUS_DECODER()
* Description: Macro checks for any error in function execution
               based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_AACPLUS_DECODER(AACPlusDecRoutine, name)   \
    pHandler->mLastAACPlusDecError = AACPlusDecRoutine; \
    if( (pHandler->mLastAACPlusDecError != AACPD_ERROR_NO_ERROR) )\
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File %s, line %d]", __FUNCTION__, name, pHandler->mLastAACPlusDecError, __FILE__, __LINE__ );\
        return TFAIL;\
    }

#define CALL_AACPLUS_DECODER_NR(AACPlusDecRoutine, name)   \
    pHandler->mLastAACPlusDecError = AACPlusDecRoutine; \
    if( (pHandler->mLastAACPlusDecError != AACPD_ERROR_NO_ERROR) && (AACPD_ERROR_EOF!=pHandler->mLastAACPlusDecError) )\
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File %s, line %d]", __FUNCTION__, name, pHandler->mLastAACPlusDecError, __FILE__, __LINE__ );\
    }

#define TST_RESM(s,format,...) \
{\
    if( LOAD != gTestappConfig.mTestCase ) \
        pthread_mutex_lock( &gMutex );\
    tst_resm((s), (format), ##__VA_ARGS__);\
    if( LOAD != gTestappConfig.mTestCase ) \
        pthread_mutex_unlock( &gMutex );\
}

#define DPRINTF(fmt,...) /*printf((fmt), ##__VA_ARGS__)*/

#if defined(USE_AUDIOLIB) || defined(USELIB_AF)
#error This test case version doesn't have full support of USE_AUDIOLIB and USELIB_AF
#endif        

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct
{
        char mInputFileName[MAX_STR_LEN];
        char mOutputFileName[MAX_STR_LEN];
        char mReferenceFileName[MAX_STR_LEN]; 
        
        //////////////////////////////////////////////////////////////////////////
        unsigned int mEntryIndex;
} sHandlerParams;

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct 
{
        unsigned long           mIndex;
        
        const sHandlerParams  * mpParams;    
        FILE                  * mpInputStream;
        FILE                  * mpOutputStream;
        
        int                     mLastAACPlusDecError; 
        
        AACPD_OutputFmtType     mpOutBuffer[Chans][AACP_FRAME_SIZE];
        unsigned char         * mpInpBuffer;    
        size_t                  mOutBufferSz;
        size_t                  mInpBufferSz;     
        
        AACPD_Decoder_Config    mDecConfig; 
        AACPD_Decoder_info      mDecInfo;
        AACPD_Block_Params      mDecBlockParams;
        
        sBitstream              mBitstream;
        sPortIO                 mPortIo;
        
        pthread_t               mThreadID;          
        int                     mIsThreadFinished;
        int                     mLtpRetval;        
} sAacPlusDecoderHandler;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static sAacPlusDecoderHandler gAacPlusDecHandlers[ MAX_THREADS ];
static int                    gNumThreads                     = 1;    			 
static int                    gThreadWithFocus                = -1;   	
static sLinkedList         *  gpParamsList                    = NULL;
static pthread_mutex_t        gMutex;
static void                *  gpAppTables[TABLE_SIZE];
/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void         AACPlus_InitTables ( void );
AACPD_INT8   AACPlus_SwapBuf    ( AACPD_UCHAR ** ppNewBuf, AACPD_UINT32 * pNewBufLen, AACPD_Decoder_Config * pDecConfig );

int  RunDecoder     ( void * ptr );
int  TestEngine     ( sAacPlusDecoderHandler * pHandler );
void DoDataOutput   ( sAacPlusDecoderHandler * pHandler );
void ResetHandler   ( sAacPlusDecoderHandler * pHandler );
void CleanupHandler ( sAacPlusDecoderHandler * pHandler );
int  DoBitmatch     ( sAacPlusDecoderHandler * pHandler );
int  DoFilesExist   ( const char * fname1, const char * fname2 );
void HogCpu         ( void );
void MakeEntry      ( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry );

/* Test cases */
int NominalFunctionalityTest ( void );
int RobustnessTest           ( void );
int ReLocatabilityTest       ( void );  
int ReEntranceTest           ( void );
int PreEmptionTest           ( void );
int EnduranceTest            ( void );
int LoadTest                 ( void );

/* Helper functions. */
int             Util_StrICmp        ( const char * s1, const char *s2 );        
unsigned char * Util_ReadFile       ( const char * filename, size_t * pSz );
void            Util_SwapBytes      ( short * pWords, int count );
void          * Util_AllocMem       ( size_t sz, int memType );
void            Util_FreeMem        ( void * pPtr );
void            Util_SetupBs        ( sAacPlusDecoderHandler * pHandler );
int             Util_PrepareBs      ( sBitstream * pBitstream );
void            Util_UpdateBsStatus ( sBitstream * pBitstream, int nBytesUsed );

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/
int VT_aacplus_decoder_setup( void )
{      
        pthread_mutex_init( &gMutex, NULL );
        
        int i;
        /* Reset all handlers. */
        for( i = 0; i < MAX_THREADS; ++i )
                ResetHandler( gAacPlusDecHandlers + i );    
        
        /**/
        gpParamsList = (sLinkedList*)malloc(sizeof(sLinkedList));
        gpParamsList->mpNext = NULL;
        gpParamsList->mpContent = NULL;
        
        if( !ParseConfig( gTestappConfig.mConfigFilename ) )        
        {
                TST_RESM( TWARN, "%s : can't parse the %s", 
                        __FUNCTION__, gTestappConfig.mConfigFilename );
                return TFAIL;
        }   
    
        /* Compute the actual number of threads. */
        if( RE_ENTRANCE == gTestappConfig.mTestCase || 
                PRE_EMPTION == gTestappConfig.mTestCase )
        {
                sLinkedList * pNode = gpParamsList;
                for( gNumThreads = 0; pNode && gNumThreads < MAX_THREADS; ++gNumThreads )
                        pNode = pNode->mpNext;
        }    
        
        /****************/
        /* Init tables. */
        /****************/
        AACPlus_InitTables();
        
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_aacplus_decoder_cleanup( void )
{    
        pthread_mutex_destroy( &gMutex );
        
        if( gpParamsList ) 
                LList_Delete( gpParamsList );
        
        int i;
        for( i = 0; i < MAX_THREADS; ++i )
        {
                CleanupHandler( gAacPlusDecHandlers + i );
                ResetHandler( gAacPlusDecHandlers + i );        
        }
        
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_aacplus_decoder_test( void )
{
    int rv = TFAIL;            
    
    switch( gTestappConfig.mTestCase )
    {
                case NOMINAL_FUNCTIONALITY:
                        rv = NominalFunctionalityTest();
                break;

                case ROBUSTNESS:
                        rv = RobustnessTest();
                break;

                case RELOCATABILITY:            
                        rv = ReLocatabilityTest();            
                break;       

                case RE_ENTRANCE:
                        rv = ReEntranceTest();
                break;

                case PRE_EMPTION:
                        rv = PreEmptionTest();
                break;                    

                case ENDURANCE:
                        rv = EnduranceTest();
                break;    

                case LOAD:
                        rv = LoadTest();
                break;                                    
            
                default:
                        TST_RESM( TINFO, "Wrong test case" );
                break;    
        }    
    
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
AACPD_INT8 AACPlus_SwapBuf( AACPD_UCHAR ** ppNewBuf,
                            AACPD_UINT32 * pNewBufLen,
                            AACPD_Decoder_Config * pDecConfig )
{
        /*********************************/
        /* Find the appropriate handler. */
        /*********************************/
        int i;
        sAacPlusDecoderHandler * pHandler = NULL;
        for( i = 0; i < MAX_THREADS; ++i )
        {
                void * pAddr1 = pDecConfig->aacpd_mem_info.mem_info_sub[0].app_base_ptr;
                void * pAddr2 = gAacPlusDecHandlers[i].mDecConfig.aacpd_mem_info.mem_info_sub[0].app_base_ptr;             
                if( pAddr1 == pAddr2 )
                {            
                        pHandler = &gAacPlusDecHandlers[i];
                        break;
                }
        }
        assert( pHandler );
        
        sBitstream * pBitstream = &pHandler->mBitstream;
        
        int len;
        
        if( pBitstream->mBitstreamCount <= 0 )
        {
                *ppNewBuf = NULL;
                *pNewBufLen = 0;
                return (AACPD_INT8)-1;
        }
        else
        {
                len = (pBitstream->mBitstreamCount > BS_BUF_SIZE) ? BS_BUF_SIZE : pBitstream->mBitstreamCount;
                *pNewBufLen = len;
                *ppNewBuf = (AACPD_UCHAR*)( pBitstream->mpBitstreamBuf + pBitstream->mBitstreamBufIndex );
                pBitstream->mBitstreamBufIndex += len;
                pBitstream->mBitstreamCount    -= len;
                pBitstream->mInBufDone         += len;
                pBitstream->mBytesSupplied     += len;
        }
        return 0;
}

/*================================================================================================*/
/*================================================================================================*/
void AACPlus_InitTables( void )
{
        /* table pointers initialization */
        gpAppTables[0] = (Lfract *) AACD_pow_4by3_table_lf;
        gpAppTables[1] = (int *) AACD_pred_max_bands_tbl;
        gpAppTables[2] = (unsigned short *) AACD_neg_mask;
        gpAppTables[3] = (short *) AACD_sgn_mask;
        gpAppTables[4] = (unsigned int *) AACD_hufftab_off;
        gpAppTables[5] = (int (*)[])AACD_tns_max_bands_tbl;
        gpAppTables[6] = (unsigned char **)AACD_huffstart;
        gpAppTables[7] = (Lfract *(*)[])AACD_windowPtr;
        gpAppTables[8] = (unsigned char *) AACD_HuffTable1;
        gpAppTables[9] = (unsigned char *) AACD_HuffTable2;
        gpAppTables[10] = (unsigned char *) AACD_HuffTable3;
        gpAppTables[11] = (unsigned char *) AACD_HuffTable4;
        gpAppTables[12] = (unsigned char *) AACD_HuffTable5;
        gpAppTables[13] = (unsigned char *) AACD_HuffTable6;
        gpAppTables[14] = (unsigned char *) AACD_HuffTable7;
        gpAppTables[15] = (unsigned char *) AACD_HuffTable8;
        gpAppTables[16] = (unsigned char *) AACD_HuffTable9;
        gpAppTables[17] = (unsigned char *) AACD_HuffTable10;
        gpAppTables[18] = (unsigned char *) AACD_HuffTableEsc;
        gpAppTables[19] = (unsigned char *) AACD_HuffTableScl;
        gpAppTables[20] = (SR_Info *) AACD_samp_rate_info;
        
        gpAppTables[21] = (short *) AACD_sfb_96_1024;
        gpAppTables[22] = (short *) AACD_sfb_96_128;
        gpAppTables[23] = (short *) AACD_sfb_64_1024;
        gpAppTables[24] = (short *) AACD_sfb_64_128;
        gpAppTables[25] = (short *) AACD_sfb_48_1024;
        gpAppTables[26] = (short *) AACD_sfb_48_128;
        gpAppTables[27] = (short *) AACD_sfb_32_1024;
        gpAppTables[28] = (short *) AACD_sfb_24_1024;
        gpAppTables[29] = (short *) AACD_sfb_24_128;
        gpAppTables[30] = (short *) AACD_sfb_16_1024;
        gpAppTables[31] = (short *) AACD_sfb_16_128;
        gpAppTables[32] = (short *) AACD_sfb_8_1024;
        gpAppTables[33] = (short *) AACD_sfb_8_128;
        
        gpAppTables[34] = (unsigned char *) AACD_n_lzeros_8bit;
        gpAppTables[35] = (short *) AACD_bitrev_indices_256;
        gpAppTables[36] = (short *) AACD_bitrev_indices_2048;
        
        gpAppTables[37] = (short **) AACD_bitrev_indices;
        gpAppTables[38] = (short *) AACD_num_bitrev_sets;
        gpAppTables[39] = (Lfract *) AACD_fft_radix4_twidfac;
        gpAppTables[40] = (Lfract *) AACD_prepost_twidfac;
        gpAppTables[41] = (Lfract *) AACD_scale_fac_table_lf;
        gpAppTables[42] = (Lfract *) AACD_k_tab_lf;
        
        gpAppTables[43] = (Lfract *) AACD_window_fhg_long;
        gpAppTables[44] = (Lfract *) AACD_window_fhg_short;
        gpAppTables[45] = (Lfract *) AACD_window_dol_long;
        gpAppTables[46] = (Lfract *) AACD_window_dol_short;
        gpAppTables[47] = (Lfract(*)[])log2_coef;
        gpAppTables[48] = (Lfract (*)[])pow2_coef;
        gpAppTables[49] = (Lfract *) pow2_neg_coef;
        gpAppTables[50] = (Lfract *)Coeff_CR_512;
        gpAppTables[51] = (Lfract *)Coeff_CI_512;
        gpAppTables[52] = (Lfract *)Coeff_CR_64;
        gpAppTables[53] = (Lfract *)Coeff_CI_64;
        gpAppTables[54] = (Lfract *)Coeff_PostTwid_CR_long;
        gpAppTables[55] = (Lfract *)Coeff_PostTwid_CI_long;
        gpAppTables[56] = (Lfract *)Coeff_PostTwid_CR_short;
        gpAppTables[57] = (Lfract *)Coeff_PostTwid_CI_short;

#if defined(MOT_AAC) && !defined(AAC_ONLY)
        int i = AACD_TABLE_SIZE;
        
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_start_freq_16;
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_start_freq_22;
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_start_freq_32;
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_start_freq_44;
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_start_freq_24;
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_start_freq_48;
#ifndef CALC_STOP_BAND
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_stop_freq_48;
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_stop_freq_44;
        gpAppTables[i++] = (void *)(const unsigned char *)sbr_stop_freq_32;
#endif
        gpAppTables[i++] = (void *)(const struct FRAME_INFO *)&sbr_frame_info1_16;
        gpAppTables[i++] = (void *)(const struct FRAME_INFO *)&sbr_frame_info2_16;
        gpAppTables[i++] = (void *)(const struct FRAME_INFO *)&sbr_frame_info4_16;
        gpAppTables[i++] = (void *)(const struct SBR_HEADER_DATA *)&sbr_defaultHeader;
        gpAppTables[i++] = (void *)(BITSTREAM_WATCH_ELEMENT *)&bitele;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_EnvLevel10T;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_EnvLevel10F;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_EnvBalance10T;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_EnvBalance10F;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_EnvLevel11T;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_EnvLevel11F;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_EnvBalance11T;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_EnvBalance11F;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_NoiseLevel11T;
        gpAppTables[i++] = (void *)(const char (*)[2])sbr_huffBook_NoiseBalance11T;
        gpAppTables[i++] = (void *)(const char *)sbr_limGains_e;
        gpAppTables[i++] = (void *)(const char  *)pHybridResolution;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_limGains_m;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_limiterBandsPerOctaveDiv4;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_smoothFilter;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_invIntTable;
        gpAppTables[i++] = (void *)(const SFract_C (*)[2])sbr_randomPhase;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_qmf_64;

#ifdef HIGH_QUALITY_SBR
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_t_cos_L32;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_t_sin_L32;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_t_cos_L32_ds;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_t_sin_L32_ds;
#endif /* end HIGH_QUALITY_SBR */

#ifdef PARAMETRICSTEREO
        gpAppTables[i++] = (void *)(const fract_C *)aRevLinkDecaySer;
        gpAppTables[i++] = (void *)(const char *)aRevLinkDelaySer;
        gpAppTables[i++] = (void *)(const char *)groupBorders;
        gpAppTables[i++] = (void *)(const char *)groupShift;
        gpAppTables[i++] = (void *)(const char *)bins2groupMap;
        gpAppTables[i++] = (void *)(const char *)aHybridToBin;
        gpAppTables[i++] = (void *)(const char *)aQmfToBin;
#ifdef USE_IIR_FILTER
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_IIR_B_COEFFS])aaRealSubQmfCoeffsB;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_IIR_A_COEFFS])aaRealSubQmfCoeffsA;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_IIR_B_COEFFS])aaImagSubQmfCoeffsB;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_IIR_A_COEFFS])aaImagSubQmfCoeffsA;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_IIR_B_COEFFS])aaRealQmfCoeffsB;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_IIR_A_COEFFS])aaRealQmfCoeffsA;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_IIR_B_COEFFS])aaImagQmfCoeffsB;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_IIR_A_COEFFS])aaImagQmfCoeffsA;
#else
        gpAppTables[i++] = (void *)(const SFract_C *)aFractDelayPhaseFactorReQmf;
        gpAppTables[i++] = (void *)(const SFract_C *)aFractDelayPhaseFactorImQmf;
        gpAppTables[i++] = (void *)(const SFract_C *)aFractDelayPhaseFactorReSubQmf;
        gpAppTables[i++] = (void *)(const SFract_C *)aFractDelayPhaseFactorImSubQmf;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_SERIAL_ALLPASS_LINKS])aaFractDelayPhaseFactorSerReQmf;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_SERIAL_ALLPASS_LINKS])aaFractDelayPhaseFactorSerImQmf;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_SERIAL_ALLPASS_LINKS])aaFractDelayPhaseFactorSerReSubQmf;
        gpAppTables[i++] = (void *)(const SFract_C (*)[NO_SERIAL_ALLPASS_LINKS])aaFractDelayPhaseFactorSerImSubQmf;
#endif /* end USE_IIR_FILTER */
        gpAppTables[i++] = (void *)(const SFract_C *)scaleFactors;
        gpAppTables[i++] = (void *)(const SFract_C *)scaleFactorsFine;
        gpAppTables[i++] = (void *)(const SFract_C *)alphas;
        gpAppTables[i++] = (void *)(const char (*)[2])aBookPsIidTimeDecode;
        gpAppTables[i++] = (void *)(const char (*)[2])aBookPsIidFreqDecode;
        gpAppTables[i++] = (void *)(const char (*)[2])aBookPsIccTimeDecode;
        gpAppTables[i++] = (void *)(const char (*)[2])aBookPsIccFreqDecode;
        gpAppTables[i++] = (void *)(const char (*)[2])aBookPsIidFineTimeDecode;
        gpAppTables[i++] = (void *)(const char (*)[2])aBookPsIidFineFreqDecode;
#endif  /* end PARAMETRICSTEREO */

        gpAppTables[i++] = (void *)(const SFract_C *)p2_6;
        gpAppTables[i++] = (void *)(const SFract_C *)p8_13;
        gpAppTables[i++] = (void *)(const SFract_C *)fftTwiddleTable;
        gpAppTables[i++] = (void *)(const SFract_C *)sin_twiddle_L32_III;
        gpAppTables[i++] = (void *)(const SFract_C *)sin_twiddle_L64_II;
        gpAppTables[i++] = (void *)(const SFract_C *)sin_twiddle_L32_II;
        gpAppTables[i++] = (void *)(const SFract_C *)cos_twiddle_L32_IV;
        gpAppTables[i++] = (void *)(const SFract_C *)sin_twiddle_L32_IV;
        gpAppTables[i++] = (void *)(const SFract_C *)alt_sin_twiddle_L32_IV;
        gpAppTables[i++] = (void *)(const SFract_C *)cos_twiddle_L64_IV;
        gpAppTables[i++] = (void *)(const SFract_C *)sin_twiddle_L64_IV;
        gpAppTables[i++] = (void *)(const SFract_C *)alt_sin_twiddle_L64_IV;
        gpAppTables[i++] = (void *)(const SFract_C *)trigData;
        gpAppTables[i++] = (void *)(const SFract_C *)logDualisTable;
        gpAppTables[i++] = (void *)(const SFract_C *)invTable;
        gpAppTables[i++] = (void *)(const SFract_C *)sqrtTable;

#ifdef RECURSIVE_DCT
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_cos_twiddle_L04;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_cos_twiddle_L08;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_cos_twiddle_L16;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_cos_twiddle_L32;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_sin_twiddle_L04;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_sin_twiddle_L08;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_sin_twiddle_L16;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_sin_twiddle_L32;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_alt_sin_twiddle_L04;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_alt_sin_twiddle_L08;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_alt_sin_twiddle_L16;
        gpAppTables[i++] = (void *)(const SFract_C *)sbr_alt_sin_twiddle_L32;
        gpAppTables[i++] = (dct4Twiddle *)dct4TwiddleTable;
#endif /* end RECURSIVE_DCT */

#endif
}

/*================================================================================================*/
/*================================================================================================*/
int RunDecoder( void * ptr )
{
    assert( ptr );    

    sAacPlusDecoderHandler * pHandler = (sAacPlusDecoderHandler*)ptr;
    
    /***********************************************/
    /* Set priority for the PRE_EMPTION test case. */
    /***********************************************/    
    if( PRE_EMPTION == gTestappConfig.mTestCase )
    {
            int nice_inc = (int)(( 20.0f / gNumThreads ) * pHandler->mIndex);
            if( nice(nice_inc) < 0 )
            {
                    TST_RESM( TWARN, "%s : nice(%d) has failed", 
                            __FUNCTION__, nice_inc );
            }
    }
    
    /*******************/
    /* Run TestEngine. */
    /*******************/    
    pHandler->mLtpRetval = TestEngine( pHandler );        
    
    /*************************/
    /* Perform bit-matching. */
    /*************************/    
    DoBitmatch( pHandler );
    
    /*******************************************/
    /* Give the focus to an incomplete thread. */
    /*******************************************/        
    pHandler->mIsThreadFinished = TRUE;    
    if( (int)pHandler->mIndex == gThreadWithFocus )
    {
            int i;
            /* Search a first incomplete thread and assign them focus. */
            for( i = 0; i < gNumThreads; ++i )
            {            
                    if( !gAacPlusDecHandlers[i].mIsThreadFinished )
                    {            
                            gThreadWithFocus = gAacPlusDecHandlers[i].mIndex;
                            break;
                    }
            }
    }
    
    /************/
    /* Cleanup. */
    /************/
    CleanupHandler( pHandler );
    
    /**********************/
    /* Return LTP result. */
    /**********************/    
    return pHandler->mLtpRetval;
}


/*================================================================================================*/
/*================================================================================================*/
int TestEngine( sAacPlusDecoderHandler * pHandler ) 
{
        assert( pHandler );
        assert( pHandler->mpParams );
        
        const sHandlerParams   * pParams         = pHandler->mpParams;
        AACPD_Decoder_Config   * pDecConfig      = &pHandler->mDecConfig;
        AACPD_Decoder_info     * pDecInfo        = &pHandler->mDecInfo;
        AACPD_Block_Params     * pDecBlockParams = &pHandler->mDecBlockParams;
        sBitstream             * pBitstream      = &pHandler->mBitstream; 
        sPortIO                * pPortIo         = &pHandler->mPortIo;
        int                      i;
        int                      shouldOpen      = 0;
        int                      shouldClose     = 0;
        
        /*****************************/
        /* Open all necessary files. */
        /*****************************/
        
        /* Input file. */
        pHandler->mpInputStream = fopen( pParams->mInputFileName, "rb" );
        if( !pHandler->mpInputStream )
        {
                TST_RESM( TWARN, "%s : Can't open input file \'%s\'",
                        __FUNCTION__, pParams->mInputFileName );
                return TFAIL;                    
        }
        
        /* Output file. */
        /*
        pHandler->mpOutputStream = fopen( pParams->mOutputFileName, "wb" );
        if( !pHandler->mpOutputStream )
        {
                TST_RESM( TWARN, "%s : Can't create output file \'%s\'",
                        __FUNCTION__, pParams->mOutputFileName );
                return TFAIL;                    
        }*/
        
        /************************************************************/
        /* Initialize data start to table and query decoder memory. */
        /************************************************************/
        pDecConfig->aacd_initialized_data_start = (AACPD_UINT8*)gpAppTables;
#if defined(MOT_AAC) && !defined(AAC_ONLY)
        /* Initialization for SBR decoder */
        pDecConfig->sbrd_dec_config.sbrd_down_sample = 0;
        pDecConfig->sbrd_dec_config.sbrd_stereo_downmix = 0;    
#endif
    
        /* Call query mem function to know mem requirement of library. */
        CALL_AACPLUS_DECODER(
                aacpd_query_dec_mem( pDecConfig ),
                "aacpd_query_dec_mem" );   
        
        /*****************************/
        /* Allocate required memory. */
        /*****************************/    
        AACPD_Mem_Alloc_Info * pMemInfo = &pDecConfig->aacpd_mem_info;    
        for( i = 0; i < pMemInfo->aacpd_num_reqs; ++i )
        {
                AACPD_Mem_Alloc_Info_Sub * pMemInfoSub = &pMemInfo->mem_info_sub[i];
                pMemInfoSub->app_base_ptr = Util_AllocMem( pMemInfoSub->aacpd_size, 
                                                           pMemInfoSub->aacpd_type );
                if( !pMemInfoSub->app_base_ptr )
                {
                        tst_brkm( TBROK, (void(*)())VT_aacplus_decoder_cleanup, 
                                "Can't allocate %d bytes memory", pMemInfoSub->aacpd_size );
                }
        }

        /***************************/
        /* Initialize the decoder. */
        /***************************/
        pDecConfig->app_swap_buf = AACPlus_SwapBuf;
#ifndef OUTPUT_24BITS
        pDecConfig->num_pcm_bits     = AACPD_16_BIT_OUTPUT;
#else
        pDecConfig->num_pcm_bits     = AACPD_24_BIT_OUTPUT;
#endif  /*OUTPUT_24BITS*/
        
        //////////////////////////////////////////////////////////////////////////
        // Is it a client code?
        memset( pDecConfig->aacpd_mem_info.mem_info_sub[0].app_base_ptr,
                0, sizeof(AACD_global_struct));
        
        CALL_AACPLUS_DECODER(
                aacpd_decoder_init( pDecConfig ),
                "aacpd_decoder_init" );
        
        /*****************************************************************/
        /* Allocate memory for input buffer and read the input j2k file. */
        /*****************************************************************/ 
        pHandler->mpInpBuffer = Util_ReadFile( (const char*)pParams->mInputFileName, &pHandler->mInpBufferSz );    
        if( !pHandler->mpInpBuffer )
        {
                TST_RESM( TWARN, "%s : Can't allocate input buffer (%d bytes)",
                        __FUNCTION__, pHandler->mInpBufferSz );
                return TFAIL;
        }     
        Util_SetupBs( pHandler );
        
        /*****************/
        /* Parse header. */
        /*****************/
        int len = Util_PrepareBs( pBitstream );
        
        App_bs_readinit( pBitstream, 
                pBitstream->mpBitstreamBuf + (pBitstream->mBitstreamBufIndex-len), 
                len );
        
        unsigned long fileType = App_bs_look_bits( pBitstream, 32 );
        
        if( App_FindFileType( pBitstream, fileType ) != 0 )
        {
                TST_RESM( TWARN, "%s : Input file \'%s\' is NOT AAC", __FUNCTION__, 
                        pParams->mInputFileName );
                return TFAIL;
        }
        
        Util_UpdateBsStatus( pBitstream, 0 );
        
        if( pBitstream->mAppAdifHeaderPresent )
        {
                len = Util_PrepareBs( pBitstream );
                App_bs_readinit( pBitstream,
                        pBitstream->mpBitstreamBuf + (pBitstream->mBitstreamBufIndex-len), 
                        len );
                pBitstream->mBitsInHeader = 0;
                App_get_adif_header( pBitstream, pDecBlockParams );
                pDecConfig->params = pDecBlockParams;
                Util_UpdateBsStatus( pBitstream, pBitstream->mBitsInHeader / 8 );
        }  
        
        /*************************/
        /* Prepare for decoding. */
        /*************************/
        
        /* Print some information for the verbose mode. */
        if( gTestappConfig.mVerbose )
        {    
                
        }	      
        
        /* Will assign focus, if it is not assigned. */
        if( gThreadWithFocus == -1 )
                gThreadWithFocus = pHandler->mIndex;    
        
        shouldOpen = 1;
        shouldClose = 0;    
        
        strncpy( pDecInfo->output_path, 
                pParams->mOutputFileName,           
                AACPD_PATH_LEN < MAX_STR_LEN ? AACPD_PATH_LEN : MAX_STR_LEN );
        strncpy( pPortIo->outpath,
                pParams->mOutputFileName,
                PATH_LEN );                    
        
        AACD_global_struct * ptr = (AACD_global_struct *)pDecConfig->aacpd_decode_info_struct_ptr;
        
#ifdef DEBUG_TEST
        if( RE_ENTRANCE != gTestappConfig.mTestCase && PRE_EMPTION != gTestappConfig.mTestCase )
        {
                MemStat_GetStat();
        }
#endif
        
        /***************************************************/
        /*  Get ADTS-Header if present and start decoding. */
        /***************************************************/    
        while( 1 )
        {
                if( pBitstream->mBitstreamCount <= 0 )
                {
                        shouldClose = 1;
                        if (AACD_writeout1( pPortIo,
                                pDecInfo, 
                                pHandler->mpOutBuffer, 
                                ptr->AACD_mip, ptr, &shouldOpen, &shouldClose ) != 0 )
                        {
                                tst_brkm( TBROK, (void(*)())VT_aacplus_decoder_cleanup, "%s : I/O error",
                                        __FUNCTION__ );
                        }
                        break;
                }
                if( pBitstream->mAppAdtsHeaderPresent )
                {
                        pBitstream->mBitsInHeader = 0;
                        len = Util_PrepareBs( pBitstream );
                        App_bs_readinit( pBitstream, 
                                pBitstream->mpBitstreamBuf + ( pBitstream->mBitstreamBufIndex-len), 
                                len );
                        App_get_adts_header( pBitstream, pDecBlockParams );
                        pDecConfig->params = pDecBlockParams;
                        Util_UpdateBsStatus( pBitstream, pBitstream->mBitsInHeader/8 );
                }
                len = Util_PrepareBs( pBitstream );
                CALL_AACPLUS_DECODER_NR(
                        aacpd_decode_frame( pDecConfig, 
                        pDecInfo, 
                        pHandler->mpOutBuffer,
                        pBitstream->mpBitstreamBuf + (pBitstream->mBitstreamBufIndex-len),
                        len ),
                        "aacpd_decode_frame" );        
                if( pHandler->mLastAACPlusDecError != AACPD_ERROR_NO_ERROR )
                {            
                        shouldClose = 1;
                        if( AACD_writeout1( pPortIo,
                                pDecInfo,
                                pHandler->mpOutBuffer, 
                                ptr->AACD_mip, 
                                ptr, &shouldOpen, &shouldClose) != 0 )
                        {
                                tst_brkm( TBROK, (void(*)())VT_aacplus_decoder_cleanup, "%s : I/O error",
                                        __FUNCTION__ );
                        }
                        if( AACPD_ERROR_EOF != pHandler->mLastAACPlusDecError )
                                return TFAIL;
                        else
                                break;                
                }
                Util_UpdateBsStatus( pBitstream, pDecInfo->BitsInBlock/8 );
                
                /* commented to write initial 2 frames data also */
#ifdef SKIP_IST2_BLOCK
                if (ptr->AACD_bno > 2)
#endif
                {
                        if( AACD_writeout1( pPortIo,
                                pDecInfo,
                                pHandler->mpOutBuffer, 
                                ptr->AACD_mip, 
                                ptr, &shouldOpen, &shouldClose) != 0 )
                        {
                                tst_brkm( TBROK, (void(*)())VT_aacplus_decoder_cleanup, "%s : I/O error",
                                        __FUNCTION__ );
                        }
                }
                if( ptr->AACD_bno == 3 && gTestappConfig.mVerbose && 0 )
                {
                        TST_RESM( TINFO, "Thread[%lu] sampling_rate = %d", pHandler->mIndex, pDecInfo->aacpd_sampling_frequency );
                        TST_RESM( TINFO, "Thread[%lu] bit_rate = %d", pHandler->mIndex, pDecInfo->aacpd_bit_rate );
                        TST_RESM( TINFO, "Thread[%lu] num_chans = %d", pHandler->mIndex, pDecInfo->aacpd_num_channels );
                        TST_RESM( TINFO, "Thread[%lu] len = %d", pHandler->mIndex, pDecInfo->aacpd_len );                
                }
        } /*end-while*/
        
        if( gTestappConfig.mVerbose )
        {
                TST_RESM( TINFO, "Thread[%lu] Decoding complited", pHandler->mIndex );
        }                
        
        /************************/
        /* Cleanup the handler. */
        /************************/    
        CleanupHandler( pHandler );

#ifdef DEBUG_TEST
        if( RE_ENTRANCE != gTestappConfig.mTestCase && PRE_EMPTION != gTestappConfig.mTestCase )
        {
                MemStat_GetStat();
        }
#endif
        
        /* Return success. */
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
void DoDataOutput( sAacPlusDecoderHandler * pHandler )
{
        assert( pHandler );    
        assert( pHandler->mpParams );                
}


/*================================================================================================*/
/*================================================================================================*/
void ResetHandler( sAacPlusDecoderHandler * pHandler )
{
        assert( pHandler );
        
        memset( pHandler, 0, sizeof(sAacPlusDecoderHandler) );    
        pHandler->mIndex             = 0;
        pHandler->mIsThreadFinished  = FALSE;    
        pHandler->mOutBufferSz       = sizeof(pHandler->mpOutBuffer);
}


/*================================================================================================*/
/*================================================================================================*/
void CleanupHandler( sAacPlusDecoderHandler * pHandler )
{   
        assert( pHandler );
        
        /******************************/
        /* Close all files we opened. */
        /******************************/
        
        /* Close the input file. */
        if( pHandler->mpInputStream )
        {
                fclose( pHandler->mpInputStream );
                pHandler->mpInputStream = NULL;
        }
        
        /* Close the output file. */
        /*
        if( pHandler->mpOutputStream )
        {
                fclose( pHandler->mpOutputStream );
                pHandler->mpOutputStream = NULL;
        }*/
        
        /**/
        
        AACPD_Mem_Alloc_Info * pMemInfo = &pHandler->mDecConfig.aacpd_mem_info;    
        int i;
        for( i = 0; i < pMemInfo->aacpd_num_reqs; ++i )
        {
                AACPD_Mem_Alloc_Info_Sub * pMemInfoSub = &pMemInfo->mem_info_sub[i];
                if( pMemInfoSub->app_base_ptr )
                {
                        Util_FreeMem( pMemInfoSub->app_base_ptr );                    
                        pMemInfoSub->app_base_ptr = NULL; 
                }
        }

        /******************************/
        /* Free input/output buffers. */
        /******************************/    
        SAFE_DELETE( pHandler->mpInpBuffer );
        pHandler->mInpBufferSz = 0;    
        
}


/*================================================================================================*/
/*================================================================================================*/
int CompareFiles( const char * fname1, const char * fname2 )
{
        assert( fname1 && fname2 );
        
        int out, ref;
        struct stat fstat_out, fstat_ref;
        char *fptr_out, *fptr_ref;
        size_t filesize;
        size_t i;
        
        if( (out = open(fname1, O_RDONLY)) < 0 )
        {
                return FALSE;
        }
        if( (ref = open(fname2, O_RDONLY)) < 0 )
        {
                close(out);
                return FALSE;
        }
        fstat( out, &fstat_out );
        fstat( ref, &fstat_ref );
        if( fstat_out.st_size != fstat_ref.st_size )
        {
                close(out);
                close(ref);
                return FALSE;
        }
        filesize = fstat_out.st_size;
        fptr_out = (char*)mmap( 0, filesize, PROT_READ, MAP_SHARED, out, 0 );
        if( fptr_out == MAP_FAILED )
        {
                close( out );
                close( ref );
                return FALSE;
        }
        fptr_ref = (char*) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
        if( fptr_ref == MAP_FAILED )
        {
                close( out );
                close( ref );
                return FALSE;
        }
        close( out );
        close( ref );
        int n = 0;
        float e = 0.0f;
        for( i = 0; i < filesize; ++i )
        {
                if( *(fptr_ref + i) != *(fptr_out + i) )
                {
                        float e1 = (float)*(fptr_out + i);
                        float e2 = (float)*(fptr_ref + i);
                        float diff = e1 > e2 ? e1 - e2 : e2 - e1;
                        e += diff / 256.0f;
                        ++n;
                }
        }
        
        n > 0 ? e /= (float)n : 0;
        
        munmap( fptr_ref, filesize );
        munmap( fptr_out, filesize );
        return e < 0.1f;        
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( sAacPlusDecoderHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );

        int i, res = TRUE, m = 0;
        char suffix[MAX_STR_LEN], fname1[MAX_STR_LEN], fname2[MAX_STR_LEN];
        #define MAKE_SUFFIX(c,n) sprintf( suffix, "_%c%02d.pcm", c, n )
    
        for( i = 0; i < FChans; ++i )
        {
                MAKE_SUFFIX( 'f', i );
                strncpy( fname1, pHandler->mpParams->mOutputFileName, MAX_STR_LEN );
                strncpy( fname2, pHandler->mpParams->mReferenceFileName, MAX_STR_LEN );
                strncat( fname1, suffix, MAX_STR_LEN );
                strncat( fname2, suffix, MAX_STR_LEN );
                if( DoFilesExist( fname1, fname2 ) )
                {
                        if( !CompareFiles( fname1, fname2 ) )
                        {
                                TST_RESM( TWARN, "Thread[%lu] Files %s and %s are different", 
                                        pHandler->mIndex, fname1, fname2 );
                                res = FALSE;
                        }       
                        ++m;                         
                }                                                        
        } 
        
        for( i = 0; i < SChans; ++i )
        {
                MAKE_SUFFIX( 's', i );
                strncpy( fname1, pHandler->mpParams->mOutputFileName, MAX_STR_LEN );
                strncpy( fname2, pHandler->mpParams->mReferenceFileName, MAX_STR_LEN );
                strncat( fname1, suffix, MAX_STR_LEN );
                strncat( fname2, suffix, MAX_STR_LEN );
                if( DoFilesExist( fname1, fname2 ) )
                {
                        if( !CompareFiles( fname1, fname2 ) )
                        {
                                TST_RESM( TWARN, "Thread[%lu] Files %s and %s are different", 
                                        pHandler->mIndex, fname1, fname2 );
                                res = FALSE;
                        }   
                        ++m;                             
                }                                                        
        } 
        
        for( i = 0; i < BChans; ++i )
        {
                MAKE_SUFFIX( 'b', i );
                strncpy( fname1, pHandler->mpParams->mOutputFileName, MAX_STR_LEN );
                strncpy( fname2, pHandler->mpParams->mReferenceFileName, MAX_STR_LEN );
                strncat( fname1, suffix, MAX_STR_LEN );
                strncat( fname2, suffix, MAX_STR_LEN );
                if( DoFilesExist( fname1, fname2 ) )
                {
                        if( !CompareFiles( fname1, fname2 ) )
                        {
                                TST_RESM( TWARN, "Thread[%lu] Files %s and %s are different", 
                                        pHandler->mIndex, fname1, fname2 );
                                res = FALSE;
                        }   
                        ++m;                             
                }                                                        
        } 
        
        for( i = 0; i < LChans; ++i )
        {
                MAKE_SUFFIX( 'l', i );
                strncpy( fname1, pHandler->mpParams->mOutputFileName, MAX_STR_LEN );
                strncpy( fname2, pHandler->mpParams->mReferenceFileName, MAX_STR_LEN );
                strncat( fname1, suffix, MAX_STR_LEN );
                strncat( fname2, suffix, MAX_STR_LEN );
                if( DoFilesExist( fname1, fname2 ) )
                {
                        if( !CompareFiles( fname1, fname2 ) )
                        {
                                TST_RESM( TWARN, "Thread[%lu] Files %s and %s are different", 
                                        pHandler->mIndex, fname1, fname2 );
                                res = FALSE;
                        }          
                        ++m;                               
                }                                               
        } 
        
        if( !res && m > 0 )
        {
                if( gTestappConfig.mVerbose )
                {
                        TST_RESM( TFAIL, "Thread[%lu] Bitmatch failed (%s vs %s)",
                            pHandler->mIndex, pHandler->mpParams->mOutputFileName, pHandler->mpParams->mReferenceFileName );
                }                                                    
                pHandler->mLtpRetval = TFAIL;
        }
        else if( m > 0 )
        {
                if( gTestappConfig.mVerbose )
                {
                        TST_RESM( TINFO, "Thread[%lu] Bitmatch passed (%s vs %s)",
                            pHandler->mIndex, pHandler->mpParams->mOutputFileName, pHandler->mpParams->mReferenceFileName );
                }                            
        }
        
        return res;
}


/*================================================================================================*/
/*================================================================================================*/
int DoFilesExist( const char * fname1, const char * fname2 )
{
        FILE * fstream1 = fopen( fname1, "r" );
        if( fstream1 )
        {
                fclose( fstream1 );
                FILE * fstream2 = fopen( fname2, "r" );
                if( fstream2 )
                {
                        fclose( fstream2 );
                        return TRUE;
                }
        }
        return FALSE;
}


/*================================================================================================*/
/*================================================================================================*/
void HogCpu( void )
{
        while( 1 )
        {
                sqrt( rand() );
        }
}


/*================================================================================================*/
/*================================================================================================*/
void MakeEntry( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry )
{
        sHandlerParams * pParams = (sHandlerParams*)malloc( sizeof(sHandlerParams) );    
        
        int n = 0;  
        strncpy( pParams->mInputFileName,      entry[n++], MAX_STR_LEN );   
        strncpy( pParams->mOutputFileName,     entry[n++], MAX_STR_LEN );
        strncpy( pParams->mReferenceFileName,  entry[n++], MAX_STR_LEN );      
        
        pParams->mEntryIndex = nEntry;
        
        /* Adjust/check parameters here... */  
        
        if( !Util_StrICmp( pParams->mOutputFileName, NA ) )
        {        
                tst_brkm( TBROK, (void(*)())VT_aacplus_decoder_cleanup,
                        "Wrong output file name %s. The output file name must not be %s."
                        "Please check %s, line/entry #%lu", 
                        pParams->mOutputFileName, NA, gTestappConfig.mConfigFilename, nEntry + 1 );
        }        
        
        LList_PushBack( gpParamsList, pParams );
}


/*================================================================================================*/
/*================================================================================================*/
int NominalFunctionalityTest( void )
{
        sLinkedList * pNode;
        int i;
        int rv = TPASS;    
        sAacPlusDecoderHandler * pHandler = gAacPlusDecHandlers;     
        
        /* For the each entry */
        for( pNode = gpParamsList, i = 0; pNode; pNode = pNode->mpNext, ++i )
        {
                /* Reset the handler. */
                ResetHandler( pHandler );
                
                /* Get content. */
                pHandler->mpParams = (sHandlerParams*)pNode->mpContent;
                
                if( gTestappConfig.mVerbose )            
                {
                        TST_RESM( TINFO, "Thread[%lu] Input: %s, Output: %s, Reference: %s", 
                                pHandler->mIndex, 
                                pHandler->mpParams->mInputFileName, 
                                pHandler->mpParams->mOutputFileName,
                                pHandler->mpParams->mReferenceFileName );            
                }                        
                
                /* Run the Decoder. */
                rv += RunDecoder( pHandler );                
        }
        
        return rv;    
}


/*================================================================================================*/
/*================================================================================================*/
int RobustnessTest( void )
{
        sLinkedList * pNode;
        int i;
        int rv = TPASS;    
        sAacPlusDecoderHandler * pHandler = gAacPlusDecHandlers;     
        
        /* For the each entry */
        for( pNode = gpParamsList, i = 0; pNode; pNode = pNode->mpNext, ++i )
        {
                /* Reset the handler. */
                ResetHandler( pHandler );
                
                /* Get content. */
                pHandler->mpParams = (sHandlerParams*)pNode->mpContent;
                
                if( gTestappConfig.mVerbose )            
                {
                        if( gTestappConfig.mVerbose )            
                        {
                                TST_RESM( TINFO, "Thread[%lu] Input: %s, Output: %s, Reference: %s", 
                                        pHandler->mIndex, 
                                        pHandler->mpParams->mInputFileName, 
                                        pHandler->mpParams->mOutputFileName,
                                        pHandler->mpParams->mReferenceFileName );            
                        }       
                }                        
                
                /* Run the Decoder. */
                int res = RunDecoder( pHandler );        
                if( TPASS == res )
                {
                        if( gTestappConfig.mVerbose )
                        {
                                TST_RESM( TWARN, "Robustness to %s failed", pHandler->mpParams->mInputFileName );
                        }
                        rv = TFAIL;
                }
                else
                {
                        if( gTestappConfig.mVerbose )
                        {
                                TST_RESM( TPASS, "Robustness to %s passed", pHandler->mpParams->mInputFileName );
                        }
                }        
        }
        
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int ReLocatabilityTest( void )
{
        sLinkedList * pNode;
        int i, j;
        int rv = TPASS;    
        sAacPlusDecoderHandler * pHandler = gAacPlusDecHandlers;     
        
        /* For the each entry */
        for( pNode = gpParamsList, i = 0; pNode; pNode = pNode->mpNext, ++i )
        {    
                for( j = 0; j < gTestappConfig.mNumIter; ++j )
                {
                        /* Reset the handler. */
                        ResetHandler( pHandler );
                        
                        /* Get content. */
                        pHandler->mpParams = (sHandlerParams*)pNode->mpContent;
                        
                        if( gTestappConfig.mVerbose && j == 0 )            
                        {
                                if( gTestappConfig.mVerbose )            
                                {
                                        TST_RESM( TINFO, "Thread[%lu] Input: %s, Output: %s, Reference: %s", 
                                                pHandler->mIndex, 
                                                pHandler->mpParams->mInputFileName, 
                                                pHandler->mpParams->mOutputFileName,
                                                pHandler->mpParams->mReferenceFileName );            
                                }     
                        }                        
                        
                        /* Run the Decoder. */
                        rv += RunDecoder( pHandler );        
                        
                        if( gTestappConfig.mVerbose )            
                        {
                                TST_RESM( TINFO, "Thread[%lu] Data memory was relocated", pHandler->mIndex );
                        }            
                }
        }    
        return rv;    
}


/*================================================================================================*/
/*================================================================================================*/
int ReEntranceTest( void )
{
        int ReEntranceTestCore( sLinkedList * pHead );
        sLinkedList * pHead = gpParamsList;
        int rv = TPASS;
        int i;
        
        while( pHead )
        {   
                gThreadWithFocus = -1;     
                rv += ReEntranceTestCore( pHead );
                for( i = 0; i < gNumThreads && pHead; ++i )
                {
                        ResetHandler( &gAacPlusDecHandlers[i] );
                        gAacPlusDecHandlers[i].mIndex = i;
                        pHead = pHead->mpNext;
                }            
        }
        
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int ReEntranceTestCore( sLinkedList * pHead )
{
        assert( pHead );
        
        sLinkedList * pNode;
        int i;
        int rv = TPASS;    
        sAacPlusDecoderHandler * pHandler;        
        
        /* Run all bitstreams in separate threads. */
        for( pNode = pHead, i = 0; pNode && i < gNumThreads; pNode = pNode->mpNext, ++i )
        {        
                pHandler = gAacPlusDecHandlers + i;
                ResetHandler( pHandler );
                pHandler->mIndex = i;
                
                /* Get content. */
                pHandler->mpParams = (sHandlerParams*)pNode->mpContent;
                
                if( gTestappConfig.mVerbose )            
                {
                        if( gTestappConfig.mVerbose )            
                        {
                                TST_RESM( TINFO, "Thread[%lu] Input: %s, Output: %s, Reference: %s", 
                                        pHandler->mIndex, 
                                        pHandler->mpParams->mInputFileName, 
                                        pHandler->mpParams->mOutputFileName,
                                        pHandler->mpParams->mReferenceFileName );            
                        }     
                }                        
                
                if( pthread_create( &pHandler->mThreadID, NULL, (void*(*)(void*))&RunDecoder, pHandler ) )
                {
                        TST_RESM( TWARN, "%s : error creating thread %d", __FUNCTION__, i );
                        return TFAIL;	    
                }	
        }    
        
        /* Wait for the each thread. */
        for( i = 0; i < gNumThreads; ++i )
        {
                pHandler = gAacPlusDecHandlers + i;     
                pthread_join( pHandler->mThreadID, NULL );
        }
        for( i = 0; i < gNumThreads; ++i )
        {
                pHandler = gAacPlusDecHandlers + i;     
                rv += pHandler->mLtpRetval;
        }
        
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int PreEmptionTest( void )
{
        return ReEntranceTest();
}


/*================================================================================================*/
/*================================================================================================*/
int EnduranceTest( void )
{
        int i;
        int rv = TPASS;
        
        for( i = 0; i < gTestappConfig.mNumIter; ++i )
        {
                if( gTestappConfig.mVerbose )            
                        TST_RESM( TINFO, "The %d iteration has been started", i+1 );
                rv += NominalFunctionalityTest();	
                if( gTestappConfig.mVerbose )
                        TST_RESM( TINFO, "The %d iteration has been completed", i+1 );
        }    
        
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int LoadTest( void )
{
        int rv = TFAIL;
        pid_t pid;
        
        switch( pid = fork() )
        {
                case -1:
                        TST_RESM( TWARN, "%s : fork failed", __FUNCTION__ );	    
                        return rv;
                case 0:       
                        /* child process */
                        HogCpu();
                default:               
                        /* parent */
                        sleep(2);
                        rv = NominalFunctionalityTest();
                        /* kill child process once decode/encode loop has ended */
                        if( kill( pid, SIGKILL ) != 0 )
                        {
                                TST_RESM( TWARN, "%s : Kill(SIGKILL) error", __FUNCTION__ );
                                return rv;
                        }
        }
        
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int Util_StrICmp( const char * s1, const char *s2 )
{
        for (;;)
        {
                int c1 = *s1;
                int c2 = *s2;
                if( isupper(c1) )
                        c1 = tolower(c1);
                if( isupper(c2) )
                        c2 = tolower(c2);
                if( c1 != c2 )
                        return c1 - c2;
                if( c1 == '\0' )
                        break;
                ++s1;
                ++s2;
        }
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
unsigned char * Util_ReadFile( const char * filename, size_t * pSz )
{
        FILE * pInStream = fopen( filename, "rb" );    
        if( pInStream )
        {        
                fseek( pInStream, 0, SEEK_END );
                size_t sz = ftell( pInStream );
                fseek( pInStream, 0, SEEK_SET );
                unsigned char * pData = (unsigned char*)Util_AllocMem( sizeof(char) * sz, 0 );
                fread( pData, 1, sz, pInStream );
                fclose( pInStream );
                if( pSz ) 
                        *pSz = sz;
                return pData;
        }
        return NULL;
}


/*================================================================================================*/
/*================================================================================================*/
void Util_SwapBytes( short * pWords, int count )
{
        char * pByte;
        while( count-- )
        {
                pByte = (char*)(pWords+count);
                pByte[0] ^= pByte[1] ^= pByte[0] ^= pByte[1];
        }
}


/*================================================================================================*/
/*================================================================================================*/
void * Util_AllocMem( size_t sz, int memType )
{
#ifdef DEBUG_TEST
        return RE_ENTRANCE != gTestappConfig.mTestCase && PRE_EMPTION != gTestappConfig.mTestCase ? 
               MemStat_Alloc( sz*2 ) : malloc( sz );
#else
        return malloc( sz );
#endif                        
}

/*================================================================================================*/
/*================================================================================================*/
void Util_FreeMem( void * ptr )
{
#ifdef DEBUG_TEST
        return RE_ENTRANCE != gTestappConfig.mTestCase && PRE_EMPTION != gTestappConfig.mTestCase ? 
               MemStat_Free( ptr ) : free( ptr );
#else
        return free( ptr );
#endif                      
}


/*================================================================================================*/
/*================================================================================================*/
void Util_SetupBs( sAacPlusDecoderHandler * pHandler )
{
        assert( pHandler );
        sBitstream * pBitstream = &pHandler->mBitstream;
        
        pBitstream->mpBitstreamBuf = (char*)pHandler->mpInpBuffer;
        pBitstream->mBitstreamCount = (int)pHandler->mInpBufferSz;
}


/*================================================================================================*/
/*================================================================================================*/
int Util_PrepareBs( sBitstream * pBitstream )
{
        assert( pBitstream );
        assert( pBitstream->mpBitstreamBuf );
        
        int len;
        
        len = (pBitstream->mBitstreamCount > BS_BUF_SIZE) ? BS_BUF_SIZE : pBitstream->mBitstreamCount;
        pBitstream->mBitstreamBufIndex += len;
        pBitstream->mBitstreamCount    -= len;
        pBitstream->mInBufDone         += len;
        pBitstream->mBytesSupplied     += len;
        
        return len;
}


/*================================================================================================*/
/*================================================================================================*/
void Util_UpdateBsStatus( sBitstream * pBitstream, int nBytesUsed )
{
        int nUnusedBytes = pBitstream->mBytesSupplied - nBytesUsed;
        
        pBitstream->mBytesSupplied = 0;
        
        pBitstream->mBitstreamCount     += nUnusedBytes;
        pBitstream->mBitstreamBufIndex  -= nUnusedBytes;
        pBitstream->mInBufDone          -= nUnusedBytes;
}


#ifdef __cplusplus
}
#endif

