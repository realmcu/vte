#!/bin/sh
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#    @file   detect_alsa_dev.sh
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

if [ "$1" = "-h" ]; then
    cat <<-EOF
        Use this command to test ALSA default device
        usage: ./${0##*/} ADC|DAC
            default will be DAC
EOF
    exit 255
fi

if [ $(uname -r | grep 2.6.38|wc -l) -eq 1 ]; then
   HW_keyword=sgtl5000audio
else
   HW_keyword=imx3stack
fi

if [ $(platfm.sh) = "IMX6-SABREAUTO" ] || [ $(platfm.sh) = "IMX6ARM2" ]; then
   HW_keyword="card 0"
fi

if [ "$1" = "ADC" ]; then
    dfl_alsa_dev=`arecord -l |grep -i "$HW_keyword" | awk '{ print $2 }'|sed 's/://'`
else
    dfl_alsa_dev=`aplay -l |grep -i "$HW_keyword" |awk '{ print $2 }'|sed 's/://'`
fi

if [ "$dfl_alsa_dev" != "" ]; then
    return $dfl_alsa_dev
else
    echo "ALSA DAC device not found"
    return 255
fi
