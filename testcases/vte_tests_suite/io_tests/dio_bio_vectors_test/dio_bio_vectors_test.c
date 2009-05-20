/*====================*/
/**
    @file   dio_bio_vectors_test.c

    @brief  dio_bio_vectors test scenario C source.
*/
/*======================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             09/06/2004     TLSbo39741   Initial version

====================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

======================*/


/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>
#include <signal.h>     /* kill() */
#include <sys/uio.h>    /* struct iovec */
#include <stdio.h>      /* fprintf(), sprintf() */
#include <stdlib.h>     /* malloc(), valloc(), free(), exit() */
#include <unistd.h>     /* close(), unlink(), write(), read(), lseek(), fsync(), fork() */
#include <sys/types.h>  /* open(), lseek(), fork(), waitpid() */
#include <sys/wait.h>   /* waitpid() */
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>     /* strerror() */

#ifdef __cplusplus
extern "C"{
#endif

/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}

#endif

/* Verification Test Environment Include Files */
#include "dio_bio_vectors_test.h"

/*======================
                                        LOCAL MACROS
======================*/


/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/


/*======================
                                       LOCAL CONSTANTS
======================*/
#define BUFSIZE 4096
#define LEN 30
#define READ_DIRECT 1
#define WRITE_DIRECT 2
#define RDWR_DIRECT 3

/*======================
                                       LOCAL VARIABLES
======================*/
static int iter = 100;  /* Iterations. Default 100 */
static char filename[LEN];  /* Test data file */
off64_t         offset = 0;  /* Offset. Default 0 */
int bufsize;

/*======================
                                       GLOBAL CONSTANTS
======================*/


/*======================
                                       GLOBAL VARIABLES
======================*/


/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/


/*======================
                                       LOCAL FUNCTIONS
======================*/


/*====================*/
/*= VT_dio_bio_vectors_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_dio_bio_vectors_setup(void)
{
    int rv = TFAIL;
    int fd = 0;  /* File descriptor for testing filesystem support of O_DIRECT */
    char fname[] = "testfile"; /* File name for testing filesystem support of O_DIRECT */

    /* Test for filesystem support of O_DIRECT */
    if ((fd = open(fname, O_DIRECT|O_RDWR|O_CREAT, 0666)) < 0)
    {
 tst_resm(TCONF, "ERROR: O_DIRECT is not supported by this filesystem. Error: %d, %s",
     errno, strerror(errno));
 return rv;
    }
    else
    {
 close(fd);
 unlink(fname);
    }
    rv = TPASS;
    return rv;
}


/*====================*/
/*= VT_dio_bio_vectors_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_dio_bio_vectors_cleanup(void)
{
    int rv = TPASS;
    return rv;
}


/*====================*/
/*= VT_dio_bio_vectors_test =*/
/**
@brief  Template test scenario 4 function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_dio_bio_vectors_test(int argc, char** argv)
{
    int rv = TFAIL;
    int *pidlst;        /* Pointer to array of child PIDs */
    int numchild = 5; /* Number of children. Default 5 */
    int fail_count = 0, /* Number of failed tests */
        failed = 0,     /* Bool value - true if at least one testblock failed */
        total = 0,      /* Number of total tests */
        opt;

    bufsize = BUFSIZE;
    sprintf(filename,"testdata-4.%d", getpid());
    /* Options */
    while ((opt = getopt(argc, argv, "b:o:i:n:f:")) != -1)
    {
        switch(opt)
        {
     case 'b':
         if ((bufsize = atoi(optarg)) <= 0)
                {
      fprintf(stderr, "bufsize must be > 0\n");
      prg_usage();
  }
  if (bufsize % 4096 != 0)
                {
      fprintf(stderr, "bufsize must be multiple of 4k\n");
      prg_usage();
  }
  break;
     case 'o':
         if ((offset = atoi(optarg)) <= 0)
                {
      fprintf(stderr, "offset must be > 0\n");
      prg_usage();
  }
  break;
     case 'i':
         if ((iter = atoi(optarg)) <= 0)
                {
      fprintf(stderr, "iterations must be > 0\n");
      prg_usage();
  }
  break;
     case 'n':
         if ((numchild = atoi(optarg)) <= 0)
                {
      fprintf(stderr, "no of children must be > 0\n");
      prg_usage();
  }
  break;
     case 'f':
         strcpy(filename, optarg);
  break;
     default:
         prg_usage();
 }
    }

    /* Testblock-1: Read with Direct IO, Write without */
    if (forkchldrn(&pidlst, numchild, READ_DIRECT, child_function) < 0 )
    {
        tst_resm (TFAIL, "Read with Direct IO, Write without");
        failed = 1;
 fail_count++;
    }
    else
    {
        if (waitchldrn(&pidlst, numchild) < 0)
        {
            tst_resm (TFAIL, "Read with Direct IO, Write without");
     failed = 1;
     fail_count++;
        }
 else tst_resm (TPASS, "Read with Direct IO, Write without");
    }
    unlink(filename);
    free(pidlst);
    total++;

    /* Testblock-2: Write with Direct IO, Read without */
    if (forkchldrn(&pidlst, numchild, WRITE_DIRECT, child_function) < 0)
    {
 tst_resm (TFAIL, "Write with Direct IO, Read without");
    failed = 1;
 fail_count++;
    }
    else
    {
        if (waitchldrn(&pidlst, numchild) < 0)
        {
            tst_resm (TFAIL, "Write with Direct IO, Read without");
     failed = 1;
     fail_count++;
 }
 else tst_resm (TPASS, "Write with Direct IO, Read without");
    }
    unlink(filename);
    free(pidlst);
    total++;

    /* Testblock-3: Read, Write with Direct IO. */
    if (forkchldrn(&pidlst, numchild, RDWR_DIRECT, child_function) < 0)
    {
 tst_resm (TFAIL, "Read, Write with Direct IO");
        failed = 1;
 fail_count++;
    }
    else
    {
        if (waitchldrn(&pidlst, numchild) < 0)
        {
            tst_resm (TFAIL, "Read, Write with Direct IO");
     failed = 1;
     fail_count++;
 }
 else tst_resm (TPASS, "Read, Write with Direct IO");
    }
    unlink(filename);
    free(pidlst);
    total++;

    if (failed)
    {
 tst_resm(TINFO, "dio_bio_vectors_test: %d/%d testblocks failed", fail_count, total);
 return rv;
    }
    tst_resm(TINFO, "dio_bio_vectors_test: %d testblocks %d iterations completed", total, iter);

    rv = TPASS;
    return rv;
}

