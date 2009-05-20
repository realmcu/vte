/*====================*/
/**
    @file   ioerrors_test.c

    @brief  GPIO ioerrors test scenario C source.
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
Igor Semenchukov             10/06/2004     TLSbo39741   Initial version

====================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

======================*/


/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>
#include <stdio.h>      /* fprintf(), sprintf() */
#include <stdlib.h>     /* valloc(), free() */
#include <unistd.h>     /* close(), unlink(), write(), read(), lseek(), sbrk() */
#include <sys/types.h>  /* open(), lseek() */
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/mman.h>   /* mmap(), munmap() */
#include <string.h>     /* strerror() */
#include <sys/file.h>
#include <signal.h>

#ifdef __cplusplus
extern "C"{
#endif

/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}

#endif

/* Verification Test Environment Include Files */
#include "ioerrors_test.h"

/*======================
                                       LOCAL CONSTANTS
======================*/
#define BUFSIZE 4096
#define LEN     30
#define TRUE    1

/*====================*/
/*= VT_ioerrors_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ioerrors_setup(void)
{
    int fd = 0;  /* File descriptor for testing filesystem support of O_DIRECT */
    char fname[] = "testfile"; /* File name for testing filesystem support of O_DIRECT */

    /* Test for filesystem support of O_DIRECT */
    if ((fd = open(fname, O_DIRECT|O_RDWR|O_CREAT, 0666)) < 0)
    {
 tst_resm(TCONF, "ERROR: O_DIRECT is not supported by this filesystem. Error: %d, %s",
     errno, strerror(errno));
 return TPASS;
    }
    else
    {
 close(fd);
 unlink(fname);
    }
    return TPASS;
}


