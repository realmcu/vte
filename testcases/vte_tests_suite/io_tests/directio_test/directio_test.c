/*================================================================================================*/
/**
    @file   directio_test.c

    @brief  GPIO directio test scenario C source.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             08/06/2004     TLSbo39741   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <stdio.h>      /* fprintf() */
#include <stdlib.h>     /* valloc() */
#include <unistd.h>     /* close(), unlink(), write(), read(), lseek() */
#include <sys/types.h>  /* open(), lseek() */
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <ctype.h>      /* isprint() */
#include <string.h>     /* strcpy(), strerror() */
#ifdef __cplusplus
extern "C"{
#endif
    
/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}
#endif

/* Verification Test Environment Include Files */
#include "directio_test.h"

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
char	infile[LEN];		/* Input file. Default "infile" */
char	outfile[LEN];		/* Output file. Default "outfile" */
int	fd1 = 0, fd2 = 0;	/* File descriptors for input & output files */
char	*buf = NULL;		/* Read/write buffer */

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
/*===== VT_directio_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_directio_setup(void)
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
/*===== VT_directio_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_directio_cleanup(void)
{
    return TPASS;
}


/*================================================================================================*/
/*===== VT_directio_test =====*/
/**
@brief  Template test scenario 1 function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_directio_test(int argc, char** argv)
{
    int	bufsize = BUFSIZE;	/* Buffer size. Default 8k */
    int	numblks = NBLKS;	/* Number of blocks. Default 20 */
    int	idx,
	num,			/* Bytes read/wrote from/to file */
	offset,			/* Position in file */
        opt;

    strcpy(infile, "infile");	/* Input file */
    strcpy(outfile, "outfile");	/* Outfile file */
    /* Options */
    while ((opt = getopt(argc, argv, "b:n:i:o:")) != -1)
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
	    case 'n':
	        if ((numblks = atoi(optarg)) <= 0)
                {
	            fprintf(stderr, "numblks must be > 0\n");
		    prg_usage();
	        }
	        break;
	    case 'i':
		strcpy(infile, optarg);
		break;
	    case 'o':
		strcpy(outfile, optarg);
		break;
	    default:
		prg_usage();
        }
    }

    /* Open files */
    if ((fd1 = open(infile, O_DIRECT|O_RDWR|O_CREAT, 0666)) < 0)
    {
	fprintf(stderr, "ERROR: open() infile failed: %s\n", strerror(errno));
	return TFAIL;
    }

    if ((fd2 = open(outfile, O_DIRECT|O_RDWR|O_CREAT, 0666)) < 0)
    {
	fprintf(stderr, "ERROR: open() outfile failed: %s\n", strerror(errno));
	return TFAIL;
    }

    /* Allocate for buf */
    if ((buf = valloc(bufsize)) == NULL) 
    {
	fprintf(stderr, "ERROR: valloc() buf failed: %s\n", strerror(errno));
	return TFAIL;
    }
    
    /* Fill buf, write data to input file */
    for (idx = 0; idx < numblks; idx++)
    {
	fillbuf(buf, bufsize, (char)(idx % 256));
	if (write(fd1, buf, bufsize) < 0)
	{ 
	    fprintf(stderr, "ERROR: write() infile failed: %s\n", strerror(errno));
	    return TFAIL;
	}
    }
    
    /* Copy infile to outfile using direct read and direct write */
    offset = 0;
    if (lseek(fd1, offset, SEEK_SET) < 0)
    {
	fprintf(stderr, "ERROR: lseek() infile failed: %s\n", strerror(errno));
	return TFAIL;
    }

    while ((num = read(fd1, buf, bufsize)) > 0)
    {
	if (lseek(fd2, offset, SEEK_SET) < 0)
	{
	    fprintf(stderr, "ERROR: lseek() outfile failed: %s\n", strerror(errno));
	    return TFAIL;
	}
	if (write(fd2, buf, num) < num)
	{
	    fprintf(stderr, "ERROR: write() outfile failed: %s\n", strerror(errno));
	    return TFAIL;
	}
	offset += num;
	if (lseek(fd1, offset, SEEK_SET) < 0)
	{
	    fprintf(stderr, "ERROR: lseek() infile failed: %s\n", strerror(errno));
	    return TFAIL;
	}
    }

    /* Verify files */
    if (filecmp(infile, outfile) != 0)
    {
        fprintf(stderr, "ERROR: file compare failed for %s and %s\n", infile, outfile);
        return TFAIL;
    }

    /* Close and unlink files */
    if (close(fd1) < 0)
    {
	tst_resm(TWARN, "ERROR: close() infile failed: %s", strerror(errno));
    }
    if (close(fd2) < 0)
    {
        tst_resm(TWARN, "ERROR: close() outfile failed: %s", strerror(errno));
    }
    unlink(infile);
    unlink(outfile);
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
    fprintf(stderr, "Usage: directio_test [-b bufsize] [-n numblks] [-i infile] [-o outfile]\n");
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
/*===== filecmp =====*/
/**
@brief  Compares two files

@param  Input:	f1, f2 - pointers to arrays containing file names
	Output: None
		
@return	Files mismatch or failure - returns -1
	Files match               - returns  0
*/
/*================================================================================================*/
int filecmp(char *f1, char *f2)
{
    int idx;
    int fd1, fd2;
    int num1, num2 = 0;
    char buf1[BUFSIZ], buf2[BUFSIZ];

    /* Open files for read */
    if ((fd1 = open(f1, O_RDONLY)) == -1)
    {
        fprintf(stderr, "ERROR: compare_files: open failed %s: %s\n", f1, strerror(errno));
	return -1;
    }
    if ((fd2 = open(f2, O_RDONLY)) == -1)
    {
	close(fd1);
        fprintf(stderr, "ERROR: compare_files: open failed %s: %s\n", f2, strerror(errno));
	return -1;
    }

    /* Compare the files */
    while ((num1 = read(fd1, buf1, BUFSIZ)) > 0)
    {
        num2 = read(fd2, buf2, BUFSIZ);
        if (num1 != num2)
        {
            fprintf(stderr, "ERROR: compare_files: file length mistmatch:\n    read: %d from %s, %d from %s\n",
		num1, f1, num2, f2);
            close(fd1);
            close(fd2);
            return -1;
        }
        for (idx = 0; idx < num1; idx++)
        {
            if ( buf1[idx] != buf2[idx] )
            {
                fprintf(stderr, 
		    "ERROR: compare_file: char mismatch:\n   %s offset %d: 0x%02x %c\n   %s offset %d: 0x%02x %c\n",
		    f1, idx, buf1[idx], isprint(buf1[idx]) ? buf1[idx] : '.',
		    f2, idx, buf2[idx], isprint(buf2[idx]) ? buf2[idx] : '.');
                close(fd1);
                close(fd2);
                return -1;
            }
        }
    }
    close(fd1);
    close(fd2);
    return 0;
}