/*====================*/
/*= prg_usage =*/
/**
@brief  Display the program usage.

@param  Input: None
 Output: None

@return None
*/
/*====================*/
void prg_usage()
{
    fprintf(stderr, "Usage: dio_bio_vectors_test [-b bufsize] [-o offset]"
        "[-n numchild] [-i iterations] [-f filename]\n");
    tst_resm (TBROK, "usage");
    tst_exit();
}

/*====================*/
/*= fillbuf =*/
/**
@brief  Fill buffer of given size with given character value

@param  Input: buf - pointer to filling buffer
  count - filling length
  value - filling character
 Output: None

@return None
*/
/*====================*/
void fillbuf(char *buf, int count, char value)
{
    while (count-- > 0) *buf++ = value;
}

/*====================*/
/*= vfillbuf =*/
/**
@brief  Fill vector array with given character value

@param  Input: iv    - pointer to vector array
  vcnt  - length of array
  value - filling character
 Output: None

@return None
*/
/*====================*/
void vfillbuf(struct iovec *iv, int vcnt, char value)
{
    int idx;

    for (idx = 0; idx < vcnt; iv++, idx++)
    {
        fillbuf(iv->iov_base, iv->iov_len, (char)value);
    }
}

/*====================*/
/*= bufcmp =*/
/**
@brief  Compare two buffers with same size

@param  Input: buf1 - pointer to 1st buffer
  buf2 - pointer to 2nd buffer
  bsize - size of buffers
 Output: None

@return Buffers mismatch - returns -1
 Buffers match    - returns  0
*/
/*====================*/
int bufcmp(char *buf1, char *buf2, int bsize)
{
    int idx;

    for (idx = 0; idx < bsize; idx++)
    {
 if (buf1[idx] != buf2[idx])
 {
            fprintf(stderr, "ERROR: bufcmp, offset %d, expected: 0x%x, got 0x%x\n", idx, buf1[idx], buf2[idx]);
     return -1;
 }
    }
    return 0;
}

/*====================*/
/*= vbufcmp =*/
/**
@brief  Compare buffers of two vector arrays with same length of buffers and arrays

@param  Input: iv1 - pointer to 1st array
  iv2 - pointer to 2nd array
  vcnt - arrays length
 Output: None

@return Buffers mismatch - returns -1
 Buffers match    - returns  0
*/
/*====================*/
int vbufcmp(struct iovec *iv1, struct iovec *iv2, int vcnt)
{
    int idx;

    for (idx = 0; idx < vcnt; iv1++, iv2++, idx++)
    {
        if (bufcmp(iv1->iov_base, iv2->iov_base, iv1->iov_len) < 0)
        {
     fprintf(stderr, "ERROR: vbufcmp, Vector: %d, iv1base=%s, iv2base=%s\n",
                idx, (char *)iv1->iov_base, (char *)iv2->iov_base);
     return -1;
 }
    }
    return 0;
}


