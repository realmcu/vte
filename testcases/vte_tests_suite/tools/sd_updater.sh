#!/bin/sh  -x

# Default Offset values
OFF_KERNEL=1048576    			# 1M after the start
OFF_REDBOOT=1024			# 1K after the MBR
DEF_DEVNODE="/dev/sdb"		# default applies to target
LOGFILE=updater.log

showhelp() {
bn=`basename $0`
ver=`echo '$Revision: 1.1 $' | sed 's/.Revision. \(.*\)./\1/'`
cat << eot
====================  $bn Version: $ver ==================

usage $bn [-h] [-k <zImage name>] [-b <redboot name>] [-r <ext2 image file>] [-n <device node>] [-o <offset>] [-i] [-c]
  -h				displays this help message
  -k <zImage name>              update the kernel.
  -b <redboot name>		update redboot
  -r <ext2 image file>          update the root file system. Device node is different than -b and -k
  -n <device node>		device node to use. Default is ${DEF_DEVNODE}.
  -o <offset> 			offset to use. In Decimal
				Default offset for redboot=${OFF_REDBOOT}
				Default offset for kernel=${OFF_KERNEL}
  -i 				Initiliaze the device (-n) with a default MBR and redboot
  -c 				Erase the contents of the logfile (${LOGFILE}) before executing

eot
}



# default actions. None
DO_REDBOOT=0
DO_KERNEL=0
DO_OFFSET=0			# if -k and -r and -o specified, what does offset mean ?
DO_INIT=0
DO_CLEAN=0
DO_RFS=0
DEVNODE=${DEF_DEVNODE}

# default commands
DD=/bin/dd
OD=/usr/bin/od
HEAD=/usr/bin/head
SYNC=/bin/sync
GUNZIP=/bin/gunzip

if [ ! -e ${DD} ] ; then
	echo "Error: ${DD} not found"
	exit -1
fi

if [ ! -e ${OD} ] ; then
        echo "Error: ${OD} not found"
        exit -1
fi

if [ ! -e ${HEAD} ] ; then
        echo "Error: ${HEAD} not found"
        exit -1
fi

if [ ! -e ${SYNC} ] ; then
	echo "Error: ${SYNC} not found"
	exit -1
fi

if [ ! -e ${GUNZIP} ] ; then
	echo "Error: ${GUNZIP} not found"
	exit -1
fi
	
check_padding() {
    local REDBOOT=$1
    # REF is the first bytes of the padding
    local REF="0000004 0000 0000 0000 0000 0000 0000 0000 0000"
    local NEW="`${OD} -x ${REDBOOT} +4 | ${HEAD} -1`"
    local RET=0
	
    if [ "${REF}" = "${NEW}" ] ; then
        RET=1
    fi

    return ${RET}
}

update_chunk() {
	local FILE=$1
 	local OFFSET=$3
	local NODE=$2

        # echo "running: ${DD} if=${FILE} of=${NODE} bs=${OFFSET} seek=1"
        ${DD} if=${FILE} of=${NODE} bs=${OFFSET} seek=1 >> ${LOGFILE} 2>&1
	ECODE=$?
	if [ ${ECODE} -ne 0 ] ; then
		echo "Error: ${DD} failed with exit code ${ECODE}"
		exit -1
	fi
}

init_device() {

	local DEVNODE=$1
	local INITFILE="data/babbage_sdinitblocks.bin"
	local RET=0
	
	if [ ! -e ${INITFILE} ] ; then
		echo "Error: ${INITFILE}: no such file or directory"
		RET=1
	else
		${DD} if=${INITFILE} of=${DEVNODE} >> ${LOGFILE} 2>&1
		RET=$?
	fi	
	
	return ${RET}
}

