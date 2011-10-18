#!/bin/bash
#Copyright 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

#filename: prepare.sh
#Author                          Date        Description of Changes
#-------------------------   ------------    -------------------------------------------
#Victor Cui                   31/07/2008     Prepare for hardware connection and filesystem
#Spring Zhang                 20/04/2009     add return value in preparation 
#                                            used by Linux plugin
#Spring Zhang                 17/06/2009     add prepare vpu for 37&51 
#Victor Cui                   09/07/2009     add -M(mount) option
#Spring Zhang                 03/03/2010     change interpreter from sh to bash
#                                            for 'let' command
#Spring Zhang                 29/04/2010     MX53:change SD partition 1 from cylinder 80
#Spring Zhang                 11/05/2010     MX53:change SD partition 1 
#                                            from relative cylinders
#Spring Zhang                 23/06/2010     kernel change: get right block device size
#Spring Zhang                 13/07/2010     Add MX53 SATA support
#
#notes:
# -I insert modules(SD, ATA, V4L, BT, USBH or ALL)          
# 	eg. ./auto_prepare.sh -I SD
# -R remove modules(SD, ATA, V4L, BT, USBH or ALL)          
#	eg. ./auto_prepare.sh -R SD
# -M mount devices(NAND, NOR, SD, ATA, USBH)          
#	eg. ./auto_prepare.sh -M SD
# -P partition, format and mount device to /mnt/~   
#    the devices may be NAND, SD, ATA, USBH or ALL
#	eg. ./auto_prepare.sh -P SD
# -C copy vte test bin to board                      
#	eg. ./auto_prepare.sh -C /mnt/flc

prepare_platform()
{
	find=0;
        
	#change the judge method because of 31 and 32 cpuinfo "Hardware" item 
	# "Hardware        : Freescale MX31/MX32 3-Stack Board"
	
	# add a blank to grep "*.*"
	# Revision        : 35120
	
        find=`cat /proc/cpuinfo | grep "Revision" | grep " 25.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		platform=MX25;
		ata_dev_point=sda;
		vte_name=imx25stack-vte-test;
	fi
	
	#find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX31" | wc -l`;
        find=`cat /proc/cpuinfo | grep "Revision" | grep " 31.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		platform=MX31;
		#ata_dev_point=hda;
		ata_dev_point=sda;
	        vte_name=imx31stack-vte-test;
	fi
	
	#find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX32" | wc -l`;
	find=`cat /proc/cpuinfo | grep "Revision" | grep " 32.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		platform=MX32;
		ata_dev_point=hda;
	        vte_name=mx32_vte;
	fi
	
	#find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX35" | wc -l`;
        find=`cat /proc/cpuinfo | grep "Revision" | grep " 35.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		platform=MX35;
		ata_dev_point=sda;
		vte_name=imx35stack-vte-test;
	fi

	#find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX37" | wc -l`;
	find=`cat /proc/cpuinfo | grep "Revision" | grep " 37.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		platform=MX37;
		ata_dev_point=sda;
		vte_name=imx37stack-vte-test;
	fi

	#find=`cat /proc/cpuinfo | grep "Hardware" | grep "MX51" | wc -l`;
	find=`cat /proc/cpuinfo | grep "Revision" | grep " 51.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		platform=MX51;
		ata_dev_point=sda;
		vte_name=imx51stack-vte-test;
	fi

	find=`cat /proc/cpuinfo | grep "Revision" | grep " 53.*" | wc -l`;
	if [ $find -eq 1 ]; then
   		platform=MX53
		ata_dev_point=sda
        if [ -e /dev/sda ]; then
            ata_dev_point=/dev/sdb
        fi
	fi

	board=$(platfm.sh)

	echo $platform;
	echo $ata_dev_point;
	echo $vte_name;
}

modules_buildin()
{
	# sd
	ls /sys/block/ | grep mmcblk;
	if [ $? -eq 0 ]; then
		sd_buildin=1;
		sd_dev_point=`ls /sys/block/ | grep mmcblk`;
		echo "sd/mmc device has been detected! do not need to insmod modules! the device point is $sd_dev_point";
	fi
	# ata/usbh	
        # ata device: size > 33554432 (16G); usbh device: size <= 33554432 (16G)	
	for line in `ls /sys/block | grep sdd*`
	do
		#echo $line
		if [ -e /sys/block/$line/device/block:$line/size ]; then
			size=`cat /sys/block/$line/device/block:$line/size`
		else 
			size=`cat /sys/block/$line/device/block/$line/size`
		fi
	      	if [ $size -gt 33554432 ]; then
			ata_buildin=1;
			ata_dev_point=$line;
			echo "ata has been detected! do not need to insmod modules! the device point is $ata_dev_point";
		else
			usbh_buildin=1;
			usbh_dev_point=$line;
			dev_node_usbh=$usbh_dev_point;
			echo "usbh has been detected! do not need to insmod modules! the device point is $usbh_dev_point";
		fi
        done     
}

