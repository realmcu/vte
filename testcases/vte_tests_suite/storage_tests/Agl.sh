#!/bin/bash
#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###############################################################################
# Notes:
# 1. use the #!/bin/bash; because of the read command "-t timeout" option
# 2. example command
#   ./Agl.sh /hive/LinuxReleaseCandidate/L2.6.28_4.4.0_SS_Jul2009_RC1/Source/\
#     BSP/L2.6.28_4.4.0_SS_Jul2009_source.tar.gz imx25_agl_pkg_ER4SP1.tgz
# help
# $1 is the path of source; please using the linux directory format
# $2 is the download agl package name; http://compass.freescale.net/go/208116829
#    you should download the package to the working directory.
###############################################################################

###############################################################################
# create directory
###############################################################################
WORKHOME=`pwd`
echo $WORKHOME

if [ -e agl_source ]; then
	rm -rf agl_source
fi
mkdir agl_source
AGL_SOURCE=${WORKHOME}/agl_source

if [ -e kernel_source ]; then
	rm -rf kernel_source
fi
mkdir kernel_source
KERNEL_SOURCE=${WORKHOME}/kernel_source

if [ -e image_dir ]; then
	rm -rf image_dir
fi
mkdir image_dir
IMAGE_DIR=${WORKHOME}/image_dir

echo $AGL_SOURCE
echo $KERNEL_SOURCE
echo $IMAGE_DIR

###############################################################################
# prepare kernel source code
###############################################################################
cd $KERNEL_SOURCE

HOST_SERVER=10.192.224.48
KERNEL_VER=2.6.28

SDK_VER=4.4.0
read -p "please input sdk version(now is 4.4.0):" -t 10 SDK_VER
echo
if [ -z $SDK_VER ]; then
	SDK_VER=4.4.0
fi
echo $SDK_VER

LINUX_KERNEL=linux-${KERNEL_VER}.tar.bz2
PATCHES=linux-${KERNEL_VER}-imx_${SDK_VER}.bz2

if [ $HOST_SERVER = "10.192.224.48" ]; then
	RELEASE_PATH="/pub1($1)"
	echo $RELEASE_PATH
	RELEASE_PATH=$(echo $RELEASE_PATH | sed 's/hive//g')
	echo $RELEASE_PATH
	RELEASE_PATH=$(echo $RELEASE_PATH | sed 's/10.192.224.48//g')
	echo $RELEASE_PATH
	RELEASE_PATH=$(echo $RELEASE_PATH | sed 's/(//g')
	echo $RELEASE_PATH
	RELEASE_PATH=$(echo $RELEASE_PATH | sed 's/)//g')
	echo $RELEASE_PATH                                                            
fi

RELEASE_PACKAGE=$(basename $RELEASE_PATH)
echo $RELEASE_PACKAGE

RELEASE_PATH=$(dirname $RELEASE_PATH)
echo $RELEASE_PATH
                                                            
RC=1

echo "get the package"
scp b20222@${HOST_SERVER}:${RELEASE_PATH}/${RELEASE_PACKAGE} ${KERNEL_SOURCE} || exit 1  

echo "unzip the package"
tar xzvf ${KERNEL_SOURCE}/$RELEASE_PACKAGE -C ${KERNEL_SOURCE} || exit 2
RELEASE_TARGET=$(ls -l ${KERNEL_SOURCE} | grep ^d | awk '{print $NF}')
echo $RELEASE_TARGET

cp ${KERNEL_SOURCE}/${RELEASE_TARGET}/pkgs/${LINUX_KERNEL} .

tar xjf ${LINUX_KERNEL} || return $RC
cp ${KERNEL_SOURCE}/${RELEASE_TARGET}/pkgs/${PATCHES} linux-${KERNEL_VER}

cd linux-${KERNEL_VER}
tar xjf ${PATCHES} || return $RC
rm -f ${PATCHES}

./patches/patch-kernel.sh || return $RC

cd ..
tar czvf linux-${KERNEL_VER}_wb.tar.gz linux-${KERNEL_VER}
rm -f ${LINUX_KERNEL}
#echo "kernel srouce deploy OK" | mutt -s "kernel source is OK" \
#b19309@freescale.com &

cd linux-${KERNEL_VER}
KERNEL_DIR=`pwd`
echo $KERNEL_DIR

echo "kernel deploy ok"

###############################################################################
# build the kernel
###############################################################################
cd $KERNEL_DIR
make distclean
export | grep "arm"
if [ $? -ne 0 ]; then
	export PATH=$PATH:/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin 
	export ARCH=arm 
	export CROSS_COMPILE=arm-none-linux-gnueabi- 
fi
make imx25_3stack_defconfig 
make 

cp arch/arm/boot/zImage $IMAGE_DIR/zImage_normal
###############################################################################
# prepare the AGL source code
# download agl pkg form http://compass.freescale.net/go/208116829 to $AGL_SOURCE
###############################################################################
cd $WORKHOME
mv $2 $AGL_SOURCE
cd $AGL_SOURCE
tar xvzf $2
cd agl

