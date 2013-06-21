#!/bin/bash
# Copyright (C) 2010, 2012, 2013 Freescale Semiconductor, Inc. All Rights Reserved.
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-----------------           ------------    ----------  -------------------------------
#Andy Tian                   2012/06/19        NA        Add port skip for auto suspend
#Andy Tian                   2013/03/20        NA        Enable wakeup only for supported
#Andy Tian                   2013/04/23        NA        Fix a bug when enable device remote wakeup
#Andy Tian                   2013/06/21        NA        3.5.7 version
#--------           ------------    ----------  -------------------------------

SKIP_USBPORT=$1

SYS_USB_DEV="/sys/bus/usb/devices/*"

enable_usb_wakeup()
{
	#for i in $(ls $SYS_USB_DEV/*/power/wakeup)
	for i in $(find /sys -name wakeup | grep usb)
	do
		echo enabled > $i
		echo "echo enabled > $i"
	done
}

auto_usb_dev()
{
 prd_list=$(ls ${SYS_USB_DEV}/configuration)
 is_skip=""
 for i in $prd_list
 do
			if [ ! -z "$SKIP_USBPORT" ]; then
				is_skip=$(echo $i | grep $SKIP_USBPORT)
			fi
      if [ -z "$is_skip" ]; then
	  	ctrl=$(dirname $i)/power/control
		if [ -e "$ctrl" ]; then
        	echo auto > $ctrl
      		echo "echo auto > $ctrl"
			sleep 2
		fi
      fi
  done
}


#check if root
is_root=$(whoami)
if [ $is_root != "root" ];then
echo "not root!"
exit 255
fi

enable_usb_wakeup
auto_usb_dev

sleep 5
exit $RT
