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
# Spring Zhang          27/11/2008       n/a        Initial ver. 
# Spring                28/11/2008       n/a        Modify COPYRIGHT header
#############################################################################
# Find the platform type
determine_platform()
{
    local find=0
    
    # Determine the platform
    find=`cat /proc/cpuinfo | grep "Revision" | grep "31.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        platform=mx31
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep "35.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        platform=mx35
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep "37.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        platform=mx37
    fi

    find=`cat /proc/cpuinfo | grep "Revision" | grep "51.*" | wc -l`;
    if [ $find -eq 1 ]
    then
        platform=mx51
    fi

    #find STMP378X
    find=`cat /proc/cpuinfo | grep "Hardware" | grep "378X" | wc -l`;
    if [ $find -eq 1 ]
    then
        platform=SMTP378X
    fi

    if [ $platform = "mx31" ]
    then
        echo "Platform MX31"
        RC=31
    elif [ $platform = "mx35" ]
    then
        echo  "Platform MX35" 
        RC=35
    elif [ $platform = "mx37" ]
    then
        echo  "Platform MX37" 
        RC=37
    elif [ $platform = "mx51" ]
    then
        echo  "Platform MX51" 
        RC=51
    elif [ $platform = "SMTP378X" ]
    then
        echo  "Platform SMTP378X" 
        let RC=378%128
    else
        echo  "Platform not recognized!"
        RC=67
    fi

    return $RC
}


# main
RC=0

determine_platform || exit $?

