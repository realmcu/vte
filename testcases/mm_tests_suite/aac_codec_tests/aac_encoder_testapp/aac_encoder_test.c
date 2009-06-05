/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/
	
/**
@file aac_encoder_test.c
	
@brief VTE C header template
	
@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/
	
/*======================== REVISION HISTORY ==================================
		
Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  09/08/2005   TLSbo53249   Initial version  
D.Simakov / smkd001c  19/08/2005   TLSbo53249   Replaced WARN->FAIL for the bitmatching
D.Simakov / smkd001c  20/12/2005   TLSbo57009   Updated for the AAC Encoder rev 2.1
=============================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "aac_encoder_test.h"

#include <pthread.h>
#include <math.h>
#include <stdarg.h>

/* AAC Encoder API. */
#include <aace_enc_interface.h>
#include <glb_table.h>

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define MAX_THREADS 4
#define SAFE_DELETE(p) {if(p){free(p);p=NULL;}}
#define NA "n/a"
#define M(m){printf("<<<--- %s --->>>\n",m);fflush(stdout);}


/**********************************************************************
* Macro name:  CALL_AAC_ENCODER()
* Description: Macro checks for any error in function execution
               based on the retun value. Incase of error, the function exits.
**********************************************************************/
#define CALL_AAC_ENCODER(AACEncRoutine, name)   \
    pHandler->mLastAACEncError = AACEncRoutine; \
    if( (pHandler->mLastAACEncError != AACE_OK) )\
    {\
        TST_RESM( TWARN, "%s : %s fails #%d", __FUNCTION__, name, pHandler->mLastAACEncError);\
		return TFAIL;\
	}    

