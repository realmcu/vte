/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
        @file   dio_sparse_test.c

        @brief  GPIO dio_sparse test scenario C source.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             10/06/2004     TLSbo39741  Initial version 
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
            If not, indicate specific reasons why is it not portable.

==================================================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <sys/mman.h>
#include <sys/wait.h>

#ifdef __cplusplus
extern "C"{
#endif
    
/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}

#endif

/* Verification Test Environment Include Files */
#include "dio_sparse_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
volatile int got_signal;

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_dio_sparse_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_sparse_setup(void)
{
    int rv = TPASS;
    
    /** insert your code here */
    
    return rv;
}


/*================================================================================================*/
/*===== VT_dio_sparse_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_sparse_cleanup(void)
{
    return TPASS;
}


/*================================================================================================*/
/*===== VT_dio_sparse_test =====*/
/**
@brief  Template dio_sparse test scenario function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_sparse_test(int argc, char** argv)
{
        int pid[NUM_CHILDREN];
        int idx, kidx;
        int num_children = NUM_CHILDREN;
        char *filename = "testfile";
        long alignment = 512;
        int readsize = 65536;
        int writesize = 65536;
        int filesize = 100*1024*1024;
        int optchar;
        int children_errors = 0;
        extern char *optarg;
        extern int optind, optopt, opterr;

    while ((optchar = getopt(argc, argv, "dr:w:n:a:s:")) != -1)
    {
        char *endp;
        switch (optchar)
        {
            case 'a':
                alignment = strtol(optarg, &endp, 0);
                alignment = (int)scale_by_kmg((long long)alignment, *endp);
                break;
            case 'r':
                readsize = strtol(optarg, &endp, 0);
                readsize = (int)scale_by_kmg((long long)readsize, *endp);
                break;
            case 'w':
                writesize = strtol(optarg, &endp, 0);
                writesize = (int)scale_by_kmg((long long)writesize, *endp);
                break;
            case 's':
                filesize = strtol(optarg, &endp, 0);
                filesize = (int)scale_by_kmg((long long)filesize, *endp);
                break;
            case 'n':
                num_children = atoi(optarg);
                if (num_children > NUM_CHILDREN)
                {
                    fprintf(stderr, "number of children limited to %d\n", NUM_CHILDREN);
                    num_children = NUM_CHILDREN;
                }
                break;
            case '?':
                usage();
                break;
        }
    }

    /* Create some dirty free blocks by allocating, writing, syncing, and then unlinking and freeing  */
    dirty_freeblocks(filesize);

    /* fork num_children children */
    for (idx = 0; idx < num_children; idx++)
    {
        if ((pid[idx] = fork()) < 0)
        {
            /* fork() error */
            perror("fork error");
            for (kidx = 0; kidx < num_children; kidx++)
            {
                if (pid[kidx] > 0)
                    kill(pid[idx], SIGTERM);
            }
            return TFAIL;
        }
        /* each child does buffered read */
        if (pid[idx] == 0)
            read_sparse(filename, filesize);
    }

    /* Parent write to a hole in a file using direct i/o */
    if(dio_sparse(filename, alignment, writesize, filesize))
        return TFAIL;

    for (idx = 0; idx < num_children; idx++)
    {
        kill(pid[idx], SIGTERM);
    }

    for (idx = 0; idx < num_children; idx++)
    {
        int status;
        pid_t p;

        p = waitpid(pid[idx], &status, 0);
        if (p < 0)
        {
            perror("waitpid");
        }
        else
        {
            if (WIFEXITED(status) && WEXITSTATUS(status) == 10)
                children_errors++;
        }
    }
    if (children_errors)
        return TFAIL;
        
    return TPASS;
}

/*================================================================================================*/
/*===== usage =====*/
/**
@brief  Prints usage information and exit.

@param  Input:  None
        Output: None 
@return Returns 1 
*/
/*================================================================================================*/
int usage(void)
{
    fprintf(stderr, "usage: dio_sparse [-n children] [-s filesize]"
                " [-w writesize] [-r readsize] [-a align]\n");
    exit(1);
}

/*================================================================================================*/
/*===== scale_by_kmg =====*/
/**
@brief  Scale value by kilo, mega, or giga.

@param  Input:  value - value scaled
                scale - scale character
        Output: None 
@return Scaled value
*/
/*================================================================================================*/
long long scale_by_kmg(long long value, char scale)
{
    switch (scale)
    {
        case 'g':
        case 'G':
            value *= 1024;
        case 'm':
        case 'M':
            value *= 1024;
        case 'k':
        case 'K':
            value *= 1024;
            break;
        case '\0':
            break;
        default:
            usage();
            break;
    }
    return value;
}

