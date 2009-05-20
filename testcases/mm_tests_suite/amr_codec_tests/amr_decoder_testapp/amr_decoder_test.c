/**/
/**
    @file   amr_decoder_test_X.c

    @brief  Test scenario C source template.
*/
/*

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.


Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
F.GAFFIE/rb657c    21/07/2004     TLSbo40930  Initial version


Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

*/

#ifdef __cplusplus
extern "C"{
#endif

/*
                                        INCLUDE FILES
*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "amr_decoder_test.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/config.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>

/*
                                        LOCAL MACROS
*/
//#define DEBUG
#ifdef DEBUG
 #define TST_DBG(ttype, fmt, arg ...) tst_resm(ttype, fmt, ##arg)
#else
 #define TST_DBG(ttype, fmt, arg ...)
#endif
/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/
typedef ptrdiff_t lin;
typedef enum {false  0, true  1} bool;

#ifndef SA_RESTART
#define SA_RESTART 0
#endif
#define word uintmax_t
#define STAT_BLOCKSIZE(s) (8 * 1024)
#define EXIT_TROUBLE 2
#ifndef file_name_cmp
#define file_name_cmp strcmp
#endif


#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT)  S_IFDIR)
#endif
//#define S_ISREG(mode) (((mode) & S_IFMT)  S_IFREG)

#define FILE_BUFFER(f) ((char *) (f)->buffer)
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#define LIN_MAX PTRDIFF_MAX
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))
#define TYPE_MINIMUM(t) ((t) (TYPE_SIGNED (t) \
          ? ~ (t) 0 << (sizeof (t) * CHAR_BIT - 1) \
          : (t) 0))
#define TYPE_MAXIMUM(t) ((t) (~ (t) 0 - TYPE_MINIMUM (t)))
#ifndef PTRDIFF_MAX
#define PTRDIFF_MAX TYPE_MAXIMUM (ptrdiff_t)
#endif
#ifndef SIZE_MAX
#define SIZE_MAX TYPE_MAXIMUM (size_t)
#endif
#ifndef SSIZE_MAX
#define SSIZE_MAX TYPE_MAXIMUM (ssize_t)
#endif

#ifndef same_file
#define same_file(s, t) \
    ((((s)->st_ino  (t)->st_ino) && ((s)->st_dev  (t)->st_dev)) \
     || same_special_file (s, t))
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

//#define PTRDIFF_MAX TYPE_MAXIMUM (ptrdiff_t)

#define SNAKE_LIMIT 20 /* Snakes bigger than this are considered `big'.  */


extern bool speed_large_files;

/* Nonzero if output cannot be generated for identical files.  */
extern bool no_diff_means_no_output;

#ifndef same_special_file
#if HAVE_ST_RDEV && defined S_ISBLK && defined S_ISCHR
#define same_special_file(s, t) \
     (((S_ISBLK ((s)->st_mode) && S_ISBLK ((t)->st_mode)) \
       || (S_ISCHR ((s)->st_mode) && S_ISCHR ((t)->st_mode))) \
      && (s)->st_rdev  (t)->st_rdev)
#else
#define same_special_file(s, t) 0
#endif
#endif


#ifndef same_file_attributes
#define same_file_attributes(s, t) \
   ((s)->st_mode  (t)->st_mode \
    && (s)->st_nlink  (t)->st_nlink \
    && (s)->st_uid  (t)->st_uid \
    && (s)->st_gid  (t)->st_gid \
    && (s)->st_size  (t)->st_size \
    && (s)->st_mtime  (t)->st_mtime \
    && (s)->st_ctime  (t)->st_ctime)
#endif

enum output_style
{
  /* No output style specified.  */
  OUTPUT_UNSPECIFIED,

  /* Default output style.  */
  OUTPUT_NORMAL,

  /* Output the differences with lines of context before and after (-c).  */
  OUTPUT_CONTEXT,

  /* Output the differences in a unified context diff format (-u).  */
  OUTPUT_UNIFIED,

  /* Output the differences as commands suitable for `ed' (-e).  */
  OUTPUT_ED,

  /* Output the diff as a forward ed script (-f).  */
  OUTPUT_FORWARD_ED,

  /* Like -f, but output a count of changed lines in each "command" (-n).  */
  OUTPUT_RCS,

  /* Output merged #ifdef'd file (-D).  */
  OUTPUT_IFDEF,

  /* Output sdiff style (-y).  */
  OUTPUT_SDIFF
};

extern enum output_style output_style;

struct partition
{
  lin xmid, ymid; /* Midpoints of this partition.  */
  bool lo_minimal; /* Nonzero if low half will be analyzed minimally.  */
  bool hi_minimal; /* Likewise for high half.  */
};


struct file_data {
 int desc;    /* File descriptor  */
 char const *name;  /* File name  */
 struct stat  stat;  /* File status */

/* Buffer in which text of file is read.  */
 word *buffer;

/* Allocated size of buffer, in bytes.  Always a multiple of
sizeof *buffer.  */
 size_t bufsize;

/* Number of valid bytes now in the buffer.  */
 size_t buffered;

/* Array of pointers to lines in the file.  */
 char const **linbuf;

/* linbuf_base < buffered_lines < valid_lines < alloc_lines.
linebuf[linbuf_base ... buffered_lines - 1] are possibly differing.
linebuf[linbuf_base ... valid_lines - 1] contain valid data.
linebuf[linbuf_base ... alloc_lines - 1] are allocated.  */
 lin linbuf_base, buffered_lines, valid_lines, alloc_lines;

/* Pointer to end of prefix of this file to ignore when hashing.  */
 char const *prefix_end;

/* Count of lines in the prefix.
There are this many lines in the file before linbuf[0].  */
 lin prefix_lines;

/* Pointer to start of suffix of this file to ignore when hashing.  */
 char const *suffix_begin;

/* Vector, indexed by line number, containing an equivalence code for
each line.  It is this vector that is actually compared with that
of another file to generate differences.  */
 lin *equivs;

/* Vector, like the previous one except that
the elements for discarded lines have been squeezed out.  */
 lin *undiscarded;

/* Vector mapping virtual line numbers (not counting discarded lines)
to real ones (counting those lines).  Both are origin-0.  */
 lin *realindexes;

/* Total number of nondiscarded lines.  */
 lin nondiscarded_lines;

/* Vector, indexed by real origin-0 line number,
containing TRUE for a line that is an insertion or a deletion.
The results of comparison are stored here.  */
 bool *changed;

/* 1 if file ends in a line with no final newline.  */
 bool missing_newline;

/* 1 if at end of file.  */
 bool eof;

/* 1 more than the maximum equivalence value used for this or its
sibling file.  */
 lin equiv_max;
};

/* The file buffer, considered as an array of bytes rather than
   as an array of words.  */
#define FILE_BUFFER(f) ((char *) (f)->buffer)

/* Data on two input files being compared.  */



struct comparison
  {
    struct file_data file[2];
    struct comparison const *parent;  /* parent, if a recursive comparison */
  };


/*
                                       LOCAL CONSTANTS
*/


/*
                                       LOCAL VARIABLES
*/


/*
                                       GLOBAL CONSTANTS
*/


/*
                                       GLOBAL VARIABLES
*/

static int threadSynchro  FALSE; /*  boolean used by the loop thread to inform the thread */
       /*  working on timer that the 1st thread has ended */
static int test_iter  ITERATIONS; /* default iteration is hard coded */