#fdisk cmd: 1st partition from 20~disk/4, 2st partition from disk/4~END
gen_fdisk_cmd()
{
    cylinders=`fdisk -l $DEVNODE |grep cylinders|grep -v of | awk '{print $5}'`
    #leave 1% space(40MB for 4G) for uboot, its env and partition table
    partition1_start=`expr $cylinders / 100`
    partition1_end=`expr $cylinders / 4 + $partition1_start`
    partition2_start=`expr $partition1_end + 1`
    echo ""
    echo "d"
    echo "1"
    echo "d"
    echo "2"
    echo "d"
    echo "3"
    echo "d"
    echo "4"
    echo ""
    echo "n"
    echo "p"
    echo "1"
    echo "$partition1_start"
    echo "$partition1_end"
    echo "n"
    echo "p"
    echo "2"
    echo "$partition2_start"
    echo ""
    echo "w"
}

	
#################### parse command line arguments
## This loop works only if all switches are preceeded with a "-"
##
moreoptions=1
while [ "$moreoptions" = 1  -a $# -gt 0 ] ; do
#  echo parse $1
  case $1 in
    -h) showhelp ; exit ;;
    -k) ZIMAGE=$2; KN_OFFSET=${OFF_KERNEL} ; DO_KERNEL=1 ; shift ;;
    -b) REDBOOT=$2 ; BL_OFFSET=${OFF_REDBOOT} ; DO_REDBOOT=1 ; shift ;;
    -r) RFS=$2 ; DO_RFS=1 ; shift ;;
    -o) OFFSET=$2 ; DO_OFFSET=1 ; shift ;;
    -n) DEVNODE=$2 ; shift ;;   
    -i) DO_INIT=1 ;;
    -c) DO_CLEAN=1 ;;
    -*) echo "ERROR wrong option $1" ;  exit 1; ;;
    *) moreoptions=0 ;;
  esac
  a=$1
  [ "$moreoptions" = 1 ] && shift
done

############### here the script starts
# can not have -k and -r
#if [ $DO_REDBOOT -eq 1 -a $DO_KERNEL -eq 1 ] ; then
#	echo "Error: Should offset apply to -k or -r ? Choose"
#	exit -1
#fi

# anything to do ?
if [ $DO_REDBOOT -eq 0 -a $DO_KERNEL -eq 0 -a $DO_INIT -eq 0 -a $DO_CLEAN -eq 0 -a $DO_RFS -eq 0 ] ; then
	echo "Nothing to do. Exit"
	exit 0
fi

# does the device node exist ?
if [ ! -e ${DEVNODE} ] ; then
	echo "${DEVNODE}: no such file or directory"
	exit -1
elif [ "$DEVNODE" = "/dev/sda" ]; then
    echo "device node should not be sda"
    exit 67
fi

if [ $DO_CLEAN -eq 1 ] ; then
	rm -f ${LOGFILE}
fi

echo "~~~~~~ new execution: " `date` >> ${LOGFILE}

# do I have to init the device ?
if [ $DO_INIT -eq 1 ] ; then
	echo -n "initing device ($DEVNODE)... "
	init_device $DEVNODE
	ECODE=$?
	if [ $ECODE -ne 0 ] ; then
		echo " Failed"
		echo "Error: initalization of the device failed ($ECODE)"
		exit -1
	fi
	echo " Done"
fi

# do I have to update the kernel ?
if [ $DO_KERNEL -eq 1 ] ; then
	echo -n "Writing the kernel ${ZIMAGE} to ${DEVNODE}..."
	if [ ! -e ${ZIMAGE} ] ; then
		echo " Failed"
		echo "${ZIMAGE}: no such file or directory"
		exit -1
	fi
	#            FILE       NODE       OFFSET  
	update_chunk ${ZIMAGE} ${DEVNODE} ${KN_OFFSET} 	
	echo " Done"
fi

