#!/bin/sh
#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
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
# Input:	- 
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
    export TCID="TGE_LV_ASRC_GRP"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    sample_rate=$1

    trap "cleanup" 0

    if [ ! -e /unit_tests/mxc_asrc_test.out ]
    then
        tst_resm TBROK "ASRC utilities are not ready, pls check!"
        RC=65
        return $RC
    fi

    [ ! -z $STREAM_PATH ] || {
        tst_resm TBROK "STREAM_PATH not set, pls check!" 
        RC=66
        return $RC
    } 

    [ -e $STREAM_PATH/asrc_stream/audio${sample_rate}k16S.wav ] || {
        tst_resm TBROK "Source file S16_LE not ready, pls check!"
        RC=67
        return $RC
    }
    [ -e $STREAM_PATH/asrc_stream/audio${sample_rate}k24S-S24_LE.wav ] || {
        tst_resm TBROK "Source file S24_LE not ready, pls check!"
        RC=67
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
# Description:  - test with basic ASRC supported sample rate, if equal, ignore
# input:  32k, 44.1k, 48k, 96k
# output: 32k, 44.1k, 48k, 96k
#
# Exit:         - zero on success
#               - non-zero on failure.
#
asrc_with_sample_rate()
{
    RC_S16=0
    RC_S24=0
    basic_rates="32 44 48 96"
    for dst_rate in $basic_rates; do
        #Test converting to dst rate
        tst_resm TINFO "Test $sample_rate converting to ${dst_rate}KHz"
        #destination sampling rate revise
        case $dst_rate in
            32)  act_rate=32000;;
            44)  act_rate=44100;;
            48)  act_rate=48000;;
            64)  act_rate=64000;;
            88)  act_rate=88200;;
            96)  act_rate=96000;;
            176) act_rate=176400;;
            192) act_rate=192000;;
        esac
        [ ${sample_rate} -ne $dst_rate ] && {
            tst_resm TINFO "ASRC S16_LE conversion from $sample_rate to $dst_rate"
            asrc_test1.sh -to $act_rate $STREAM_PATH/asrc_stream/audio${sample_rate}k16S.wav ||RC_S16=$?
            tst_resm TINFO "ASRC S24_LE conversion from $sample_rate to $dst_rate"
            asrc_test1.sh -to $act_rate $STREAM_PATH/asrc_stream/audio${sample_rate}k24S-S24_LE.wav ||RC_S24=$?
        }
        if [ $RC_S16 -ne 0 ]; then
            tst_resm TFAIL "ASRC fail on S16_LE conversion from $sample_rate to $dst_rate"
            RC=$RC_S16
        fi
        if [ $RC_S24 -ne 0 ]; then
            tst_resm TFAIL "ASRC fail on S24_LE conversion from $sample_rate to $dst_rate"
            RC=$RC_S24
        fi
    done


    if [ $RC -eq 0 ]
    then
        tst_resm TPASS "Basic ASRC Group conversion test success."
    else
        tst_resm TFAIL "Basic ASRC Group conversion test fail."
    fi

    return $RC
}


# Function:     asrc_with_sample_rate_ext()
#
# Description:  - test with extended ASRC supported sample rate, ignore if equal
# Input: 5.512k, 8k, 11.025k, 16k, 22k, 64k, 88.2k, 176.4k, 192k
# output: 64k, 88.2k, 176.4k 192k
#
# Exit:         - zero on success
#               - non-zero on failure.
#
asrc_with_sample_rate_ext()
{

    RC_S16=0
    RC_S24=0
    ext_rates="32 44 48 64 88 96 176 192"
    for dst_rate in $ext_rates; do
        #Test converting to dst rate
        tst_resm TINFO "Test $sample_rate converting to ${dst_rate}KHz"
        #destination sampling rate revise
        case $dst_rate in
            32)  act_rate=32000;;
            44)  act_rate=44100;;
            48)  act_rate=48000;;
            64)  act_rate=64000;;
            88)  act_rate=88200;;
            96)  act_rate=96000;;
            176) act_rate=176400;;
            192) act_rate=192000;;
        esac
        [ ${sample_rate} -ne $dst_rate ] && {
            tst_resm TINFO "ASRC S16_LE conversion from $sample_rate to $dst_rate"
            asrc_test1.sh -to $act_rate $STREAM_PATH/asrc_stream/audio${sample_rate}k16S.wav ||RC_S16=$?
            tst_resm TINFO "ASRC S24_LE conversion from $sample_rate to $dst_rate"
            asrc_test1.sh -to $act_rate $STREAM_PATH/asrc_stream/audio${sample_rate}k24S-S24_LE.wav ||RC_S24=$?
        }
        if [ $RC_S16 -ne 0 ]; then
            tst_resm TFAIL "ASRC fail on S16_LE conversion from $sample_rate to $dst_rate"
            RC=$RC_S16
        fi
        if [ $RC_S24 -ne 0 ]; then
            tst_resm TFAIL "ASRC fail on S24_LE conversion from $sample_rate to $dst_rate"
            RC=$RC_S24
        fi
    done


    if [ $RC -eq 0 ]; then
        tst_resm TPASS "ASRC group conversion extend test success."
    else
        tst_resm TFAIL "ASRC group conversion extend test fail."
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

    Use this command to test ASRC MEM to MEM functions.
    Including basic ASRC supporting sampling rates and extended sampling rates
    usage I: ./${0##*/} [input sampling rate]
    [Basic input sampling rate] can be 32, 44, 48, 96
    Output sampling rate is 32, 44, 48, 96k
    e.g.: ./${0##*/} 44

    usage II: ./${0##*/} ext [input sampling rate]
    [input sampling rate] can be 5, 8, 11, 16, 22, 32, 44, 48, 64, 88, 96, 176, 192
    output sampling rates will be 32k, 44.1k, 48k, 64k, 96k, 176.4k, 192k
    

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

if [ $# -lt 1 ] || [ "$1" = "-h" ]; then
    usage
    exit 1
fi

#"" will pass the whole args to function setup()
if_extend_test=$1
if [ $if_extend_test = "ext" ]; then
    shift
fi
setup "$@" || exit $RC

if [ $if_extend_test != "ext" ]; then
    asrc_with_sample_rate "$@" || exit $RC
else
    asrc_with_sample_rate_ext "$@" || exit $RC
fi