/*====================*/
/*= forkchldrn =*/
/**
@brief  Fork the given number of children and set the function that child should execute.

@param  Input: numchld - number of children to fork
  action - type of read or write operations
  chldfunc - function executed by child
 Output: pidlst - pointer to array of children PIDs

@return Failure - returns -1
 Success - returns  0
*/
/*====================*/
int forkchldrn(int **pidlst, int numchld, int action, int (*chldfunc)())
{
    int idx, cpid;

    if ((*pidlst = ((int*)malloc(sizeof(int) * numchld))) == 0)
    {
        fprintf(stderr, "ERROR: forkchldrn: malloc failed for pidlst: %s\n", strerror(errno));
        return -1;
    }
    for(idx = 0; idx < numchld; idx++)
    {
        if((cpid = fork()) < 0)
 {
     fprintf(stderr,"ERROR: forkchldrn: fork child %d failed, %s\n", idx, strerror(errno));
            killchldrn(pidlst, idx, SIGTERM);
     return -1;
        }
 if( cpid == 0 ) exit((*chldfunc)(idx, action));
        else *(*pidlst + idx) = cpid;
    }
    return 0;
}

/*====================*/
/*= waitchldrn =*/
/**
@brief  Wait for child process listed in pidlst to finish.

@param  Input: pidlst - pointer to array of children PIDs
                numchld - number of children to fork
 Output: None

@return Failure - returns -1
 Success - returns  0
*/
/*====================*/
/*
 * waitchldrn: wait for child process listed in pidlst to finish.
*/
int waitchldrn(int **pidlst, int numchld)
{
    int idx, cpid, ret,
    errflag = 0;        /* Will < 0 if waitpid() failed or testblock in child failed */
    int status;

    for(idx = 0; idx < numchld; idx++)
    {
        cpid = *(*pidlst + idx);
 if(cpid == 0) continue;
 if( (ret = waitpid(cpid, &status, 0)) != cpid )
        {
     fprintf(stderr,"ERROR: waitchldrn: wait failed for child %d, pid %d: %s\n", idx, cpid, strerror(errno));
            errflag--;
 }
 if (status) errflag = -1;
    }
    return errflag;
}

/*====================*/
/*= killchldrn =*/
/**
@brief  Signal the children listed in pidlst with the given signal.

@param  Input: pidlst - pointer to array of children PIDs
                numchld - number of children to fork
  sig - signal to send
 Output: None

@return Failure - returns -1
 Success - returns  0
*/
/*====================*/
int killchldrn(int **pidlst, int numchld, int sig)
{
    int idx, cpid, errflag = 0;

    for(idx = 0; idx < numchld; idx++)
    {
        cpid = *(*pidlst + idx);
        if(cpid > 0)
        {
     if(kill(cpid, sig) < 0)
            {
         fprintf(stderr,"ERROR: killchldrn: kill %d failed, %s\n", cpid, strerror(errno));
                errflag--;
     }
 }
    }
    return errflag;
}

/*====================*/
/*= runtest =*/
/**
@brief  Write the data to the file. Read the data from the file and compare.
 For each iteration, write data starting at seekoff + iter * bufsize * childnum * nvector location
 in the file and read from there. Compare buffers.

@param  Input: fd_r - file descriptor for read
  fd_w - file descriptor for write
  childnum - index of this child
  action - type of read or write operations
 Output: None

@return Buffers mismatch or another failure - returns -1
 Buffers match                       - returns  0
*/
/*====================*/
int runtest(int fd_r, int fd_w, int childnum, int action)
{
    off64_t seekoff;
    int nvector = 5;  /* Size of vector array. Default 5 */
    int idx, vidx;
    struct iovec *iov1, *iov2, *iovp;

    seekoff = offset + (off64_t)childnum * (off64_t)iter * (off64_t)nvector * (off64_t)bufsize;

    /* Allocate for buffers and data pointers */
    if ((iov1 = (struct iovec *)valloc(sizeof(struct iovec) * nvector)) == NULL)
    {
        fprintf(stderr, "ERROR: runtest: valloc iov1 failed: %s\n", strerror(errno));
 return -1;
    }
    if ((iov2 = (struct iovec *)valloc(sizeof(struct iovec) * nvector)) == NULL)
    {
        fprintf(stderr, "ERROR: runtest: valloc iov2 failed: %s\n", strerror(errno));
 return -1;
    }
    for (idx = 0, iovp = iov1; idx < nvector; iovp++, idx++)
    {
        if ((iovp->iov_base = valloc(bufsize)) == NULL)
        {
     fprintf(stderr, "ERROR: runtest: valloc for iov1->iov_base failed: %s\n", strerror(errno));
            return -1;
        }
 iovp->iov_len = bufsize;
    }
    for (idx = 0, iovp = iov2; idx < nvector; iovp++, idx++)
    {
        if ((iovp->iov_base = valloc(bufsize)) == NULL)
        {
     fprintf(stderr, "ERROR: runtest: valloc for iov2->iov_base failed: %s\n", strerror(errno));
            return -1;
        }
 iovp->iov_len = bufsize;
    }

    /* seek, write, read and verify */
    for (idx = 0; idx < iter; idx++)
    {
        vfillbuf(iov1, nvector, childnum + idx);
        for (vidx = 0, iovp = iov1; vidx < nvector; vidx++, iovp++)
        {
            if (lseek(fd_w, seekoff + idx * vidx * bufsize, SEEK_SET) < 0)
            {
                fprintf(stderr, "ERROR: runtest: lseek before write failed: %s\n", strerror(errno));
                return -1;
            }
     if (write(fd_w, iovp->iov_base, iovp->iov_len) < bufsize)
            {
         fprintf(stderr, "ERROR: runtest: write failed: %s\n", strerror(errno));
         return -1;
            }
            if (action == READ_DIRECT)
            {
         /* Make sure data is on to disk before read */
         if (fsync(fd_w) < 0)
                {
             fprintf(stderr, "ERROR: runtest: fsync failed: %s\n", strerror(errno));
                    return -1;
         }
     }
        } /* vidx */
        for (vidx = 0, iovp = iov2; vidx < nvector; vidx++, iovp++)
        {
            if (lseek(fd_r, seekoff + idx * vidx * bufsize, SEEK_SET) < 0)
            {
         fprintf(stderr, "ERROR: runtest: lseek before read failed: %s\n", strerror(errno));
                return -1;
     }
     if (read(fd_r, iovp->iov_base, iovp->iov_len) < bufsize)
            {
         fprintf(stderr, "ERROR: runtest: read failed:%s\n", strerror(errno));
                return -1;
            }
        } /* vidx */
        if (vbufcmp(iov1, iov2, nvector) != 0)
        {
     fprintf(stderr, "ERROR: runtest: comparison failed. Child=%d offset=%d\n", childnum, (int)seekoff);
            return -1;
        }
    } /* idx */
    return 0;
}

