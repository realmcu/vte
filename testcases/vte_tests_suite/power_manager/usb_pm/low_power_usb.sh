#!/bin/bash
# Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
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

#control interface for usb host controler
FSL_EHCI_INTERFACE="fsl-ehci*"
#control interface for hosts controller
FSL_USB2_UDC="fsl-usb2-udc"
#control interface for OTG controller
FSL_USB2_OTG="fsl-usb2-otg"

SKIP_USBPORT=

SYS_USB_DEV="/sys/bus/usb/devices/*"

enable_usb_wakeup()
{
 #enable host control wakeup
 for i in $(ls /sys/devices/platform/${FSL_USB2_UDC}/power/wakeup)
 do
 echo enabled > $i
 echo "echo enabled > $i" 
 done
 #enable OTG control wakeup
 for i in $(ls /sys/devices/platform/${FSL_USB2_OTG}/power/wakeup)
 do
 echo enabled > $i
 echo "echo enabled > $i" 
 done
 #enable ehci host control  
 for i in $(ls /sys/devices/platform/${FSL_EHCI_INTERFACE}/power/wakeup)
 do
 echo enabled > $i
 echo "echo enabled > $i" 
 done

 #enable all usb device wakeup
 usb_ehci_ctrls=$(ls ${SYS_USB_DEV}/product)
 for i in $usb_ehci_ctrls
 do
	  ctrl=$(dirname $i)/power/wakeup
    echo enabled > $ctrl
    echo "echo enabled > $ctrl"
 done
}

auto_usb_dev()
{
 prd_list=$(ls ${SYS_USB_DEV}/product)
 is_skip=""
 for i in $prd_list
 do
			if [ ! -z "$SKIP_USB_PORT" ]; then
				is_skip=$(echo $i | grep $SKIP_USBPORT)
			fi
      if [ -z "$is_skip" ]; then
	  	ctrl=$(dirname $i)/power/control
      echo auto > $ctrl
      echo "echo auto > $ctrl"
			sleep 2
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

usb_clk_list=$(cat /proc/cpu/clocks | grep usb | cut -c 46-47 | awk {'print $1'})

RT=0
for i in $usb_clk_list
do
  if [ $i -ne 0 ]; then
     echo "usb clock is not zero"
     RT=1
  fi
done
exit $RT