insmod_SD()
{
modules_buildin;
if [ $sd_buildin -eq 0 ]; then

	echo "sd: prepare insmod modules";
	find=0;

	find=`find /lib/modules/$sys_name -name mmc_core.ko | wc -l`;
	lsmod | grep mmc_core;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe mmc_core;
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name mmc_block.ko | wc -l`;
	lsmod | grep mmc_block;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe mmc_block;
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name mx_sdhci.ko | wc -l`;
	lsmod | grep mx_sdhci
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe mx_sdhci;
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name mxc_mmc.ko | wc -l`;
	lsmod | grep mxc_mmc
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe mxc_mmc;
		sleep 2;
	fi

	find=0;
	find=`ls /dev/mmc* | wc -l`;
	if [ $find -ge 1 ]; then
		echo "sd: insmod modules success!";
	else 
		echo "sd: insmod modules fail!";
	fi	
fi
}

rmmod_SD()
{
	echo "sd: prepare rmmod modules";

	lsmod | grep mmc_core;
	if [ $? -eq 0 ]; then
		modprobe -r mmc_core;
		sleep 2
	fi

	lsmod | grep mmc_block;
	if [ $? -eq 0 ]; then
		modprobe -r mmc_block;
		sleep 2;
	fi

	lsmod | grep mx_sdhci;
	if [ $? -eq 0 ]; then
		modprobe -r mx_sdhci;
		sleep 2;
	fi 

	lsmod | grep mxc_mmc;
	if [ $? -eq 0 ]; then
		modprobe -r mxc_mmc;
		sleep 2;
	fi 

	echo "sd: rmmod modules success!"
}

insmod_ATA()
{	
modules_buildin;
if [ $ata_buildin -eq 0 ]; then

	echo "ata: prepare insmod modules";
	find=0;
	line=0;
	line_num1=0;
	line_num2=0;

	platform="MX31";
	prepare_platform;	

	line_num1=`cat /proc/partitions | wc -l`;

	if [ $platform = "MX32" ]; then
		find=`find /lib/modules/$sys_name -name ide-core.ko | wc -l`;
		lsmod | grep ide-core;
		if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        		modprobe ide-core;
			sleep 2;
		fi

		find=`find /lib/modules/$sys_name -name ide-disk.ko | wc -l`;
		lsmod | grep ide-disk;
		if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        		modprobe ide-disk;
			sleep 2;
		fi

		find=`find /lib/modules/$sys_name -name ide-generic.ko | wc -l`;
		lsmod | grep ide-generic;
		if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        		modprobe ide-generic;
			sleep 2;
		fi

		find=`find /lib/modules/$sys_name -name mxc_ide.ko | wc -l`;
		lsmod | grep mxc_ide;
		if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        		modprobe mxc_ide;
			sleep 2;
		fi

		if [ -e /dev/hda ]; then
	 		echo "ata: insmod modules success!";
		else 
			echo "ata: insmod modules fail!";
		fi
	fi

	if [ $platform = "MX31" ] || [ $platform = "MX35" ] || [ $platform = "MX37" ] || [ $platform = "MX51" ]; then
		find=`find /lib/modules/$sys_name -name pata_fsl.ko | wc -l`;
		lsmod | grep pata_fsl;
		if [ $? -ne 0 ]; then
			if [ $find -eq 1 ]; then
        			modprobe pata_fsl;
				sleep 2;

				line_num2=`cat /proc/partitions | wc -l`;

				if [ $line_num1 -lt $line_num2 ]; then
					echo "ata: insmod modules success!";
					let line=$line_num1-$line_num2;
					dev_node_ata=`cat /proc/partitions | tail -n $line | head -n 1 | awk '{print $4}'`;
					echo "the device node is:$dev_node_ata";
					ata_dev_point=$dev_node_ata;
				else
					echo "ata: insmod modules fail!";
				fi
			fi
		else
			echo "ata: the modules have existed!"
		fi
	fi

    #SATA
    if [ "$platform" = "MX53" ]; then
        find=`find /lib/modules/$sys_name -name ahci_platform.ko | wc -l`
        lsmod |grep ahci_platform
        if [ $? -ne 0 ]; then
            if [ $find -eq 1 ]; then
                modprobe ahci_platform
                sleep 2

                line_num2=`cat /proc/partitions | wc -l`;

				if [ $line_num1 -lt $line_num2 ]; then
					echo "ata: insmod modules success!";
					let line=$line_num1-$line_num2;
					dev_node_ata=`cat /proc/partitions | tail -n $line | head -n 1 | awk '{print $4}'`;
					echo "the device node is:$dev_node_ata";
					ata_dev_point=$dev_node_ata;
				else
					echo "ata: insmod modules fail!";
				fi
            fi
        else
            echo "ata: the modules have existed!"
        fi
    fi

fi
}

rmmod_ATA()
{
	echo "ata: prepare rmmod modules";
	platform="MX31";
	prepare_platform;

	if [ $platform = "MX32" ]; then
		lsmod | grep ide-core;
		if [ $? -eq 0 ]; then
        		modprobe -r ide-core;
			sleep 2;
		fi

		lsmod | grep ide-disk;
		if [ $? -eq 0 ]; then
        		modprobe -r ide-disk;
			sleep 2;
		fi

		lsmod | grep ide-generic;
		if [ $? -eq 0 ]; then
        		modprobe -r ide-generic;
			sleep 2;
		fi

		lsmod | grep mxc_ide;
		if [ $? -eq 0 ]; then
        		modprobe -r mxc_ide;
			sleep 2;
		fi

		echo "ata: rmmod modules success!"
	fi

	if [ $platform = "MX31" ] || [ $platform = "MX35" ] || [ $platform = "MX37" ] || [ $platform = "MX51" ]; then
		lsmod | grep pata_fsl;
        if [ $? -eq 0 ]; then
            modprobe -r pata_fsl;
			sleep 2;
			echo "ata: rmmod modules success!"
		else
			echo "ata: the modules are not exist!"
		fi
	fi

    if [ "$platform" = "MX53" ]; then
        lsmod | grep ahci_platform
        if [ $? -eq 0 ]; then
            modprobe ahci_platform -r
            sleep 2
            echo "ata: rmmod modules success!"
        else
            echo "ata: the modules does not exist!"
        fi
    fi
}

insmod_V4L()
{
	prepare_platform;
        echo "v4l: prepare insmod modules";
	find=0;
    #turn on backlight
    echo -e "\033[9;0]" > /dev/tty0
	#echo "enalbe tvout module"
	#echo U:720x576i-50 > /sys/class/graphics/fb0/mode
	find=`find /lib/modules/$sys_name -name ipu_prp_enc.ko | wc -l`;
	lsmod | grep ipu_prp_enc;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe ipu_prp_enc;
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name ipu_prp_vf_sdc.ko | wc -l`;
	lsmod | grep ipu_prp_vf_sdc;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe ipu_prp_vf_sdc;
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name ipu_prp_vf_sdc_bg.ko | wc -l`;
	lsmod | grep ipu_prp_vf_sdc_bg;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe ipu_prp_vf_sdc_bg;
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name ipu_still.ko | wc -l`;
	lsmod | grep ipu_still;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe ipu_still;
		sleep 2;
	fi

	# Mx25
        find=`find /lib/modules/$sys_name -name fsl_csi.ko | wc -l`;
	lsmod | grep fsl_csi;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe fsl_csi;
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name csi_v4l2_capture.ko | wc -l`;
	lsmod | grep csi_v4l2_capture;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe csi_v4l2_capture;
		sleep 2;
	fi
	#

    camera_load=0

	find=`find /lib/modules/$sys_name -name ov2640_camera.ko | wc -l`;
	lsmod | grep ov2640_camera;
	if [ $? -ne 0 ] && [ $find -eq 1 ] && [ $camera_load -eq 0  ]; then
        	modprobe ov2640_camera;
		sleep 2;
		camera_load=1
	fi

	find=`find /lib/modules/$sys_name -name ov3640_camera.ko | wc -l`;
	lsmod | grep ov3640_camera;
	if [ $? -ne 0 ] && [ $find -eq 1 ] && [ $platform == MX51 ] && [ $camera_load -eq 0  ]; then
        	modprobe ov3640_camera;
		camera_load=1
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name ov5642_camera.ko | wc -l`;
	lsmod | grep ov5642_camera;
	if [ $? -ne 0 ] && [ $find -eq 1 ] && [ $camera_load -eq 0 ]; then
        	modprobe ov5642_camera;
		camera_load=1
		sleep 2;
	fi

	find=`find /lib/modules/$sys_name -name mxc_v4l2_capture.ko | wc -l`;
	lsmod | grep mxc_v4l2_capture;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe mxc_v4l2_capture;
		sleep 2;
	fi

    # fix it, /dev/video0 existing can not know ov2640/3640 inserted.
	if [ -e /dev/video0 ]; then
	 	echo "v4l: insmod modules success!"
	else 
		echo "v4l: insmod modules fail!"
	fi
}

