/*====================*/
/**
    @file   fs_perms_test.c

    @brief  Permissioons test, set permissions on file
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
S. ZAVJALOV / ------         10/06/2004     TLSbo39741  Initial version

====================
Portability:  ARM GCC  gnu compiler
======================*/

/*
 *  AUTHOR      : Jeff Martin (martinjn@us.ibm.com)
 *  HISTORY     :
 *     (04/12/01)v.99  First attempt at using C for fs-regression test.  Only tests read and write bits.
 *     (04/19/01)v1.0  Added test for execute bit.
 *     (05/23/01)v1.1  Added command line parameter to specify test file.
 *     (07/12/01)v1.2  Removed conf file and went to command line parameters.
 *
 */


/*======================
Total Tests: 1

Test Executable Name:  multicast_test

Test Strategy:  A test for send and receive multicast message by use UDP sockets
=====================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "fs_perms_test.h"

int VT_fs_perms_test ( int argc, char *argv[])
{
        char fperm[1];
        int result, exresult=0,  cuserId=0, cgroupId=0, userId=0, groupId=0;
        mode_t mode;

        switch (argc)
        {
                case 8:
                        mode = strtol(argv[1],(char**)NULL,010);
                        cuserId = atoi(argv[2]);
                        cgroupId = atoi(argv[3]);
                        userId = atoi(argv[4]);
                        groupId = atoi(argv[5]);
                        fperm[0] = *argv[6];
                        exresult = atoi(argv[7]);
                break;
                default:
                      printf("Usage: %s <mode of file> <UID of file> <GID of file> <UID of tester> <GID of tester> <permission to test r|w|x> <expected result as 0|1>\n",argv[0]);
                      exit(0);
        }

        testsetup(mode,cuserId,cgroupId);
        result=testfperm(userId,groupId,fperm);
        system("rm -f test.file");
        printf("%c a %03o file owned by (%d/%d) as user/group(%d/%d)  \n",fperm[0],mode,cuserId,cgroupId,userId,groupId);
        if (result == exresult)
        {
                printf("PASS\n");
                exit(0);
        }
        else
        {
                printf("FAIL\n");
                exit(1);
        }
}

int testsetup(mode_t mode, int cuserId, int cgroupId)
{
        system("cp testx.file test.file");
        chmod("test.file",mode);
        chown("test.file",cuserId,cgroupId);
        return(0);
}

int testfperm(int userId, int groupId, char* fperm)
{
        FILE *testfile;
        pid_t PID;
        int tmpi,nuthertmpi;

/*  SET CURRENT USER/GROUP PERMISSIONS */
        printf("setting group id...\n");
        if(setegid(groupId))
        {
                printf("could not setegid to %d.\n",groupId);
                seteuid(0);
                setegid(0);
                return(-1);
        }

        printf("setting user id...\n");
        if(seteuid(userId))
        {
        printf("could not seteuid to %d.\n",userId);
           seteuid(0);
           setegid(0);
           return(-1);
        }

        switch(tolower(fperm[0]))
        {
                case 'x':
                PID = fork();
                if (PID == 0)
                {
                        execlp("./test.file","test.file",NULL);
                        exit(0);
                }

                wait(&tmpi);
                nuthertmpi=WEXITSTATUS(tmpi);
                seteuid(0);
                setegid(0);
                return(nuthertmpi);

        default:
                if((testfile=fopen("test.file",fperm)))
                {
                        fclose(testfile);
                        seteuid(0);
                        setegid(0);
                        return (1);
                }
                else
                {
                        seteuid(0);
                        setegid(0);
                        return (0);
                }
        }
}

int VT_fs_perms_test_setup()
{
        return TPASS;
}

int VT_fs_perms_test_cleanup()
{
        return TPASS;
}