#define TST_RESM(s,format,...) \
{\
    pthread_mutex_lock( &gMutex );\
    tst_resm((s), (format), ##__VA_ARGS__);\
    pthread_mutex_unlock( &gMutex );\
}

#define alloc_fast(s) malloc(s)
#define alloc_slow(s) malloc(s)

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct 
{
    const char * mName;
    FILE       * mStream;
} sFileHandle;

/*---------------------------------------------------*/
typedef struct
{
    char  mInputFileName[MAX_STR_LEN];
    char  mOutputFileName[MAX_STR_LEN];
    char  mReferenceFileName[MAX_STR_LEN]; 
    
    int   mBitrate;
    int   mStereoMode;

} sHandlerParams;

/*---------------------------------------------------*/
typedef struct 
{
    unsigned long           mIndex;
        
    const sHandlerParams  * mpParams;    
    sFileHandle             mInputFile;
    sFileHandle             mOutputFile;
    
    long                    mFramesCount; 
    AACE_RET_TYPE           mLastAACEncError; 

    int                   * mpOutBuffer;
    long                  * mpInpBuffer;    
    size_t                  mOutBufferSz;
    size_t                  mInpBufferSz;    
        
    AACE_Encoder_Config     mEncConfig;
    
    /* variables for wave file reading */
    unsigned int            mWaveFileSize;
    unsigned int            mCurrReadOffset;
    unsigned int            mSamplesRemaining;    
    unsigned int            mDataOffset;

    pthread_t               mThreadID;          
    int                     mIsThreadFinished;
    int                     mLtpRetval;
} sAacEncoderHandler;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static sAacEncoderHandler    gAacEncHandlers[ MAX_THREADS ];
static int                   gNumThreads                     = 1;    			 
static int                   gThreadWithFocus                = -1;   	
static sLinkedList         * gpParamsList                    = NULL;
static pthread_mutex_t       gMutex;
static void                * gpAppTables[120]; /* array of pointers to tables */
/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void      AACE_AppInitTables ( int sf );

int  RunEncoder     ( void * ptr );
int  TestEngine     ( sAacEncoderHandler * pHandler );
void DoDataOutput   ( sAacEncoderHandler * pHandler );
void ResetHandler   ( sAacEncoderHandler * pHandler );
void CleanupHandler ( sAacEncoderHandler * pHandler );
int  DoBitmatch     ( sAacEncoderHandler * pHandler );
int  DoFilesExist   ( const char * fname1, const char * fname2 );
void HogCpu         ();
void MakeEntry      ( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry );

/* test cases */
int NominalFunctionalityTest ();
int ReLocatabilityTest       ();  
int ReEntranceTest           ();
int PreEmptionTest           ();
int EnduranceTest            ();
int LoadTest                 ();

/* Helper functions. */
void Util_ReadUnsignedNum( unsigned int * pValue, int nBytes, FILE * in );
void Util_WaveReaderInit ( sAacEncoderHandler * pHandler, int * pNumChan, double * pFreq, long * pNumSamples );
int  Util_WaveReadData   ( sAacEncoderHandler * pHandler, 
                           short sampleBuffer[AACE_NUM_AUDIO_CHANNELS][AACE_FRAME_SIZE], 
                           unsigned int frameSize );
void Util_PackInputData  ( AACE_Encoder_Config * pEncConfig,
                           AACE_INT16 tempInput[AACE_NUM_AUDIO_CHANNELS][AACE_FRAME_SIZE] );
void Debug_MyEncodeFrame ( AACE_Encoder_Config * pEncConfig );                            


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/
int VT_aac_encoder_setup()
{      
    pthread_mutex_init( &gMutex, NULL );

    int i;
    /* Reset all handlers. */
    for( i = 0; i < MAX_THREADS; ++i )
        ResetHandler( gAacEncHandlers + i );    
        
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

    /* Init tables. */
    //AACE_AppInitTables();

    return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_aac_encoder_cleanup()
{    
    pthread_mutex_destroy( &gMutex );

    if( gpParamsList ) 
        LList_Delete( gpParamsList );

    int i;
    for( i = 0; i < MAX_THREADS; ++i )
    {
        CleanupHandler( gAacEncHandlers + i );
        ResetHandler( gAacEncHandlers + i );        
    }
        
    return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_aac_encoder_test()
{
    int rv = TFAIL;            
    
    switch( gTestappConfig.mTestCase )
    {
		case NOMINAL_FUNCTIONALITY:	    
			TST_RESM( TINFO, "Nominal functionality test" );
			rv = NominalFunctionalityTest();
			TST_RESM( TINFO, "End of nominal functionality test" );		
    		break;

        case RELOCATABILITY:
            TST_RESM( TINFO, "Relocatability test" );            
            rv = ReLocatabilityTest();
            TST_RESM( TINFO, "End relocatability test" );
            break;       
	    
		case RE_ENTRANCE:
			TST_RESM( TINFO, "Re-entrance test" );
			rv = ReEntranceTest();
			TST_RESM( TINFO, "End of re-entrance test" );		
			break;
	    
		case PRE_EMPTION:
			TST_RESM( TINFO, "Pre-emption test" );
			rv = PreEmptionTest();
			TST_RESM( TINFO, "End of pre-emption test" );		
			break;                    

		case ENDURANCE:
			TST_RESM( TINFO, "Endurance test" );
			rv = EnduranceTest();
			TST_RESM( TINFO, "End of endurance test" );		
			break;    

		case LOAD:
			TST_RESM( TINFO, "Load test" );
			rv = LoadTest();
			TST_RESM( TINFO, "End of load test" );		
			break;                                    
            
		default:
			TST_RESM( TINFO, "Wrong test case" );
			break;    
	}    
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
void AACE_AppInitTables( int sf )
{
    gpAppTables[0]  = (char*) g_huff_len_book1_table;
    gpAppTables[1]  = (char*) g_huff_len_book2_table;
    gpAppTables[2]  = (char*) g_huff_len_book3_table;
    gpAppTables[3]  = (char*) g_huff_len_book4_table;
    gpAppTables[4]  = (char*) g_huff_len_book5_table;
    gpAppTables[5]  = (char*) g_huff_len_book6_table;
    gpAppTables[6]  = (char*) g_huff_len_book7_table;
    gpAppTables[7]  = (char*) g_huff_len_book8_table;
    gpAppTables[8]  = (char*) g_huff_len_book9_table;
    gpAppTables[9]  = (char*) g_huff_len_book10_table;
    gpAppTables[10] = (char*) g_huff_len_book11_table;
    gpAppTables[11] = (char*) g_huff_len_book12_table;
    
    gpAppTables[12] = (short int*) g_AACE_tns_stepup_norm_table;
#if 0
    gpAppTables[13] = (short int*) g_bit_reverse_pairs_32_table;
    gpAppTables[14] = (short int*) g_bit_reverse_singles_32_table;
#else
    gpAppTables[13] = NULL;
    gpAppTables[14] = NULL;
#endif
    gpAppTables[15] = (short int*) g_bit_reverse_pairs_256_table;
    gpAppTables[16] = (short int*) g_bit_reverse_singles_256_table;
#if 0
    gpAppTables[17] = (short int*) g_bit_reverse_pairs_1024_table;
    gpAppTables[18] = (short int*) g_bit_reverse_singles_1024_table;
#else
    gpAppTables[17] = NULL;
    gpAppTables[18] = NULL;
#endif

    gpAppTables[19] = (short int*) g_bit_reverse_pairs_2048_table;
    gpAppTables[20] = (short int*) g_bit_reverse_singles_2048_table;
    
    gpAppTables[21] = (unsigned short*) g_AACE_tnsMinBandNumberLong_table;
    gpAppTables[22] = (unsigned short*) g_AACE_tnsMinBandNumberShort_table;
    gpAppTables[23] = (unsigned short*) g_AACE_tnsMaxBandsLongMainLow_table;
    gpAppTables[24] = (unsigned short*) g_AACE_tnsMaxBandsShortMainLow_table;
    gpAppTables[25] = (unsigned short*) &g_AACE_tnsMaxOrderLongMain;
    gpAppTables[26] = (unsigned short*) &g_AACE_tnsMaxOrderLongLow;
    gpAppTables[27] = (unsigned short*) &g_AACE_tnsMaxOrderShortMainLow;
    gpAppTables[28] = (unsigned short*) g_AACE_tnsMaxBandsLongSSR_table;
    gpAppTables[29] = (unsigned short*) g_AACE_tnsMaxBandsShortSSR_table;
    gpAppTables[30] = (unsigned short*) &g_AACE_tnsMaxOrderLongSSR;
    gpAppTables[31] = (unsigned short*) &g_AACE_tnsMaxOrderShortSSR;
    
    gpAppTables[32] = (int*) g_two_pow_3_n_by_16_ns_table;
    gpAppTables[34] = (int*) g_asin1_coef_ns_table;
    gpAppTables[35] = (int*) g_asin2_coef_ns_table;
    gpAppTables[36] = (int*) g_AACE_rnd_nonuniform_const_table;
    gpAppTables[49] = (int*) g_AACE_digrev_indices_short_table;
    gpAppTables[50] = (int*) g_AACE_digrev_indices_long_table;
    gpAppTables[51] = (int*) g_huff_val_book1_table;
    gpAppTables[52] = (int*) g_huff_val_book2_table;
    gpAppTables[53] = (int*) g_huff_val_book3_table;
    gpAppTables[54] = (int*) g_huff_val_book4_table;
    gpAppTables[55] = (int*) g_huff_val_book5_table;
    gpAppTables[56] = (int*) g_huff_val_book6_table;
    gpAppTables[57] = (int*) g_huff_val_book7_table;
    gpAppTables[58] = (int*) g_huff_val_book8_table;
    gpAppTables[59] = (int*) g_huff_val_book9_table;
    gpAppTables[60] = (int*) g_huff_val_book10_table;
    gpAppTables[61] = (int*) g_huff_val_book11_table;
    gpAppTables[62] = (int*) g_huff_val_book12_table;
    gpAppTables[64] = (int*) g_n_lzeros_8bit_table;
    gpAppTables[72] = (unsigned long*)
        g_AACE_tnsSupportedSamplingRates_table;
    gpAppTables[73] = (long int*)  g_two_pow_3_n_by_16_table;
    gpAppTables[74] = (long int*)  g_asin1_coef_lf_table;
    gpAppTables[75] = (long int*)  g_asin2_coef_lf_table;
    gpAppTables[76] = (long int*)  g_AACE_window_fhg_long_lf_table;
    gpAppTables[77] = (long int*)  g_AACE_window_fhg_short_lf_table;
    gpAppTables[78] = (long int*)  g_AACE_index_table_lf_table;
    gpAppTables[79] = (long int*)  g_AACE_one_by_n_tbl_lf_table;
    gpAppTables[80] = (long int*)  g_MP3E_pow_4_by_3_lf_table;
    gpAppTables[81] = (long int*)  g_MP3E_step_fract_table;
    gpAppTables[82] = (long int*)  g_one_by_n_table;
    gpAppTables[83] = (long int*)  &g_oneby3_table;
    gpAppTables[84] = (long int*)  g_sqrt_coeff_lf_table;
    gpAppTables[85] = (long int*)  g_pow2_coef_table;
    gpAppTables[86] = (long int*)  g_window_long_table;
    gpAppTables[89] = (long int*)  g_window_short_table;
    gpAppTables[97] = (long int*)  g_xpow3by4_coef_table;
    gpAppTables[98] = (long int*)  g_table_val_table;
    gpAppTables[99] = (long int*)  g_log2_coef_table;

    gpAppTables[100] = (long int*) g_AACE_fft_twidfac_short_lf_table;
    gpAppTables[101] = (long int*) g_AACE_fft_twidfac_long_lf_table;
    gpAppTables[102] = (long int*) g_AACE_prepost_twidfac_long_lf_table;
    gpAppTables[103] = (long int*) g_AACE_prepost_twidfac_short_lf_table;
    gpAppTables[104] = (long int*) g_sine_table_lf_table;
    
    gpAppTables[105] = (PARTITION_TABLE_SHORT*)
        g_AACE_part_tbl_short_all_table;
    gpAppTables[106] = (PARTITION_TABLE_LONG*)
        g_AACE_part_tbl_long_all_table;
    gpAppTables[107] = (SR_INFO*) g_AACE_sr_info_aac_table;

    if (sf == 44100)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_44;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_44;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_44;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_44;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_44;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_44;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_44;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_44;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_44;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_44;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_44;
        gpAppTables[87] = (long int*)  g_spread_long_table_44;
        gpAppTables[88] = (long int*)  g_spread_short_table_44;
        gpAppTables[90] = (long int*)  g_short_qthr_table_44;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_44;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_44;
        gpAppTables[93] = (long int*)  g_long_qthr_table_44;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_44;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_44;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_44;
        gpAppTables[108] = (int *)g_pb_wid_table_44;
        gpAppTables[109] = (int *)g_pb_sft_table_44;
        gpAppTables[110] = (int *)g_sfb_sft_table_44;
        gpAppTables[111] = (int *)g_pb_s_wid_table_44;
        gpAppTables[112] = (int *)g_pb_s_sft_table_44;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_44;
    }
    else if (sf == 8000)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_8;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_8;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_8;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_8;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_8;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_8;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_8;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_8;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_8;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_8;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_8;
        gpAppTables[87] = (long int*)  g_spread_long_table_8;
        gpAppTables[88] = (long int*)  g_spread_short_table_8;
        gpAppTables[90] = (long int*)  g_short_qthr_table_8;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_8;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_8;
        gpAppTables[93] = (long int*)  g_long_qthr_table_8;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_8;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_8;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_8;
        gpAppTables[108] = (int *)g_pb_wid_table_8;
        gpAppTables[109] = (int *)g_pb_sft_table_8;
        gpAppTables[110] = (int *)g_sfb_sft_table_8;
        gpAppTables[111] = (int *)g_pb_s_wid_table_8;
        gpAppTables[112] = (int *)g_pb_s_sft_table_8;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_8;
    }
    else if (sf == 16000)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_16;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_16;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_16;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_16;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_16;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_16;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_8;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_8;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_8;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_16;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_16;
        gpAppTables[87] = (long int*)  g_spread_long_table_16;
        gpAppTables[88] = (long int*)  g_spread_short_table_16;
        gpAppTables[90] = (long int*)  g_short_qthr_table_16;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_16;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_16;
        gpAppTables[93] = (long int*)  g_long_qthr_table_16;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_16;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_16;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_16;
        gpAppTables[108] = (int *)g_pb_wid_table_16;
        gpAppTables[109] = (int *)g_pb_sft_table_16;
        gpAppTables[110] = (int *)g_sfb_sft_table_16;
        gpAppTables[111] = (int *)g_pb_s_wid_table_16;
        gpAppTables[112] = (int *)g_pb_s_sft_table_16;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_16;
    }
	else if (sf == 32000)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_32;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_32;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_32;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_32;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_32;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_32;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_8;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_8;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_8;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_32;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_32;
        gpAppTables[87] = (long int*)  g_spread_long_table_32;
        gpAppTables[88] = (long int*)  g_spread_short_table_32;
        gpAppTables[90] = (long int*)  g_short_qthr_table_32;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_32;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_32;
        gpAppTables[93] = (long int*)  g_long_qthr_table_32;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_32;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_32;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_32;
        gpAppTables[108] = (int *)g_pb_wid_table_32;
        gpAppTables[109] = (int *)g_pb_sft_table_32;
        gpAppTables[110] = (int *)g_sfb_sft_table_32;
        gpAppTables[111] = (int *)g_pb_s_wid_table_32;
        gpAppTables[112] = (int *)g_pb_s_sft_table_32;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_32;
    }
	else if (sf == 24000)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_24;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_24;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_24;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_24;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_24;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_24;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_8;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_8;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_8;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_24;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_24;
        gpAppTables[87] = (long int*)  g_spread_long_table_24;
        gpAppTables[88] = (long int*)  g_spread_short_table_24;
        gpAppTables[90] = (long int*)  g_short_qthr_table_24;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_24;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_24;
        gpAppTables[93] = (long int*)  g_long_qthr_table_24;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_24;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_24;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_24;
        gpAppTables[108] = (int *)g_pb_wid_table_24;
        gpAppTables[109] = (int *)g_pb_sft_table_24;
        gpAppTables[110] = (int *)g_sfb_sft_table_24;
        gpAppTables[111] = (int *)g_pb_s_wid_table_24;
        gpAppTables[112] = (int *)g_pb_s_sft_table_24;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_24;
    }
	else if (sf == 22050)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_22;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_22;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_22;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_22;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_22;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_22;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_8;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_8;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_8;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_22;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_22;
        gpAppTables[87] = (long int*)  g_spread_long_table_22;
        gpAppTables[88] = (long int*)  g_spread_short_table_22;
        gpAppTables[90] = (long int*)  g_short_qthr_table_22;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_22;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_22;
        gpAppTables[93] = (long int*)  g_long_qthr_table_22;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_22;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_22;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_22;
        gpAppTables[108] = (int *)g_pb_wid_table_22;
        gpAppTables[109] = (int *)g_pb_sft_table_22;
        gpAppTables[110] = (int *)g_sfb_sft_table_22;
        gpAppTables[111] = (int *)g_pb_s_wid_table_22;
        gpAppTables[112] = (int *)g_pb_s_sft_table_22;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_22;
    }
	else if (sf == 12000)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_12;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_12;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_12;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_12;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_12;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_12;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_8;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_8;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_8;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_12;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_12;
        gpAppTables[87] = (long int*)  g_spread_long_table_12;
        gpAppTables[88] = (long int*)  g_spread_short_table_12;
        gpAppTables[90] = (long int*)  g_short_qthr_table_12;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_12;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_12;
        gpAppTables[93] = (long int*)  g_long_qthr_table_12;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_12;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_12;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_12;
        gpAppTables[108] = (int *)g_pb_wid_table_12;
        gpAppTables[109] = (int *)g_pb_sft_table_12;
        gpAppTables[110] = (int *)g_sfb_sft_table_12;
        gpAppTables[111] = (int *)g_pb_s_wid_table_12;
        gpAppTables[112] = (int *)g_pb_s_sft_table_12;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_12;
    }
	else if (sf == 11025)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_11;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_11;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_11;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_11;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_11;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_11;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_8;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_8;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_8;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_11;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_11;
        gpAppTables[87] = (long int*)  g_spread_long_table_11;
        gpAppTables[88] = (long int*)  g_spread_short_table_11;
        gpAppTables[90] = (long int*)  g_short_qthr_table_11;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_11;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_11;
        gpAppTables[93] = (long int*)  g_long_qthr_table_11;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_11;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_11;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_11;
        gpAppTables[108] = (int *)g_pb_wid_table_11;
        gpAppTables[109] = (int *)g_pb_sft_table_11;
        gpAppTables[110] = (int *)g_sfb_sft_table_11;
        gpAppTables[111] = (int *)g_pb_s_wid_table_11;
        gpAppTables[112] = (int *)g_pb_s_sft_table_11;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_11;
    }
	else if (sf == 48000)
    {
        gpAppTables[33] = (int*) g_AACE_sprdf_indices_long_table_48;
        gpAppTables[37] = (int*) g_rnorm_short_ns_table_48;
        gpAppTables[38] = (int*) g_short_qthr_ns_table_48;
        gpAppTables[39] = (int*) g_rnorm_long_ns_table_48;
        gpAppTables[40] = (int*) g_long_qthr_ns_table_48;
        gpAppTables[41] = (int*) g_short_mask_level_diff_ns_table_48;
#if 0
        gpAppTables[42] = (int*) g_sfb_table_whole_short_shift_table_44;
        gpAppTables[43] = (int*) g_sfb_table_whole_long_shift_table;
        gpAppTables[44] = (int*) g_part_table_whole_long_shift_table;
        gpAppTables[45] = (int*) g_part_sfb_table_long_width_table;
        gpAppTables[46] = (int*) g_part_table_whole_short_shift_table_44;
        gpAppTables[47] = (int*) g_AACE_numshifts_band_s_table_44;
        gpAppTables[66] = (int*) g_sfb_table_whole_short_width_table_8;
        gpAppTables[67] = (int*) g_sfb_table_whole_long_width_table;
        gpAppTables[68] = (int*) g_part_table_whole_short_width_table_8;
        gpAppTables[69] = (int*) g_part_table_whole_long_width_table;
        gpAppTables[71] = (int*) g_part_sfb_table_long_table;
        gpAppTables[70] = (int*) g_part_sfb_table_short_table_8;
#else
        gpAppTables[42] = NULL;
        gpAppTables[43] = NULL;
        gpAppTables[44] = NULL;
        gpAppTables[45] = NULL;
        gpAppTables[46] = NULL;
        gpAppTables[47] = NULL;
        gpAppTables[66] = NULL;
        gpAppTables[67] = NULL;
        gpAppTables[69] = NULL;
        gpAppTables[68] = NULL;
        gpAppTables[70] = NULL;
        gpAppTables[71] = NULL;
#endif
        gpAppTables[63] = (int*) g_AACE_sprdf_indices_short_table_48;
        gpAppTables[65] = (int*) g_mask_level_diff_ns_table_48;
        gpAppTables[87] = (long int*)  g_spread_long_table_48;
        gpAppTables[88] = (long int*)  g_spread_short_table_48;
        gpAppTables[90] = (long int*)  g_short_qthr_table_48;
        gpAppTables[91] = (long int*)  g_rnorm_short_table_48;
        gpAppTables[92] = (long int*)  g_rnorm_long_table_48;
        gpAppTables[93] = (long int*)  g_long_qthr_table_48;
        gpAppTables[94] = (long int*)  g_short_mask_level_diff_table_48;
        gpAppTables[95] = (long int*)  g_fixed_ratio_long_table_48;
        gpAppTables[96] = (long int*)  g_mask_level_diff_table_48;
        gpAppTables[108] = (int *)g_pb_wid_table_48;
        gpAppTables[109] = (int *)g_pb_sft_table_48;
        gpAppTables[110] = (int *)g_sfb_sft_table_48;
        gpAppTables[111] = (int *)g_pb_s_wid_table_48;
        gpAppTables[112] = (int *)g_pb_s_sft_table_48;
        gpAppTables[113] = (int *)g_sfb_s_sft_table_48;
    }    
}


