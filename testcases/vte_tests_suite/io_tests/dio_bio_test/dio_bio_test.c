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
    @file   dio_bio_test.c

    @brief  dio_bio test scenario C source.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             09/06/2004     TLSbo39741   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <stdio.h>      /* fprintf(), sprintf() */
#include <stdlib.h>     /* valloc() */
#include <unistd.h>     /* close(), unlink(), write(), read(), lseek() */
#include <sys/types.h>  /* open(), lseek() */
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
#include "dio_bio_test.h"

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
int bufsize;

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
/*===== VT_dio_bio_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_bio_setup(void)
{
    int fd = 0;		/* File descriptor for testing filesystem support of O_DIRECT */
    char fname[] = "testfile"; /* File name for testing filesystem support of O_DIRECT */

    /* Test for filesystem support of O_DIRECT */
    if ((fd = open(fname, O_DIRECT|O_RDWR|O_CREAT, 0666)) < 0)
    {
	tst_resm(TCONF, "ERROR: O_DIRECT is not supported by this filesystem. Error: %d, %s",
	    errno, strerror(errno));
	return TFAIL;
    }
    else
    {
	close(fd);
	unlink(fname);
    }
    return TPASS;
}


/*================================================================================================*/
/*===== VT_dio_bio_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_bio_cleanup(void)
{
    return TPASS;
}


/*================================================================================================*/
/*===== VT_dio_bio_test =====*/
/**
@brief  dio_bio test scenario function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_dio_bio_test(int argc, char** argv)
{
    int	iter = 100;		/* Iterations. Default 100 */
    int	action,			/* What testblock should we run */
	fd_r, fd_w;		/* File descriptors for read and write */
    int	fail_count = 0,		/* Number of failed tests */
	total = 0,		/* Number of total tests */
	failed = 0,		/* Bool value - true if at least one testblock failed */
        opt;
    off64_t	offset = 0;	/* Offset. Default 0 */
    char	filename[LEN];	/* Name of test file */

    bufsize = BUFSIZE;
    sprintf(filename, "testdata-2.%d", getpid());
    /* Options */
    while ((opt = getopt(argc, argv, "b:o:i:f:")) != -1)
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
	    case 'f':
	        strcpy(filename, optarg);
		break;
	    default:
	        prg_usage();
	}
    }
    
    /* Testblock-1: Read with Direct IO, Write without */
    action = READ_DIRECT;
    if ((fd_w = open(filename, O_WRONLY|O_CREAT, 0666)) < 0)
    {
	fprintf(stderr, "ERROR: open for write failed for %s: %s\n", filename, strerror(errno));
	return TFAIL;
    }
    if ((fd_r = open(filename, O_DIRECT|O_RDONLY, 0666)) < 0)
    {
	fprintf(stderr, "ERROR: open for read failed for %s: %s\n", filename, strerror(errno));
	close(fd_w);
	unlink(filename);
	return TFAIL;
    }
    if (runtest(fd_r, fd_w, iter, offset, action) < 0)
    {
	tst_resm(TFAIL, "Read with Direct IO, Write without");
	failed = 1;
	fail_count++;
    }
    else tst_resm (TPASS, "Read with Direct IO, Write without");
    
    close(fd_w);
    close(fd_r);
    unlink(filename);
    total++;
    
    /* Testblock-2: Write with Direct IO, Read without */
    action = WRITE_DIRECT;
    if ((fd_w = open(filename, O_DIRECT|O_WRONLY|O_CREAT, 0666)) < 0)
    {
	fprintf(stderr, "ERROR: open for write failed for %s: %s\n", filename, strerror(errno));
	return TFAIL;
    }
    if ((fd_r = open(filename, O_RDONLY|O_CREAT, 0666)) < 0)
    {
	fprintf(stderr, "ERROR: open for read failed for %s: %s\n", filename, strerror(errno));
	close(fd_w);
	unlink(filename);
	return TFAIL;
    }
    if (runtest(fd_r, fd_w, iter, offset, action) < 0)
    {
	tst_resm (TFAIL, "Write with Direct IO, Read without");
	failed = 1;
	fail_count++;
    }
    else  tst_resm (TPASS, "Write with Direct IO, Read without");

    close(fd_w);
    close(fd_r);
    unlink(filename);
    total++;

    /* Testblock-3: Read, Write with Direct IO. */
    action = RDWR_DIRECT;
    if ((fd_w = open(filename, O_DIRECT|O_WRONLY|O_CREAT, 0666)) < 0)
    {
	fprintf(stderr, "ERROR: open for write failed for %s: %s\n", filename, strerror(errno));
	return TFAIL;
    }
    if ((fd_r = open(filename, O_DIRECT|O_RDONLY|O_CREAT, 0666)) < 0)
    {
	fprintf(stderr, "ERROR: open for read failed for %s: %s\n", filename, strerror(errno));
	close(fd_w);
	unlink(filename);
	return TFAIL;
    }
    if (runtest(fd_r, fd_w, iter, offset, action) < 0)
    {
	tst_resm (TFAIL, "Read, Write with Direct IO");
	failed = 1;
	fail_count++;
    }
    else  tst_resm (TPASS, "Read, Write with Direct IO");
    close(fd_w);
    close(fd_r);
    unlink(filename);
    total++;

    if (failed)
    {
	tst_resm(TINFO, "dio_bio_test: %d/%d testblocks failed", fail_count, total);
	return TFAIL;
    }
    tst_resm(TINFO, "dio_bio_test: %d testblocks %d iterations completed", total, iter);
    return TPASS;
}

