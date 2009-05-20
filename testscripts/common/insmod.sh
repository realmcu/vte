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
##########################################################
## For IMX Platform                           ##########
## Enable USB OTG module                        ##########
## Enable SD module                             ##########
## Enable ATA
##########################################################
##File:      insmod.sh
# Description: This script just enalbe usbotg and SD module
#              for ringo platform.If the paltform changed,
#              please change the script soon.
#
# To run the script,please connect your hardware and SD to board
# in advance.
#
D_path="lib/modules/`uname -r`/kernel/drivers"
MC_path="mmc/core"
MA_path="mmc/card"
MT_path="mmc/host"
I_path="ide"
M_path="mnt"
V4L2_path="lib/modules/'uname -r'/kernel/drivers/media/video/mxc/capture"
BT_path="lib/modules/`uname -r`/kernel/drivers/mxc/bt/

# insmod mxc_bt.ko

enable_bt()
{
     echo "###### start to insert BT modular #######"
     insmod mxc_bt.ko
     sleep 1
     echo "#####  BT is ok ######################"
}
enable_v4l2()
{
    ipu_prp_enc=0
    ipu_prp_vf_sdc=0
    ipu_prp_vf_sdc_bg=0
    ipu_still=0
    mxc_v4l2_capture=0
    ov2640_camera=0

     ipu_prp_enc='lsmod | grep "ipu_prp_enc" | wc -l'
     ipu_prp_vf_sdc='lsmod | grep "ipu_prp_vf_sdc" | wc -l'
     ipu_prp_vf_sdc_bg='lsmod | grep "ipu_prp_vf_sdc_bg" | wc -l'
     ipu_still='lsmod | grep "ipu_still" | wc -l'
     ov2640_camera='lsmod | grep "ov2640_camera" | wc -l'
     mxc_v4l2_capture='lsmod | grep "mxc_v4l2_capture" | wc -l'

    echo "### star to insert v4l2 modular ###"
    cd /$V4L2_path
    if [ $ipu_prp_ecn -eq 0 ]
    insmod ipu_prp_enc.ko
    sleep 1
    fi

    if [ $ipu_prp_vf_sdc -eq 0 ]
    insmod ipu_prp_vf_sdc.ko
    sleep 1
    fi

    if [ $ipu_prp_vf_sdc_bg -eq 0 ]
    insmod ipu_prp_vf_sdc_bg.ko
    sleep 1
    fi

    if [ $ipu_still -eq 0 ]
    insmod ipu_still.ko
    sleep 1
    fi

    if [ $ov2640_camera -eq 0 ]
    insmod ov2640_camera.ko
    sleep 1
    fi

    if [ $mxc_v4l2_capture -eq 0 ]
    insmod mxc_v4l2_capture.ko
    sleep 1
    fi
  echo "### V4L2 is OK ###################"


}
mount_SD()
{
     echo "#### star to mount SD ####"
     if [ -e /mnt/mmcblkp01 ]; then
           umount /$M_path/mmcblk0p1
           sleep 2
           rm -rf /$M_path/mmcblkp01
           sleep 1
           fi
          if [ -e /$M_path/mmcblk0p2 ]; then
           umount /$M_path/mmcblk0p2
           sleep 2
           rm -rf /$M_path/mmcblk0p2
           sleep 1
           fi
          if [ -e /$M_path/mmcblk0p3 ]; then
           umount /$M_path/mmcblk0p3
           sleep 2
           rm -rf /$M_path/mmcblk0p3
           sleep 1
           fi
          if [ -e /$M_path/mmcblk0p4 ]; then
           umount /$M_path/mmcblk0p4
           sleep 2
           rm -rf /$M_path/mmcblk0p4
           sleep 1
           fi
          if [ -e /$M_path/mmcblk0p5 ]; then
           umount /$M_path/mmcblk0p5
           sleep 2
           rm -rf /$M_path/mmcblk0p5
           sleep 1
           fi

          if [ -e /dev/mmcblk0p1 ]; then
            mkfs.vfat /dev/mmcblk0p1
            sleep 10
            echo "format mmcblkp1 completely."
            mkdir /$M_path/mmcblk0p1
            sleep 1
            mount -t vfat /dev/mmcblk0p1 /$M_path/mmcblk0p1
            sleep 4
            echo "mmcblk0p1 is mounted"
            fi


          if [ -e /dev/mmcblk0p2 ]; then
            mkfs.vfat /dev/mmcblk0p2
            sleep 15
            echo "format mmcblk0p2 completely."
            mkdir /$M_path/mmcblk0p2
            sleep 1
            mount -t vfat /dev/mmcblk0p3 /$M_path/mmcblk0p3
            sleep 4
            echo "mmcblk0p2 is mounted"
            fi


          if [ -e /dev/mmcblk0p3 ]; then
            mkfs.vfat /dev/mmcblk0p3
            sleep 10
            echo "format mmcblk0p3 completely."
            mkdir /$M_path/mmcblk0p3
            sleep 1
            mount -t vfat /dev/mmcblk0p3 /$M_path/mmcblk0p3
            sleep 4
            echo "mmcblk0p3 is mounted"
            fi


          if [ -e /dev/mmcblk0p4 ]; then
            mkfs.vfat /dev/mmcblk0p4
            sleep 10
            echo "format mmcblk0p4 completely."
            mkdir /$M_path/mmcblk0p4
            sleep 1
            mount -t vfat /dev/mmcblk0p4 /$M_path/mmcblk0p4
            sleep 4
            echo "mmcblk0p4 is mounted"
            fi


          if [ -e /dev/mmcblk0p5 ]; then
            mkfs.vfat /dev/mmcblk0p5
            sleep 10
            echo "format mmcblkp5 completely."
            mkdir /$M_path/mmcblk0p5
            sleep 1
            mount -t vfat /dev/mmcblk0p5 /$M_path/mmcblk0p5
            sleep 4
            echo "mmcblk0p5 is mounted"
            fi
          echo "#### mount SD completely ####"
}