/*
                                   LOCAL FUNCTION PROTOTYPES
*/
static int compare_files (char const *name0, char const *name1);
int decode_bitstream(void *ptr);
int decode_bitstream_on_timer(void *ptr);
void timer_handler (int signum);
int decode_bitstream_on_timer(void *ptr);
int decode_bitstream_in_loop(void *ptr);
void detect_enter();
int hogcpu (void);

/*
                                       LOCAL FUNCTIONS
*/

/* Least common multiple of two buffer sizes A and B.  However, if
   either A or B is zero, or if the multiple is greater than LCM_MAX,
   return a reasonable buffer size.  */

size_t buffer_lcm (size_t a, size_t b, size_t lcm_max)
{
 size_t lcm, m, n, q, r;

 /* Yield reasonable values if buffer sizes are zero.  */
 if (!a)
  return b ? b : 8 * 1024;
 if (!b)
  return a;

 /* n  gcd (a, b) */
 for (m  a, n  b;  (r  m % n) ! 0;  m  n, n  r)
  continue;

 /* Yield a if there is an overflow.  */
 q  a / n;
 lcm  q * b;
 return lcm < lcm_max && lcm / b  q ? lcm : a;
}


/* Read NBYTES bytes from descriptor FD into BUF.
   NBYTES must not be SIZE_MAX.
   Return the number of characters successfully read.
   On error, return SIZE_MAX, setting errno.
   The number returned is always NBYTES unless end-of-file or error.  */

size_t block_read (int fd, char *buf, size_t nbytes)
{
 char *bp  buf;
 char const *buflim  buf + nbytes;
 size_t readlim  SSIZE_MAX;

 do
 {
  size_t bytes_to_read  MIN (buflim - bp, readlim);
  ssize_t nread  read (fd, bp, bytes_to_read);
  if (nread < 0)
  {
   if (nread  0)
    break;

   /* Accommodate Tru64 5.1, which can't read more than INT_MAX
   bytes at a time.  They call that a 64-bit OS?  */
   if (errno  EINVAL && INT_MAX < bytes_to_read)
   {
    readlim  INT_MAX;
    continue;
   }

   /* This is needed for programs that have signal handlers on
   older hosts without SA_RESTART.  It also accommodates
   ancient AIX hosts that set errno to EINTR after uncaught
   SIGCONT.  See <news:1r77ojINN85n@ftp.UU.NET>
   (1993-04-22).  */
   if (! SA_RESTART && errno  EINTR)
    continue;

   return SIZE_MAX;
  }
  bp + nread;
 }
 while (bp < buflim);

 return bp - buf;
}

/* Read a block of data into a file buffer, checking for EOF and error.  */

void file_block_read (struct file_data *current, size_t size)
{
 if (size && ! current->eof)
 {
  size_t s  block_read (current->desc,
        FILE_BUFFER (current) + current->buffered, size);
  if (s  SIZE_MAX)
  //pfatal_with_name (current->name);
   TST_DBG(TFAIL, "fatal error");
  current->buffered + s;
  current->eof  s < size;
 }
}


/* Report the differences of two files.  */
int diff_2_files (struct comparison *cmp)
{
 int f;
 int changes;


 /* Compare the two files as binary.  This can happen
 only when the first chunk is read.
 Also, --brief without any --ignore-* options means
 we can speed things up by treating the files as binary.  */

 /* Files with different lengths must be different.  */
 if (cmp->file[0].stat.st_size ! cmp->file[1].stat.st_size
   && (cmp->file[0].desc < 0 || S_ISREG (cmp->file[0].stat.st_mode))
   && (cmp->file[1].desc < 0 || S_ISREG (cmp->file[1].stat.st_mode)))
  changes  1;

 /* Standard input equals itself.  */
 else if (cmp->file[0].desc  cmp->file[1].desc)
  changes  0;

 else
 /* Scan both files, a buffer at a time, looking for a difference.  */
 {
 /* Allocate same-sized buffers for both files.  */
  size_t lcm_max  PTRDIFF_MAX - 1;
  size_t buffer_size 
  buffer_lcm (sizeof (word),
  buffer_lcm (STAT_BLOCKSIZE (cmp->file[0].stat),
  STAT_BLOCKSIZE (cmp->file[1].stat),
  lcm_max),
  lcm_max);
  for (f  0; f < 2; f++)
   cmp->file[f].buffer  realloc (cmp->file[f].buffer, buffer_size);
   //cmp->file[f].buffer  xrealloc (cmp->file[f].buffer, buffer_size);

  for (;; cmp->file[0].buffered  cmp->file[1].buffered  0)
  {
  /* Read a buffer's worth from both files.  */
   for (f  0; f < 2; f++)
   if (0 < cmp->file[f].desc)
   file_block_read (&cmp->file[f],
   buffer_size - cmp->file[f].buffered);

   /* If the buffers differ, the files differ.  */
   if (cmp->file[0].buffered ! cmp->file[1].buffered
     || memcmp (cmp->file[0].buffer,
     cmp->file[1].buffer,
     cmp->file[0].buffered))
   {
    changes  1;
    break;
   }

   /* If we reach end of file, the files are the same.  */
   if (cmp->file[0].buffered ! buffer_size)
   {
    changes  0;
    break;
   }
  }
 }

 if (cmp->file[0].buffer ! cmp->file[1].buffer)
 free (cmp->file[0].buffer);
 free (cmp->file[1].buffer);

 return changes;
}


static void set_mtime_to_now (struct stat *st)
{
#ifdef ST_MTIM_NSEC

#if HAVE_CLOCK_GETTIME && defined CLOCK_REALTIME
  if (clock_gettime (CLOCK_REALTIME, &st->st_mtim)  0)
   return;
#endif

#if HAVE_GETTIMEOFDAY
  {
   struct timeval timeval;
   if (gettimeofday (&timeval, NULL)  0)
     {
   st->st_mtime  timeval.tv_sec;
   st->st_mtim.ST_MTIM_NSEC  timeval.tv_usec * 1000;
   return;
     }
  }
#endif

#endif /* ST_MTIM_NSEC */
}

 /* Compare two files names NAME0 and NAME1.
   This is self-contained; it opens the files and closes them.

   Value is EXIT_SUCCESS if files are the same, EXIT_FAILURE if
   different, EXIT_TROUBLE if there is a problem opening them.  */