###############################################################################
# build the btcd-consloe and btcs-latencytester
###############################################################################
cd agl-test
# btcs-consloe
cd btcs-console
sed -i "s/KDIR=/\#KDIR=/" Makefile
sed -i "/KDIR=/a\KDIR=$KERNEL_DIR" Makefile
make clean
make
cp btcs-console  $IMAGE_DIR

# btcs-latencytester
cd ../btcs-latencytester
sed -i "s/KDIR=/\#KDIR=/" Makefile
sed -i "/KDIR=/a\KDIR=$KERNEL_DIR" Makefile
make clean
make

cp btcs-latencytester $IMAGE_DIR
cp gpiomod.ko  $IMAGE_DIR

###############################################################################
# rebuild the kernel with AGL config and patch
###############################################################################

cd $KERNEL_DIR
patch -p1 < ${AGL_SOURCE}/agl/0001-AGL-logo.patch
patch -p1 < ${AGL_SOURCE}/agl/0002-Add-BTCS.patch
cp ${AGL_SOURCE}/agl/imx25_3stack_agl_defconfig arch/arm/configs/imx25_3stack_defconfig
make distclean
export | grep "arm"
if [ $? -ne 0 ]; then
	export PATH=$PATH:/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin 
	export ARCH=arm 
	export CROSS_COMPILE=arm-none-linux-gnueabi- 
fi
make imx25_3stack_defconfig
make

###############################################################################
# build the normal AGL image
###############################################################################

cd $AGL_SOURCE/agl
tar xvfz bootloaders.tgz
cd bootloaders/nandboot
sed -i "s/KBUILD=\//\#KBUILD=\//" domake
sed -i "/KBUILD=\//a\KBUILD=$KERNEL_DIR" domake
sed -i "s/KSOURCE=\//\#KSOURCE=\//" domake
sed -i "/KSOURCE=\//a\KSOURCE=$KERNEL_DIR" domake
./domake
cd ..
cp bin/nb_linux.bin $IMAGE_DIR/nb_linux_normal.bin
cp bin/btcs_test $IMAGE_DIR/

###############################################################################
# build the AGL rootfs image
###############################################################################
cd $AGL_SOURCE/agl
mkdir -p /tmp/root
sudo tar xvzf agljffs2rootfsimage.tgz -C /tmp/root/
sudo cp ${AGL_SOURCE}/agl/bootloaders/bin/btcs_app /tmp/root/bin/
sudo mkfs.jffs2 -e 262144 -n -s 2048 -r /tmp/root -o agljffs2rootfsimage -l -v
sumtool -e 262144 -n -p -i agljffs2rootfsimage -o agljffs2rootfsimage.summed
cp agljffs2rootfsimage.summed $IMAGE_DIR

###############################################################################
# build the btcs-latencytracer AGL Image
###############################################################################
cd ${AGL_SOURCE}/agl/bootloaders
patch -p1 < ${AGL_SOURCE}/agl/agl-test/btcs-latencytracer/patch-bootloaders-timetrace
cd $KERNEL_DIR
patch -p1 < ${AGL_SOURCE}/agl/agl-test/btcs-latencytracer/patch-linux-2.6-imx-timetrace

make distclean
export | grep "arm"
if [ $? -ne 0 ]; then
	export PATH=$PATH:/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin 
	export ARCH=arm 
	export CROSS_COMPILE=arm-none-linux-gnueabi- 
fi
make imx25_3stack_defconfig 
make

cd $AGL_SOURCE/agl
cd bootloaders/nandboot
./domake
cd ..
cp bin/nb_linux.bin $IMAGE_DIR/nb_linux_tracer.bin

###############################################################################
# build the snoopserial-reset AGL Image
###############################################################################
# remove the patch
cd $KERNEL_DIR
patch -R -p1 < ${AGL_SOURCE}/agl/agl-test/btcs-latencytracer/patch-linux-2.6-imx-timetrace

#enable printk
cat arch/arm/configs/imx25_3stack_defconfig | grep "CONFIG_PRINTK=y"
if [ $? -ne 0 ]; then
	sed -i "/CONFIG_PRINTK/a\CONFIG_PRINTK=y" arch/arm/configs/imx25_3stack_defconfig
fi

make distclean
export | grep "arm"
if [ $? -ne 0 ]; then
	export PATH=$PATH:/opt/freescale/usr/local/gcc-4.1.2-glibc-2.5-nptl-3/arm-none-linux-gnueabi/bin 
	export ARCH=arm 
	export CROSS_COMPILE=arm-none-linux-gnueabi- 
fi
make imx25_3stack_defconfig 
make

cd $AGL_SOURCE/agl
cd bootloaders/nandboot
./domake
cd ..
cp bin/nb_linux.bin $IMAGE_DIR/nb_linux_sns.bin

###############################################################################
# build the snoopserial-reset
###############################################################################
cd ${AGL_SOURCE}/agl/agl-test/snoopserial-reset
make clean
make 
cp sns-rst $IMAGE_DIR

###############################################################################
# save all image to /rootfs/wb/test_stream/, so that the host board could 
# access them
###############################################################################
if [ -e /rootfs/wb/test_stream/image_dir ]; then
	sudo rm  /rootfs/wb/test_stream/image_dir -rf
fi

sudo cp -rf $IMAGE_DIR /rootfs/wb/test_stream

