#!/bin/sh
##############################################################################
#
#  Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
# Spring Zhang          27/11/2008       n/a        Initial ver. 
# Spring                28/11/2008       n/a        Modify COPYRIGHT header
# Spring                15/01/2008       n/a        Add MX35TO2 judgement,
#                                                   move to tools/
# Spring                02/04/2009       n/a        Add MX51Babbage support
#############################################################################
# Usage1(return string):
#   platform=`platfm.sh`
#   Then use $platform to judge the platform
#
#   $platform   PLATFORM
#   IMX31ADS    IMX31ADS
#   IMX32ADS    IMX32ADS
#   IMX25_3STACK IMX25_3STACK
#   IMX31_3STACK IMX31_3STACK
#   IMX35_3STACK IMX35_3STACK
#   IMX37_3STACK IMX37_3STACK
#   IMX51_3STACK IMX51_3STACK
#   IMX51_BABBAGE IMX51_BABBAGE
#
#
# Usage2(return number): 
#   platfm.sh || platform=$?
#   Then use $platform to judge the platform
#
# Return value:
# 1. 31~51 for mx31~mx51 board.
#     rt value  Board
# e.g.  31       mx31
#       35       mx35
#       41       mx51 babbage
#
# 2. 378%256(=122) for SMTP378X board.(for return value is 0~255)
#       rt value    Board
# e.g.  378%256   SMTP378X
#
# 3. 67 for "Platform not recognized".(67 can be defined to other num)

# Some Platform Info
#35 TO2 platform:
#Revision        : 35120

# Find the platform type
determine_platform()
{
    local find=0
    
    # Determine the platform
    find=`cat /proc/cpuinfo | grep "Revision" | grep " 31.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX31_3STACK
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep " 35.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX35_3STACK
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep " 37.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX37_3STACK
    fi

    # MX51 TO2.0: Revision: 51020
    find=`cat /proc/cpuinfo | grep "Revision" | grep " 51.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX51_3STACK
    fi

    # MX51 Babbage TO1.1: Revision: 51011
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "Babbage" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=IMX51_BABBAGE
    fi

    #find STMP378X
    find=`cat /proc/cpuinfo | grep "Hardware" | grep " 378X" | wc -l`;
    if [ $find -eq 1 ]
    then
        p=SMTP378X
    fi

    if [ $p = "IMX31_3STACK" ]
    then
        #echo "Platform MX31"
        RC=31
    elif [ $p = "IMX35_3STACK" ]
    then
        #echo  "Platform MX35" 
        RC=35
    elif [ $p = "IMX37_3STACK" ]
    then
        #echo  "Platform MX37" 
        RC=37
    elif [ $p = "IMX51_3STACK" ]
    then
        #echo  "Platform MX51" 
        RC=51
    elif [ $p = "IMX51_BABBAGE" ]
    then
        #echo  "Platform MX51 Babbage"
        RC=41
    elif [ $p = "SMTP378X" ]
    then
        #echo  "Platform SMTP378X" 
        let RC=378%256
    else
        #echo  "Platform not recognized!"
        RC=67
    fi

    return $RC
}


# main
RC=0
if [ -e /unit_tests/test-utils.sh ] 
then
    #source unit test, align to Dev.
    source /unit_tests/test-utils.sh 
    p=`platform`
    echo "$p"
    # get $RC to exit
    determine_platform
else
    #if no file, use our own
    determine_platform
    echo "$p"
fi

exit $RC