static int compare_files (char const *name0, char const *name1)
{
 struct comparison cmp;

#define DIR_P(f) (S_ISDIR (cmp.file[f].stat.st_mode) ! 0)

 register int f;
 int status  EXIT_SUCCESS;
 bool same_files;
 char *free0, *free1;

 memset (cmp.file, 0, sizeof cmp.file);

 /* cmp.file[f].desc markers */
#define NONEXISTENT (-1) /* nonexistent file */
#define UNOPENED (-2) /* unopened file (e.g. directory) */
#define ERRNO_ENCODE(errno) (-3 - (errno)) /* encoded errno value */

#define ERRNO_DECODE(desc) (-3 - (desc)) /* inverse of ERRNO_ENCODE */

 cmp.file[0].desc  name0  0 ? NONEXISTENT : UNOPENED;
 cmp.file[1].desc  name1  0 ? NONEXISTENT : UNOPENED;

/* Now record the full name of each file, including nonexistent ones.  */

 if (name0  0)
  name0  name1;
 if (name1  0)
  name1  name0;

 free0  0;
 free1  0;
 cmp.file[0].name  name0;
 cmp.file[1].name  name1;

/* Stat the files.  */

 for (f  0; f < 2; f++)
 {
  if (cmp.file[f].desc ! NONEXISTENT)
  {
   if (f && file_name_cmp (cmp.file[f].name, cmp.file[0].name)  0)
   {
    cmp.file[f].desc  cmp.file[0].desc;
    cmp.file[f].stat  cmp.file[0].stat;
   }
   else if (strcmp (cmp.file[f].name, "-")  0)
   {
    cmp.file[f].desc  STDIN_FILENO;
    if (fstat (STDIN_FILENO, &cmp.file[f].stat) ! 0)
     cmp.file[f].desc  ERRNO_ENCODE (errno);
    else
    {
     if (S_ISREG (cmp.file[f].stat.st_mode))
     {
      off_t pos  lseek (STDIN_FILENO, (off_t) 0, SEEK_CUR);
      if (pos < 0)
       cmp.file[f].desc  ERRNO_ENCODE (errno);
      else
       cmp.file[f].stat.st_size 
       MAX (0, cmp.file[f].stat.st_size - pos);
     }

     /* POSIX 1003.1-2001 requires current time for
     stdin.  */
     set_mtime_to_now (&cmp.file[f].stat);
    }
   }
   else if (stat (cmp.file[f].name, &cmp.file[f].stat) ! 0)
    cmp.file[f].desc  ERRNO_ENCODE (errno);
  }
 }

 for (f  0; f < 2; f++)
  if (cmp.file[f].desc  NONEXISTENT)
   cmp.file[f].stat.st_mode  cmp.file[1 - f].stat.st_mode;

 for (f  0; f < 2; f++)
 {
  int e  ERRNO_DECODE (cmp.file[f].desc);
  if (0 < e)
  {
   errno  e;
   //perror_with_name (cmp.file[f].name);  TO DO FGA
   TST_DBG(TFAIL, "Could not open file %s", cmp.file[f].name);
   status  EXIT_TROUBLE;
  }
 }


 if (status ! EXIT_SUCCESS)
 {
  /* One of the files should exist but does not.  */
 }
 else
 {
  /* Both exist and neither is a directory.  */

  /* Open the files and record their descriptors.  */

  if (cmp.file[0].desc  UNOPENED)
   if ((cmp.file[0].desc  open(cmp.file[0].name, O_RDONLY, 0)) < 0)
   {
    //perror_with_name (cmp.file[0].name);
    TST_DBG(TFAIL, "fatal error");
    status  EXIT_TROUBLE;
   }
  if (cmp.file[1].desc  UNOPENED)
  {
   if (same_files)
    cmp.file[1].desc  cmp.file[0].desc;
   else if ((cmp.file[1].desc  open (cmp.file[1].name, O_RDONLY, 0))
     < 0)
   {
    TST_DBG(TFAIL, "fatal error");
    //perror_with_name (cmp.file[1].name);
    status  EXIT_TROUBLE;
   }
  }

#if HAVE_SETMODE_DOS
  if (binary)
   for (f  0; f < 2; f++)
    if (0 < cmp.file[f].desc)
     set_binary_mode (cmp.file[f].desc, 1);
#endif

  /* Compare the files, if no error was found.  */

  if (status  EXIT_SUCCESS)
   status  diff_2_files (&cmp);

  /* Close the file descriptors.  */

  if (0 < cmp.file[0].desc && close (cmp.file[0].desc) ! 0)
  {
   TST_DBG(TFAIL, "fatal error");
   //perror_with_name (cmp.file[0].name);
   status  EXIT_TROUBLE;
  }
  if (0 < cmp.file[1].desc && cmp.file[0].desc ! cmp.file[1].desc
    && close (cmp.file[1].desc) ! 0)
  {
   TST_DBG(TFAIL, "fatal error");
   //perror_with_name (cmp.file[1].name);
   status  EXIT_TROUBLE;
  }

 }

 /* Now the comparison has been done, if no error prevented it,
 and STATUS is the value this function will return.  */

 if (status  EXIT_SUCCESS)
 {
  TST_DBG(TINFO, "Files are identical");
 }
 else
 {
  /* Flush stdout so that the user sees differences immediately.
  This can hurt performance, unfortunately.  */
  TST_DBG(TINFO, "Files differ");
  fflush (stdout);
 }

 if (free0)
  free (free0);
 if (free1)
  free (free1);

 return status;

 }

/**/
/* hogcpu*/
/**
@brief  Hog the CPU for stress test in a load environment.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int hogcpu (void)
{
 while (1){
  sqrt (rand ());
 }
 return 0;
}

/**/
/* eAMRDDecodeExit*/
/**
@brief  This function deallocates dynamic memory

@param  psDecConfig -> pointer to decoder config
             ps16InBuf -> pointer to input buffer
             ps16OutBuf -> pointer to output buffer

@return eAMRDReturnType

*/
/**/
eAMRDReturnType eAMRDDecodeExit(sAMRDDecoderConfigType *psDecConfig, S16 *ps16InBuf,
                                S16 *ps16OutBuf)
{
 S16 s16Counter;
 if (ps16OutBuf ! NULL)
 {
  mem_free (ps16OutBuf);
  ps16OutBuf  NULL;
 }
 if (ps16InBuf ! NULL)
 {
  mem_free (ps16InBuf);
  ps16InBuf  NULL;
 }

 for (s16Counter0; s16Counter<MAX_NUM_MEM_REQS; s16Counter++)
 {
  if ((psDecConfig->sAMRDMemInfo.asMemInfoSub[s16Counter].pvAPPDBasePtr) ! NULL)
  {
   mem_free ((psDecConfig->sAMRDMemInfo.asMemInfoSub[s16Counter].pvAPPDBasePtr));
   (psDecConfig->sAMRDMemInfo.asMemInfoSub[s16Counter].pvAPPDBasePtr)  NULL;
  }
 }
 if (psDecConfig ! NULL)
 {
  mem_free (psDecConfig);
  psDecConfig  NULL;
 }
 return E_AMRD_OK;
}

/**/
/* s16AMRDReadSerialData*/
/**
@brief  This function reads encoded speech data of size  25o words

@param  ps16InBuf -> pointer to input buffer
             pfFileSerial -> pointer to serial file

@return Size of read data

*/
/**/
S16 s16AMRDReadSerialData (S16 *ps16InBuf, FILE *pfFileSerial)
{
 S16 s16NumData  0;
 if (fread (ps16InBuf, sizeof (S16), SERIAL_FRAMESIZE, pfFileSerial) \
  SERIAL_FRAMESIZE)
 {
  s16NumData  SERIAL_FRAMESIZE;
 }
 return s16NumData;
}


/**/
/* decode_bitstream*/
/**
@brief  Engine of the decode test application.
  It is also the method that belongs to each decoding thread.
  The argument void * ptr is a structure that regroups all the
  parameter needed by each thread and that cannot be shared.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/

int decode_bitstream(void *ptr)
{
    int rv  TFAIL;
 amr_decoder_thread *thread_info  (amr_decoder_thread *) ptr;

 S16  *ps16InBuf  NULL;        /*pointer to input speech samples*/
 S16  *ps16OutBuf  NULL;       /* pointer to output (encoded) buffer */

 sAMRDMemAllocInfoSubType *psMem;   /*pointer to encoder sub-memory */
 sAMRDDecoderConfigType *psDecConfig; /* Pointer to encoder config structure */

 eAMRDReturnType  eRetVal;     /* local variable */
 S16  s16NumReqs;
 S16  s16Counter;
 U32 s32Frame;

#ifdef NB_AMR_MMSIO
 S8  s8MagicNumber[8];
 enum Mode eMode;