rmmod_V4L()
{
	echo "v4l: prepare rmmod modules";
	
	lsmod | grep ov2640_camera;
        if [ $? -eq 0 ]; then
        	modprobe -r ov2640_camera;
		sleep 2;
	fi

	lsmod | grep ov3640_camera;
        if [ $? -eq 0 ]; then
        	modprobe -r ov3640_camera;
		sleep 2;
	fi

	lsmod | grep ov5642_camera;
        if [ $? -eq 0 ]; then
        	modprobe -r ov5642_camera;
		sleep 2;
	fi

	lsmod | grep mxc_v4l2_capture;
        if [ $? -eq 0 ]; then
        	modprobe -r mxc_v4l2_capture;
		sleep 2;
	fi

	lsmod | grep ipu_prp_enc;
        if [ $? -eq 0 ]; then
        	modprobe -r ipu_prp_enc;
		sleep 2;
	fi

	lsmod | grep ipu_prp_vf_sdc;
        if [ $? -eq 0 ]; then
        	modprobe -r ipu_prp_vf_sdc;
		sleep 2;
	fi

	lsmod | grep ipu_prp_vf_sdc_bg;
        if [ $? -eq 0 ]; then
        	modprobe -r ipu_prp_vf_sdc_bg;
		sleep 2;
	fi

	lsmod | grep ipu_still;
        if [ $? -eq 0 ]; then
        	modprobe -r ipu_still;
		sleep 2;
	fi


	#Mx25
	lsmod | grep fsl_csi;
        if [ $? -eq 0 ]; then
        	modprobe -r fsl_csi;
		sleep 2;
	fi
	
	lsmod | grep csi_v4l2_capture;
        if [ $? -eq 0 ]; then
        	modprobe -r csi_v4l2_capture;
		sleep 2;
	fi
	#

	echo "v4l: rmmod modules success!";

}

insmod_BT()
{
	echo "bt: prepare insmod modules";
	find=0;

	find=`find /lib/modules/$sys_name -name mxc_bt.ko | wc -l`;
	lsmod | grep mxc_bt;
	if [ $? -ne 0 ] && [ $find -eq 1 ]; then
        	modprobe mxc_bt;
		sleep 2;
	fi
	
	echo "bt: insmod modules success!";
}

