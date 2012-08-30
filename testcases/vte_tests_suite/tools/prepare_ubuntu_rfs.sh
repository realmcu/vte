#!/bin/sh
#Usage: Use the script to make test related setting to Ubuntu/Linaro rootfs.
#Author: Spring Zhang
#Version:
# v1, Aug.21,2012, modify prepare_staf to prepare normal ubuntu rootfs.

# Can't work now because chroot need a qemu to execute
#exit 2

prepare_staf()
{
    # Copy staf binary, can't work on x86, so commented
    #cp -a $staf_bin/* $rfs

    # Install auto run scripts
    i=91
    set -x
    for s in "staf vte update-deb"; do
        #cp $sys_script/$s $rfs/etc/init.d
        chroot $rfs update-rc.d $s defaults $i
        chroot $rfs update-rc.d $s enable 2345
        i=`expr $i + 1`
    done
    set +x
    echo "General Setting for Ubuntu System Finished"
}

# Prepare general rootfs for daily use
general()
{
    echo "localhost" > $rfs/etc/hostname
    echo '127.0.0.1       localhost' > $rfs/etc/hosts

    # Enable X-apps
    echo "export DISPLAY=:0" >> $rfs/etc/profile
    echo "export XAUTHORITY=/home/linaro/.Xauthority" >> $rfs/etc/profile

    # Use default governor
    update-rc.d ondemand disable

    # dump /etc/rc.local
    sed -i '/exit 0/d' $rfs/etc/rc.local
    cat >> $rfs/etc/rc.local <<-EOF
echo 7 > /proc/sys/kernel/printk
mkdir -p /mnt/nfs && mount -t nfs -o nolock 10.192.225.222:/rootfs/wb /mnt/nfs/
echo 256 > /proc/sys/vm/lowmem_reserve_ratio
#echo 1 1 > /proc/sys/vm/lowmem_reserve_ratio
rm -f /var/cache/apt/*.bin.*
ntpdate 10.208.0.120
EOF

        # clear net iface
    echo "" > $rfs/etc/udev/rules.d/70-persistent-net.rules

    echo "General Setting for Ubuntu System Finished"
}

# The script only work on Ubuntu-like script
check_os()
{
    if [ -e $rfs/etc/issue ]; then
        if cat $rfs/etc/issue |grep -i ubuntu || cat $rfs/etc/issue |grep -i linaro
        then
            echo
        else
            no_ubuntu=1
        fi
    else
        no_ubuntu=1
    fi

    if [ -n "$no_ubuntu" ]; then
        echo "Not Ubuntu-like rootfs, quit..."
        return 2
    fi
}

usage()
{
    cat <<-EOF
    ./${0##*/} [type] [rootfs path]
    type: "rootfs" - execute normal process
    type: "staf" - execute normal and staf process
    "staf" mode can only work on ARM machine
    e.g.: ./${0##*/} rootfs /
    e.g.: ./${0##*/} rootfs /mnt/msc
    e.g.: ./${0##*/} staf /
EOF
    exit 1
}

# main

check_os || exit $?

if [ $# -lt 2 ]; then
    echo "Please provide working rootfs"
    exit 1
fi

# Working directory, need to be a ubuntu rootfs
rfs=$2

#if [ -n "$3" ]; then
#    staf_bin=$3
#else
#    staf_bin="STAF"
#fi

# STAF init script location
#sys_script="."

case $1 in
    rootfs)
        general
        ;;
    staf)
        general
        prepare_staf
        ;;
    *)
        usage
        ;;
esac

echo "Finished"
