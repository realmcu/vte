#!/bin/sh
#Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
#                          Modification     Tracking
# Author                       Date          Number    Description of Changes
#-----------------------   ------------    ----------  ---------------------
# Spring Zhang              Mar.17,2011       n/a        Initial ver. 
#

# Description: look for default ALSA DAC device index, e.g. stgl5000 for mx5x
# Output: 
#   return value: index, if not found, it will return 255, 
#       use match_alsa_dev.sh || rt=$? to get
#   and it will echo a string "ALSA DAC device not found", 
#       use rtstring=`match_alsa_dev.sh` to get
#

if [ "$1" == "-h" ]; then
    cat <<-EOF

        Use this command to test ALSA default device
        usage: ./${0##*/} ADC|DAC
            default will be DAC
EOF
    exit 255
fi

if [ "$1" == "ADC" ]; then
    dfl_alsa_dev=`arecord -l |grep -i imx3stack|awk '{ print $2 }'|sed 's/://'`
else
    dfl_alsa_dev=`aplay -l |grep -i imx3stack|awk '{ print $2 }'|sed 's/://'`
fi

if [ "$dfl_alsa_dev" != "" ]; then
    return $dfl_alsa_dev
else
    echo "ALSA DAC device not found"
    return 255
fi