/*================================================================================================*/
/*================================================================================================*/
int RunEncoder( void * ptr )
{
    assert( ptr );    

    sAacEncoderHandler * pHandler = (sAacEncoderHandler*)ptr;
    
    /* Set priority for the PRE_EMPTION test case. */
    if( PRE_EMPTION == gTestappConfig.mTestCase )
    {
		int nice_inc = (int)(( 20.0f / gNumThreads ) * pHandler->mIndex);
		if( nice(nice_inc) < 0 )
		{
			TST_RESM( TWARN, "%s : nice(%d) has failed", 
                      __FUNCTION__, nice_inc );
		}
    }
    
    /* Run TestEngine. */
    pHandler->mLtpRetval = TestEngine( pHandler );        
    
    /* Perform bitmatching. */
    const char * fileName1 = pHandler->mOutputFile.mName;
    const char * fileName2 = pHandler->mpParams->mReferenceFileName;
    if( DoFilesExist( fileName1, fileName2 ) )
    {              
        if( !DoBitmatch( pHandler ) )
        {
            if( gTestappConfig.mVerbose )
                TST_RESM( TFAIL, "Thread[%lu] Bitmatch failed (%s vs %s)", 
                          pHandler->mIndex, fileName1, fileName2 );        
            pHandler->mLtpRetval = TFAIL;
        }
        else
        {
            if( gTestappConfig.mVerbose )
                TST_RESM( TINFO, "Thread[%lu] Bitmatch passed (%s vs %s)", 
                          pHandler->mIndex, fileName1, fileName2 );                    
        }        
    }
    
    /* Return LTP result. */
    return pHandler->mLtpRetval;
}


