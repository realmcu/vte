/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file imelody_conv_test.c

@brief VTE C source imelody to midi converter test case

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
S.ZAVJALOV/zvjs001c   19/10/2004   TLSbo47116   Initial version.
S.ZAVJALOV/zvjs001c   07/02/2005   TLSbo47116   -
D.Simakov/smkd001c    07/04/2005   TLSbo47116   Imroved, endurance, load and
                                                robustness test cases were added.
D.Simakov/smkd001c    24/10/2005   TLSbo57009   Update
*/

#ifdef __cplusplus
extern "C"{
#endif

/* INCLUDE FILES */
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "imelody_conv_test.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>

/* LOCAL CONSTANTS */

/* LOCAL MACROS */

/* LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) */

/* LOCAL VARIABLES */

static imy_thread thread[MAX_iMY_CONV_THREADS];

/* GLOBAL CONSTANTS */

/* GLOBAL VARIABLES */

extern int vb_mode;
extern int iter_num;
flist_t *file_list;

/* LOCAL FUNCTION PROTOTYPES */

int nominal_functionality_test();
int reentrance_test();
int endurance_test();
int load_test();
int robustness_test();

void * run_convert_thread(void *param);

int imelody_test_engine(imy_thread *engine_param);
int compare_files(flist_t *node, int thr_num);
void close_files(imy_thread *thr);
int open_files(imy_thread *sthread, flist_t *file_list);
flist_t *mk_entry(const char *inp_fname, const char *out_fname, const char *ref_fname);
void destroy_filelist (flist_t *file_list);
IMYCONV_Void    vAPPImyGetData (IMYCONV_U32 u32Offset,
                                IMYCONV_U8 *pu8InBuf,
                                IMYCONV_U16 *pu16Size,
                                IMYCONV_Void *pvConfig
                               );
IMYCONV_Void  vAPPImyPutData (IMYCONV_U8 *pu8OutBuf,
                              IMYCONV_U16 *pu16OutBufSize,
                              IMYCONV_U8  u8BufReqFlag,
                              IMYCONV_Void *pvConfig
                             );
eIMYConvReturnType eIMYConvFreeMemory ( sIMYConvConfigType *psImyConfig,
     sIMYConvInputDetailType *psInDetails,
     sIMYConvOutputDetailType *psOutDetails
                                       );
int make_filelist (char *cfg_file_name);

/* LOCAL FUNCTIONS */

int do_files_exist( const char * fname1, const char * fname2 )
{
    FILE * fstream1  fopen( fname1, "r" );
    if( fstream1 )
    {
        fclose( fstream1 );
        FILE * fstream2  fopen( fname2, "r" );
        if( fstream2 )
        {
            fclose( fstream2 );
            return TRUE;
        }
    }
    return FALSE;
}

/* nominal_functionality_test */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int nominal_functionality_test()
{
    int rv  TPASS;
    flist_t *node;
    char buffer[256];
    int tid  0;

    node  file_list;

    while( node )
    {
        tst_resm( TINFO, "input file : %s", node->inp_fname );

        /* call test engine */
        if( open_files(&thread[tid], node) ! TPASS )
        return TFAIL;
     rv + imelody_test_engine( &thread[tid] );
    close_files( &thread[tid] );

        /* do bitmatch */
        sprintf( buffer, "%sthr%d.mid", node->out_fname, tid );
        if( do_files_exist( buffer, node->ref_fname ) )
        {
         int cmp  compare_files( node, tid );
            if( cmp ! TPASS )
            {
                tst_resm( TWARN, "bitmach (%s vs %s) failed", buffer, node->ref_fname );
                rv  TFAIL;
            }
            else
            {
                tst_resm( TINFO, "bitmach (%s vs %s) passed", buffer, node->ref_fname );
            }
        }
        node  node->next;
    }
    return rv;
}

/* reentrance_test */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int reentrance_test()
{
    int i, th_count;
    int ret  TPASS;
    char buffer[256];

    for (i  0; i < MAX_iMY_CONV_THREADS; i++)
    {
        if (pthread_create(&(thread[i].tid), NULL, (void *)&run_convert_thread, (void *)&(thread[i])) ! 0)
    {
         tst_resm(TWARN, "ERROR in reentrance_test(): cannot create thread %d: %s", i + 1, strerror(errno));
        ret  TFAIL;
         break;
    }
    }
    th_count  i;
    for (i  0; i < th_count; i++)
     pthread_join(thread[i].tid, NULL);

    for (i  0; i < th_count; i++)
    if (thread[i].th_err ! TPASS)
         ret  TFAIL;

    for( i  0; i < th_count; ++i )
    {
        flist_t * node  file_list;
        while( node )
        {
            /* do bitmatch */
            sprintf( buffer, "%sthr%d.mid", node->out_fname, i );
            if( do_files_exist( buffer, node->ref_fname ) )
            {
             int cmp  compare_files( node, i );
                if( cmp ! TPASS )
                {
                    tst_resm( TWARN, "bitmach (%s vs %s) failed", buffer, node->ref_fname );
                    ret  TFAIL;
                }
                else
                {
                    tst_resm( TINFO, "bitmach (%s vs %s) passed", buffer, node->ref_fname );
                }
            }
            node  node->next;
        }
    }


    return ret;
}

int endurance_test()
{
    int i;
    int rv  TPASS;

    for( i  0; i < iter_num; ++i )
    {
        tst_resm( TINFO, "The %d iteration is started", i+1 );
        rv + nominal_functionality_test();
        tst_resm( TINFO, "The %d iteration is completed", i+1 );
    }
    return rv;
}

/**/
/**/
void hogcpu()
{
    while( 1 )
    {
        sqrt( rand() );
    }
}

int load_test()
{
    int rv  TFAIL;
    pid_t pid;

    switch( pid  fork() )
    {
     case -1:
         tst_resm( TWARN, "load_test : fork failed" );
        return rv;
     case 0:
            /* child process */
         hogcpu();
    default:
            /* parent */
         sleep(1);
         rv  nominal_functionality_test();
        /* kill child process once decode/encode loop has ended */
         if( kill( pid, SIGKILL ) ! 0 )
         {
       tst_resm( TWARN, "load_test : Kill(SIGKILL) error" );
         return rv;
         }
    }
    return rv;
}

int robustness_test()
{
    int rv  TPASS;
    flist_t * node;
    int tid  0;

    node  file_list;

    while( node )
    {
        tst_resm( TINFO, "input file : %s", node->inp_fname );

        /* call test engine */
        if( open_files(&thread[tid], node) ! TPASS )
        return TFAIL;
     int res  imelody_test_engine( &thread[tid] );
    close_files( &thread[tid] );

        rv + res;

        node  node->next;
    }
    return rv ! TPASS ? TPASS : TFAIL;
}

/* run_convert_thread */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void *run_convert_thread(void *param)
{
    imy_thread *thr;
    flist_t *next;
    flist_t *node;

    node  file_list;
    thr  (imy_thread *)param;
    while (node)
    {
        next  node->next;
 if (open_files(thr, node) ! TPASS)
 {
     thr->th_err  TFAIL;
     return NULL;
 }
 if (imelody_test_engine(thr) ! TPASS)
     thr->th_err  TFAIL;
 close_files(thr);
        node  next;
    }
    return NULL;
}

/* compare_files */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int compare_files(flist_t *node, int thr_num)
{
    int out, ref;
    struct stat fstat_out, fstat_ref;
    char *fptr_out, *fptr_ref;
    size_t filesize;
    int i;
    char out_filename[MAX_STR_LEN];

    sprintf(out_filename, "%sthr%d.mid", node->out_fname, thr_num);
    if ((out  open(out_filename, O_RDONLY)) < 0)
    {
 tst_resm(TWARN, "ERROR in compare_files(): %s open() returns %s", node->out_fname, strerror(errno));
        return TFAIL;
    }
    if ((ref  open(node->ref_fname, O_RDONLY)) < 0)
    {
 tst_resm(TWARN, "ERROR in compare_files(): %s open() returns %s", node->ref_fname, strerror(errno));
 close(out);
        return TFAIL;
    }
    if (vb_mode)
 printf("Comparison %s and %s in progress", node->out_fname, node->ref_fname);
    fstat(out, &fstat_out);
    fstat(ref, &fstat_ref);
    if (fstat_out.st_size ! fstat_ref.st_size)
    {
 if (vb_mode)
    printf("   Failed\n");
 tst_resm(TWARN, "ERROR in compare_files(): file size %s is not equal to %s", node->out_fname, node->ref_fname);
 close(out);
 close(ref);
 return TFAIL;
    }
    filesize  fstat_out.st_size;
    fptr_out  mmap(0, filesize, PROT_READ, MAP_SHARED, out, 0);
    if (fptr_out  MAP_FAILED)
    {
 close(out);
 close(ref);
 tst_resm(TWARN, "ERROR in compare_files(): can not make mmap");
 return TFAIL;
    }
    fptr_ref  (char *) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
    if (fptr_ref  MAP_FAILED)
    {
 close(out);
 close(ref);
 tst_resm(TWARN, "ERROR in compare_files(): can not make mmap");
 return TFAIL;
    }
    close(out);
    close(ref);
    for(i  0; i < filesize; i++)
    {
 if (*(fptr_ref + i) ! *(fptr_out + i))
 {
     if (vb_mode)
  printf("   Failed\n");
     tst_resm(TWARN, "ERROR in compare_files(): file %s is not equal to %s", node->out_fname, node->ref_fname);
     munmap(fptr_ref, fstat_ref.st_size);
     munmap(fptr_out, fstat_out.st_size);
     return TFAIL;
 }
    }
    munmap(fptr_ref, filesize);
    munmap(fptr_out, filesize);
    if (vb_mode)
 printf("   Ok\n");
    return TPASS;
}

/* close_files */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void close_files(imy_thread *thr)
{
    fclose(thr->inp_file.ptr);
    fclose(thr->out_file.ptr);
}

/* open_files */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int open_files(imy_thread *thr, flist_t *node)
{
    sprintf(thr->inp_file.name, "%s", node->inp_fname);
    sprintf(thr->out_file.name, "%sthr%d.mid", node->out_fname, thr->number);
    thr->inp_file.ptr  fopen(thr->inp_file.name, "r");
    if (thr->inp_file.ptr  NULL)
    {
 tst_resm(TWARN, "ERROR in open_files(): %s fopen() returns %s", thr->inp_file.name, strerror(errno));
        return TFAIL;
    }
    thr->out_file.ptr  fopen(thr->out_file.name, "w");
    if (thr->out_file.ptr  NULL)
    {
 tst_resm(TWARN, "ERROR in open_files(): %s fopen() returns %s", thr->out_file.name, strerror(errno));
 fclose(thr->out_file.ptr);
        return TFAIL;
    }
    return TPASS;
}

/* mk_entry */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
flist_t *mk_entry(const char *inp_fname, const char *out_fname, const char *ref_fname)
{
    flist_t *list  malloc(sizeof(flist_t));
    if (list)
    {
        if ( (strlen(inp_fname) < MAX_STR_LEN) && (strlen(out_fname) < MAX_STR_LEN))
        {
            strcpy(list->inp_fname, inp_fname);
            strcpy(list->out_fname, out_fname);
            strcpy(list->ref_fname, ref_fname);
        }
        else
            tst_resm(TWARN, "ERROR in mk_entry(): one of file names too long");
    }
    else
        tst_resm(TWARN, "ERROR in mk_entry(): malloc() returns %s", strerror(errno));
    return list;
}

/* destroy_filelist */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void destroy_filelist (flist_t *node)
{
    flist_t *next;

    while(node)
    {
        next  node->next;
        free(node);
        node  next;
    }
}

/* eIMYConvFreeMemory */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
eIMYConvReturnType eIMYConvFreeMemory (sIMYConvConfigType *psImyConfig,
                                       sIMYConvInputDetailType *psInDetails,
                                       sIMYConvOutputDetailType *psOutDetails
                                       )
{
    if (psImyConfig->pvIMYConvInfoStructPtr ! IMYCONV_NULL)
    {
        mem_free(psImyConfig->pvIMYConvInfoStructPtr);
        psImyConfig->pvIMYConvInfoStructPtr  IMYCONV_NULL;
    }
    if (psImyConfig->pu8APPIMYConvInfoStructPtr ! IMYCONV_NULL)
    {
        mem_free (psImyConfig->pu8APPIMYConvInfoStructPtr);
        psImyConfig->pu8APPIMYConvInfoStructPtr  IMYCONV_NULL;
    }
    if (psImyConfig->pvAPPImyGetData ! IMYCONV_NULL)
    {
        psImyConfig->pvAPPImyGetData  IMYCONV_NULL;
    }
    if (psImyConfig->pvAPPImyPutData ! IMYCONV_NULL)
    {
        psImyConfig->pvAPPImyPutData  IMYCONV_NULL;
    }

    if (psInDetails ! IMYCONV_NULL)
    {
        if (psInDetails->pu8InBuf ! IMYCONV_NULL)
        {
            mem_free (psInDetails->pu8InBuf);
            psInDetails->pu8InBuf  IMYCONV_NULL;
        }
        mem_free (psInDetails);
        psInDetails  IMYCONV_NULL;
    }
    /* return success */
    return E_IMYCONV_OK;
}

/* vAPPImyPutData */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
IMYCONV_Void  vAPPImyPutData (IMYCONV_U8 *pu8OutBuf,
                              IMYCONV_U16 *pu16OutBufSize,
                              IMYCONV_U8  u8BufReqFlag,
                              IMYCONV_Void *pvConfig
                             )
{
    IMYCONV_U16 u16Size;
    IMYCONV_S8  as8TrackSize[4]  {0,0,0,0};

    sIMYConvConfigType *psImyConfig  IMYCONV_NULL;
    psImyConfig  (sIMYConvConfigType *) pvConfig;

    IMYCONV_FILE *OutFileName;
    int i;

    for (i  0; i < MAX_iMY_CONV_THREADS; i++)
    {
        if (thread[i].psImyConfig  psImyConfig)
        {
            OutFileName  thread[i].out_file.ptr;
            break;
        }
    }
    psImyConfig->u32MIDIBufferSize + *pu16OutBufSize;
    /* Write output data to file, file pointer  pfOutFile */
    u16Size  fwrite (pu8OutBuf, sizeof(IMYCONV_U8), *pu16OutBufSize, OutFileName);
    if (u16Size ! *pu16OutBufSize)
    {
        /* error in writting data */
        pu8OutBuf  IMYCONV_NULL;
        *pu16OutBufSize  0;
    }
    else
 if (u8BufReqFlag  IMYCONV_TRUE)
 {
        /* allocate new buffer */
        pu8OutBuf  pu8OutBuf;
        *pu16OutBufSize  IMYCONV_OUT_BUF_SIZE;
 }
 else
 {
        /* must be end of conversion */
        /* update MIDI buffer size */
        psImyConfig->u32MIDIBufferSize - IMYCONV_MIDI_TRACK_OFFSET;

        as8TrackSize[0]  ((psImyConfig->u32MIDIBufferSize >>24) & 0xFF);
        as8TrackSize[1]  ((psImyConfig->u32MIDIBufferSize >>16) & 0xFF);
     as8TrackSize[2]  ((psImyConfig->u32MIDIBufferSize >>8) & 0xFF);
     as8TrackSize[3]  ((psImyConfig->u32MIDIBufferSize) & 0xFF);

        /* seek at track size position */
     fseek(OutFileName, (IMYCONV_MIDI_TRACK_OFFSET-4), SEEK_SET);

     /* update 4 byte of track size */
     fwrite (as8TrackSize, sizeof(IMYCONV_S8), 4, OutFileName);

        pu8OutBuf  IMYCONV_NULL;
        *pu16OutBufSize  0;
        }
    return;
}

/* vAPPImyGetData */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
IMYCONV_Void    vAPPImyGetData (IMYCONV_U32 u32Offset,
                                IMYCONV_U8 *pu8InBuf,
                                IMYCONV_U16 *pu16Size,
                                IMYCONV_Void *pvConfig
                               )
{
    IMYCONV_U16 u16Size  0;
    IMYCONV_U16 u16InBufSize  0;
    sIMYConvConfigType *psImyConfig  IMYCONV_NULL;
    IMYCONV_FILE *InFileName;
    int i;

    psImyConfig  (sIMYConvConfigType *) pvConfig;

    for (i  0; i < MAX_iMY_CONV_THREADS; i++)
    {
 if (thread[i].psImyConfig  psImyConfig)
 {
     InFileName  thread[i].inp_file.ptr;
     break;
 }
    }
    //u16InBufSize  psImyConfig->psImyInDetails->u16InBufSize;
    /* requested size */
    u16InBufSize  *pu16Size;
    /* seek to requested offset */
    fseek(InFileName, u32Offset, SEEK_SET);
    /* Read iMelody data from file, file pointer  InFileName */
    u16Size  fread (pu8InBuf, sizeof(IMYCONV_U8), u16InBufSize, InFileName);
    if ( (u16Size ! u16InBufSize) && (feof(InFileName)  0) )
    {
        psImyConfig->u8EndOfFileReached  IMYCONV_TRUE;
        /* error condition, set return size to zero so that converter
         * takes appropriate action
         */
        *pu16Size  0;
        /*Free all the allocated memory;*/
 tst_resm(TWARN, "ERROR in vAPPImyGetData: error condition");
        return;
    }
    else /* no error in reading */
    {
        *pu16Size  u16Size;
    }
    return;
}

/* imelody_test_engine */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int imelody_test_engine(imy_thread *engine_param)
{
    IMYCONV_U8  *pu8InBuf  IMYCONV_NULL;   /* Input buffer */
    IMYCONV_U8  *pu8OutBuf  IMYCONV_NULL;  /* SMF output buffer */
    sIMYConvMemAllocInfoSubType *psMem  IMYCONV_NULL; /* memory info sub str */
    eIMYConvReturnType  eRetVal  E_IMYCONV_ERROR;    /* to store return value */
    sIMYConvInputDetailType *psInDetails  IMYCONV_NULL; /* pointer to input details structure */
    sIMYConvOutputDetailType  *psOutDetails  IMYCONV_NULL; /* pointer to o/p
                                                             detail structure */
    sIMYConvImelodyInfoType *psImyInfo  IMYCONV_NULL;  /* pointer to structure
                                                           that contains imelody
                                                           file information */
    IMYCONV_S32     s32Index  0;   /* Local couter */
    IMYCONV_S32     s32Nr  0;
    IMYCONV_U32     u32FrameNumber  0;  /* Frame counter */
    IMYCONV_U32     u32FrameDuration  1000; /* frame duration in msec */
    struct timeval  ti,to;
    float           s32EncodeTime;

    engine_param->psImyConfig  &engine_param->sImyConfig;

    /* Initialize config structure member */
    engine_param->psImyConfig->pu8APPIMYConvInfoStructPtr  BEGIN_IMYCONV_DATA;
    engine_param->psImyConfig->pvIMYConvInfoStructPtr  IMYCONV_NULL;
    /* Instrument to be used for MIDI playback */
    engine_param->psImyConfig->u8APPImyInstruments  IMYCONV_DEFAULT_INSTRUMENT;
    /* Manufacturer ID , some default */
    engine_param->psImyConfig->u32APPImyManufacturerID  0x7D;
    /* Manufacturer sub ID */
    engine_param->psImyConfig->u8APPImySubID  0x1;
    /* Initialize with callback function pointer */
    engine_param->psImyConfig->pvAPPImyGetData  vAPPImyGetData;
    engine_param->psImyConfig->pvAPPImyPutData  vAPPImyPutData;

    engine_param->psImyConfig->u32APPImyStartTime  0;
    engine_param->psImyConfig->u32APPImyDuration  0;
    engine_param->psImyConfig->u32ImyCurrentTime  0;

    /* to indicate end of imelody file */
    engine_param->psImyConfig->u8EndOfFileReached  IMYCONV_FALSE;
    /* midi buffer size, will get update in putdata function */
    engine_param->psImyConfig->u32MIDIBufferSize  0;
    /* Initialize configuration structure memory to IMYCONV_NULL */
    for (s32Index0; s32Index < IMYCONV_MAX_NUM_MEM_REQS ; s32Index++)
    {
        engine_param->psImyConfig->sIMYConvMemInfo.asMemInfoSub[s32Index].pvAPPIMYConvMemBasePtr  IMYCONV_NULL;
    }
    /* Query for memory */
    eRetVal  eIMYConvQueryMem (engine_param->psImyConfig);

    if (eRetVal ! E_IMYCONV_OK)
    {
        /* deallocate memory allocated for converter config structure */
        mem_free(engine_param->psImyConfig);
        return TFAIL;
    }

    /* Get number of memory chunk requests by the iMelody converter */
    s32Nr  engine_param->psImyConfig->sIMYConvMemInfo.s32IMYConvNumReqs;

    /* allocate memory requested by the converter */
    for(s32Index  0; s32Index < s32Nr; s32Index++)
    {
        psMem  &(engine_param->psImyConfig->sIMYConvMemInfo.asMemInfoSub[s32Index]);
        if (psMem->s32IMYConvMemFS  IMYCONV_FAST_MEMORY )
        {
            psMem->pvAPPIMYConvMemBasePtr  alloc_fast(psMem->u16IMYConvSize);
        }
        else
        {
            psMem->pvAPPIMYConvMemBasePtr  alloc_slow(psMem->u16IMYConvSize);
        }
    }
    /* allocate memory for input detail structure */
    psInDetails  (sIMYConvInputDetailType *)alloc_slow(sizeof(sIMYConvInputDetailType));
    if (psInDetails  NULL)
    {
        /* mem alloc error */
        return TFAIL;
    }

    pu8InBuf  alloc_slow(IMYCONV_INP_BUF_SIZE * sizeof(IMYCONV_U8));
    if (pu8InBuf  IMYCONV_NULL)
    {
        /* Mem alloc error */
        return TFAIL;
    }

    /* initialize members of input detail structure */
    psInDetails->pu8InBuf  pu8InBuf;

    /* initialize input buffer size */
    psInDetails->u16InBufSize  IMYCONV_INP_BUF_SIZE;
    /* currently data not available in this buffer */
    psInDetails->u16BytesInBuf  0;

    /* allocate memory for o/p detail structure */
    psOutDetails  (sIMYConvOutputDetailType *)alloc_slow (sizeof (sIMYConvOutputDetailType));

    /* allocate memory for output buffer */
    pu8OutBuf  alloc_slow (IMYCONV_OUT_BUF_SIZE * sizeof (IMYCONV_U8));
    if (pu8OutBuf  IMYCONV_NULL)
    {
        /* memory allocation error */
        return TFAIL;
    }
    psOutDetails->pu8OutBuf  pu8OutBuf;
    psOutDetails->u16OutBufSize  IMYCONV_OUT_BUF_SIZE;

    /* Initialize iMelody-to-MIDI conveter */
    eRetVal  eIMYConvInit (engine_param->psImyConfig, psInDetails, psOutDetails);

    if (eRetVal ! E_IMYCONV_OK)
    {
 tst_resm(TWARN, "ERROR in imelody_test_engine(): Initialization error");
        /* free all the allocated memory */
        eIMYConvFreeMemory(engine_param->psImyConfig, psInDetails, psOutDetails);
        return TFAIL;
    }

    /* allocate memory for imelody file infor structure */
    psImyInfo  (sIMYConvImelodyInfoType *)alloc_slow (sizeof (sIMYConvImelodyInfoType));

    /* intialize members of imelody info structure */
    psImyInfo->u32ImyPlaybackTime  0;
    psImyInfo->ps8ImyName  IMYCONV_NULL;
    psImyInfo->ps8ImyComposer  IMYCONV_NULL;

    /* get imelody input file information */
    eRetVal  eIMYConvGetInfo (engine_param->psImyConfig, psImyInfo);

    if (eRetVal ! E_IMYCONV_OK)
    {
 tst_resm(TWARN, "ERROR in imelody_test_engine(): Get iMelody info error");
        /* free all the allocated memory */
        eIMYConvFreeMemory(engine_param->psImyConfig, psInDetails, psOutDetails);
        return TFAIL;
    }
    if (vb_mode)
    {
 tst_resm(TINFO, "Imelody File Name  %s", psImyInfo->ps8ImyName);
 tst_resm(TINFO, "Imelody Composer Name  %s", psImyInfo->ps8ImyComposer);
 tst_resm(TINFO, "Imelody File Duration  %d", psImyInfo->u32ImyPlaybackTime);
    }
    /* intialize frame number */
    u32FrameNumber  0;
    while (engine_param->psImyConfig->u8EndOfFileReached  IMYCONV_FALSE)
    {
        /* initialize start time and frame duration */
        engine_param->psImyConfig->u32APPImyStartTime  engine_param->psImyConfig->u32ImyCurrentTime;
        engine_param->psImyConfig->u32APPImyDuration  u32FrameDuration;
        u32FrameNumber++;

 gettimeofday(&ti, NULL);
        /* call converter to convert iMelody to MIDI */
        eRetVal  eIMYConvConvert (engine_param->psImyConfig, psOutDetails);
 gettimeofday(&to, NULL);

 if (vb_mode)
 {
     s32EncodeTime  (to.tv_sec-ti.tv_sec)*1000000. + (to.tv_usec-ti.tv_usec);
     tst_resm(TINFO, "Imelody to MIDI frame%d convertion time  %0.2f msec", u32FrameNumber, s32EncodeTime);
 }

        /* Check whether converter return value */
        if (eRetVal ! E_IMYCONV_OK)
        {
            /* free all the allocated memory */
            eIMYConvFreeMemory(engine_param->psImyConfig, psInDetails, psOutDetails);
            return TFAIL;
        }
    }
    if (vb_mode)
 tst_resm(TINFO, "Total Number of Frames  %d", u32FrameNumber);

    /* free all allocated memory */
    eIMYConvFreeMemory(engine_param->psImyConfig, psInDetails, psOutDetails);
    return TPASS;
}

/* make_filelist */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int make_filelist (char *cfg_file_name)
{
    FILE *in;
    int  i;
    char column[3][MAX_STR_LEN];
    int  ret  TRUE;
    flist_t *fist_elem  NULL;

    file_list  NULL;
    in  fopen(cfg_file_name, "r");
    if (in  NULL)
    {
        tst_resm(TWARN, "ERROR in VT_imelody_conv_setup(): cannot open config file %s: %s", cfg_file_name, strerror(errno));
        return TFAIL;
    }
    i  0;
    while (!feof(in) && (ret ! FALSE) )
    {
        if (fscanf(in, "%s", column[i]) < 0)
            continue;
        if (i  2)
        {
     if (strcmp(column[0], "") ! 0)
     {
  if (!fist_elem)
  {
      fist_elem  mk_entry(column[0], column[1], column[2]);
      file_list  fist_elem;
      if (!file_list)
   ret  FALSE;
  }
  else
  {
      file_list->next  mk_entry(column[0], column[1], column[2]);
      file_list  file_list->next;
                    if (!file_list)
                        ret  FALSE;
  }
     }
     else
     {
                tst_resm(TWARN, "ERROR in read_cfg(): failure read file name");
  return TFAIL;
     }
 }
 i++;
 i % 3;
    }
    file_list  fist_elem;
    return TPASS;
}

/* GLOBAL FUNCTIONS */

/* VT_imelody_conv_setup */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int VT_imelody_conv_setup()
{
    int i;
    for (i  0; i < MAX_iMY_CONV_THREADS; i++)
    {
 thread[i].number  i;
    thread[i].psImyConfig  NULL;
    }
    return TPASS;
}

/* VT_imelody_conv_cleanup */
/**
Description of the function
@brief  assumes the post-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int VT_imelody_conv_cleanup(void)
{
    destroy_filelist(file_list);
    return TPASS;
}

/* VT_imelody_conv_test */
/**
@brief  Template test scenario X function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int VT_imelody_conv_test(int test_case, char *cfg_file_name)
{
    int rv  TFAIL;

    if (make_filelist(cfg_file_name) ! TPASS)
    {
 return TFAIL;
    }
    switch (test_case)
    {
        case iMY_NOMINAL:
            tst_resm( TINFO, "Nominal functionality test" );
         rv  nominal_functionality_test();
            tst_resm( TINFO, "End of nominal functionality test" );
            break;

        case iMY_REENTER:
            tst_resm( TINFO, "Reentrance test" );
         rv  reentrance_test();
            tst_resm( TINFO, "End of reentrance test" );
            break;

        case iMY_ENDURANCE:
            tst_resm( TINFO, "Endurance test" );
            rv  endurance_test();
            tst_resm( TINFO, "End of endurance test" );
            break;

        case iMY_LOAD:
            tst_resm( TINFO, "Load test" );
            rv  load_test();
            tst_resm( TINFO, "End of load test" );
            break;

        case iMY_ROBUSTNESS:
            tst_resm( TINFO, "Robustness test" );
            rv  robustness_test();
            tst_resm( TINFO, "End of robustness test" );
            break;

        default:
            tst_brkm(TBROK , cleanup, "Error: This test case has been broken");
    }
    return rv;
}

#ifdef __cplusplus
}
#endif