/*====================*/
/*= child_function =*/
/**
@brief  Open the file for read and write. Call the runtest routine.

@param  Input: childnum - index of this child
  action - type of read or write operations
 Output: None

@return Failure - returns -1
 Success - returns  0
*/
/*====================*/
int child_function(int childnum, int action)
{
    int fd_w, fd_r;

    switch(action)
    {
        case READ_DIRECT:
     if ((fd_w = open(filename, O_WRONLY|O_CREAT, 0666)) < 0)
            {
         fprintf(stderr, "ERROR: open for write failed for %s: %s\n", filename, strerror(errno));
                return -1;
     }
     if ((fd_r = open(filename, O_DIRECT|O_RDONLY, 0666)) < 0)
            {
         fprintf(stderr, "ERROR: open for read failed for %s: %s\n", filename, strerror(errno));
                close(fd_w);
  return -1;
            }
     if (runtest(fd_r, fd_w, childnum, action) == -1)
            {
         fprintf(stderr, "ERROR: Read Direct - child %d failed\n", childnum);
                close(fd_w);
  close(fd_r);
  return -1;
     }
     break;
        case WRITE_DIRECT:
     if ((fd_w = open(filename, O_DIRECT|O_WRONLY|O_CREAT, 0666)) < 0)
            {
         fprintf(stderr, "ERROR: open for write failed for %s: %s\n", filename, strerror(errno));
                return -1;
     }
     if ((fd_r = open(filename, O_RDONLY, 0666)) < 0)
            {
                fprintf(stderr, "ERROR: open for read failed for %s: %s\n", filename, strerror(errno));
  close(fd_w);
  return -1;
     }
     if (runtest(fd_r, fd_w, childnum, action) == -1)
            {
                fprintf(stderr, "ERROR: Write Direct - child %d failed\n", childnum);
  close(fd_w);
  close(fd_r);
  return -1;
     }
     break;
        case RDWR_DIRECT:
     if ((fd_w = open(filename, O_DIRECT|O_WRONLY|O_CREAT, 0666)) < 0)
            {
         fprintf(stderr, "ERROR: open for write failed for %s: %s\n", filename, strerror(errno));
  return -1;
     }
     if ((fd_r = open(filename, O_DIRECT|O_RDONLY, 0666)) < 0)
            {
         fprintf(stderr, "ERROR: open for read failed for %s: %s\n", filename, strerror(errno));
  close(fd_w);
  return -1;
     }
     if (runtest(fd_r, fd_w, childnum, action) == -1)
            {
         fprintf(stderr, "ERROR: RDWR Direct - child %d failed\n", childnum);
                close(fd_w);
  close(fd_r);
  return -1;
     }
     break;
        default:
     fprintf(stderr, "Invalid Action Value\n");
     return -1;
 }
 close(fd_w);
 close(fd_r);
        return 0;
}