/*================================================================================================*/
/*===== dirty_freeblocks =====*/
/**
@brief  Creates some dirty free blocks on disk.

@param  Input:  size - size of blocks created
        Output: None 
@return On success - return 0
        On failure - return -1
*/
/*================================================================================================*/
int dirty_freeblocks(int size)
{
    int fd;
    void *p;
    int pg;
    char filename[1024];

    pg = getpagesize();
    size = ((size + pg - 1) / pg) * pg;
    sprintf(filename, "file.xx.%d", getpid());
    fd = open(filename, O_CREAT|O_RDWR, 0666);
    if (fd < 0)
    {
        perror("cannot open file");
        return -1;
    }
    if (ftruncate(fd, size))
    {
        perror("cannot truncate file");
        return -1;
    }
    p = mmap(0, size, PROT_WRITE|PROT_READ, MAP_SHARED|MAP_FILE, fd, 0);
    if (p == MAP_FAILED)
    {
        perror("cannot mmap file");
        return -1;
    }
    memset(p, 0xaa, size);
    if (msync(p, size, MS_SYNC))
    {
        perror("cannot msync file");
        return -1;
    }
    if (munmap(p, size) == -1)
    {
        perror("cannot munmap file");
        return -1;
    }
    close(fd);
    unlink(filename);
    return 0;
}

/*================================================================================================*/
/*===== dio_sparse =====*/
/**
@brief  Do DIO writes to a sparse file

@param  Input:  filename - name of file
                align - alignment of address of memory allocated
                writesize - size of write blocks
                filesize - size of created file
        Output: None 
@return On success - return 0
        On failure - return -1
*/
/*================================================================================================*/
int dio_sparse(char *filename, int align, int writesize, int filesize)
{
    int fd;
    void *bufptr;
    int idx;
    int ret;
    static struct sigaction s;

    s.sa_sigaction = sig_term_func;
    s.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &s, 0);

    fd = open(filename, O_DIRECT|O_WRONLY|O_CREAT, 0666);
    if (fd < 0)
    {
        perror("cannot create file");
            return -1;
    }
    ftruncate(fd, filesize);
    if (posix_memalign(&bufptr, align, writesize))
    {
        perror("cannot malloc aligned memory");
        return -1;
    }
    memset(bufptr, 0, writesize);
    for (idx = 0; idx < filesize; )
    {
        if ((ret = write(fd, bufptr, writesize)) != writesize)
        {
            fprintf(stderr, "write %d returned %d\n", idx, ret);
            return -1;
        }
        idx += ret;
        if (got_signal)
            break;
    }
    close(fd);
    unlink(filename);
    return 0;
}

/*================================================================================================*/
/*===== read_sparse =====*/
/**
@brief  Do buffered read from a sparse file.

@param  Input:  filename - name of file
                filesize - size of created file
        Output: None 
@return On success - return 0
        On failure - return 10
*/
/*================================================================================================*/
int read_sparse(char *filename, int filesize)
{
    int fd;
    int idx;
    int off;
    int ret;
    unsigned char buf[4096];

    while ((fd = open(filename, O_RDONLY)) < 0)
    {
        sleep(1);        /* wait for file to be created */
    }

    for (idx = 0 ; idx < 100000000; idx++)
    {
        off_t offset = 0;
        unsigned char *badbuf;
    
        lseek(fd, offset, SEEK_SET);
        for (off = 0; off < filesize + 1; off += sizeof(buf))
        {
            ret = read(fd, buf, sizeof(buf));
            if (ret > 0)
            {
                if ((badbuf = check_zero(buf, ret)))
                {
                    kill(getppid(), SIGTERM);
                    exit(10);
                }
            }
            offset += ret;
        }
    }
    exit(0);
}

void sig_term_func(int i, siginfo_t *si, void *p)
{
    got_signal++;
}

/*================================================================================================*/
/*===== check_zero =====*/
/**
@brief  Checks that buffer contents will be all zeroes.

@param  Input:  buf - pointer to checked buffer
                size - buffer size
        Output: None 
@return On success - return 0
        On failure - return mismatch value
*/
/*================================================================================================*/
unsigned char *check_zero(unsigned char *buf, int size)
{
    unsigned char *cur = buf;

    while (size > 0)
    {
        if (*buf != 0)
        {
            fprintf(stderr, "non zero buffer at buf[%d] => 0x%02x,%02x,%02x,%02x\n",
                cur - buf, (unsigned int)cur[0],
                size > 1 ? (unsigned int)cur[1] : 0,
                size > 2 ? (unsigned int)cur[2] : 0,
                size > 3 ? (unsigned int)cur[3] : 0);
            fprintf(stderr, "cur %p, buf %p\n", cur, buf);
            return cur;
        }
        cur++;
        size--;
    }
    return 0;        /* all zeros */
}