rmmod_BT()
{
	echo "bt: prepare rmmod modules";

	lsmod | grep mxc_bt;
	if [ $? -eq 0 ]; then
        	modprobe -r mxc_bt;
		sleep 2;
		echo "bt: rmmod modules success!";
	else
		echo "bt: the modules are not exist!";
	fi
}

insmod_USBH()
{
modules_buildin;
if [ $usbh_buildin -eq 0 ]; then

	echo "usbh: prepare insmod modules";
	find=0;
	line=0;
	line_num1=0;
	line_num2=0;

	line_num1=`cat /proc/partitions | wc -l`;

	find=`find /lib/modules/$sys_name -name ehci-hcd.ko | wc -l`;
	lsmod | grep ehci*;
	if [ $? -ne 0 ]; then
	       	if [ $find -eq 1 ]; then
        		modprobe ehci-hcd;
			sleep 10;

			line_num2=`cat /proc/partitions | wc -l`;

			if [ $line_num1 -lt $line_num2 ]; then
       				echo "usbh: insmod modules success!";
				let line=$line_num1-$line_num2;
				dev_node_usbh=`cat /proc/partitions | tail -n $line | head -n 1 | awk '{print $4}'`;
				echo "the device node is:$dev_node_usbh";
				usbh_dev_point=$dev_node_usbh;
			else
        			echo "usbh: insmod modules fail!";
			fi
		fi
	else
		echo "usbh: the modules have existed!";
	fi
fi	
}

rmmod_USBH()
{
	echo "usbh: prepare rmmod modules";

	lsmod | grep ehci*;
	if [ $? -eq 0 ]; then
        	modprobe -r ehci-hcd;
		sleep 2;
		echo "usbh: rmmod modules success!";
	else
		echo "usbh: the modules are not exist!";
	fi
}

prepare_nand()
{
	return_value=0;
	mtd_nand_number=0;
	find=0;
	mount_point=/mnt/flc;

        prepare_platform;
	
	mtd_nand_number=`cat /proc/mtd | wc -l`;
	echo $mtd_nand_number;
	let mtd_nand_number=$mtd_nand_number-2;
	echo $mtd_nand_number;

	confirm=`cat /proc/mtd | grep "mtd${mtd_nand_number}" | grep "nand.userfs" | wc -l`;
	if [ $confirm -eq 1 ]; then
		echo "It is right!";
	else
		echo "not found right mtd number!";
        return 1
	fi

	# mount
	find=`cat /proc/mounts | grep "mtdblock$mtd_nand_number" | wc -l`;
	if [ $find -eq 1 ]; then
		umount /dev/mtdblock$mtd_nand_number;
		if [ $? -eq 0 ]; then
			echo "umount /dev/mtdblock$mtd_nand_number success!";
			sleep 1;
		else
			echo "umount /dev/mtdblock$mtd_nand_number fail!";
			sleep 1;
		fi	
		
	fi

	flash_eraseall /dev/mtd$mtd_nand_number;
	sleep 2;

	if [ ! -d $mount_point ]; then
		mkdir $mount_point;
	fi

	mount -t jffs2 /dev/mtdblock${mtd_nand_number} $mount_point;

	if [ $? -eq 0 ]; then
		echo "the /dev/mtdblock${mtd_nand_number} mount to $mount_point success!";
		sleep 3;
        return 0
	else
		echo "the /dev/mtdblock${mtd_nand_number} mount to $mount_point fail!"
        return 1
	fi
}

prepare_nor()
{
	return_value=0;
	mtd_nor_number=0;
	find=0;
	mount_point=/mnt/flb;

        prepare_platform;

	if [ $platform = MX35 ]; then
		mtd_nor_number=2;

		# mount
		find=`cat /proc/mounts | grep "mtdblock$mtd_nor_number" | wc -l`;
		if [ $find -eq 1 ]; then
			umount /dev/mtdblock$mtd_nor_number;
			if [ $? -eq 0 ]; then
				echo "umount /dev/mtdblock$mtd_nor_number success!";
				sleep 1;
			else
				echo "umount /dev/mtdblock$mtd_nor_number fail!";
				sleep 1;
			fi	
		fi

		flash_eraseall /dev/mtd$mtd_nor_number;
		sleep 2;

		if [ ! -d $mount_point ]; then
			mkdir $mount_point;
		fi

		mount -t jffs2 /dev/mtdblock${mtd_nor_number} $mount_point;

		if [ $? -eq 0 ]; then
			echo "the /dev/mtdblock${mtd_nor_number} mount to $mount_point success!";
			sleep 3;
		else
			echo "the /dev/mtdblock${mtd_nor_number} mount to $mount_point fail!"
            return 1
		fi
	else
		echo "Only Mx35 platform has nor device"
        return 1
	fi	

    return 0
}