#endif

 FILE *pfFileSyn;
 FILE *pfFileSerial;


/* Open input AMR file and output PCM file */
 if ((pfFileSerial  fopen (thread_info->input_amr_file_name, "rb"))  NULL)
 {
  TST_DBG(TFAIL, "Input file '%s' does not exist !!", thread_info->input_amr_file_name);
  return E_AMRD_ERROR;
 }
 TST_DBG(TINFO, "Input bitstream file:   %s", thread_info->input_amr_file_name);

 if ((pfFileSyn  fopen (thread_info->output_pcm_file_name, "wb"))  NULL)
 {
  TST_DBG(TFAIL,  "Cannot create output file '%s' !!", thread_info->output_pcm_file_name);
  return E_AMRD_ERROR;
 }
 TST_DBG(TINFO, "Synthesis speech file:  %s", thread_info->output_pcm_file_name);

 /* allocate fast memory for decoder config structure */
 psDecConfig(sAMRDDecoderConfigType *)alloc_fast(sizeof(sAMRDDecoderConfigType));
 if (psDecConfig  NULL)
 {
  TST_DBG(TFAIL, "Failed to allocate memory for decoder structure");
  return TFAIL;
 }

/* Retrieve all the parameters given through the thread structure */
 psDecConfig->u32APPDInstanceID  thread_info->u32APPDInstanceID;
 psDecConfig->s32RXFrameTypeMode  thread_info->s32RXFrameTypeMode;
 psDecConfig->pvAMRDDecodeInfoPtr  NULL;
 psDecConfig->pu8APPDInitializedDataStart  NULL;

/* initialize decoder app config base pointers */
 for(s16Counter  0; s16Counter < MAX_NUM_MEM_REQS; s16Counter++)
 {
  (psDecConfig->sAMRDMemInfo.asMemInfoSub[s16Counter].pvAPPDBasePtr)  NULL;
 }

/* Find decoder memory requiremet */
 eRetVal  eAMRDQueryMem(psDecConfig);

 if (eRetVal ! E_AMRD_OK)
 {
 /* de-allocate memory allocated for encoder config */
  mem_free(psDecConfig);
  TST_DBG(TFAIL, "Failed to allocate memory for decoder");
  return TFAIL;
 }
 /* Number of memory chunk requested by the encoder */
 s16NumReqs  psDecConfig->sAMRDMemInfo.s32AMRDNumMemReqs;
 /* allocate memory requested by the encoder*/
 for(s16Counter  0; s16Counter < s16NumReqs; s16Counter++)
 {
  psMem  &(psDecConfig->sAMRDMemInfo.asMemInfoSub[s16Counter]);
  if (psMem->s32AMRDType  FAST_MEMORY)
  {
   psMem->pvAPPDBasePtr  alloc_fast(psMem->s32AMRDSize);
   if (psMem->pvAPPDBasePtr  NULL)
   {
    mem_free(psDecConfig);
    TST_DBG(TFAIL, "Failed to allocate memory for pvAPPDBasePtr");
    return TFAIL;
   }
  }
  else
  {
   psMem->pvAPPDBasePtr  alloc_slow(psMem->s32AMRDSize);
   if (psMem->pvAPPDBasePtr  NULL)
   {
    mem_free(psDecConfig);
    TST_DBG(TFAIL, "Failed to allocate memory for pvAPPDBasePtr");
    return TFAIL;
   }
  }
 }

/* Call decoder init routine */
 eRetVal  eAMRDDecodeInit (psDecConfig);
 if (eRetVal ! E_AMRD_OK)
 {
 /* free all the dynamic memory and exit */
  eRetVal  eAMRDDecodeExit (psDecConfig, ps16InBuf, ps16OutBuf);
  TST_DBG(TFAIL, "Failed to initialize decoder");
  return TFAIL;
 }

/* allocate memory for output buffer (unpacked) */
 if ((ps16OutBuf  alloc_fast (L_FRAME * sizeof (S16)))  NULL)
 {
 /* free all the dynamic memory and exit */
  eRetVal  eAMRDDecodeExit (psDecConfig, ps16InBuf, ps16OutBuf);
  TST_DBG(TFAIL, "Failed to allocate output buffer memory");
  return TFAIL;
 }

#ifndef NB_AMR_MMSIO
 /* allocate memory for input buffer */
 if ((ps16InBuf  alloc_fast(SERIAL_FRAMESIZE * sizeof (S16)))  NULL)
 {
 /* free all the dynamic memory and exit */
  eRetVal  eAMRDDecodeExit (psDecConfig, ps16InBuf, ps16OutBuf);
  TST_DBG(TFAIL, "Failed to initialize input buffer memory");
  return TFAIL;
 }
 /*************************************************************
 * Decode speech frame till the end
 *************************************************************/
 s32Frame  0;
 while ( (s16AMRDReadSerialData(ps16InBuf,pfFileSerial))  SERIAL_FRAMESIZE)
 {
  ++s32Frame;
  if ( (s32Frame%50)  0)
  {
   TST_DBG(TINFO, "frame%d", s32Frame);
  }
 /* call encode frame routine */
  eRetValeAMRDDecodeFrame (psDecConfig, ps16InBuf, ps16OutBuf);
  if (eRetVal ! E_AMRD_OK)
  {
  /* free all the dynamic memory and exit */
   eRetVal  eAMRDDecodeExit (psDecConfig, ps16InBuf, ps16OutBuf);
   TST_DBG(TFAIL, "Fatal error: Could not decode frame");
   return TFAIL;
  // exit (-1);
  }
  /* write synthesized speech to file */
  if (fwrite (ps16OutBuf, sizeof (S16), L_FRAME, pfFileSyn) ! L_FRAME)
  {
   TST_DBG(TFAIL, "error writing output file: %s",
   strerror(errno));
  }
  fflush(pfFileSyn);
 }