/*====================*/
/*= VT_ioerrors_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ioerrors_cleanup(void)
{
    return TPASS;
}


/*====================*/
/*= VT_ioerrors_test =*/
/**
@brief  ioerrors test scenario function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ioerrors_test(int argc, char** argv)
{
    int     fblocks = 1;  /* Iterations. Default 1 */
    int     bufsize = BUFSIZE;
    int     count, ret;
    int     offset;
    int     fd, newfd;
    int     idx,
            l_fail = 0,
     fail_count = 0,             /* Number of failed tests */
            failed = 0,                 /* Bool value - true if at least one testblock failed */
            total = 0;                  /* Number of total tests */
    int     pgsz = getpagesize();
    int     pagemask = ~(sysconf(_SC_PAGE_SIZE) - 1);
    char    *buf0,
            *buf1,                      /* Used only in test-14 */
            *buf2;
    char    filename[LEN];
    caddr_t shm_base;
    struct  sigaction act;

    act.sa_handler = SIG_IGN;
    sigaction(SIGXFSZ, &act, NULL);
    sprintf(filename,"testdata-3.%d", getpid());

    /* Open file, allocate for buffer 1, fill buffer 1, write it to file, close file */
    if ((fd = open(filename, O_DIRECT|O_RDWR|O_CREAT, 0666)) < 0)
    {
        fprintf(stderr, "ERROR: open failed for %s: %s\n", filename, strerror(errno));
        return TFAIL;
    }
    if ((buf0 = valloc(BUFSIZE)) == NULL)
    {
        fprintf(stderr, "ERROR: valloc buf0 failed: %s\n", strerror(errno));
        unlink(filename);
        return TFAIL;
    }
    for (idx = 1; idx < fblocks; idx++)
    {
        fillbuf(buf0, BUFSIZE, (char)idx);
        if (write(fd, buf0, BUFSIZE) < 0)
        {
     fprintf(stderr, "ERROR: write failed for %s: %s\n", filename, strerror(errno));
            close(fd);
            unlink(filename);
            return TFAIL;
        }
    }
    close(fd);

    if ((buf2 = valloc(BUFSIZE)) == NULL)
    {
        fprintf(stderr, "ERROR: valloc buf2 failed: %s\n", strerror(errno));
        unlink(filename);
        return TFAIL;
    }
    if ((fd = open(filename, O_DIRECT|O_RDWR)) < 0)
    {
        fprintf(stderr, "ERROR: open failed for %s: %s\n", filename, strerror(errno));
        unlink(filename);
        return TFAIL;
    }

    /* Test-1: Negative Offset */
    offset = -1;
    count = bufsize;
    ret = lseek(fd, offset, SEEK_SET);
    if ((ret >= 0) || (errno != EINVAL))
    {
 fprintf(stderr, "lseek allows negative offset. Returns %d: %s", ret, strerror(errno));
 tst_resm(TFAIL, "Negative offset");
 failed = TRUE;
        fail_count++;
    }
    else tst_resm(TPASS, "Negative offset");
    total++;

    /* Test-2: Odd count of read and write */
    offset = 0;
    count = 1;
    lseek(fd, 0, SEEK_SET);
    ret = write(fd, buf2, 4096);
    if (ret < 0)
    {
        fprintf(stderr,"ERROR: test-2: can't write to file %d: %s\n", ret, strerror(errno));
    }
    ret = runtest_f(fd, buf2, offset, count, EINVAL, 2, "odd count");
    if (ret != 0)
    {
        tst_resm (TFAIL, "Odd count of read and write");
 failed = TRUE;
 fail_count++;
    }
    else tst_resm (TPASS, "Odd count of read and write");
    total++;

    /* Test-3: Read beyond the file size */
    offset = BUFSIZE * (fblocks + 10);
    count = bufsize;
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        fprintf(stderr, "ERROR: test-3.1: lseek failed: %s\n", strerror(errno));
 tst_resm(TFAIL, "Read beyond the file size");
        failed = TRUE;
 fail_count++;
    }
    else
    {
        ret = read(fd, buf2, count);
        if (ret > 0 || errno != EINVAL)
        {
            fprintf(stderr,"ERROR: test-3: allows read beyond file size. Returns %d: %s\n",
     ret, strerror(errno));
            tst_resm(TFAIL, "Read beyond the file size");
            failed = TRUE;
     fail_count++;
        }
 else tst_resm(TPASS, "Read beyond the file size");
    }
    total++;

    /* Test-4: Invalid file descriptor */
    offset = 4096;
    count = bufsize;
    newfd = -1;
    ret = runtest_f(newfd, buf2, offset, count, EBADF, 4, "negative fd");
    if (ret != 0)
    {
 tst_resm(TFAIL, "Invalid file descriptor");
        failed = TRUE;
 fail_count++;
    }
    else tst_resm(TPASS, "Invalid file descriptor");
    total++;

    /* Test-5: Out of range file descriptor */
    offset = 4096;
    count = bufsize;
    if ((newfd = getdtablesize()) < 0)
    {
        fprintf(stderr, "ERROR: test-5: getdtablesize failed: %s\n", strerror(errno));
        tst_resm(TFAIL, "Out of range file descriptor");
        failed = TRUE;
 fail_count++;
    }
    else
    {
        ret = runtest_f(newfd, buf2, offset, count, EBADF, 5, "out of range fd");
        if (ret != 0)
        {
     tst_resm(TFAIL, "Out of range file descriptor");
     failed = TRUE;
            fail_count++;
 }
 else tst_resm(TPASS, "Out of range file descriptor");
    }
    total++;

    /* Test-6: Closed file descriptor */
    offset = 4096;
    count = bufsize;
    if (close(fd) < 0)
    {
        fprintf(stderr, "ERROR: test-6: can't close fd %d: %s\n", fd, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "close failed");
        return TFAIL;
    }
    ret = runtest_f(fd, buf2, offset, count, EBADF, 6, "closed fd");
    if (ret != 0)
    {
 tst_resm(TFAIL, "Closed file descriptor");
        failed = TRUE;
 fail_count++;
    }
    else tst_resm(TPASS, "Closed file descriptor");
    total++;

    /* Test-7: Character device (/dev/null) read, write */
    offset = 0;
    count = bufsize;
    if ((newfd = open("/dev/null", O_DIRECT|O_RDWR)) < 0)
    {
        fprintf(stderr, "ERROR: test-7: Direct I/O on /dev/null is not supported, skip test #9.\n");
        tst_resm (TCONF, "Skipped Character device read, write");
    }
    else
    {
        ret = runtest_s(newfd, buf2, offset, count, 9, "/dev/null");
 if (ret != 0)
        {
     tst_resm(TFAIL, "character device read, write");
     failed = TRUE;
     fail_count++;
 }
 else tst_resm(TPASS, "character device read, write");
    }
    total++;

    /* Test-8: read, write to a mmapped file */
    shm_base = (char *)(((long)sbrk(0) + (pgsz-1)) & ~(pgsz-1));
    if (shm_base == NULL)
    {
        fprintf(stderr, "ERROR: test-8: sbrk failed: %s\n", strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "sbrk failed");
        return TFAIL;
    }
    offset = 4096;
    count = bufsize;
    if ((fd = open(filename, O_DIRECT|O_RDWR)) < 0)
    {
        fprintf(stderr, "ERROR: can't open %s: %s\n", filename, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "Open failed");
        return TFAIL;
    }
    shm_base = mmap(shm_base, 0x100000, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, fd, 0);
    if (shm_base == (caddr_t)-1)
    {
        fprintf(stderr, "ERROR: test-8: can't mmap file: %s\n", strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "mmap failed");
        return TFAIL;
    }
    ret = runtest_s(fd, buf2, offset, count, 10, "mmapped file");
    if (ret != 0)
    {
 tst_resm(TFAIL, "read, write to a mmapped file");
 failed = TRUE;
 fail_count++;
    }
    else tst_resm(TPASS, "read, write to a mmaped file");
    total++;

    /* Test-9: read, write to an unmapped file with munmap */
    if ((ret = munmap(shm_base, 0x100000)) < 0)
    {
        fprintf(stderr, "ERROR: test-9: can't unmap file: %s\n", strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "munmap failed");
        return TFAIL;
    }
    ret = runtest_s(fd, buf2, offset, count, 11, "unmapped file");
    if (ret != 0)
    {
        failed = TRUE;
 fail_count++;
 tst_resm(TFAIL, "read, write to an unmapped file");
    }
    else tst_resm (TPASS, "read, write to an unmapped file");
    if (close(fd) < 0)
    {
        fprintf(stderr, "ERROR: test-9: can't close fd %d: %s\n", fd, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "close failed");
        return TFAIL;
    }
    total++;

    /* Test-10: read from file not open for reading */
    offset = 4096;
    count = bufsize;
    if ((fd = open(filename, O_DIRECT|O_WRONLY)) < 0)
    {
        fprintf(stderr, "ERROR: test-10: can't open %s: %s\n", filename, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "open failed");
        return TFAIL;;
    }
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        fprintf(stderr, "ERROR: test-10: lseek failed: %s\n", strerror(errno));
    failed = TRUE;
 fail_count++;
 tst_resm(TFAIL, "lseek failed");
    }
    else
    {
        ret = read(fd, buf2, count);
 if (ret >= 0 || errno != EBADF)
        {
     fprintf(stderr, "ERROR: test-10: allows read on file not open for reading. Returns %d: %s\n",
                ret, strerror(errno));
     failed = TRUE;
     fail_count++;
     tst_resm(TFAIL, "Read from file not open for reading");
        }
 else tst_resm(TPASS, "Read from file not open for reading");
    }
    close(fd);
    total++;

    /* Test-11: write to file not open for writing */
    offset = 4096;
    count = bufsize;
    if ((fd = open(filename, O_DIRECT|O_RDONLY)) < 0)
    {
        fprintf(stderr, "ERROR: test-11: can't open %s: %s\n", filename, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "open failed");
 return TFAIL;
    }
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        fprintf(stderr, "ERROR: test-11: lseek failed:%s\n", strerror(errno));
 failed = TRUE;
 fail_count++;
 tst_resm(TFAIL, "lseek failed");
    }
    else
    {
 ret = write(fd, buf2, count);
        if (ret >= 0 || errno != EBADF)
        {
            fprintf(stderr, "ERROR: test-11: allows write on file not open for writing. Returns %d: %s\n",
         ret, strerror(errno));
     failed = TRUE;
     fail_count++;
     tst_resm(TFAIL, "Write to file not open for writing");
    }
        else tst_resm(TPASS, "Write to file not open for writing");
    }
    close(fd);
    total++;

    /* Test-12: read, write with non-aligned buffer */
    offset = 4096;
    count = bufsize;
    l_fail = 0;
    if ((fd = open(filename, O_DIRECT|O_RDWR)) < 0)
    {
        fprintf(stderr, "ERROR: test-12: can't open %s: %s\n", filename, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "open failed");
        return TFAIL;
    }
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        fprintf(stderr, "ERROR: test-12: lseek before read failed: %s\n", strerror(errno));
        l_fail = TRUE;
    }
    else
    {
        if ((ret = read(fd, buf2 + 1, count)) != -1)
        {
            fprintf(stderr,"ERROR: test-12: allows read nonaligned buf. returns %d:%s\n", ret, strerror(errno));
            l_fail = TRUE;
        }
    }
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        fprintf(stderr, "ERROR: test-12: lseek before read failed: %s\n", strerror(errno));
        l_fail = TRUE;
    }
    else
    {
        if ((ret = write(fd, buf2 + 1, count)) != -1)
        {
     fprintf(stderr,"ERROR: test-12: allows write nonaligned buf. Returns %d: %s\n", ret, strerror(errno));
            l_fail = TRUE;
        }
    }
    if (l_fail)
    {
        failed = TRUE;
 fail_count++;
 tst_resm(TFAIL, "Read, write with non-aligned buffer");
    }
    else tst_resm(TPASS, "Read, write with non-aligned buffer");
    total++;
    close(fd);

    /* Test-13: read, write buffer in read-only space */
    offset = 4096;
    count = bufsize;
    l_fail = 0;
    if ((fd = open(filename, O_DIRECT|O_RDWR)) < 0)
    {
        fprintf(stderr, "ERROR: test-13: can't open %s: %s\n", filename, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "open failed");
        return TFAIL;
    }
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        fprintf(stderr, "ERROR: test-13: lseek before read failed: %s\n", strerror(errno));
        l_fail = TRUE;
    }
    else {
#if defined(__powerpc64__)
        ret = read(fd, (char*)(((ulong *)main)[0] & pagemask), count);
#else
        ret = read(fd, (char*)((ulong)main & pagemask), count);
#endif
 if (ret >= 0 || errno != EFAULT)
        {
     fprintf(stderr,"ERROR: test-13: read to read-only space. Returns %d:%s\n", ret, strerror(errno));
            l_fail = TRUE;
        }
    }
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        fprintf(stderr, "ERROR: test-13: lseek before write failed: %s\n", strerror(errno));
        l_fail = TRUE;
    }
    else {
#if defined(__powerpc64__)
        ret = write(fd, (char *)(((ulong *)main)[0] & pagemask), count);
#else
 ret = write(fd, (char *)((ulong)main & pagemask), count);
#endif
 if (ret < 0 )
        {
     fprintf(stderr,"[15] write to read-only space. Returns %d: %s\n", ret, strerror(errno));
            l_fail = TRUE;
 }
    }
    if (l_fail)
    {
        failed = TRUE;
 fail_count++;
 tst_resm(TFAIL, "Read, write buffer in read-only space");
    }
    else tst_resm(TPASS, "Read, write buffer in read-only space");
    close(fd);
    total++;

    /* Test-14: read, write in non-existant space */
    offset = 4096;
    count = bufsize;
    if ((buf1 = (char *) (((long)sbrk(0) + (pgsz-1)) & ~(pgsz-1))) == NULL)
    {
        fprintf(stderr,"ERROR: test-14: sbrk: %s\n", strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "sbrk failed");
        return TFAIL;
    }
    if ((fd = open(filename, O_DIRECT|O_RDWR)) < 0)
    {
        fprintf(stderr, "ERROR: test-14: can't open %s: %s\n", filename, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "open failed");
        return TFAIL;
    }
    ret =runtest_f(fd, buf1, offset, count, EFAULT, 16, " nonexistant space");
    if (ret != 0)
    {
        failed = TRUE;
 fail_count++;
 tst_resm(TFAIL, "Read, write in non-existant space");
    }
    else tst_resm(TPASS, "Read, write in non-existant space");
    total++;
    close(fd);

    /* Test-15: read, write for file with O_SYNC */
    offset = 4096;
    count = bufsize;
    if ((fd = open(filename, O_DIRECT|O_RDWR|O_SYNC)) < 0)
    {
        fprintf(stderr, "ERROR: test-15: can't open %s:%s\n", filename, strerror(errno));
        unlink(filename);
 tst_resm(TFAIL, "open failed");
        return TFAIL;;
    }
    ret = runtest_s(fd, buf2, offset, count, 17, "opened with O_SYNC");
    if (ret != 0)
    {
        failed = TRUE;
 fail_count++;
 tst_resm(TFAIL, "Read, write for file with O_SYNC");
    }
    else tst_resm(TPASS, "Read, write for file with O_SYNC");
    total++;
    close(fd);

    unlink(filename);
    if (failed)
    {
        fprintf(stderr, "GPIO_TestAPP_3: %d/%d test blocks failed\n", fail_count, total);
        return TFAIL;
    }
    fprintf(stderr, "GPIO_TestAPP_3: %d testblocks completed\n", total);
    return TPASS;
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
/*= runtest_f =*/
/**
@brief  Do read, writes. Verify the error value obtained by running
        read or write with the expected error value (errnum).

@param  Input: fd - file descriptor for read or write
  buf - read/write buffer
  offset - file read/write offset
                count - number of bytes to read/write
                errnum - error expected
                testnum - testblock number
                msg - testblock message
 Output: None

@return Failure - returns -1
        Success - returns 0
*/
/*====================*/
int runtest_f(int fd, char *buf, int offset, int count, int errnum, int testnum, char *msg)
{
    int ret;
    int l_fail = 0;

    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        if (errno != errnum)
        {
     fprintf(stderr, "ERROR: test-%d: lseek before read failed: %s\n", testnum, strerror(errno));
            l_fail = 1;
        }
    }
    else
    {
        ret = read(fd, buf, count);
        if (ret >= 0 || errno != errnum)
        {
     fprintf(stderr, "ERROR: test-%d: read allows %s. Returns %d: %s\n",
                testnum, msg, ret, strerror(errno));
     l_fail = 1;
 }
    }
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        if (errno != errnum)
        {
     fprintf(stderr, "ERROR: test-%d: lseek before write failed: %s\n", testnum, strerror(errno));
            l_fail = 1;
        }
    }
    else
    {
        ret = write(fd, buf, count);
        if (ret >= 0 || errno != errnum)
        {
     fprintf(stderr, "ERROR: test-%d: write allows %s. Returns %d: %s\n",
                testnum, msg, ret, strerror(errno));
     l_fail = 1;
 }
    }
    return(l_fail);
}