prepare_sd()
{
	device_total=0;

	prepare_platform;
        insmod_SD;

	if [ -e format_command_sd ]; then
		rm format_command_sd;
        fi

        device_total=`ls /dev/ | grep "mmcblk" | wc -l`;
	if [ $device_total -gt 0 ]; then
		# umount devices
		cat /proc/mounts | grep "mmcblk.p*" | awk '{print $1}'| while read line
		do
  			echo $line;
  			umount $line;
			if [ $? -eq 0 ]; then
				echo "umount $line succeed!"
			else
				echo "umount $line fail!"
			fi
		done
		
                
		# get the number of cyclinders
		number_cyclinders=0;
		third_cyclinders=0;
                middle_cyclinders=0;

		number_cyclinders=`fdisk -l /dev/mmcblk0 | grep "head" | awk -F , '{print $3}' | awk '{print $1}'`;
		echo "number_cyclinders=$number_cyclinders";
		let third_cyclinders=$number_cyclinders/3;
		let middle_cyclinders=$third_cyclinders*2;
        echo "middle_cyclinders=$middle_cyclinders";

		# create the fdisk command line file
		echo "" >> format_command_sd;
		echo "d" >> format_command_sd;
		echo "1" >> format_command_sd;
		echo "d" >> format_command_sd;
		echo "2" >> format_command_sd;
		echo "d" >> format_command_sd;
		echo "3" >> format_command_sd;
		echo "d" >> format_command_sd;
		echo "4" >> format_command_sd;
		echo "" >> format_command_sd;

		echo "n" >> format_command_sd;
		echo "p" >> format_command_sd;
		echo "1" >> format_command_sd;
        #use $3 instead $5 to get MB size directly
        total_size=`fdisk -l /dev/mmcblk0 | sed -n '/Disk \/dev\/mmcblk0/p'| awk '{ print $3 }'`
		#reserve 16M for uboot and uboot config, 16 is MB
		first_par_start=`expr 16 \* $number_cyclinders / $total_size`
		echo "$first_par_start" >> format_command_sd;
		echo "$middle_cyclinders" >> format_command_sd;

		echo "n" >> format_command_sd;
		echo "p" >> format_command_sd;
		echo "2" >> format_command_sd;
		second_par_start=`expr $middle_cyclinders + 1`
		echo "$second_par_start" >> format_command_sd;
		echo "" >> format_command_sd;

		echo "" >> format_command_sd;

		echo "w" >> format_command_sd;
		
		sleep 1;

		# using fdisk to re_partition sd/mmc
		fdisk /dev/mmcblk0 < ./format_command_sd;
		sleep 1;

		if [  $? -eq 0 ]; then
			sleep 3;
			echo "partition sd succeed! then format";
   			
			# ensure umount sd/mmc device
			cat /proc/mounts | grep "mmcblk.p*" | awk '{print $1}'| while read line
			do
  				echo $line;
  				umount $line;
				if [ $? -eq 0 ]; then
					echo "umount $line succeed!"
				else
					echo "umount $line fail!"
				fi
			done
			
			/sbin/mkdosfs /dev/mmcblk0p1;
			if [ $? -eq 0 ]; then
				echo "format partition 1 succeed!"
			else
				echo "format partition 1 fail!";
			fi
			sleep 1;
  			/sbin/mkdosfs /dev/mmcblk0p2;
                        if [ $? -eq 0 ]; then
				echo "format partition 2 succeed!";
			else
				echo "format partition 2 fail!"
			fi
			sleep 1;
		else
			echo "partition sd fail!";
		fi

		# mount
		if [ ! -d /mnt/mmcblk0p1 ]; then
			mkdir /mnt/mmcblk0p1;
		fi
		if [ ! -d /mnt/mmcblk0p2 ]; then
			mkdir /mnt/mmcblk0p2;
		fi

		mount -t vfat /dev/mmcblk0p1 /mnt/mmcblk0p1;
		if [ $? -eq 0 ]; then
			echo "mount partition 1 succeed!";
		else
			echo "mount partition 1 fail!";
            return 1
		fi
		sleep 3;

		mount -t vfat /dev/mmcblk0p2 /mnt/mmcblk0p2;
		if [ $? -eq 0 ]; then
			echo "mount partition 2 succeed!";
		else
			echo "mount partition 2 fail!";
		fi
		sleep 3;		
	else
		echo "not found the sd device!";
        return 1
	fi	

    return 0
}

