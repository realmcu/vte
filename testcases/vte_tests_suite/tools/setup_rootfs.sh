#!/bin/bash -x

test_rootfs=/root/st_rootfs
check_emmc_card()
{
  cards=$(ls /sys/devices/platform/*/mmc_host/*/*/boot_info)
        for i in $cards
        do
   emmc_card=$(dirname $i)
        done
}
p_node()
{
 RC=1
 SZ=$(fdisk -lu $1 | grep -m 1 "Units =" | awk '{print $9}')
 SHIFT=$(echo "((10*1024*1024*10)/${SZ}+9)/10" | bc)
 SHIFT=$(expr $SHIFT + 1)
 tmpfile=$(mktemp -p /tmp)
 if [ $? -ne 0 ]; then
  return $RC
 fi
 cat >$tmpfile <<EOF

d
1
d
2
d
3
d
4
n
p
1
$SHIFT


w
EOF

fdisk -u $1 < $tmpfile
if [ $? -eq 0 ]; then
	RC=0
fi

rm -f $tmpfile
return $RC
}

copy_rootfs()
{
  cp -a ${1}/* ${2}/
}

sd_rootfs()
{
	device_node=$2
	uboot=$1
	if [ ! -e $device_node ]; then
		exit 1
	fi
	if [ ! -e /root/$uboot ]; then
		exit 2
	fi
	platfm=$(/mnt/nfs/tools/platfm.sh)
	p_node $device_node || exit -1
	sleep 5
	mkfs.ext3 ${device_node}p1 || exit -1
	dd if=/root/${1} of=${device_node} bs=1024 seek=1 skip=1
	dd if=/root/u-boot-${platfm}-config.bin of=${device_node} bs=1024 seek=768
	/mnt/nfs/tools/platfm.sh
	kernel=uImage_mx$?
	tftp -r ${kernel} -g  10.192.244.7
	dd if=${kernel} of=${device_node} bs=1M seek=1
	bootargs="setenv bootargs \${bootargs} root=${device_node}p1 rootwait"
	bootcmd_emmc="run bootargs_base bootargs_mmc; mmc dev ${3}; mmc read \${loadaddr} 0x800 0x2000;bootm"
	/mnt/nfs/tools/setenv -d ${device_node} bootargs_mmc "$bootargs"
	/mnt/nfs/tools/setenv -d ${device_node} bootcmd_mmc "$bootcmd_emmc"
	/mnt/nfs/tools/setenv -d ${device_node} bootcmd "run bootcmd_mmc"
	tmp=$(mktemp -d)
	mount ${device_node}p1 $tmp
	copy_rootfs ${test_rootfs} $tmp
	umount $tmp
}

emmc_rootfs()
{
        device_node=$2
        uboot=$1
        if [ ! -e $device_node ]; then
                exit 1
        fi
        if [ ! -e /root/$uboot ]; then
                exit 2
        fi
        platfm=$(/mnt/nfs/tools/platfm.sh)
        p_node $device_node || exit -1
        sleep 5
        mkfs.ext3 ${device_node}p1 || exit -1
        emmc_card=""
        check_emmc_card
        echo 1 > ${emmc_card}/boot_config || exit 1
        cat ${emmc_card}/boot_info
        dd if=/root/${1} of=${device_node} bs=1024 seek=1 skip=1
        echo 0 > ${emmc_card}/boot_config || exit 1
	cat ${emmc_card}/boot_info
        dd if=/root/u-boot-${platfm}-config.bin of=${device_node} bs=1024 seek=768
        echo 8 > ${emmc_card}/boot_config || exit 1
	cat ${emmc_card}/boot_info
        /mnt/nfs/tools/platfm.sh
        kernel=uImage_mx$?
        tftp -r ${kernel} -g  10.192.244.7
        dd if=${kernel} of=${device_node} bs=1M seek=1
        bootargs="setenv bootargs \${bootargs} root=${device_node}p1 rootwait"
        bootcmd_emmc="run bootargs_base bootargs_mmc; mmc dev ${3}; mmc read \${loadaddr} 0x800 0x2000;bootm"
        /mnt/nfs/tools/setenv -d ${device_node} bootargs_mmc "$bootargs"
        /mnt/nfs/tools/setenv -d ${device_node} bootcmd_mmc "$bootcmd_emmc"
        /mnt/nfs/tools/setenv -d ${device_node} bootcmd "run bootcmd_mmc"
        tmp=$(mktemp -d)
        mount ${device_node}p1 $tmp
        copy_rootfs ${test_rootfs} $tmp
        umount $tmp
}

sata_rootfs()
{
	device_node=$2
	uboot=$1
	if [ ! -e $device_node ]; then
		exit 1
	fi
	if [ ! -e /root/$uboot ]; then
		exit 2
	fi
	platfm=$(/mnt/nfs/tools/platfm.sh)
	p_node $device_node || exit -1
	mkfs.ext3 ${device_node}1 || exit -1
    	dd if=/root/${1} of=${device_node} bs=1024 seek=1 skip=1
	dd if=/root/u-boot-${platfm}-config.bin of=${device_node} bs=1024 seek=768
	/mnt/nfs/tools/platfm.sh
	kernel=uImage_mx$?
	tftp -r ${kernel} -g  10.192.244.7
	dd if=${kernel} of=${device_node} bs=1M seek=1
	bootargs="setenv bootargs \${bootargs} root=${device_node}1 rootwait"
	bootcmd_saenv="run bootargs_base bootargs_sata;bootp \${loadaddr} \${tftpaddr}:${kernel};bootm"
	/mnt/nfs/tools/setenv -d ${device_node} bootargs_sata "$bootargs"
	/mnt/nfs/tools/setenv -d ${device_node} bootcmd_sata "$bootcmd_saenv"
	/mnt/nfs/tools/setenv -d ${device_node} bootcmd "run bootcmd_sata;bootm"
	tmp=$(mktemp -d)
	mount ${device_node}1 $tmp
	copy_rootfs ${test_rootfs} $tmp
	umount $tmp
}

spinor_rootfs()
{
	flash_eraseall /dev/mtd0
	dd if=/root/${1} of=/dev/mtd0 bs=512
	#/mnt/nfs/tools/platfm.sh
	#kernel=uImage_mx$?
	#tftp -r ${kernel} -g  10.192.244.7
	#flash_erase /dev/mtd2 0 0
	#dd if=${kernel} of=/dev/mtd2
	#echo "please manually set in uboot env to boot kernel from spi nor"
	#echo "setenv bootcmd 'run bootcmd_spi'"
	#platfm=$(/mnt/nfs/tools/platfm.sh)
	#flash_erase /dev/mtd0 262144 0
	#dd if=/root/u-boot-${platfm}-config.bin of=/dev/mtd0 bs=1024 seek=256
	#/mnt/nfs/tools/setenv -d /dev/mtd0  -u -e -o 262144 bootcmd "res"
}


pnor_rootfs()
{
	platfm=$(/mnt/nfs/tools/platfm.sh)
	#flash_erase /dev/mtd0 0 0
	flash_eraseall /dev/mtd0
	dd if=/root/${1} of=/dev/mtd0
	/mnt/nfs/tools/platfm.sh
	kernel=uImage_mx$?
	tftp -r ${kernel} -g  10.192.244.7
	#flash_erase /dev/mtd2 0 0
	flash_eraseall /dev/mtd2
	dd if=${kernel} of=/dev/mtd2
	#flash_erase /dev/mtd3 0 0
	flash_eraseall /dev/mtd3
	ubiformat /dev/mtd3
	ubiattach /dev/ubi_ctrl -m 3
	ubimkvol /dev/ubi0 -N rootfs -s 26214400
	mkdir -p /mnt/mtd3
	mount -t ubifs ubi0:rootfs /mnt/mtd3
	copy_rootfs ${test_rootfs} /mnt/mtd3
	umount /mnt/mtd3
}


nand_rootfs()
{
	platfm=$(/mnt/nfs/tools/platfm.sh)
	#flash_erase /dev/mtd0 0 0
	flash_eraseall /dev/mtd0
	kobs-ng init -v $1
	#nandwrite -p /dev/mtd0 -s 0x100000 /root/${1}
	/mnt/nfs/tools/platfm.sh
	kernel=uImage_mx$?
	tftp -r ${kernel} -g  10.192.244.7
	#flash_erase /dev/mtd1 0 0
	flash_eraseall /dev/mtd0
	nandwrite -p /dev/mtd1 ${kernel}
	#flash_erase /dev/mtd2 0 0
	flash_eraseall /dev/mtd2
	ubiformat /dev/mtd2
	ubiattach /dev/ubi_ctrl -m 2
	ubimkvol /dev/ubi0 -N rootfs -s 67108864
	mkdir -p /mnt/mtd2
	mount -t ubifs ubi0:rootfs /mnt/mtd2
	copy_rootfs ${test_rootfs} /mnt/mtd2
	umount /mnt/mtd2
}
#main

case "$1" in
"SD0")
	sd_rootfs $2 $3 0 || exit 1
	;;
"SD1")
	sd_rootfs $2 $3 1 || exit 1
	;;
"SD2")
	sd_rootfs $2 $3 2  || exit 1
	;;
"eMMC")
	emmc_rootfs $2 $3 3 || exit 1
	;;
"NAND")
	nand_rootfs $2 || exit 1
	;;
"SPINOR")
	spinor_rootfs $2 || exit 1
	;;
"PNOR")
	pnor_rootfs $2 || exit 1
	;;
"SATA")
	sata_rootfs $2 $3 2 || exit 1
	;;
*)
	echo "$0 <device> <uboot Image> [device node]" 
	echo "device can be:"
	echo "-- SDx/eMMC/NAND/SPINOR/PNOR/SATA"
	echo "device node can be:"
	echo "-- /dev/mmcblk0/1/2"
	echo "-- /dev/sda"
	exit 0
	;;
esac

echo "update sucess"
echo "now reboot to start test"
reboot