mount_ata()
{
  echo "#### start to mount hard disk ####"
  if [ -e /$M_path/hda1 ]; then
           umount /$M_path/hda1
           sleep 2
           rm -rf /$M_path/hda1
           sleep 1
          fi
        if [ -e /$M_path/hda2 ]; then
           umount /$M_path/hda2
           sleep 2
           rm -rf /$M_path/hda2
           sleep 1
           fi
        if [ -e /$M_path/hda3 ]; then
           umount /$M_path/hda3
           sleep 2
           rm -rf /$M_path/hda3
           sleep 1
           fi
        if [ -e /$M_path/hda4 ]; then
           umount /$M_path/hda4
           sleep 2
           rm -rf /$M_path/hda4
           sleep 1
           fi
        if [ -e /$M_path/hda5 ]; then
           umount /$M_path/hda5
           sleep 2
           rm -rf /$M_path/hda5
           sleep 1
        fi



        if [ -e /dev/hda2 ]; then
            mkfs.vfat /dev/hda2
            sleep 10
            echo "format hda2 completely."
            mkdir /$M_path/hda2
            sleep 1
            mount -t vfat /dev/hda2 /$M_path/hda2
            sleep 4
            echo "hda2 is mounted"
            fi
        if [ -e /dev/hda3 ]; then
            mkfs.vfat /dev/hda3
            sleep 15
            echo "format hda3 completely."
            mkdir /$M_path/hda3
            sleep 1
            mount -t vfat /dev/hda3 /$M_path/hda3
            sleep 4
            echo "hda3 is mounted"
            fi
        if [ -e /dev/hda4 ]; then
            mkfs.vfat /dev/hda4
            sleep 10
            echo "format hda4 completely."
            mkdir /$M_path/hda4
            sleep 1
            mount -t vfat /dev/hda4 /$M_path/hda4
            sleep 4
            echo "hda4 is mounted"
            fi
        if [ -e /dev/hda5 ]; then
            mkfs.vfat /dev/hda5
            sleep 10
            echo "format hda5 completely."
            mkdir /$M_path/hda5
            sleep 1
            mount -t vfat /dev/hda5 /$M_path/hda5
            sleep 4
            echo "hda5 is mounted"
            fi
         ehco "#####   mount hard disk completely #####"
}


if [ $# -eq 1 ]; then
     platform=$1
     if [ $platform = "mx35_3stack" ]; then
      echo "## mx35 platform ##"
      echo "Enable SD moudle"
      cd  /$D_path/$MC_path
      insmod mmc_core.ko
      sleep 2
      cd /$D_path/$MA_path
      insmod mmc_block.ko
      sleep 2
      cd /$D_path/$MT_path
      insmod mx_sdhci.ko
      sleep 2
      cd /
      mount_SD
      sleep 2
       enable_v4l2
       sleep 1




     elif [ $platform = "mx31_3stack" ]; then
       M_path="tmp"
       echo "## mx31 platform ##"
       echo "enalbe tvout moudule"

       echo U:720x576i-50 > /sys/class/graphics/fb0/mode
       sleep 1
       cd /

        enable_v4l2
        sleep 1

         enable_bt
         sleep 1

    elif [ $platform = "mx37_3stack" ]; then
      echo "## mx37 platform ##"
      echo "Enable SD moudle"
      cd  /$D_path/$MC_path
      insmod mmc_core.ko
      sleep 1
      cd /$D_path/$MA_path
      insmod mmc_block.ko
      sleep 1
      cd /$D_path/$MT_path
      insmod mx_sdhci.ko
      sleep 2
      mount_SD
      sleep 2

      echo "enalbe tvout module"
      echo U:720x576i-50 > /sys/class/graphics/fb0/mode
      sleep 1
      cd /

       enable_v4l2
       sleep 1

     elif [ $platform = "mx32_ads" ]; then
      M_path="tmp"
      echo "## mx32 platform ##"
      echo "enalbe ATA module"
      cd /$D_path/$I_path
      insmod ide-core.ko
      sleep 2
      insmod ide-disk.ko
      sleep 1
      insmod ide-generic.ko
      sleep 1
      cd arm
      insmod mxc_ide.ko
      sleep 5
      cd /
      mount_ata
      mount_SD
       enable_v4l2
       sleep 1

     fi
fi

exit $platform