prepare_ata()
{
	device_total=0;
      
    prepare_platform;
	insmod_ATA;
	
	if [ -e format_command_ata ]; then
		rm format_command_ata;
        fi


    device_total=`ls /dev/ | grep $ata_dev_point | wc -l`;
	if [ $device_total -gt 0 ]; then
		# umount devices
		cat /proc/mounts | grep "$ata_dev_point" | awk '{print $1}'| while read line
		do
  			echo $line;
  			umount $line;
			if [ $? -eq 0 ]; then
				echo "umount $line succeed!";
			else
				echo "umount $line fail!";
			fi
		done
		
                
		# get the number of cyclinders
		number_cyclinders=0;
	        fifth_cyclinders=0;
                middle1_cyclinders=0;
		middle2_cyclinders=0;

		number_cyclinders=`fdisk -l /dev/$ata_dev_point | grep "head" | awk -F , '{print $3}' | awk '{print $1}'`;
		echo "number_cyclinders=$number_cyclinders";
		let fifth_cyclinders=$number_cyclinders/5;
		let middle1_cyclinders=$fifth_cyclinders*2;
		let middle2_cyclinders=$fifth_cyclinders*4;
		echo "middle1_cyclinders=$middle1_cyclinders";
		echo "middle2_cyclinders=$middle2_cyclinders";

		# create the fdisk command line file
		echo "" >> format_command_ata;
		echo "d" >> format_command_ata;
		echo "1" >> format_command_ata;
		echo "d" >> format_command_ata;
		echo "2" >> format_command_ata;
		echo "d" >> format_command_ata;
		echo "3" >> format_command_ata;
		echo "d" >> format_command_ata;
		echo "4" >> format_command_ata;
		echo "" >> format_command_ata;

		echo "n" >> format_command_ata;
		echo "p" >> format_command_ata;
		echo "1" >> format_command_ata;
		echo "" >> format_command_ata;
		echo "$middle1_cyclinders" >> format_command_ata;

		echo "n" >> format_command_ata;
		echo "p" >> format_command_ata;
		echo "2" >> format_command_ata;
		echo "" >> format_command_ata;
                echo "$middle2_cyclinders" >> format_command_ata;

		echo "n" >> format_command_ata;
		echo "p" >> format_command_ata;
		echo "3" >> format_command_ata;
		echo "" >> format_command_ata;
                echo "" >> format_command_ata;

		echo "" >> format_command_ata;

		echo "w" >> format_command_ata;
		
		sleep 1;

		# using fdisk to re_partition sd/mmc
		fdisk /dev/$ata_dev_point < ./format_command_ata;
		sleep 1;

		if [  $? -eq 0 ]; then
			sleep 3;
   			/sbin/mkdosfs /dev/${ata_dev_point}1;
			if [ $? -eq 0 ]; then
				echo "format ata partition 1 succeed!";
			else 
				echo "format ata partition 1 fail!";
			fi
			sleep 1;

  			/sbin/mkdosfs /dev/${ata_dev_point}2;
                        	if [ $? -eq 0 ]; then
				echo "format ata partition 2 succeed!"
			else 
				echo "format ata partition 2 fail!"
			fi
			sleep 1;

			/sbin/mkdosfs /dev/${ata_dev_point}3;
                        	if [ $? -eq 0 ]; then
				echo "format ata partition 3 succeed!";
			else 
				echo "format ata partition 3 fail!";
			fi
			sleep 1;

		else 
			echo "format ata partition fail!";
		fi

		# mount
		#if [ ! -d /mnt/${ata_dev_point}1 ]; then
		#	mkdir /mnt/${ata_dev_point}1;
		#fi
		#if [ ! -d /mnt/${ata_dev_point}2 ]; then
		#	mkdir /mnt/${ata_dev_point}2;
		#fi
		#if [ ! -d /mnt/${ata_dev_point}3 ]; then
		#	mkdir /mnt/${ata_dev_point}3;
		#fi
	
        	# mount
		if [ ! -d /mnt/ata1 ]; then
			mkdir /mnt/ata1;
		fi
		if [ ! -d /mnt/ata2 ]; then
			mkdir /mnt/ata2;
		fi
		if [ ! -d /mnt/ata3 ]; then
			mkdir /mnt/ata3;
		fi

		#mount -t vfat /dev/${ata_dev_point}1 /mnt/${ata_dev_point}1;
		mount -t vfat /dev/${ata_dev_point}1 /mnt/ata1;
		if [ $? -eq 0 ]; then
			echo "mount ata partition 1 succeed!";
		else 
			echo "mount ata partition 1 fail!";
            return 1
		fi
		sleep 3;

		#mount -t vfat /dev/${ata_dev_point}2 /mnt/${ata_dev_point}2;
		mount -t vfat /dev/${ata_dev_point}2 /mnt/ata2;
		if [ $? -eq 0 ]; then
			echo "mount ata partition 2 succeed!";
		else 
			echo "mount ata partition 2 fail!";
		fi
		sleep 3;
                
		#mount -t vfat /dev/${ata_dev_point}3 /mnt/${ata_dev_point}3;
		mount -t vfat /dev/${ata_dev_point}3 /mnt/ata3;
		if [ $? -eq 0 ]; then
			echo "mount ata partition 3 succeed!";
		else 
			echo "mount ata partition 3 fail!";
		fi
		sleep 3;		
	else
		echo "not found the ata device!";
        return 1
	fi	

    return 0
}

prepare_usbh()
{
	find=0;
	mount_point=/mnt/msc;


	prepare_platform;

        lsmod | grep ehci*;
	if [ $? -eq 0 ]; then
		modprobe -r ehci_hcd;
		sleep 2;
	fi

	insmod_USBH;

	if [ -e format_command_usbh ]; then
		rm format_command_usbh;
	fi
	

	find=`cat /proc/mounts | grep "$mount_point" | wc -l`;
	if [ $find -eq 1 ]; then
		umount $mount_point;
	fi

	# create the fdisk command line file
	echo "" >> format_command_usbh;
	echo "d" >> format_command_usbh;
	echo "1" >> format_command_usbh;
	echo "d" >> format_command_usbh;
	echo "2" >> format_command_usbh;
	echo "d" >> format_command_usbh;
	echo "3" >> format_command_usbh;
	echo "d" >> format_command_usbh;
	echo "4" >> format_command_usbh;
	echo "" >> format_command_usbh;

	echo "n" >> format_command_usbh;
	echo "p" >> format_command_usbh;
	echo "1" >> format_command_usbh;
	echo "" >> format_command_usbh;
	echo "" >> format_command_usbh;

	echo "w" >> format_command_usbh;
		
	sleep 1;

	# using fdisk to re_partition usbh
	fdisk /dev/${dev_node_usbh} < ./format_command_usbh;
	sleep 1;

	if [ -n /dev/${dev_node_usbh}1 ]; then
		mkdosfs /dev/${dev_node_usbh}1;
		echo "usbh: format success!"
	else
		echo "usbh: the usb host device node is not exist!"
	fi

	if [ ! -d $mount_point ]; then
		mkdir $mount_point;
	fi
	
	mount -t vfat /dev/${dev_node_usbh}1 $mount_point;
	sleep 2;

	find=`cat /proc/mounts | grep "$mount_point" | wc -l`;
	if [ $find -eq 1 ]; then
		echo "usbh: prepare usbh success!";
	else
		echo "usbh: prepare usbh fail!"
        return 1
	fi

    return 0
}

