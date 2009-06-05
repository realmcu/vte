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
#                       Modification     Tracking
# Author                    Date          Number    Description of Changes
#--------------------   ------------    ----------  ---------------------
# Spring Zhang           06/08/2008       n/a      Initial ver. 
# Spring                 05/09/2008       n/a      Add mx51 support
# Spring                 26/09/2008       n/a      Fix some volume 0 bug
# Spring                 27/10/2008       n/a      Add mx35&mx37 support  
# Spring                 28/11/2008       n/a      Modify COPYRIGHT header
#############################################################################
# Portability:   ARM sh 
# File Name:     dac_vol_adj.sh   
# Total Tests:   1
# Test Strategy: play audio streams with volume up and down
# 
# Use command "./dac_vol_adj.sh [audio stream]" 
# Tested on : i.MX51&35&37
# Support: i.MX31&MX51&MX35&MX37

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
    export TCID="TGE_LV_DAC_VOL_ADJ"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -lt 1 ]
    then
        usage
        exit 1
    fi

    trap "cleanup" 0

    if [ ! -e /usr/bin/aplay ]
    then
        tst_resm TBROK "Test #1: ALSA utilities are not ready, \
        pls check..."
        RC=65
        return $RC
    fi

    if [ ! -e $1 ]
    then
        tst_resm TBROK "Test #1: audio stream is not ready, \
        pls check..."
        RC=66
        return $RC
    fi

}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup() 
{
    RC=0
    echo "clean up environment..."
    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 50 >/dev/null
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 120 > /dev/null
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 150 >/dev/null
    fi
    echo "clean up environment end"
    return $RC
}

# Function:     adjust volume
#
# Description:  - adjust voice volume and play a audio file
#
# Exit:         - zero on success
#               - non-zero on failure.
#
adj_vol()
{
    RC=0    # Return value from setup, and test functions.

    platfm.sh || platfm=$?
    if [ $platfm -eq 67 ]
    then
        RC=$platfm
        return $RC
    fi
    platform="mx$platfm"

    tst_resm TINFO "Test #1: play the audio stream, please check the HEADPHONE,\
 hear if the voice is from MIN(0) to MAX(MX31:100, MX51:128)."
    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 0
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        #Min:0, Max:127
        amixer -c 0 cset name='Headphone Volume' 0
    elif [ $platform == "mx37" ]
    then
        #Min:0, Max:255
        amixer cset numid=3,iface=MIXER,name='Playback PCM Volume' 0
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 10
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 30
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 50
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 30
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 60
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 100
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 50
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 90
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 150
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 80
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 110
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 200
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 100
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 127
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 255
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    tst_resm TINFO "Do you hear the voice volume from MIN to MAX?[y/n]"
    read answer
    if [ $answer = "y" ]
    then
        tst_resm TPASS "Test #1: ALSA DAC volume adjust test from MIN to MAX success."
    else
        tst_resm TFAIL "Test #1: ALSA DAC volume adjust test fail"
        RC=67
        return $RC
    fi

    # adjust volume from MAX to MIN
    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 100
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 127
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 255
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 80
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 110
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 200
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 50
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 90 
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 150
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 30
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 60
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 100
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 15
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 30
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 50
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    if [ $platform = "mx31" ]
    then
        amixer -c 0 cset numid=2,iface=MIXER,name='Master Playback Volume' 0
    elif [ $platfm -eq 35 ] || [ $platfm -eq 51 ] || [ $platfm -eq 41 ]  #sgtl5k
    then
        amixer -c 0 cset name='Headphone Volume' 0
    elif [ $platform == "mx37" ]
    then
        amixer cset numid=3 0
    fi
    aplay -M -N -D hw:0,0 $1 2>/dev/null || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: play error, please check the stream file"
        return $RC
    fi

    tst_resm TINFO "Do you hear the voice volume from MAX to MIN?[y/n]"
    read answer
    if [ $answer = "y" ]
    then
        tst_resm TPASS "Test #1: ALSA DAC volume adjust test success."
    else
        tst_resm TFAIL "Test #1: ALSA DAC volume adjust test fail"
        RC=67
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

    Use this command to test ALSA DAC volume adjust functions.
    The volume will turn from MIN(0) to MAX(100) and then from MAX to MIN.
    usage: ./${0##*/} [audio stream]
    e.g.: ./${0##*/} audio48k16S.wav

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

adj_vol "$@" || exit $RC