/*================================================================================================*/
/*================================================================================================*/
int TestEngine( sAacEncoderHandler * pHandler ) 
{
    assert( pHandler );
    assert( pHandler->mpParams );
    
    const sHandlerParams  * pParams = pHandler->mpParams;
    AACE_Encoder_Config   * pEncConfig = &pHandler->mEncConfig;
           	
    /* Open all necessary files. */
    if( pHandler->mInputFile.mName )
    {
        pHandler->mInputFile.mStream = 
            fopen( pHandler->mInputFile.mName, "rb" );
        if( !pHandler->mInputFile.mStream )
        {
            TST_RESM( TWARN, "%s : Can't open %s", 
                      __FUNCTION__, pHandler->mInputFile.mName );
            return TFAIL;
        }
    }
    if( pHandler->mOutputFile.mName )
    {
        pHandler->mOutputFile.mStream = 
            fopen( pHandler->mOutputFile.mName, "wb" );
        if( !pHandler->mOutputFile.mStream )
        {
            TST_RESM( TWARN, "%s : Can't create %s", 
                      __FUNCTION__, pHandler->mOutputFile.mName );
            return TFAIL;
        }
    }        

    /* Init wave input. */
    int nWChannels;
    double wFreq;
    long nWSamples;
    Util_WaveReaderInit( pHandler, &nWChannels, &wFreq, &nWSamples );
        
    long samplingRate = (long)(wFreq+0.5);
    
    pHandler->mFramesCount = -2;
    
    AACE_AppInitTables( samplingRate );

    /* Fill up the relocated data position. */
    pEncConfig->app_initialized_data_start = gpAppTables;

    /* The input samples to the encoder is always 1024. */
    pEncConfig->app_num_samples = AACE_FRAME_SIZE;

    /* Set the bitrate for encoding. Eg for 96kbps. */
    pEncConfig->app_bitrate = pParams->mBitrate;

    /* Set the sampling frequency at which the audio samples are sampled. */
    pEncConfig->app_sampling_freq = samplingRate;

    /* Set the stero/mono mode for encoding. */
    //pEncConfig->app_stereo_mode = pParams->mStereoMode ? AACE_STEREO : AACE_MONO;    
    pEncConfig->app_stereo_mode = AACE_STEREO;    

    /* Query for memory. */
    CALL_AAC_ENCODER(
        AACE_query_enc_mem( pEncConfig ),
        "AACE_query_enc_mem" );    

    /* Allocate the required memory. */	    
    int nReq = pEncConfig->aace_mem_info.aace_num_reqs;
    int i;
    for( i = 0; i < nReq; ++i )
    {
        AACE_Mem_Alloc_Info_Sub * pMemInfoSub = &pEncConfig->aace_mem_info.mem_info_sub[i];
        
        if( AACE_FAST_MEMORY == pMemInfoSub->aace_type )
        {
            pMemInfoSub->app_base_ptr = alloc_fast( pMemInfoSub->aace_size );
            if( !pMemInfoSub->app_base_ptr )
            {
                TST_RESM( TWARN, "%s : Can't allocate %d bytes memory",
                    __FUNCTION__, pMemInfoSub->aace_size );
                return TFAIL;
            }
        }
        else
        {
            pMemInfoSub->app_base_ptr = alloc_slow( pMemInfoSub->aace_size );
            if( !pMemInfoSub->app_base_ptr )
            {
                TST_RESM( TWARN, "%s : Can't allocate %d bytes memory",
                    __FUNCTION__, pMemInfoSub->aace_size );
                return TFAIL;
            }
        }
    }

    /* Allocate memory for input buffer. */    
    pHandler->mInpBufferSz = sizeof(short)*2*AACE_FRAME_SIZE;
    pHandler->mpInpBuffer = (long*)alloc_fast( pHandler->mInpBufferSz );
    if( !pHandler->mpInpBuffer )
    {
        TST_RESM( TWARN, "%s : Can't allocate input buffer (%d bytes)",
            __FUNCTION__, pHandler->mInpBufferSz );
        return TFAIL;
    }    
    pEncConfig->app_input_buffer = (AACE_INT32*)pHandler->mpInpBuffer;
    
    /* Allocate memory for the output buffer. */    
    pEncConfig->aace_outbuffer_length = (AACE_INT32*) alloc_fast( sizeof(int) );
    pEncConfig->aace_output_bitstream = (AACE_UINT8*) alloc_fast( AACE_OUTBUF_SIZE );
    if( !pEncConfig->aace_outbuffer_length ||
        !pEncConfig->aace_output_bitstream )
    {
        TST_RESM( TWARN, "%s : Can't allocate output buffer",
            __FUNCTION__ );
        return TFAIL;
    }

    /* All the encoder related intializations are done here. This also includes
       table initializations and other variable initializations. */
    CALL_AAC_ENCODER(
        AACE_Encoder_Init( pEncConfig ),
        "AACE_Encoder_Init" );    

        
    /* Print some information for the verbose mode. */
    if( gTestappConfig.mVerbose )
    {
       
    }	      
    
    #define ENCODE_ROUTINE AACE_Encode_Frame
                     
    /* Main encoding loop */
    int encodingResult = AACE_OK;    
    AACE_INT16 tempSampleBuffer [AACE_NUM_AUDIO_CHANNELS][AACE_FRAME_SIZE];
    int nSamples;
    while( 0 != (nSamples = Util_WaveReadData( pHandler, tempSampleBuffer, AACE_FRAME_SIZE )) )
    {            
        if( AACE_FRAME_SIZE != nSamples )
        {
            memset(&tempSampleBuffer[0][nSamples], 0, 
                (AACE_FRAME_SIZE - nSamples) * sizeof(short) );
            memset(&tempSampleBuffer[1][nSamples], 0,
                (AACE_FRAME_SIZE - nSamples) * sizeof(short) );
        }
        
        Util_PackInputData( pEncConfig, tempSampleBuffer );
                
        /* Call aac Encoder main routine. */        
        CALL_AAC_ENCODER(
            ENCODE_ROUTINE( pEncConfig ),
            "AACE_Encode_Frame" );                                          
            
        /* Handle the decoding result. */
        encodingResult = pHandler->mLastAACEncError;    
        if( AACE_OK == encodingResult )
        {
            if( pHandler->mOutputFile.mStream )
                DoDataOutput( pHandler );        
        }                  
        
        /* Increase the frame counter. */
        ++pHandler->mFramesCount;
    }
    //////////////////////////////////////////////////////////////////////////    
    memset( &tempSampleBuffer[0][0], 0, AACE_FRAME_SIZE * sizeof(short) );
    memset( &tempSampleBuffer[1][0], 0, AACE_FRAME_SIZE * sizeof(short) );
    Util_PackInputData( pEncConfig , tempSampleBuffer );
    CALL_AAC_ENCODER(
            ENCODE_ROUTINE( pEncConfig ),
            "AACE_Encode_Frame" );   
    if( pHandler->mOutputFile.mStream )
        DoDataOutput( pHandler );  
        
    memset( &tempSampleBuffer[0][0], 0, AACE_FRAME_SIZE * sizeof(short) );
    memset( &tempSampleBuffer[1][0], 0, AACE_FRAME_SIZE * sizeof(short) );
    Util_PackInputData( pEncConfig , tempSampleBuffer );
    CALL_AAC_ENCODER(
            ENCODE_ROUTINE( pEncConfig ),
            "AACE_Encode_Frame" );   
    if( pHandler->mOutputFile.mStream )
        DoDataOutput( pHandler );  
        
    TST_RESM( TINFO, "Thread[%lu] Encoding complited", pHandler->mIndex );        

    /* Cleanup the handler */
    CleanupHandler( pHandler );
    
    /* Return succees */
    return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
void DoDataOutput( sAacEncoderHandler * pHandler )
{
    assert( pHandler );
    assert( pHandler->mOutputFile.mStream );
    assert( pHandler->mpParams );  
    
    AACE_Encoder_Config * pConfig = &pHandler->mEncConfig;
    FILE * out = pHandler->mOutputFile.mStream;
    
    unsigned char * pOutBuf = pConfig->aace_output_bitstream;
    size_t outBufSz = (size_t)(*( pConfig->aace_outbuffer_length ));
    
#ifdef ADIF_HEADER_ENABLE
	int numBits = pConfig->NumHeaderBits;

    /* Write the output bitstream in file */
    if( -2 != pHandler->mFramesCount )
    {
		outBufSz = (size_t)(*(pConfig->aace_outbuffer_length)-(numBits/8));
		pOutBuf  += numBits / 8;
	}	
#endif
    
    size_t i;    
    for( i = 0; i < outBufSz; ++i )
        fwrite( &pOutBuf[i], 1, 1, out );
    fflush( out );     
}


/*================================================================================================*/
/*================================================================================================*/
void ResetHandler( sAacEncoderHandler * pHandler )
{
    assert( pHandler );
        
    memset( pHandler, 0, sizeof(sAacEncoderHandler) );    
    pHandler->mIndex             = 0;
    pHandler->mIsThreadFinished  = FALSE;    
}


/*================================================================================================*/
/*================================================================================================*/
void CleanupHandler( sAacEncoderHandler * pHandler )
{
    /* Close all the open files */
    if( pHandler->mInputFile.mStream )
    {
        fclose( pHandler->mInputFile.mStream );
        pHandler->mInputFile.mStream = NULL;
    }
    if( pHandler->mOutputFile.mStream )
    {       
        fclose( pHandler->mOutputFile.mStream );
        pHandler->mOutputFile.mStream = NULL;
    }        

    /* Free input/output buffers. */
    SAFE_DELETE( pHandler->mpInpBuffer );
    pHandler->mInpBufferSz = 0;
    SAFE_DELETE( pHandler->mpOutBuffer );
    pHandler->mOutBufferSz = 0;
    
    /* Free memory allocated by the QueryDecMem. */   
    int i;
    for( i = 0; i < AACE_MAX_NUM_MEM_REQS; ++i )
    {
        AACE_Mem_Alloc_Info_Sub * pMemInfoSub = &pHandler->mEncConfig.aace_mem_info.mem_info_sub[i];
        SAFE_DELETE( pMemInfoSub->app_base_ptr );
    }
        
    SAFE_DELETE( pHandler->mEncConfig.aace_outbuffer_length );
    SAFE_DELETE( pHandler->mEncConfig.aace_output_bitstream );    
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( sAacEncoderHandler * pHandler )
{
    assert( pHandler );
    assert( pHandler->mpParams );
    
    const char * fname1 = pHandler->mpParams->mOutputFileName;
    const char * fname2 = pHandler->mpParams->mReferenceFileName;
    
    int out, ref;
    struct stat fstat_out, fstat_ref;
    char *fptr_out, *fptr_ref;
    size_t filesize;
    int i;

    if( (out = open(fname1, O_RDONLY)) < 0 )
    {
        TST_RESM( TWARN, "%s : Thread[%lu] Can't open %s", __FUNCTION__, pHandler->mIndex, fname1 );
        return FALSE;
    }
    if ((ref = open(fname2, O_RDONLY)) < 0)
    {   
        TST_RESM( TWARN, "%s : Thread[%lu] Can't open %s", __FUNCTION__, pHandler->mIndex, fname2 );
    	close(out);
        return FALSE;
    }
    fstat( out, &fstat_out );
    fstat( ref, &fstat_ref );
    if( fstat_out.st_size != fstat_ref.st_size )
    {
        TST_RESM( TWARN, "%s : Thread[%lu] Files (%s vs %s) have the different sizes (%lu vs %lu)", 
                  __FUNCTION__, pHandler->mIndex, fname1, fname2, 
                  (unsigned long)fstat_out.st_size,
                  (unsigned long)fstat_ref.st_size );
	    close(out);
    	close(ref);
	    return FALSE;
    }
    filesize = fstat_out.st_size;
    fptr_out = (char*)mmap( 0, filesize, PROT_READ, MAP_SHARED, out, 0 );
    if( fptr_out == MAP_FAILED )
    {
        TST_RESM( TWARN, "%s : Thread[%lu] (FATAL) mmap failed for %s", __FUNCTION__, pHandler->mIndex, fname1 );
    	close( out );
	    close( ref );
        return FALSE;
    }
    fptr_ref = (char*) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
    if( fptr_ref == MAP_FAILED )
    {
        TST_RESM( TWARN, "%s : Thread[%lu] (FATAL) mmap failed for %s", __FUNCTION__, pHandler->mIndex, fname2 );
	    close( out );
    	close( ref );
	    return FALSE;
    }
    close( out );
    close( ref );
    for( i = 0; i < filesize; ++i )
    {
    	if( *(fptr_ref + i) != *(fptr_out + i) )
	    {
            TST_RESM( TWARN, "%s : Thread[%lu] (%s vs %s) byte %d", __FUNCTION__, pHandler->mIndex, fname1, fname2, i );
	        munmap( fptr_ref, fstat_ref.st_size );
	        munmap( fptr_out, fstat_out.st_size );
    	    return FALSE;
    	}
    }
    munmap( fptr_ref, filesize );
    munmap( fptr_out, filesize );
    return TRUE;
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
void HogCpu()
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
    strncpy( pParams->mInputFileName,     entry[n++], MAX_STR_LEN );   
    strncpy( pParams->mOutputFileName,    entry[n++], MAX_STR_LEN );
    strncpy( pParams->mReferenceFileName, entry[n++], MAX_STR_LEN );      
    pParams->mBitrate = atoi( entry[n++] );
    pParams->mStereoMode = 1;
    /* Adjust parameters here... */    
    
    LList_PushBack( gpParamsList, pParams );
}


/*================================================================================================*/
/*================================================================================================*/
int NominalFunctionalityTest()
{
    sLinkedList * pNode;
    int i;
    int rv = TPASS;    
    sAacEncoderHandler * pHandler = gAacEncHandlers;     
        
    /* For the each entry */
    for( pNode = gpParamsList, i = 0; pNode; pNode = pNode->mpNext, ++i )
    {
        /* Reset the handler. */
        ResetHandler( pHandler );
    
        /* Get content. */
        pHandler->mpParams = (sHandlerParams*)pNode->mpContent;

        if( gTestappConfig.mVerbose )            
        {
            TST_RESM( TINFO, "Thread[%lu] Input bitstream: %s, Reference: %s", 
                      pHandler->mIndex, pHandler->mpParams->mInputFileName, 
                      pHandler->mpParams->mReferenceFileName );
        }                

        pHandler->mInputFile.mName = 
            strcmp( pHandler->mpParams->mInputFileName, NA ) != 0 ? 
            pHandler->mpParams->mInputFileName : NULL;
        pHandler->mOutputFile.mName = 
            strcmp( pHandler->mpParams->mOutputFileName, NA ) != 0 ? 
            pHandler->mpParams->mOutputFileName : NULL;
                
        /* Run the Encoder. */
        rv += RunEncoder( pHandler );        
    }
    
    return rv;    
}


/*================================================================================================*/
/*================================================================================================*/
int ReLocatabilityTest()
{
    sLinkedList * pNode;
    int i, j;
    int rv = TPASS;    
    sAacEncoderHandler * pHandler = gAacEncHandlers;     
        
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
                TST_RESM( TINFO, "Thread[%lu] Input bitstream: %s, Reference: %s", 
                      pHandler->mIndex, pHandler->mpParams->mInputFileName, 
                      pHandler->mpParams->mReferenceFileName );
            }                

            pHandler->mInputFile.mName = 
                strcmp( pHandler->mpParams->mInputFileName, NA ) != 0 ? 
                pHandler->mpParams->mInputFileName : NULL;
            pHandler->mOutputFile.mName = 
                strcmp( pHandler->mpParams->mOutputFileName, NA ) != 0 ? 
                pHandler->mpParams->mOutputFileName : NULL;
                
            /* Run the Encoder. */
            rv += RunEncoder( pHandler );        
            
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
int ReEntranceTest()
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
            ResetHandler( &gAacEncHandlers[i] );
            gAacEncHandlers[i].mIndex = i;
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
    sAacEncoderHandler * pHandler;        
    
    /* Run all bitstreams in separate threads. */
    for( pNode = pHead, i = 0; pNode && i < gNumThreads; pNode = pNode->mpNext, ++i )
    {        
        pHandler = gAacEncHandlers + i;
        ResetHandler( pHandler );
        pHandler->mIndex = i;
        
        /* Get content. */
        pHandler->mpParams = (sHandlerParams*)pNode->mpContent;

        if( gTestappConfig.mVerbose )            
        {
            TST_RESM( TINFO, "Thread[%lu] Input bitstream: %s, Reference: %s", 
                      pHandler->mIndex, pHandler->mpParams->mInputFileName, 
                      pHandler->mpParams->mReferenceFileName );
        }                
        
        pHandler->mInputFile.mName = 
            strcmp( pHandler->mpParams->mInputFileName, NA ) != 0 ? 
            pHandler->mpParams->mInputFileName : NULL;
        pHandler->mOutputFile.mName = 
            strcmp( pHandler->mpParams->mOutputFileName, NA ) != 0 ? 
            pHandler->mpParams->mOutputFileName : NULL;
        
        if( pthread_create( &pHandler->mThreadID, NULL, (void*)&RunEncoder, pHandler ) )
        {
            TST_RESM( TWARN, "%s : error creating thread %d", __FUNCTION__, i );
            return TFAIL;	    
        }	
    }    
    
    /* Wait for the each thread. */
    for( i = 0; i < gNumThreads; ++i )
    {
        pHandler = gAacEncHandlers + i;     
        pthread_join( pHandler->mThreadID, NULL );
    }
    for( i = 0; i < gNumThreads; ++i )
    {
	    pHandler = gAacEncHandlers + i;     
        rv += pHandler->mLtpRetval;
    }
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int PreEmptionTest()
{
    return ReEntranceTest();
}


/*================================================================================================*/
/*================================================================================================*/
int EnduranceTest()
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
int LoadTest()
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
void Util_ReadUnsignedNum( unsigned int * pValue, int nBytes, FILE * in )
{
    assert( pValue && in );

    int i;
    unsigned char temp;
    int temp1;
    *pValue = 0;
    for( i = 0; i < nBytes; ++i )
    {
        fread( &temp, 1, 1, in );
        temp1 = temp;
        *pValue = *pValue | (temp1 << i*8);
    }
}


/*================================================================================================*/
/*================================================================================================*/
void Util_WaveReaderInit( sAacEncoderHandler * pHandler, 
                          int * pNumChan, 
                          double * pFreq, 
                          long * pNumSamples )
{
    assert( pHandler );
    assert( pHandler->mInputFile.mStream );
    assert( pNumChan && pFreq && pNumSamples );

    unsigned int bps = 0, pcm = 0;
    unsigned int byteRate = 0;
    unsigned int junk = 0;
    char chunkIDFound = 0;
    unsigned int sampleRate = 0;
    FILE * in = pHandler->mInputFile.mStream;
        
    fseek(in, 20, SEEK_SET );
    
    /* Read numbers in little endian format. */
    Util_ReadUnsignedNum( &pcm, 2, in );
    Util_ReadUnsignedNum( pNumChan, 2, in );
    
    /* Supports only stereo. */
    if( *pNumChan == 1 )
        return;        
    Util_ReadUnsignedNum( &sampleRate, 4, in );
    *pFreq = (double)sampleRate;
    Util_ReadUnsignedNum( &byteRate, 4, in );
    Util_ReadUnsignedNum( &junk, 2, in );
    Util_ReadUnsignedNum( &bps, 2, in );
    if( pcm != 1 )
    {
        TST_RESM( TWARN, "%s : Read only PCM data!!!",
            __FUNCTION__ );
    }
    
    pHandler->mDataOffset = 36; 
    while( chunkIDFound == 0 )
    {
        char byte[4];
        
        fseek( in, pHandler->mDataOffset, SEEK_SET );
        fread( &byte[0], 1, 1, in );
        
        if( 'd' == byte[0] )
        {
            fread( &byte[1], 1, 1, in );
            fread( &byte[2], 1, 1, in );
            fread( &byte[3], 1, 1, in );
            if( byte[1] == 'a' && byte[2] == 't' &&  byte[3] == 'a' )
            {
            pHandler->mDataOffset += (4+4); /* Account for the
                                            size of data field and data
            chunk ID*/
            pHandler->mCurrReadOffset = pHandler->mDataOffset;                    
            chunkIDFound = 1;
            }
        }
        else
        {
            /* Check for end of file */
            if( byte[0] == (unsigned char)EOF )            
                return;                
            else
                pHandler->mDataOffset++;                
        }
    }
    
    /* Read the data size*/
    Util_ReadUnsignedNum( &pHandler->mWaveFileSize, 4, in );
    pHandler->mSamplesRemaining = pHandler->mWaveFileSize/4;
    *pNumSamples = pHandler->mSamplesRemaining;    
}


/*================================================================================================*/
/*================================================================================================*/
int  Util_WaveReadData   ( sAacEncoderHandler * pHandler, 
                           short sampleBuffer[AACE_NUM_AUDIO_CHANNELS][AACE_FRAME_SIZE], 
                           unsigned int frameSize )
{
    assert( pHandler );
    assert( pHandler->mInputFile.mStream );
        
    int numSampToRead = 0;
    int index = 0;
    unsigned char byte1;
    unsigned char byte2;
    unsigned short sampVal = 0;
    short actVal;
    FILE * in = pHandler->mInputFile.mStream;

    fseek( in, pHandler->mCurrReadOffset, SEEK_SET );

    if( pHandler->mSamplesRemaining > frameSize )
    {
        numSampToRead = frameSize;
        /* Update the read offset position - numSamples * bytesPersample*
         * numChanels */
        pHandler->mCurrReadOffset += (frameSize*4);
    }
    else
        numSampToRead = pHandler->mSamplesRemaining;
    
    if( numSampToRead )
        pHandler->mSamplesRemaining -= numSampToRead;
    
    for(index = 0; index < numSampToRead; index++)
    {
        fread( &byte1, 1, 1, in );
        fread( &byte2, 1, 1, in );
        sampVal = byte2 << 8 | byte1 ;
        actVal = (short)sampVal;
        sampleBuffer[0][index] = actVal;

        fread( &byte1, 1, 1, in );
        fread( &byte2, 1, 1, in );
        sampVal = byte2 << 8 | byte1 ;
        actVal = (short)sampVal;
        sampleBuffer[1][index] = actVal;
    }
    return numSampToRead; 
}


/*================================================================================================*/
/*================================================================================================*/
void Util_PackInputData( AACE_Encoder_Config * pEncConfig, 
                         AACE_INT16 tempInput[AACE_NUM_AUDIO_CHANNELS][AACE_FRAME_SIZE] )
{
    int samp;
    for( samp = 0; samp < AACE_FRAME_SIZE; ++samp )
    {
        int dummy, dummy1;

        dummy = 0;
        dummy  = tempInput[0][samp] << 16;
        dummy1 = tempInput[1][samp];
        dummy1 = dummy1 & 0xffff;
        dummy = dummy | dummy1;

        pEncConfig->app_input_buffer[samp] = dummy;
    }
}

/*================================================================================================*/
/*================================================================================================*/
void Debug_MyEncodeFrame( AACE_Encoder_Config * pEncConfig )
{
    assert( pEncConfig );
    *pEncConfig->aace_outbuffer_length = AACE_OUTBUF_SIZE;
    unsigned char * pOutputBuffer = pEncConfig->aace_output_bitstream;
    int i;
    for( i = 0; i < AACE_OUTBUF_SIZE; ++i )
        pOutputBuffer[i] = (unsigned char)(i % 256);
}


void CommonExit(
                int errorCode,                /* in: error code for exit() */
                char *message,                /* in: error message */
                ...)                          /* in: args as for printf */
{
    va_list args;

    va_start(args,message);
    fflush(stdout);
    fprintf(stderr,"%s: ERROR[%d]: ",__FUNCTION__,errorCode);
    vfprintf(stderr,message,args);
    fprintf(stderr,"\n");
    va_end(args);
    exit (errorCode);
}


void CommonWarning(
                   char *message,                /* in: warning message */
                   ...)                          /* in: args as for printf */
{
    va_list args;

    va_start(args,message);
    fflush(stdout);
    fprintf(stderr,"%s: WARNING: ",__FUNCTION__);
    vfprintf(stderr,message,args);
    fprintf(stderr,"\n");
    va_end(args);
}


#ifdef __cplusplus
}
#endif