mount_nand()
{
	return_value=0;
	mtd_nand_number=0;
	find=0;
	mount_point=/mnt/flc;

        prepare_platform;
	
	mtd_nand_number=`cat /proc/mtd | wc -l`;
	echo $mtd_nand_number;
	let mtd_nand_number=$mtd_nand_number-2;
	echo $mtd_nand_number;

	confirm=`cat /proc/mtd | grep "mtd${mtd_nand_number}" | grep "nand.userfs" | wc -l`;
	if [ $confirm -eq 1 ]; then
		echo "It is right!";
	else
		echo "not found right mtd number!";
        return 1
	fi

	# mount
	find=`cat /proc/mounts | grep "mtdblock$mtd_nand_number" | wc -l`;
	if [ $find -eq 1 ]; then
		umount /dev/mtdblock$mtd_nand_number;
		if [ $? -eq 0 ]; then
			echo "umount /dev/mtdblock$mtd_nand_number success!";
			sleep 1;
		else
			echo "umount /dev/mtdblock$mtd_nand_number fail!";
			sleep 1;
		fi	
		
	fi

	if [ ! -d $mount_point ]; then
		mkdir $mount_point;
	fi

	mount -t jffs2 /dev/mtdblock${mtd_nand_number} $mount_point;

	if [ $? -eq 0 ]; then
		echo "the /dev/mtdblock${mtd_nand_number} mount to $mount_point success!";
		sleep 3;
        return 0
	else
		echo "the /dev/mtdblock${mtd_nand_number} mount to $mount_point fail!"
        return 1
	fi	
}

mount_nor()
{
	return_value=0;
	mtd_nor_number=0;
	find=0;
	mount_point=/mnt/flb;

        prepare_platform;

	if [ $platform = MX35 ]; then
		mtd_nor_number=2;

		# mount
		find=`cat /proc/mounts | grep "mtdblock$mtd_nor_number" | wc -l`;
		if [ $find -eq 1 ]; then
			umount /dev/mtdblock$mtd_nor_number;
			if [ $? -eq 0 ]; then
				echo "umount /dev/mtdblock$mtd_nor_number success!";
				sleep 1;
			else
				echo "umount /dev/mtdblock$mtd_nor_number fail!";
				sleep 1;
			fi	
		fi

		if [ ! -d $mount_point ]; then
			mkdir $mount_point;
		fi

		mount -t jffs2 /dev/mtdblock${mtd_nor_number} $mount_point;

		if [ $? -eq 0 ]; then
			echo "the /dev/mtdblock${mtd_nor_number} mount to $mount_point success!";
			sleep 3;
		else
			echo "the /dev/mtdblock${mtd_nor_number} mount to $mount_point fail!"
            return 1
		fi
	else
		echo "Only Mx35 platform has nor device"
        return 1
	fi	

    return 0
}

mount_sd()
{
	echo "only mount the sd first partition!"
	insmod_SD;
	sd_mount_point=`cat /proc/partitions | grep "mmcblk.p1" | awk '{print $4}'`
	mkdir -p /mnt/mmcblk0p1;
	cat /proc/mounts | grep $sd_mount_point
	if [ $? -eq 0 ]; then
		echo "the $sd_mount_point has been mounted!";
	else
		mount -t vfat /dev/$sd_mount_point /mnt/mmcblk0p1;
		sleep 2;
		cat /proc/mounts | grep $sd_mount_point
		if [ $? -eq 0 ]; then
			echo "mount sd/mmc success!"
			return 0;
		else
			echo "please use -P to prepare!"
			return 1;
		fi
	fi 	
}

mount_ata()
{
	echo "only mount the ata first partition!"
	insmod_ATA;
	ata_mount_point=`cat /proc/partitions | grep "${ata_dev_point}1" | awk '{print $4}'`
	mkdir -p /mnt/ata1;
	cat /proc/mounts | grep $ata_mount_point
	if [ $? -eq 0 ]; then
		echo "the $ata_mount_point has been mounted!";
	else
		mount -t vfat /dev/$ata_mount_point /mnt/ata1;
		sleep 2;
		cat /proc/mounts | grep $ata_mount_point
		if [ $? -eq 0 ]; then
			echo "mount ata success!"
			return 0;	
		else
			echo "please use -P to prepare!"
			return 1;
		fi
	fi
}

mount_usbh()
{
	echo "only mount the usbh first partition!"
	insmod_USBH;
	usbh_mount_point=`cat /proc/partitions | grep "${usbh_dev_point}1" | awk '{print $4}'`
	mkdir -p /mnt/msc;
	cat /proc/mounts | grep $usbh_mount_point
	if [ $? -eq 0 ]; then
		echo "the $usbh_mount_point has been mounted!";
	else
		mount -t vfat /dev/$usbh_mount_point /mnt/msc;
		sleep 2;
		cat /proc/mounts | grep $usbh_mount_point
		if [ $? -eq 0 ]; then
			echo "mount usbh success!"
			return 0;
		else
			echo "please use -P to prepare!"
			return 1;
		fi	
	fi
}

#for MX37&51 only
prepare_vpu()
{
    echo -e "\033[9;0]" > /dev/tty0
    echo 1 > /proc/sys/vm/lowmem_reserve_ratio
    return 0
}

