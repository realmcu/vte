#!/bin/sh
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
##############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          09/12/2008        n/a        Initial ver.
#############################################################################
# Portability:  ARM sh
#
# File Name:
# Total Tests:
# Test Strategy:
#
# Input: -
#
# Return:       -
#
# Use command:

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
    # Initialize return code to zero.
    RC=0                 # Exit values of system commands used

    export TST_TOTAL=1   # Total number of test cases in this file.
    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    export TCID="TGE_LV_ASRC_SMP"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -ne 1 ]
    then
        usage
        exit 1
    fi

    sample_rate=$1

    trap "cleanup" 0

    if [ ! -e /unit_tests/mxc_asrc_test.out ]
    then
        tst_resm TBROK "Test #1: ASRC utilities are not ready, pls check!"
        RC=65
        return $RC
    fi

    [ ! -z $STREAM_PATH ] || {
        tst_resm TBROK "Test #1: STREAM_PATH not set, pls check!"
        RC=66
        return $RC
    }
}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
    echo "cleanup"
}

# Function:     asrc_with_sample_rate()
#
# Description:  - test with different sample rate, if equal, ignore
#
# Exit:         - zero on success
#               - non-zero on failure.
#
asrc_with_sample_rate()
{
    [ -e $STREAM_PATH/asrc_stream/audio${sample_rate}k16S.wav ] || {
        tst_resm TBROK "Test #1: target file not ready, pls check!"
        RC=67
        return $RC
    }

    #Test converting to 32KHz
    tst_resm TINFO "Test #1: Test converting to 32KHz"
    [ ${sample_rate} -ne 32 ] && {
        asrc_test1.sh -to 32000 $STREAM_PATH/asrc_stream/audio${sample_rate}k16S.wav ||RC=$?
    }
    [ $RC -eq 0 ] || {
        tst_resm TFAIL "Test #1: ASRC test fail"
        return $RC
    }

    #Test converting to 44.1KHz
    tst_resm TINFO "Test #2: Test converting to 44.1KHz"
    [ ${sample_rate} -ne 44 ] && {
        asrc_test1.sh -to 44100 $STREAM_PATH/asrc_stream/audio${sample_rate}k16S.wav ||RC=$?
    }
    [ $RC -eq 0 ] || {
        tst_resm TFAIL "Test #2: ASRC test fail"
        return $RC
    }

    #Test converting to 48KHz
    tst_resm TINFO "Test #3: Test converting to 48KHz"
    [ ${sample_rate} -ne 48 ] && {
        asrc_test1.sh -to 48000 $STREAM_PATH/asrc_stream/audio${sample_rate}k16S.wav ||RC=$?
    }
    [ $RC -eq 0 ] || {
        tst_resm TFAIL "Test #3: ASRC test fail"
        return $RC
    }

    #Test converting to 96KHz
    tst_resm TINFO "Test #4: Test converting to 96KHz"
    [ ${sample_rate} -ne 96 ] && {
        asrc_test1.sh -to 96000 $STREAM_PATH/asrc_stream/audio${sample_rate}k16S.wav ||RC=$?
    }
    [ $RC -eq 0 ] || {
        tst_resm TFAIL "Test #4: ASRC test fail"
        return $RC
    }

    if [ $RC -eq 0 ]
    then
        tst_resm TPASS "Test #1: ASRC test success."
    else
        tst_resm TFAIL "Test #1: ASRC test fail"
    fi

    return $RC
}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF

    Use this command to test ASRC functions.
    usage: ./${0##*/} [input sampling rate]
    [input sampling rate] can be 32, 44, 48, 96
    e.g.: ./${0##*/} 44

EOF
}


# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
RC=0    # Return value from setup, and test functions.

#"" will pass the whole args to function setup()
setup "$@" || exit $RC

asrc_with_sample_rate "$@" || exit $RC
