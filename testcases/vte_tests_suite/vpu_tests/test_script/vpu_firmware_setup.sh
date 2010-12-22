#!/bin/sh
#Copyright (C) 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html


TARGET="51"


install()
{
RC=0

FIRMWARE_BASE=${STREAM_PATH}/vpu_firmware/VPU_firmware_release_v$2
FIRMWARE_sur=$2
PLATF=""
case "$1" in
RV_ONLY | rv_only)
	FIRMWARE_sur=14.${FIRMWARE_sur}
  ;;
FULL | full)
	FIRMWARE_sur=1.${FIRMWARE_sur}
  ;;
DIVX_ONLY | divx_only)
	FIRMWARE_sur=15.${FIRMWARE_sur}
	;;
*)
	echo "wrong setting"
	;;
esac

if [ $TARGET = "51" ] || [ $TARGET = "41" ]; then
cp  /lib/firmware/vpu/vpu_fw_imx51.bin /lib/firmware/vpu/vpu_fw_imx51_bk.bin
PLATF=51
elif [ $TARGET = "53" ]; then
cp /lib/firmware/vpu/vpu_fw_imx53.bin /lib/firmware/vpu/vpu_fw_imx53_bk.bin
PLATF=53
fi
if [ ! -z $PLATF ]; then
echo "setup"
cp ${FIRMWARE_BASE}/vpu_fw_imx${PLATF}.bin.${FIRMWARE_sur} /lib/firmware/vpu/vpu_fw_imx${PLATF}.bin || RC=1
fi

if [ $RC -eq 0  ]; then
echo "default vpu firmware backup OK"
return 0
fi
echo "vpu install fail"
exit $RC
}

restore()
{
if [ $TARGET = "51" ] || [ $TARGET = "41" ]; then
cp /lib/firmware/vpu/vpu_fw_imx51_bk.bin /lib/firmware/vpu/vpu_fw_imx51.bin
elif [ $TARGET = "53" ]; then
cp /lib/firmware/vpu/vpu_fw_imx53_bk.bin /lib/firmware/vpu/vpu_fw_imx53.bin
fi
}


platfm.sh
TARGET=$?

if [ $# -lt 3 ];then
echo "The parameters are not enough"
echo "setup INSTALL/RESTORE RV_ONLY/FULL/DIVX_ONLY version"
echo "e.g ./setup INSTALL DIVX_ONLY 4.24"
echo
echo "available VPU firmware version:"
echo `ls -d ${STREAM_PATH}/vpu_firmware/*/`
exit 1
fi

case $1 in
   INSTALL | install)
	 install $2 $3
	 ;;
	 RESTORE | restore)
	 restore
	 ;;
	 *)
	 echo "wrong parameter"
	 ;;
esac

echo "now system need reboot"
reboot