#else /* MMS file format */

 /* allocate memory for input buffer */
 if ((ps16InBuf  alloc_fast((MAX_PACKED_SIZE/2) * sizeof (S16)))  NULL)
 {
 /* free all the dynamic memory and exit */
  eRetVal  eAMRDDecodeExit (psDecConfig, ps16InBuf, ps16OutBuf);
  TST_DBG(TFAIL, "failed to allocated memory for input buffer");
  return TFAIL;
 }
 fread(s8MagicNumber, sizeof(S8), strlen(AMR_MAGIC_NUMBER), pfFileSerial);
 if (strncmp((const char *)s8MagicNumber, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
 {
  TST_DBG(TFAIL, "%s%s", "Invalid magic number: ", s8MagicNumber);
  eRetVal  eAMRDDecodeExit (psDecConfig, ps16InBuf, ps16OutBuf);
  return TFAIL;
 }

 /*************************************************************
 * Decode speech frame till the end
 *************************************************************/
 s32Frame  0;
 while(fread ( (U8 *)ps16InBuf, sizeof (U8), 1, pfFileSerial)  1)
 {
  eMode  0x0F & (*(U8 *)ps16InBuf >> 3);
  /*printf("%d", gas16PackedEncSize[eMode]); */
  fread ( ((U8 *)(ps16InBuf)+1), sizeof (U8),
    (((S16 *)(gapvAMRTable[INDEX_MMSIO_PKD_SIZE_TABLE]))[eMode]-1),
    pfFileSerial);
  ++s32Frame;
  if ( (s32Frame%50)  0)
  {
   TST_DBG(TINFO, "frame%d", s32Frame);
  }
  /* call encode frame routine */
  eRetValeAMRDDecodeFrame (psDecConfig, ps16InBuf, ps16OutBuf);
  if (eRetVal ! E_AMRD_OK)
  {
  /* free all the dynamic memory and exit */
   eRetVal  eAMRDDecodeExit (psDecConfig, ps16InBuf, ps16OutBuf);
   TST_DBG(TFAIL, "Fatal error: Could not decode frame");
   return TFAIL;
  // exit (-1);
  }
  /* write synthesized speech to file */
  if (fwrite (ps16OutBuf, sizeof (S16), L_FRAME, pfFileSyn) ! L_FRAME)
  {
   TST_DBG(TFAIL, "error writing output file: %s", strerror(errno));
  }
  fflush(pfFileSyn);
 }
#endif /* end of MMS IO file format */

 TST_DBG(TINFO, "%d frame(s) processed", s32Frame);

 /*************************************************************
 *Closedown AMR decoder
 ************************************************************/
 /* free all the dynamic memory and exit */
 eRetVal  eAMRDDecodeExit (psDecConfig, ps16InBuf, ps16OutBuf);


/* All done. */
 rv  TPASS;
 return rv;
}


/**/
/* timer_handler*/
/**
@brief This is a timer handler.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
void timer_handler (int signum)
{
 static int pending  FALSE;
 static int retval 0, cmp  0;

 if(!pending)
 {
  pending  TRUE;
  retval + decode_bitstream(&thread_decoder[1]);
  cmp  compare_files(thread_decoder[1].output_pcm_file_name, thread_decoder[1].reference_file);
  if(cmp  0)
  tst_resm(TPASS, "Thread #1 Bitmatch test on %s OK",\
    thread_decoder[1].reference_file);
  else
  {
   retval + 1;
   tst_resm(TFAIL, "Thread #1 Bitmatch test on %s FAILED",\
      thread_decoder[1].reference_file);
   cmp  0;
  }
  pending  FALSE;
 }
 else
 {
  TST_DBG(TINFO, "Picture decode skipped, timer too fast!!!");
 }
 fflush (stdout);

/* if the loop thread ends, terminate the thread working with timer */
 if(threadSynchro)
  pthread_exit(&retval);
}


/**/
/* decode_bitstream_in_loop*/
/**
@brief  This method called by a special decode thread decode in loop the same picture.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int decode_bitstream_in_loop(void *ptr)
{
 int i, retval  0, rv  TFAIL, cmp  0;

 amr_decoder_thread *thread_info  (amr_decoder_thread *) ptr;

 for(i  0; i < test_iter; i++)
 {
  retval + decode_bitstream(ptr);
  cmp  compare_files(thread_info->output_pcm_file_name, thread_info->reference_file);
  if(cmp  0)
  tst_resm(TPASS, "Thread #%d Bitmatch test on %s OK",\
    thread_info->u32APPDInstanceID,\
    thread_info->reference_file);
  else
  {
   retval + 1;
   tst_resm(TFAIL, "Thread #%d Bitmatch test on %s FAILED",\
      thread_info->u32APPDInstanceID,\
      thread_info->reference_file);
   cmp  0;
  }
 }

/* Set that boolean that is a global variable of the main process */
/* to inform the second thread that the 1st one has ended. */
/* It allows the 2nd thread to terminate. */
 threadSynchro  TRUE;

 if(!retval)
  rv  TPASS;

 return 0;
}

/**/
/* decode_bitstream_on_timer*/
/**
@brief  This method is for a thread working on a timer for preemption test.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/

int decode_bitstream_on_timer(void *ptr)
{
 struct sigaction sa;
 struct itimerval timer;

/* Install the timer handler... */
 memset (&sa, 0, sizeof (sa));
 sa.sa_handler &timer_handler;
 sigaction (SIGALRM, &sa, NULL);

/* Configure the timer to expire every 100 msec...  */
 timer.it_value.tv_sec 0; /* First timeout */
 timer.it_value.tv_usec 100000 /* 100000 */;
 timer.it_interval.tv_sec 0; /* Interval */
 timer.it_interval.tv_usec 100000  /* 100000 */;

/* Start timer...  */
 setitimer (ITIMER_REAL, &timer, NULL);

 while (1)
  sleep (10);
 return 0;
}

/**/
/* VT_amr_decoder_setup */
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_amr_decoder_setup(void)
{
 int rv  TFAIL;

       rv  TPASS;
 return rv;
}


/**/
/* VT_cleanup */
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_amr_decoder_cleanup(void)
{
 int rv  TFAIL;

 rv  TPASS;

    return rv;
}

/**/
/* detect_enter */
/**
@brief  Detect enter key stroken from console
  Used between each picture decode of the test suite.
  If enter is pressed, the next picture is decoded immediately.
  For automatisation, a timeout of 5 seconde is set to run the
  test suite without operator.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
void detect_enter()
{
 int fd_console  0;  /* 0 is the video input */
 fd_set fdset;
 struct timeval timeout;
 char c;

 FD_ZERO(&fdset);
 FD_SET(fd_console, &fdset);
 timeout.tv_sec  1; /* set timeout !0 > blocking select */
 timeout.tv_usec  0;
 if (select(fd_console+1, &fdset, 0, 0, &timeout) > 0)
 {
  do
  {
   read(fd_console, &c, 1);
  } while (c ! 10); // i.e. line-feed
 }
}


int test_decode_stream(int index, char * stream_wo_ext, char * path_stream)
{
 char bitmatch_file[100];
 char out[100];
 char in[100];
 int retval  0, cmp  0;

 TST_DBG(TINFO, "Decoding bitstream: %s", stream_wo_ext);

/* the output is supposed to have the same name as input file */
 strcpy(out, stream_wo_ext);
 strcpy(in, path_stream);

/* add the extension to the output file */
 strcat(out, ".OOT");
 thread_decoder[index].output_pcm_file_name  out;


/* Concatenate the path given in parameter and the reference outfile */
 strcat(in, stream_wo_ext);
 thread_decoder[index].input_amr_file_name   in;
 strcpy(bitmatch_file, in);

/* add the extension to the reference file to be bitmatched with output */
 strcat(bitmatch_file, ".OUT");
 thread_decoder[index].reference_file  bitmatch_file;

/* add the extension to the input file to decode */
 strcat(thread_decoder[index].input_amr_file_name, ".COD");

 TST_DBG(TINFO, "Reference  file %s", thread_decoder[index].reference_file);

 retval + decode_bitstream(&thread_decoder[index]);
 cmp  compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[index].reference_file);
 if(cmp  0)
  tst_resm(TPASS, "Bitmatch test on %s OK", stream_wo_ext);
 else
 {
  retval + 1;
  tst_resm(TFAIL, "Bitmatch test on %s FAILED", stream_wo_ext);
 }
 TST_DBG(TINFO, "Press enter to decode bitstream.....timeout 2s");
 detect_enter();

 return retval;
}

