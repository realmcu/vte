# File :        mmc_test.sh
#
# Description:  This is a test to format MMC, create an ext2 file system on it and read/write some data
#
#======================================================================
#
#                             Freescale SemiconductorConfidential Proprietary
#                   (c) Copyright 2004, Freescale Semiconductor, Inc.  All rights reserved.  
#            
#Presence of a copyright notice is not an acknowledgement of publication.  
#This software file listing contains information of Freescale Semiconductor, Inc. that is of a confidential and 
#proprietary nature and any viewing or use of this file is prohibited without specific written 
#permission from Freescale Semiconductor, Inc.
     
#=====================================================================================
#Revision History:
#                            Modification     Tracking
# Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#V. Becker / rc023c        07/09/2004  TLSbo40423   Initial version 
#L.Delaspre/rc149c            12/10/2004     TLSbo42821   VTE 1.5.1 integration 

#!/bin/ash

#Set path variable to add vte binaries
#export TESTCASES_HOME= `pwd`
#export PATH=${PATH}:${TESTCASES_HOME}

# Function:     setup
#        
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#   
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
setup()
{
  # Total number of test cases in this file. 
   export TST_TOTAL=1

   #The TCID and TST_COUNT variables are required by the LTP 
   #command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="mmc_test"
    # Set up is initialized as test 0
    export TST_COUNT=0

    # Initialize cleanup function to execute on program exit. 
    # This function will be called before the test program exits.
    trap "cleanup" 0
    RC=0
    return $RC
}

cleanup()
{
    #Unmount MMC
    echo "CLEANUP : Unmounting file system";
    umount /mnt/mmc_part1
}

mmc_test()
{
    TCID="mmc_test"
    TST_COUNT=1
    RC=0

    #Get MMC card capabilities
    echo "Get MMC card capabilities"
    fdisk -l /dev/mmc/blk0/disc || RC=$?

    if [ $RC -ne 0 ]
    then
	echo "Test FAIL : Failed to get MMC card capabilities with fdisk"
	return $RC
    fi

    # Check to see if MMC comes pre-formatted or not
    if [ -f /dev/mmc/blk0/part1 ]
    then
	echo "Card is formatted in /dev/mmc/blk0/part1"
    else
	echo "Card is not formatted"
	echo "We are going to format it"
	fdisk /dev/mmc/blk0/disc || RC=$?

	if [ $RC -ne 0 ]
	    then
		echo "Test FAIL : Failed to create partition"
		return $RC
	fi
	return $RC
    fi

    #Format the MMC by creating an ext2 file system type on it
    mkfs -t ext2 /dev/mmc/blk0/part1
    #Or mkfs.ext2 /dev/mmc/blk0/part1

    #Mount the file system on /mnt/mmc_part1
    mkdir /mnt/mmc_part1
    mount -t ext2 /dev/mmc/blk0/part1 /mnt/mmc_part1

    #Erase MMC card
    echo "Erase MMC contents"
    dd if=/dev/zero of=/dev/mmc/blk0/disc bs=1024k count=5 || RC=$?
    if [ $RC -ne 0 ]
	then
	    echo "Test FAIL : Failed to erase MMC card with dd"
	    return $RC
    fi

    #Write a file in MMC : let's write a test !
    echo "Write a file in MMC"
    dd if=/tmp/vte/testcases/bin/mmc_testapp_1 of=/dev/mmc/blk0/disc || RC=$? 
    if [ $RC -ne 0 ]
	then
	    echo "Test FAIL : Failed to write MMC card with dd"
	    return $RC
    fi

    #Read 1MB of data in MMC 
    echo "Read 1 megabyte of data in MMC"
    dd if=/dev/mmc/blk0/disc of=output_file bs=1024k count=1 || RC=$?
    if [ $RC -ne 0 ]
	then
	    echo "Test FAIL : Failed to read 1 megabyte of data in MMC with dd"
	    return $RC
    fi

    #Set test to PASS
    echo "Test PASS !"
}

# Function:     main
# 
# Description:  - Execute all tests, exit with test status.
#               
# Exit:         - zero on success
#               - non-zero on failure.
#
# Return value from setup, and test functions.
RC=0

setup  || exit $RC

mmc_test || exit $RC