scp_vte()
{
	prepare_platform;
	echo "account: guest      password: guest (please input password manual)";
    	scp -r guest@10.192.225.217:/home/Release/$vte_name/ $vte_path;
	if [ $? -eq 0 ]; then
		echo "copy vte form 10.192.225.217 to $vte_path success!";
	else
		echo "copy vte form 10.192.225.217 to $vte_path fail!";
        fi
}

help()
{
	echo "--------------------------------------------------------------------------------";
	echo "-I insert modules(SD, ATA, V4L, BT, USBH or ALL)";          
 	echo "		eg. ./auto_prepare.sh -I SD";
        echo "-R remove modules(SD, ATA, V4L, BT, USBH or ALL)";          
	echo "		eg. ./auto_prepare.sh -R SD";
	echo "-M mount devices(NAND, NOR, SD, ATA, USBH)";          
	echo "		eg. ./auto_prepare.sh -M SD";
        echo "-P partition, format and mount device to /mnt/~";   
        echo "the devices may be NAND, NOR, SD, ATA, USBH or ALL";
	echo "		eg. ./auto_prepare.sh -P SD";
        echo "-C copy vte test bin to the board";                      
	echo "		eg. ./auto_prepare.sh -C /mnt/flc";
	echo "-H print help docement"
	echo "--------------------------------------------------------------------------------";
    return 1
}

echo "--------------------------------------------------------------------------------";
return_value=0;
platform=MX31;
sd_dev_point=mmcblk0
ata_dev_point=hda;
usbh_dev_point=sdb;
vte_name=imx31stack-vte-test;
vte_path=/home;
sys_name=`uname -r`/
board=
sd_buildin=0;
ata_buildin=0;
usbh_buildin=0;
bt_buildin=0;

case $# in
	1) if [ $1 = "-H" ]; then
	   	help;
	   else
		echo "Please input right parameters";
		help;
	   fi
           ;;
    2) case $1 in
		"-I") case $2 in
				"sd" | "SD" | "mmc" | "MMC") insmod_SD;
					;;
				"ata" | "ATA") insmod_ATA;
					;;
				"v4l" | "V4L") insmod_V4L;
					;;
				"bt" | "BT") insmod_BT;
					;;
				"usbh" | "USBH") insmod_USBH;
					;;
				"all" | "ALL") insmod_SD;
				               insmod_ATA;
					       insmod_V4L;
					       insmod_BT;
					       insmod_USBH;
					;;
				*) help;
					;;
		      esac
		      ;;
		"-R") case $2 in
				"sd" | "SD" | "mmc" | "MMC") rmmod_SD;
					;;
				"ata" | "ATA") rmmod_ATA;
					;;
				"v4l" | "V4L") rmmod_V4L;
					;;
				"bt" | "BT") rmmod_BT;
					;;
				"usbh" | "USBH") rmmod_USBH;
					;;
				"all" | "ALL") rmmod_SD;
				               rmmod_ATA;
					       rmmod_V4L;
					       rmmod_BT;
					       rmmod_USBH;
					;;
				*) help;
					;;
		      esac
		      ;;
		"-M") case $2 in
			
				"nand" | "NAND") mount_nand;
					;;
				"nor" | "NOR") mount_nor;
					;;
				"sd" | "SD" | "mmc" | "MMC") mount_sd;
					;;
				"ata" | "ATA") mount_ata;
					;;
				"usbh" | "USBH") mount_usbh;
					;;
				*) help;
					;;
		      esac
		      ;;
		"-P") case $2 in
				"nand" | "NAND") prepare_nand;
					;;
				"nor" | "NOR") prepare_nor;
					;;
				"sd" | "SD" | "mmc" | "MMC") prepare_sd;
					;;
				"ata" | "ATA") prepare_ata;
					;;
				"usbh" | "USBH") prepare_usbh;
					;;
                #only for MX37&51
				"VPU" | "vpu") prepare_vpu;
					;;
				"all" | "ALL") prepare_nand;
				 	       prepare_nor;
				 	       prepare_sd;
					       prepare_ata;
					       prepare_usbh;
					;;

				*) help;
					;;
		      esac
              exit $?
		      ;;
		"-C") vte_path=$2;
			if [ -d $vte_path ]; then
				scp_vte;
			else
				echo "$vte_path is not exist!";
			fi
		      ;;
		*) help;
		      ;;
           esac
           ;;
     4) if [ $1 = "-P" ] && [ $3 = "-C" ]; then
     		 case $2 in
				"nand" | "NAND") prepare_nand;
					;;
				"nor" | "NOR") prepare_nor;
					;;
				"sd" | "SD" | "mmc" | "MMC") prepare_sd;
					;;
				"ata" | "ATA") prepare_ata;
					;;
				"usbh" | "USBH") prepare_usbh;
					;;
				"all" | "ALL") prepare_nand;
					       prepare_nor;
				 	       prepare_sd;
					       prepare_ata;
					       prepare_usbh;
					;;
				*) help;
					;;
		 esac

		 vte_path=$4;
		 if [ -d $vte_path ]; then
			scp_vte;
		 else
			echo "$vte_path is not exist!";
		 fi
	 else
		 echo "Please use right parameters, ./auto_prepare.sh -P * -C *"
		 help;
	 fi
           ;;
        *) help;
           ;;
esac
echo "--------------------------------------------------------------------------------";