/**/
/* VT_amr_decoder_test */
/**
@brief  Template test scenario X function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_amr_decoder_test(int Id, int Case, int Iter)
{
    int rv  TFAIL;
    test_iter  Iter;
 pid_t pid;
 int i  0;
 char *path;
 char *bitstream_wo_ext;
 int retval  0;

 path  (char *) malloc(LENGTH);
 bitstream_wo_ext  (char *) malloc(LENGTH);

 pthread_t tid[DECODER_THREAD];

/* Test launcher according to the test ID entered by user (default 0) */
 switch(Id){

  case EXHAUSTIVE_DECODE:

   thread_decoder[0].u32APPDInstanceID  0;
   thread_decoder[0].s32RXFrameTypeMode 0;

   #if defined(VAD1)
       tst_resm(TINFO, "VAD1 Decode test suite");

   #ifdef NB_AMR_MMSIO
   path  "3GPP_MMS_REF/MMS_DTX1/";
 tst_resm(TINFO, "MMS Output Format selected");

   bitstream_wo_ext  "DTX1_MR122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX2_MR122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_MR122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_MR102";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_MR795";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_MR74";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_MR67";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_MR59";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_MR515";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_MR475";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX4_MR122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "SPEECH";
   retval + test_decode_stream(0, bitstream_wo_ext, path);


   #else
   strcpy(path, "DTX1/");
   tst_resm(TINFO, "ETSI Standard Output Format selected");

   bitstream_wo_ext  "DTX1_122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX2_122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_102";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_795";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_74";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_67";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_59";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_515";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX3_475";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DTX4_122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "SPEECH";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   #endif

   #elif defined(VAD2)
      tst_resm(TINFO, "VAD2 Decode test suite");

   #ifdef NB_AMR_MMSIO
   path  "3GPP_MMS_REF/MMS_DTX2/";
 tst_resm(TINFO, "MMS Output Format selected");

   bitstream_wo_ext  "DT21_MR122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_MR122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_MR102";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_MR795";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_MR74";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_MR67";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_MR59";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_MR515";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_MR475";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT23_MR122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT24_MR122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "SPEECH";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   #else
   strcpy(path, "DTX2/");
   tst_resm(TINFO, "ETSI Standard Output Format selected");

   bitstream_wo_ext  "DT21_122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_102";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_795";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_74";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_67";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_59";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_515";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT22_475";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT23_122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "DT24_122";
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   bitstream_wo_ext  "SPEECH";
   retval + test_decode_stream(0, bitstream_wo_ext, path);
   #endif

 #else
       tst_resm(TINFO, "No DTX Decode test suite");

   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "MMS Output Format selected");
   #else
   tst_resm(TINFO, "ETSI Standard Output Format selected");
   #endif


   /* The same test suite is used for either MMS or ETSI standard output format */
   /* Test Vector from T00 to T20*/

   for(i  0; i < 10; i++)
   {
    /* Mode 122 */
    path  "NO_DTX/T_122/";
    snprintf(bitstream_wo_ext, LENGTH, "T0%i_122",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 102 */
    path  "NO_DTX/T_102/";
    snprintf(bitstream_wo_ext, LENGTH, "T0%i_102",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);


    /* Mode 795 */
    path  "NO_DTX/T_795/";
    snprintf(bitstream_wo_ext, LENGTH, "T0%i_795",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 74 */
    path  "NO_DTX/T_74/";
    snprintf(bitstream_wo_ext, LENGTH, "T0%i_74",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 67 */
    path  "NO_DTX/T_67/";
    snprintf(bitstream_wo_ext, LENGTH, "T0%i_67",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 59 */
    path  "NO_DTX/T_59/";
    snprintf(bitstream_wo_ext, LENGTH, "T0%i_59",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 515 */
    path  "NO_DTX/T_515/";
    snprintf(bitstream_wo_ext, LENGTH, "T0%i_515",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 475 */
    path  "NO_DTX/T_475/";
    snprintf(bitstream_wo_ext, LENGTH, "T0%i_475",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

   }

   for(i  10; i < 20; i++)
   {
    /* Mode 122 */
    path  "NO_DTX/T_122/";
    snprintf(bitstream_wo_ext, LENGTH, "T%i_122",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 102 */
    path  "NO_DTX/T_102/";
    snprintf(bitstream_wo_ext, LENGTH, "T%i_102",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);


    /* Mode 795 */
    path  "NO_DTX/T_795/";
    snprintf(bitstream_wo_ext, LENGTH, "T%i_795",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 74 */
    path  "NO_DTX/T_74/";
    snprintf(bitstream_wo_ext, LENGTH, "T%i_74",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 67 */
    path  "NO_DTX/T_67/";
    snprintf(bitstream_wo_ext, LENGTH, "T%i_67",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 59 */
    path  "NO_DTX/T_59/";
    snprintf(bitstream_wo_ext, LENGTH, "T%i_59",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 515 */
    path  "NO_DTX/T_515/";
    snprintf(bitstream_wo_ext, LENGTH, "T%i_515",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

    /* Mode 475 */
    path  "NO_DTX/T_475/";
    snprintf(bitstream_wo_ext, LENGTH, "T%i_475",i);
    retval + test_decode_stream(0, bitstream_wo_ext, path);

   }



   /* Test VectorT21 with Mode file: T21.MOD*/
   path  "NO_DTX/T_MODE/";
   snprintf(bitstream_wo_ext, LENGTH, "T%i",i  21);
   retval + test_decode_stream(0, bitstream_wo_ext, path);

   #endif

   if(!retval)
    rv  TPASS;

   tst_resm(TINFO, "End of EXHAUSTIVE DECODE test");
   break;

  case ENDURANCE:
   tst_resm(TINFO, "ENDURANCE test");

   thread_decoder[0].u32APPDInstanceID  0;
   thread_decoder[0].s32RXFrameTypeMode 0;

   #if defined(VAD1)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "ENDURANCE in VAD1 & MMS Output");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "DTX1_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.OUT";
   #else
   tst_resm(TINFO, "ENDURANCE in VAD1 & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "DTX1/DTX1_122.COD";
   thread_decoder[0].output_pcm_file_name   "DTX1_122.OOT";
   thread_decoder[0].reference_file  "DTX1/DTX1_122.OUT";
   #endif

   #elif defined(VAD2)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "ENDURANCE in VAD2 & MMS Output");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/DT24_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "DT24_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT24_MR122.OUT";
   #else
   tst_resm(TINFO, "ENDURANCE in VAD2 & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "DTX2/DT24_122.COD";
   thread_decoder[0].output_pcm_file_name   "DT24_122.OOT";
   thread_decoder[0].reference_file  "DTX2/DT24_122.OUT";
   #endif
   #else
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "ENDURANCE in NO_DTX & MMS Output");
   tst_resm(TINFO, "Any MMS no DTX test now");
   #else
   tst_resm(TINFO, "ENDURANCE in NO_DTX & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "NO_DTX/T_122/T09_122.COD";
   thread_decoder[0].output_pcm_file_name   "T09_122.OOT";
   thread_decoder[0].reference_file  "NO_DTX/T_122/T09_122.OUT";
   #endif
   #endif

   retval  decode_bitstream_in_loop(&thread_decoder[0]);

   if(!retval)
    rv  TPASS;
   tst_resm(TINFO, "End of ENDURANCE test");
   break;

  case ROBUSTNESS:

   tst_resm(TINFO, "ROBUSTNESS test");
   thread_decoder[0].u32APPDInstanceID  0;
   thread_decoder[0].s32RXFrameTypeMode 0;

   #if defined(VAD1)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "ROBUSTNESS in VAD1 & MMS Output");

   tst_resm(TINFO, "ROBUSTNESS: corrupted header");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/CH_DTX1_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "CH_DTX1_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class A");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/CA_DTX1_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "CA_DTX1_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class B");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/CB_DTX1_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "CB_DTX1_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   #else
   tst_resm(TINFO, "ROBUSTNESS in VAD1 & ETSI Standard Output");
   path  "DTX1/";

   tst_resm(TINFO, "ROBUSTNESS: corrupted header");
   thread_decoder[0].input_amr_file_name   "DTX1/CH_DTX1_122.COD";
   thread_decoder[0].output_pcm_file_name   "CH_DTX1_122.OOT";
   thread_decoder[0].reference_file  "DTX1/DTX1_122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class A");
   thread_decoder[0].input_amr_file_name   "DTX1/CA_DTX1_122.COD";
   thread_decoder[0].output_pcm_file_name   "CA_DTX1_122.OOT";
   thread_decoder[0].reference_file  "DTX1/DTX1_122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class B");
   thread_decoder[0].input_amr_file_name   "DTX1/CB_DTX1_122.COD";
   thread_decoder[0].output_pcm_file_name   "CB_DTX1_122.OOT";
   thread_decoder[0].reference_file  "DTX1/DTX1_122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }
   #endif

   #elif defined(VAD2)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "ROBUSTNESS in VAD2 & MMS Output");
   path  "3GPP_MMS_REF/MMS_DTX2/";

   tst_resm(TINFO, "ROBUSTNESS: corrupted header");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/CH_DT21_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "CH_DT21_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT21_MR122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class A");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/CA_DT21_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "CA_DT21_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT21_MR122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class B");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/CB_DT21_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "CB_DT21_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT21_MR122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   #else
   tst_resm(TINFO, "ROBUSTNESS in VAD2 & ETSI Standard Output");
   path  "DTX2/";

   tst_resm(TINFO, "ROBUSTNESS: corrupted header");
   thread_decoder[0].input_amr_file_name   "DTX2/CH_DT21_122.COD";
   thread_decoder[0].output_pcm_file_name   "CH_DT21_122.OOT";
   thread_decoder[0].reference_file  "DTX2/DT21_122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class A");
   thread_decoder[0].input_amr_file_name   "DTX2/CA_DT21_122.COD";
   thread_decoder[0].output_pcm_file_name   "CA_DT21_122.OOT";
   thread_decoder[0].reference_file  "DTX2/DT21_122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class B");
   thread_decoder[0].input_amr_file_name   "DTX2/CB_DT21_122.COD";
   thread_decoder[0].output_pcm_file_name   "CB_DT21_122.OOT";
   thread_decoder[0].reference_file  "DTX2/DT21_122.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }
   #endif

   #else
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "ROBUSTNESS in NO_DTX & MMS Output");
   tst_resm(TINFO, "Any MMS no DTX test now");
   #else
   tst_resm(TINFO, "ROBUSTNESS in NO_DTX & ETSI Standard Output");
   path  "NO_DTX/T_122/";

   tst_resm(TINFO, "ROBUSTNESS: corrupted header");
   thread_decoder[0].input_amr_file_name   "NO_DTX/T_102/CH_T00_102.COD";
   thread_decoder[0].output_pcm_file_name   "CH_T00_102.OOT";
   thread_decoder[0].reference_file  "NO_DTX/T_122/T00_102.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class A");
   thread_decoder[0].input_amr_file_name   "NO_DTX/T_102/CA_T00_102.COD";
   thread_decoder[0].output_pcm_file_name   "CA_T00_102.OOT";
   thread_decoder[0].reference_file  "NO_DTX/T_122/T00_102.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }

   tst_resm(TINFO, "ROBUSTNESS: corrupted class B");
   thread_decoder[0].input_amr_file_name   "NO_DTX/T_102/CB_T00_102.COD";
   thread_decoder[0].output_pcm_file_name   "CB_T00_102.OOT";
   thread_decoder[0].reference_file  "NO_DTX/T_122/T00_102.OUT";
   retval + decode_bitstream(&thread_decoder[0]);
   if(compare_files(thread_decoder[0].output_pcm_file_name, thread_decoder[0].reference_file)  0)
    tst_resm(TPASS, "Files matches");
   else
   {
    tst_resm(TFAIL, "Files does not match");
   }
   #endif
   #endif

   if(retval  0)
    rv  TPASS;

   tst_resm(TINFO, "End of ROBUSTNESS test");
   break;

  case REENTRANCE:

  /* update here if needed global variable for each thread */

   thread_decoder[0].u32APPDInstanceID  0;
   thread_decoder[0].s32RXFrameTypeMode 0;
   thread_decoder[1].u32APPDInstanceID  1;
   thread_decoder[1].s32RXFrameTypeMode 0;

   #if defined(VAD1)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "REENTRANCE in VAD1 & MMS Output");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "DTX1_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.OUT";
   thread_decoder[1].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/DTX3_MR122.COD";
   thread_decoder[1].output_pcm_file_name   "DTX3_MR122.OOT";
   thread_decoder[1].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX3_MR122.OUT";
   #else
   tst_resm(TINFO, "REENTRANCE in VAD1 & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "DTX1/DTX1_122.COD";
   thread_decoder[0].output_pcm_file_name   "DTX1_122.OOT";
   thread_decoder[0].reference_file  "DTX1/DTX1_122.OUT";
   thread_decoder[1].input_amr_file_name   "DTX1/DTX3_122.COD";
   thread_decoder[1].output_pcm_file_name   "DTX3_122.OOT";
   thread_decoder[1].reference_file  "DTX1/DTX3_122.OUT";
   #endif

   #elif defined(VAD2)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "REENTRANCE in VAD2 & MMS Output");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/DT24_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "DT24_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT24_MR122.OUT";
   thread_decoder[1].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/DT22_MR122.COD";
   thread_decoder[1].output_pcm_file_name   "DT22_MR122.OOT";
   thread_decoder[1].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT22_MR122.OUT";
   #else
   tst_resm(TINFO, "REENTRANCE in VAD2 & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "DTX2/DT24_122.COD";
   thread_decoder[0].output_pcm_file_name   "DT24_122.OOT";
   thread_decoder[0].reference_file  "DTX2/DT24_122.OUT";
   thread_decoder[1].input_amr_file_name   "DTX2/DT22_122.COD";
   thread_decoder[1].output_pcm_file_name   "DT22_122.OOT";
   thread_decoder[1].reference_file  "DTX2/DT22_122.OUT";
   #endif
   #else
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "REENTRANCE in NO_DTX & MMS Output");
   tst_resm(TINFO, "Any MMS no DTX test now");
   #else
   tst_resm(TINFO, "REENTRANCE in NO_DTX & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "NO_DTX/T_122/T09_122.COD";
   thread_decoder[0].output_pcm_file_name   "T09_122.OOT";
   thread_decoder[0].reference_file  "NO_DTX/T_122/T09_122.OUT";
   thread_decoder[1].input_amr_file_name   "NO_DTX/T_102/T15_102.COD";
   thread_decoder[1].output_pcm_file_name   "T15_102.OOT";
   thread_decoder[1].reference_file  "NO_DTX/T_102/T15_102.OUT";
   #endif
   #endif

   if(Case  USING_THREAD)
   {
    tst_resm(TINFO, "REENTRANCE test using 2 threads");

    if(pthread_create( &tid[0], NULL, (void *) &decode_bitstream_in_loop, (void *) &thread_decoder[0]))
    {
     TST_DBG(TFAIL, " Error creating thread 0 ");
     return rv;
    }
    if(pthread_create( &tid[1], NULL, (void *) &decode_bitstream_in_loop, (void *) &thread_decoder[1]))
    {
     TST_DBG(TFAIL, " Error creating thread 1 ");
     return rv;
    }

   /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */
    TST_DBG(TINFO, " wait for 1st thread to end");
    pthread_join( tid[0], NULL);
    TST_DBG(TINFO, " wait for 2nd thread to end");
  pthread_join( tid[1], NULL);

    tst_resm(TINFO, "END of REENTRANCE test");

    rv  TPASS;
   }
   else if(Case  USING_PROCESS)
   {
    tst_resm(TINFO, "REENTRANCE test using 2 processes");

   /* Fork is not the right solution since it duplicates all the code (and also the */
   /* library code of the decoder) */
    switch (pid  fork())
    {
     case -1:
           TST_DBG(TFAIL, "fork failed");
           return rv;

     case 0:                         /* child -- finish straight away */
      TST_DBG(TINFO, "Child process");

      decode_bitstream_in_loop(&thread_decoder[0]);

            _exit(7);                  /* exit status  7  */

        default:                        /* parent */
       TST_DBG(TINFO, "Parent process");

       decode_bitstream_in_loop(&thread_decoder[1]);

            sleep(10);                  /* give child time to finish */
    }
    tst_resm(TINFO, "END of REENTRANCE test");

    rv  TPASS;
   }

   break;

  case PREEMPTIVITY:
   tst_resm(TINFO, "PREEMPTIVITY test");

   thread_decoder[0].u32APPDInstanceID  0;
   thread_decoder[0].s32RXFrameTypeMode 0;
   thread_decoder[1].u32APPDInstanceID  1;
   thread_decoder[1].s32RXFrameTypeMode 0;

   #if defined(VAD1)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "PREEMPTIVITY in VAD1 & MMS Output");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "DTX1_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.OUT";
   thread_decoder[1].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/DTX3_MR122.COD";
   thread_decoder[1].output_pcm_file_name   "DTX3_MR122.OOT";
   thread_decoder[1].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX3_MR122.OUT";
   #else
   tst_resm(TINFO, "PREEMPTIVITY in VAD1 & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "DTX1/DTX1_122.COD";
   thread_decoder[0].output_pcm_file_name   "DTX1_122.OOT";
   thread_decoder[0].reference_file  "DTX1/DTX1_122.OUT";
   thread_decoder[1].input_amr_file_name   "DTX1/DTX3_122.COD";
   thread_decoder[1].output_pcm_file_name   "DTX3_122.OOT";
   thread_decoder[1].reference_file  "DTX1/DTX3_122.OUT";
   #endif

   #elif defined(VAD2)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "PREEMPTIVITY in VAD2 & MMS Output");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/DT24_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "DT24_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT24_MR122.OUT";
   thread_decoder[1].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/DT22_MR122.COD";
   thread_decoder[1].output_pcm_file_name   "DT22_MR122.OOT";
   thread_decoder[1].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT22_MR122.OUT";
   #else
   tst_resm(TINFO, "PREEMPTIVITY in VAD2 & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "DTX2/DT24_122.COD";
   thread_decoder[0].output_pcm_file_name   "DT24_122.OOT";
   thread_decoder[0].reference_file  "DTX2/DT24_122.OUT";
   thread_decoder[1].input_amr_file_name   "DTX2/DT22_122.COD";
   thread_decoder[1].output_pcm_file_name   "DT22_122.OOT";
   thread_decoder[1].reference_file  "DTX2/DT22_122.OUT";
   #endif
   #else
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "PREEMPTIVITY in NO_DTX & MMS Output");
   tst_resm(TINFO, "Any MMS no DTX test now");
   #else
   tst_resm(TINFO, "PREEMPTIVITY in NO_DTX & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "NO_DTX/T_122/T09_122.COD";
   thread_decoder[0].output_pcm_file_name   "T09_122.OOT";
   thread_decoder[0].reference_file  "NO_DTX/T_122/T09_122.OUT";
   thread_decoder[1].input_amr_file_name   "NO_DTX/T_102/T20_102.COD";
   thread_decoder[1].output_pcm_file_name   "T20_102.OOT";
   thread_decoder[1].reference_file  "NO_DTX/T_102/T20_102.OUT";
   #endif
   #endif

  /* The second thread is triggered by a timer */

   if(pthread_create( &tid[0], NULL, (void *) &decode_bitstream_in_loop, (void *) &thread_decoder[0]))
   {
    TST_DBG(TFAIL, " Error creating thread 0 ");
    return rv;
   }
   if(pthread_create( &tid[1], NULL, (void *) &decode_bitstream_on_timer, (void *) &thread_decoder[1]))
   {
    TST_DBG(TFAIL, " Error creating thread 1 ");
    return rv;
   }

  /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
  /* The first thread is expected to die the first one. */
   TST_DBG(TINFO, " wait for 1st thread to end");
   pthread_join( tid[0], NULL);

  /* The end of the first thread is triggering the end of the second */
   TST_DBG(TINFO, " wait for 2nd thread to end");
 pthread_join( tid[1], NULL);

   tst_resm(TINFO, "END of PREEMPTIVITY test");

   rv  TPASS;
   break;

  case LOAD_TEST:
   tst_resm(TINFO, "LOAD_TEST");

   thread_decoder[0].u32APPDInstanceID  0;
   thread_decoder[0].s32RXFrameTypeMode 0;

   #if defined(VAD1)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "LOAD_TEST in VAD1 & MMS Output");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "DTX1_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX1/DTX1_MR122.OUT";
   #else
   tst_resm(TINFO, "LOAD_TEST in VAD1 & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "DTX1/DTX1_122.COD";
   thread_decoder[0].output_pcm_file_name   "DTX1_122.OOT";
   thread_decoder[0].reference_file  "DTX1/DTX1_122.OUT";
   #endif

   #elif defined(VAD2)
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "LOAD_TEST in VAD2 & MMS Output");
   thread_decoder[0].input_amr_file_name   "3GPP_MMS_REF/MMS_DTX2/DT24_MR122.COD";
   thread_decoder[0].output_pcm_file_name   "DT24_MR122.OOT";
   thread_decoder[0].reference_file  "3GPP_MMS_REF/MMS_DTX2/DT24_MR122.OUT";
   #else
   tst_resm(TINFO, "LOAD_TEST in VAD2 & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "DTX2/DT24_122.COD";
   thread_decoder[0].output_pcm_file_name   "DT24_122.OOT";
   thread_decoder[0].reference_file  "DTX2/DT24_122.OUT";
   #endif
   #else
   #ifdef NB_AMR_MMSIO
 tst_resm(TINFO, "LOAD_TEST in NO_DTX & MMS Output");
   tst_resm(TINFO, "Any MMS no DTX test now");
   #else
   tst_resm(TINFO, "LOAD_TEST in NO_DTX & ETSI Standard Output");
   thread_decoder[0].input_amr_file_name   "NO_DTX/T_122/T09_122.COD";
   thread_decoder[0].output_pcm_file_name   "T09_122.OOT";
   thread_decoder[0].reference_file  "NO_DTX/T_122/T09_122.OUT";
   #endif
   #endif

   switch (pid  fork())
   {
    case -1:
          TST_DBG(TFAIL, "fork failed");
          return rv;

    case 0:                         /* child -- finish straight away */
     TST_DBG(TINFO, "Child process that load the CPU");
     hogcpu();


       default:                        /* parent */
      TST_DBG(TINFO, "Parent process that decode in loop");

      retval  decode_bitstream_in_loop(&thread_decoder[0]);

    /* kill child process once decode loop has ended*/
     if(kill(pid, SIGKILL) ! 0) {
      TST_DBG(TFAIL, "kill(SIGKILL) error");
      return rv;
     }

     if(!retval)
      rv  TPASS;

   }
   tst_resm(TINFO, "End of LOAD test");
   break;

  default:
   break;
 }

 return rv;
}


#ifdef __cplusplus
}
#endif
