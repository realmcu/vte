#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
echo "UART stty cmd set test begin..."
result=0
for name in ttymxc0 ttymxc1 ttymxc2
do
  echo "Device name is $name"
  stty -F /dev/$name 115200
  rate=`stty -F /dev/$name | grep "speed" | awk '{print $2}'`
  if [ "$rate" -eq "115200" ]
  then echo "/dev/$name set successfully!"
  else
    echo "/dev/$name set fail!"
    result=1
  fi
done

echo "=============================================="
if [ "$result" -eq "0" ]
then echo "    stty cmd test result: TPASS!"
else echo "    stty cmd test result: TFAIL!"
fi
echo "=============================================="