/*====================*/
/*= runtest_s =*/
/**
@brief  Do read, writes. Verify they run successfully.

@param  Input: fd - file descriptor for read or write
  buf - read/write buffer
  offset - file read/write offset
                count - number of bytes to read/write
                testnum - testblock number
                msg - testblock message
 Output: None

@return Failure - returns -1
        Success - returns 0
*/
/*====================*/
int runtest_s(int fd, char *buf, int offset, int count, int testnum, char *msg)
{
    int ret;
    int l_fail = 0;

    if (lseek(fd, offset, SEEK_SET) < 0)
    {
 fprintf(stderr, "ERROR: test-%d: lseek before read failed: %s\n", testnum, strerror(errno));
        l_fail = 1;
    }
    else
    {
        if ( (ret=read(fd, buf, count)) < 0 )
        {
     fprintf(stderr, "ERROR: test-%d: read failed for %s. Returns %d: %s\n",
                testnum, msg, ret, strerror(errno));
     l_fail = 1;
 }
    }
    if (lseek(fd, offset, SEEK_SET) < 0)
    {
        fprintf(stderr, "ERROR: test-%d: lseek before write failed: %s\n", testnum, strerror(errno));
        l_fail = 1;
    }
    else
    {
        if ( (ret=write(fd, buf, count)) < 0 )
        {
     fprintf(stderr, "ERROR: test-%d: write failed for %s. returns %d: %s\n",
  testnum, msg, ret, strerror(errno));
     l_fail = 1;
 }
    }
    return(l_fail);
}
