#!/bin/bash
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
#pre-requiste: better to remove all usb devices before run the testcases
#

FSL_EHCI_INTERFACE="fsl-ehci.*"
FSL_USB2_UDC="fsl-usb2-udc"

SKIP_USBPORT="1.6"

SYS_USB_DEV="/sys/bus/usb/devices/*"
platfm.sh || platfm=$?

enable_usb_wakeup()
{
 usb_ehci_ctrls=$(ls /sys/devices/platform/${FSL_EHCI_INTERFACE}/power/wakeup)
 usb_ctrls=$(ls /sys/devices/platform/${FSL_USB2_UDC}/power/wakeup)" "${usb_ehci_ctrls}
 if [ $platfm -eq 61 ]; then
	 usb_ctrls=$(ls /sys/devices/platform/fsl*)
 else
	 usb_ehci_ctrls=$(ls /sys/devices/platform/${FSL_EHCI_INTERFACE}/power/wakeup)
	 usb_ctrls=$(ls /sys/devices/platform/${FSL_USB2_UDC}/power/wakeup)" "${usb_ehci_ctrls}
 fi
 for i in $usb_ctrls
 do
    echo enabled > $i
    echo "echo enabled > $i"
 done
}

suspend_usb_dev()
{
 prd_list=$(ls ${SYS_USB_DEV}/product)
 for i in $prd_list
 do
   is_str=$(cat $i | grep Storage)
   if [ ! -z "$is_str" ]; then
      echo suspend > $(echo $i | sed 's/product/power/')/level
      echo "echo suspend > $(echo $i | sed 's/product/power/')/level"
   fi
 done
}

auto_usb_dev()
{
 prd_list=$(ls ${SYS_USB_DEV}/power/level)
 for i in $prd_list
 do
      is_skip=$(echo $i | grep $SKIP_USBPORT)
      if [ -z "$is_skip" ]; then
      echo auto > $i
      echo "echo auto > $i"
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
suspend_usb_dev
auto_usb_dev

echo "please unplug all usb devices in 15 seconds"

sleep 15

usb_cnt=$(lsusb | wc -l)
while [ $usb_cnt -gt 4 ]
do
echo "please unplug all usb devices"
sleep 5
usb_cnt=$(lsusb | wc -l)
done

#cat /proc/cpu/clocks | grep usb 

#usb_clk_usage=$(cat /proc/cpu/clocks | grep usb | awk {'print $3'})

if [ $platfm -eq 61 ]; then
	mount -t debugfs none /sys/kernel/debug
	pll3=$(cat /sys/kernel/debug/clock/osc_clk/pll3_usb_otg_main_clk/usb*/enable_count)
	pll7=$(cat /sys/kernel/debug/clock/osc_clk/pll7_usb_host_main_clk/enable_count)
        usb_clk_usage=${pll3}" "${pll7}
	umount /sys/kernel/debug
else
	usb_clk_usage=$(cat /proc/cpu/clocks | grep usb | awk {'print $3'})
fi

RT=0
for i in $usb_clk_usage; do
    if [ $i -ne 0 ]; then
        echo "Some usb usage is not zero, you can use '#cat /proc/cpu/clocks |grep usb' to check"
        RT=$i
        exit $RT
    fi
done

echo "Pass!!! Usb clock had been closed"

