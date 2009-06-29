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
        @file   dio_append_test.c

        @brief  GPIO dio_append test scenario C source.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                          Date          Number    Description of Changes
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
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

#ifdef __cplusplus
extern "C"{
#endif
    
/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}

#endif

/* Verification Test Environment Include Files */
#include "dio_append_test.h"

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
/*===== VT_dio_append_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_append_setup(void)
{
    return TPASS;
}


/*================================================================================================*/
/*===== VT_dio_append_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_append_cleanup(void)
{
    return TPASS;
}


/*================================================================================================*/
/*===== VT_dio_append_test =====*/
/**
@brief  Template test scenario X function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_append_test(int argc, char** argv)
{
    int   pid[NUM_CHILDREN];
    int   idx, kidx,
          status,
          ret,
          errflag = 0;
    char  *filename = "testfile";

    /* fork NUM_CHILDREN children */
    for (idx = 0; idx < NUM_CHILDREN; idx++)
    {
        if ((pid[idx] = fork()) < 0)
        {
            /* fork() error */
            perror("fork error");
            for (kidx = 0; kidx < NUM_CHILDREN; kidx++)
            {
                if (pid[kidx] > 0)
                    kill(pid[kidx], SIGTERM);
            }
            return TFAIL;
        }
        /* each child does buffered read */
        if (pid[idx] == 0)
            read_eof(filename, idx);
    }

    /* Parent appends to end of file using direct i/o */
    if (append_to_file(filename)) return TFAIL;

    for (idx = 0; idx < NUM_CHILDREN; idx++)
    {
        if (pid[idx] > 0)
            kill(pid[idx], SIGTERM);
    }
    /* check children exit status. If child test fails, status == 1 */
    for (idx = 0; idx < NUM_CHILDREN; idx++)
    {
        if (pid[idx] == 0) continue;
        if( (ret = waitpid(pid[idx], &status, 0)) != pid[idx] )
        {
            fprintf(stderr,"ERROR: waitpid: wait failed for child %d, pid %d: %s\n", idx, pid[idx], strerror(errno));
            errflag--;
        }
        if (status == 1) errflag = -1;
    }
    if (errflag < 0) return TFAIL;
    
    
    unlink(filename);
    return TPASS;
}

/*================================================================================================*/
/*===== read_eof =====*/
/**
@brief  Do buffered read from child from a file created by parent direct write.
        Compares read values with zero via check_zero().

@param  filename - name of file to create
    
@return On success - return 0
        On failure - return -1
*/
/*================================================================================================*/
int read_eof(char *filename, int childnum)
{
    int fd;
    int idx;
    int ret;
    off_t offset;
    unsigned char buf[BUFSIZE], *bufoff;

    while ((fd = open(filename, O_RDONLY)) < 0)
    {
        sleep(1);        /* wait for file to be created */
    }

    for (idx = 0 ; idx < 1000000; idx++)
    {
        offset = lseek(fd, 0, SEEK_END);
        ret = read(fd, buf, BUFSIZE);
        if (ret > 0)
        {
            if ((bufoff = check_zero(buf, ret)))
            {
                fprintf(stderr, "Child %d: non-zero read at offset %p\n", childnum,  offset + bufoff);
                close(fd);
                exit(1);
            }
        }
    }
    close(fd);
    exit(0);
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

/*================================================================================================*/
/*===== append_to_file =====*/
/**
@brief  Creates a file of length MEMSIZE * iter and fills it with zeroes

@param  filename - name of file to create
    
@return On success - return 0
        On failure - return -1
*/
/*================================================================================================*/
int append_to_file(char *filename)
{
    int fd;
    void *bufptr;       /* Address of allocated memory with posix_memalign() */
    int idx;
    int ret;
    
    fd = open(filename, O_DIRECT|O_WRONLY|O_CREAT, 0666);
    if (fd < 0)
    {
        perror("cannot create file");
        return -1;
    }
    
    /* Align memory, set with zeroes */
    if (posix_memalign(&bufptr, 4096, MEMSIZE))
    {
        perror("cannot malloc aligned memory");
        return -1;
    }
    memset(bufptr, 0, MEMSIZE);
    
    /* Write iter blocks of size MEMSIZE into file. Close file */
    for (idx = 0; idx < 3000; idx++)
    {
        if ((ret = write(fd, bufptr, MEMSIZE)) != MEMSIZE)
        {
            fprintf(stderr, "write %d block returned %d instead of %d\n", idx, ret, MEMSIZE);
            return -1; 
        }
    }
    if (close(fd) < 0)
    {
        perror("cannot close file");
        return -1;
    }
    /* Success */
    return 0;
}