/*================================================================================================*/
/*===== prg_usage =====*/
/**
@brief  Display the program usage.

@param  Input:	None
	Output: None
		
@return	None
*/
/*================================================================================================*/
void prg_usage()
{
    fprintf(stderr, "Usage: dio_bio_test [-b bufsize] [-o offset] [-i iterations] [-f filename]\n");
    tst_resm (TBROK, "usage");
    tst_exit();
}

/*================================================================================================*/
/*===== fillbuf =====*/
/**
@brief  Fill buffer of given size with given character value

@param  Input:	buf - pointer to filling buffer
		count - filling length
		value - filling character  
	Output: None
		
@return	None
*/
/*================================================================================================*/
void fillbuf(char *buf, int count, char value)
{
    while (count-- > 0) *buf++ = value;
}

/*================================================================================================*/
/*===== bufcmp =====*/
/**
@brief  Compare two buffers with same size

@param  Input:	buf1 - pointer to 1st buffer
		buf2 - pointer to 2nd buffer
		bsize - size of buffers
	Output: None
		
@return	Buffers mismatch - returns -1
	Buffers match    - returns  0
*/
/*================================================================================================*/
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

/*================================================================================================*/
/*===== runtest =====*/
/**
@brief  Write the data to the file. Read the data from the file and compare.
	For each iteration, write data starting at offset + iter * bufsize location
	in the file and read from there. Compare buffers.

@param  Input:	fd_r - file descriptor for read
		fd_w - file descriptor for write
		iter - number of iterations made
		offset - offset from beginning of the file
		action - type of read or write operations
	Output: None
		
@return	Buffers mismatch or another failure - returns -1
	Buffers match                       - returns  0
*/
/*================================================================================================*/
int runtest(int fd_r, int fd_w, int iter, off64_t offset, int action)
{
    char	*buf1;
    char	*buf2;
    int		idx;

    /* Allocate for buffers */
    if ((buf1 = valloc(bufsize)) == 0)
    {
	fprintf(stderr, "ERROR: valloc buf1: %s\n", strerror(errno));
	return -1;
    }
    if ((buf2 = valloc(bufsize)) == 0)
    {
	fprintf(stderr, "ERROR: valloc buf2: %s\n", strerror(errno));
	return -1;
    }

    /* seek bufsize * iteration and write. Seek and read. Verify. */
    for (idx = 0; idx < iter; idx++)
    {
    	fillbuf(buf1, bufsize, idx);
	if ( lseek(fd_w, offset + iter * bufsize, SEEK_SET) < 0 )
	{
	    fprintf(stderr, "ERROR: lseek before write failed: %s\n", strerror(errno));
	    return -1;
	}
	if ( write(fd_w, buf1, bufsize) < bufsize )
	{
	    fprintf(stderr, "ERROR: write failed: %s\n", strerror(errno));
	    return -1;
	}
	if ( lseek(fd_r, offset + iter * bufsize, SEEK_SET) < 0 )
	{
	    fprintf(stderr, "ERROR: lseek before read failed: %s\n", strerror(errno));
	    return -1;
	}
	if ( read(fd_r, buf2, bufsize) < bufsize )
	{
	    fprintf(stderr, "ERROR: read failed: %s\n", strerror(errno));
	    return -1;
	}
	if ( bufcmp(buf1, buf2, bufsize) != 0 )
	{
	    fprintf(stderr, "ERROR: read/write buffer comparison failed\n");
	    return -1;
	}
    }
    return 0;
}