# do I have to update redboot ?
if [ $DO_REDBOOT -eq 1 ] ; then
	echo "Writing redboot ${REDBOOT} to ${DEVNODE}..."
	if [ ! -e ${REDBOOT} ] ; then
		echo " Failed"
		echo "${REDBOOT}: no such file or directory"
		exit -1
	fi

	check_padding ${REDBOOT}
	if [ $? -eq 1 ] ; then
		echo ""
		echo -n "WARNING: ${REDBOOT} seems to have padding (leading zeros). Proceed anyway ? [yes|no]: "
		read yes_or_no
		if [ "${yes_or_no}" = "yes" ] ; then
            #            FILE       NODE       OFFSET  
            ${DD} if=${REDBOOT} of=${DEVNODE} bs=${BL_OFFSET} seek=1 skip=1
			echo "Proceed with padding data"
        else
            exit 67
		fi
    else
        #            FILE       NODE       OFFSET  
        update_chunk ${REDBOOT} ${DEVNODE} ${BL_OFFSET}
	fi

    ${SYNC} ; ${SYNC} ; ${SYNC}
	echo "  Done"
fi

# do I have to update the root file system ?
if [ $DO_RFS -eq 1 ] ; then
	#if [ $DO_REDBOOT -eq 1 -o $DO_KERNEL -eq 1 -o $DO_INIT -eq 1 ] ; then
	#	echo 'Error: the device node for -k|-b|-i and -r is different'
	#	exit -1
	#fi
	
	if [ ! -e ${RFS} ] ; then
		echo "Error: ${RFS}: no such file or directory"
		exit -1
	fi

    prefix_DEVNODE=`echo $DEVNODE |cut -c -8`
    if [ "$prefix_DEVNODE" = "/dev/mmc" ]; then
        umount ${DEVNODE}p1
        umount ${DEVNODE}p2
        umount ${DEVNODE}p5
        umount ${DEVNODE}p6
    else
        umount ${DEVNODE}1
        umount ${DEVNODE}2
        umount ${DEVNODE}5
        umount ${DEVNODE}6
    fi

    #fdisk & format
    gen_fdisk_cmd  > ./format_rootfs.cmd
    fdisk $DEVNODE < ./format_rootfs.cmd >> ${LOGFILE} 2>&1
    #determine if it's MMC card node, /dev/mmcblkX, if yes, add 'p'
    if [ "$prefix_DEVNODE" = "/dev/mmc" ]; then
        DEVNODE="${DEVNODE}p"
    fi
    mkfs.ext3 ${DEVNODE}2 >> ${LOGFILE} 2>&1
    mkfs.vfat ${DEVNODE}1 >> ${LOGFILE} 2>&1
    rm -f format_rootfs.cmd

	# compressed image ?
	echo ${RFS} | egrep -e '\.gz$' >> ${LOGFILE} 2>&1
	ISCMPD=$?
	echo ${RFS} | egrep -e '\.tar\.gz$' >> ${LOGFILE} 2>&1
    ISTAR=$?

    mkdir -p /mnt/msc
    sleep 5
    mount -t ext3 ${DEVNODE}2 /mnt/msc >> ${LOGFILE} 2>&1
    RET=$?
    [ $RET -eq 0 ] || {
        echo "mount ${DEVNODE}2 error"
        exit $RET
    }

    if [ ${ISCMPD} -eq 0 ] ; then
		EXT2_RFS=`echo rootfs.ext2.gz | sed -e 's/\.gz$//'`
		${GUNZIP} -c ${RFS} > ${EXT2_RFS}
		RET=$?
		if [ ${RET} -ne 0 ] ; then
			echo "Error: ${GUNZIP} failed (${RET})"
			exit -1
		fi
        mkdir -p /mnt/ext2
        mount -t ext2 -o loop $EXT2_RFS /mnt/ext2 >> ${LOGFILE} 2>&1
        cp -a /mnt/ext2/* /mnt/msc
        RET=$?
        if [ ${RET} -ne 0 ] ; then
            echo "Error: ${DD} failed with exit code ${RET}"
            exit -1
        fi
        #clean
        sync && sync
        umount /mnt/ext2
    elif [ ${ISTAR} -eq 0 ]; then
        tar --numeric-owner -xzf $RFS -C /mnt/msc >> ${LOGFILE} 2>&1
        sync
    fi
    umount /mnt/msc >> ${LOGFILE} 2>&1
fi

# now sync the drives!
echo "syncing drives"
${SYNC} ; ${SYNC} ; ${SYNC}

exit 0

