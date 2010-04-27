#Copyright (C) 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
# File :        nominal_mmc_test.sh
#
# Description:  This is a test to nominal functionality MMC/SD cards
#
# Author:       Sergey Zavjalov, Sergey.Zavjalov@telma.ru
#
# History:      Apr 01 2005 - Created - Sergey Zavjalov, zvjs001c, TLSbo45047, Initial version.
#               Feb 01 2007 - Added - Vitaly Khalabuda, b00306, TLSbo67307, Rework version
#
#! /bin/sh


# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)

export TESTCASES_HOME="../../../bin"
export PATH=${PATH}:${TESTCASES_HOME}


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
        export TCID="nominal_mmc_test"
        # Set up is initialized as test 0
        export TST_COUNT=0

        # Initialize cleanup function to execute on program exit.
        # This function will be called before the test program exits.
        trap "cleanup" 0
        RC=0
        return $RC
}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
        [ -f $MNT_DIR/tmp.file ] && rm -f $MNT_DIR/tmp.file
        [ -d $MNT_DIR ] && umount $MNT_DIR
        rm -rf $MNT_DIR

        RC=0
        return $RC
}

# Function:     test01
# Description   - Test if command dummy_cmd -f tst_file.in will print
#                 contents of tst_file.in
#               - Create input file tst_file.in
#               -
test01()
{
        TCID="nominal_mmc_test"    # Identifier of this testcase.
        TST_COUNT=1      # Test case number.
        RC=0             # return code from commands.

        export DISK=$MMC_DEV
        export MNT_DIR="./fs"

        [ ! -b $DISK ] && tst_resm TFAIL "No one card is present, please insert card"
        until ( [ -b $MMC_DEV ] )
        do
        sleep 1
        done

        tst_resm  TINFO "Erase all exiting partition"
        #erase all primary partions from DISK
        /sbin/sfdisk -q $DISK >&- << EOF
0,0
0,0
0,0
0,0
EOF

        DISK_CYL=`sfdisk -g $DISK | sed -ne "s/^.*:[[:blank:]]\+\([[:digit:]]\+\)[[:blank:]]\+cylinders.*$/\1/p"`

        tst_resm  TINFO "Create new partition"
        #create new primary partion
        /sbin/sfdisk -q $DISK >&- << EOF
0,$DISK_CYL
0,0
0,0
0,0
EOF

        for DISK_PART in $MMC_DEV"p1"
        do
        #Format disk
        tst_resm  TINFO "Create partion"
        ( mkfs.$FILE_SYSTEM $DISK_PART || RC=$? ) >&-
        [ $RC -ne 0 ] && {
                tst_resm  TFAIL "Unable format partion"
                return $RC
        }
        #Create directory
        [ ! -d $MNT_DIR ] && mkdir $MNT_DIR
        #Mount FS
        tst_resm  TINFO "Mount new partion"
        ( mount -t $FILE_SYSTEM $DISK_PART $MNT_DIR || RC=$? ) >&-
        [ $RC -ne 0 ] && {
                tst_resm  TFAIL "Unable mount partion"
                return $RC
        }
        #Create File
        tst_resm  TINFO "Create file"
        ( dd if=/dev/zero of=$MNT_DIR/tmp.file bs=1M count=$TMP_FILE_SZ || RC=$? ) >&-
        [ $RC -ne 0 ] && {
                tst_resm  TFAIL "Unable crate file"
                return $RC
        }
        #Cleanup
        tst_resm  TINFO "Remove file"
        rm -f $MNT_DIR/tmp.file
        tst_resm  TINFO "Unmount"
        umount $MNT_DIR
        [ $? -eq 0 ] && rm -rf $MNT_DIR
        done

        RC=0
        return $RC
}

# Function:     help
#
# Description:  - Prints usage information
#
# Exit:         - Nothing return
#
help()
{
        echo
        echo "Usage: $TCID <device_name> <file_system> <test_file_size_in_Mb>"
}      
      
# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
RC=0    # Return value from setup, and test functions.

if [ -z $1 ]
then 
        help
        exit 1
else
        export MMC_DEV=$1
fi

if [ -z $2 ]
then
        help
        exit 1
else
        export FILE_SYSTEM=$2
fi

if [ -z $3 ]
then
        help
        exit 1
else
        export TMP_FILE_SZ=$3
fi

setup || RC=$?
if [ $RC -ne 0 ]
then
        exit $RC
fi

test01 || RC=$?

if [ $RC -eq 0 ] 
then
        tst_resm  TPASS "This test case works as expected"
else
        tst_resm  TFAIL "This test case does NOT work as expected"
fi

exit $RC
