#!/bin/ash

#set -x

VTE_VERSION="1.9"

# List of possible configuration
USERS="LDE CGD BGI SVG ODV"

# Some IP address
export laramade=10.160.41.112
export rb665c_01=10.160.25.189
export rc149c_d1=10.160.24.43
export r70160c_01=10.160.24.53

# Misc.
VTE_NFS_DIR="/root/vte_nfs"
VTE_NFSPREFIX="/local/linux_baseport/"
nomount="no"
export LTPROOT=$VTE_NFS_DIR/
PLATFORM=

#
# LDE Configuration
#
### VIRTIO/WINBLOWS
#LDE_HOSTNAME="LVT_VIRTIO_LDE"
#LDE_MAC="18:0b:ad:da:db:ab"
#LDE_IP="10.160.25.205"
#LDE_NETMASK="255.255.254.0"
#LDE_BROADCAST="10.160.24.255"
#LDE_GATEWAY="10.160.25.254"
### VLAN UNIX
LDE_HOSTNAME="evbverif0"
LDE_MAC="0:2:b3:6:2e:0"
LDE_IP="10.160.40.135"
LDE_NETMASK="255.255.254.0"
LDE_BROADCAST="10.160.41.255"
LDE_GATEWAY="10.160.41.254"
### COMMON
LDE_NFSSERVER="$laramade"
LDE_NFSPATH="$VTE_NFSPREFIX/ESTR_rc149c/BUILD/vte-${VTE_VERSION}-${PLATFORM}"
LDE_FTPSERVER="$laramade"
LDE_FTPUSER="ftprc149c"
LDE_FTPPASSWD="mdpdeouf"

#
# SVG Configuration
#
### VIRTIO/WINBLOWS
#SVG_HOSTNAME="LVT_VIRTIO_SVG"
#SVG_MAC="18:0b:ad:da:db:ab"
#SVG_IP="10.160.25.205"
#SVG_NETMASK="255.255.254.0"
#SVG_BROADCAST="10.160.24.255"
#SVG_GATEWAY="10.160.25.254"
### VLAN UNIX
SVG_HOSTNAME="evbverif0"
SVG_MAC="0:2:b3:6:2e:0"
SVG_IP="10.160.40.135"
SVG_NETMASK="255.255.254.0"
SVG_BROADCAST="10.160.41.255"
SVG_GATEWAY="10.160.41.254"
### COMMON
SVG_NFSSERVER="$laramade"
SVG_NFSPATH="$VTE_NFSPREFIX/ESTR_svan01c/BUILD/vte-${VTE_VERSION}-${PLATFORM}"
SVG_FTPSERVER="$laramade"
SVG_FTPUSER="ftpsvan01c"
SVG_FTPPASSWD="mdpdeouf"

#
# CGD Configuration
#
### VIRTIO
#CGD_HOSTNAME="LVT_VIRTIO_CGD"
#CGD_IP="10.160.36.203"
#CGD_NETMASK="255.255.255.0"
#CGD_BROADCAST="10.160.36.255"
#CGD_GATEWAY="10.160.36.254"
### EVB VLAN WINBLOWS
#CGD_HOSTNAME="LVT_VIRTIO_CGD"
#CGD_MAC="18:00:de:ad:be:ef"
#CGD_IP="10.160.25.206"
#CGD_NETMASK="255.255.254.0"
#CGD_BROADCAST="10.160.24.255"
#CGD_GATEWAY="10.160.25.254"
### EVB VLAN UNIX
CGD_HOSTNAME="evbverif1"
CGD_MAC="0:2:b3:6:2e:1"
CGD_IP="10.160.40.136"
CGD_NETMASK="255.255.254.0"
CGD_BROADCAST="10.160.41.255"
CGD_GATEWAY="10.160.41.254"
### COMMON
CGD_NFSSERVER="$laramade"
CGD_NFSPATH="$VTE_NFSPREFIX/ESTR_cgag1c/BUILD/vte-${VTE_VERSION}-${PLATFORM}"
CGD_FTPSERVER="$laramade"
CGD_FTPUSER="ftpcgag1c"
CGD_FTPPASSWD="mdpdeouf"

#
# BGI Configuration
#
### VIRTIO
#BGI_HOSTNAME="LVT_VIRTIO_BGI"
#BGI_IP="10.160.36.203"
#BGI_NETMASK="255.255.255.0"
#BGI_BROADCAST="10.160.36.255"
#BGI_GATEWAY="10.160.36.254"
### EVB VLAN WINBLOWS
#BGI_HOSTNAME="LVT_VIRTIO_BGI"
#BGI_MAC="18:00:de:ad:be:ef"
#BGI_IP="10.160.25.206"
#BGI_NETMASK="255.255.254.0"
#BGI_BROADCAST="10.160.24.255"
#BGI_GATEWAY="10.160.25.254"
### EVB VLAN UNIX
BGI_HOSTNAME="evbverif2"
BGI_MAC="0:2:b3:6:2e:2"
BGI_IP="10.160.40.137"
BGI_NETMASK="255.255.254.0"
BGI_BROADCAST="10.160.41.255"
BGI_GATEWAY="10.160.41.254"
### COMMON
BGI_NFSSERVER="$laramade"
BGI_NFSPATH="$VTE_NFSPREFIX/ESTR_r70160c/BUILD/vte-${VTE_VERSION}-${PLATFORM}"
BGI_FTPSERVER="$laramade"
BGI_FTPUSER="ftpr70160c"
BGI_FTPPASSWD="mdpdeouf"

#
# ODV: Olivier Davard
#
### EVB VLAN UNIX
ODV_HOSTNAME="evbverif1"
ODV_MAC="0:2:b3:6:2e:1"
ODV_IP="10.160.40.136"
ODV_NETMASK="255.255.254.0"
ODV_BROADCAST="10.160.41.255"
ODV_GATEWAY="10.160.41.254"
### COMMON
ODV_NFSSERVER="$laramade"
ODV_NFSPATH="$VTE_NFSPREFIX/ESTR_b02578/BUILD/vte-${VTE_VERSION}-${PLATFORM}"
ODV_FTPSERVER="$laramade"
ODV_FTPUSER="ftpcgag1c"
ODV_FTPPASSWD="mdpdeouf"

VTE_CONFIG_ID=L26_1_5_COV-SCMA11-gtk
export VTE_CONFIG_ID

usage()
{
    cat <<EOF
Usage: ${0##*/} [-C|--coverage] --<METHOD> [-n|--no-mount] <USER>

Where:
  - METHOD: ftp (cramfs nfs jffs2)
  - USER: $USERS

EOF
    exit 1
}

action()
{
    echo "$*" >>$LOG
    eval "$* 2>&1 >>$LOG" && echo "OK." || { echo -e "FAILED.\nLog file is $LOG\nExiting."; exit 1;}
}

eval_user()
{
    echo -n "=> [USER] Initializing user's settings ($USER)..."
    export HOME=/root
    HOSTNAME=$(eval echo \$${USER}_HOSTNAME)
    MAC=$(eval echo \$${USER}_MAC)
    IP=$(eval echo \$${USER}_IP)
    NETMASK=$(eval echo \$${USER}_NETMASK)
    BROADCAST=$(eval echo \$${USER}_BROADCAST)
    GATEWAY=$(eval echo \$${USER}_GATEWAY)
    NFSSERVER=$(eval echo \$${USER}_NFSSERVER)
    NFSPATH=$(eval echo \$${USER}_NFSPATH)
    FTPSERVER=$(eval echo \$${USER}_FTPSERVER)
    FTPPASSWD=$(eval echo \$${USER}_FTPPASSWD)
    FTPUSER=$(eval echo \$${USER}_FTPUSER)
    # PS1 did not work with ash 
    #export PS1="[\u@\h:\w]$ " 
    #export PS1="$USER@$HOSTNAME$ "
    # cosmetic action! ;o)
    action true
}

init_network()
{
    /etc/init.d/networking stop 2>&1 > /dev/null || :
    echo -n "=> [NET] Setting mac address to $MAC..."
    action /sbin/ifconfig eth0 hw ether $MAC
    echo -n "=> [NET] Setting hostname to $HOSTNAME..."
    action hostname $HOSTNAME
    echo -n "=> [NET] Configuring eth0 ($IP)..."
    #ifdown eth0 2>>$LOG || :
    #route del default 2>>$LOG || :
    action ifconfig eth0 $IP broadcast $BROADCAST netmask $NETMASK
    echo -n "=> [NET] Configuring routes..."
    action route add default gw $GATEWAY eth0
    echo -n "=> [NET] Configuring loopback..."
    action /sbin/ifconfig lo 127.0.0.1 netmask 255.0.0.0
    
}

mount_nfs()
{
    VTE_DIR=$VTE_NFS_DIR
    echo -n "=> [NFS] Creating $VTE_NFS_DIR..."
    action mkdir -p $VTE_NFS_DIR
    [ "x$nomount" != "xyes" ] && {
        echo -n "=> [NFS] Mounting VTE NFS directory in $VTE_NFS_DIR..."
        action mount -t nfs $NFSSERVER:$NFSPATH $VTE_NFS_DIR
    }
}

init_ftp()
{
    echo -n "=> [FTP] Initializing ..."
    cat > $HOME/.netrc <<EOF
machine $FTPSERVER
login $FTPUSER
password $FTPPASSWD
EOF
    chmod 0600 $HOME/.netrc
    export VTE_FTPSERVER=$FTPSERVER
    # cosmetic action! ;o)
    action true
}


send_file()
{
    if [ x$VTE_FTP = "xyes" ];
        then
        ftp $FTPSERVER <<EOF
cd $VTE_CONFIG_ID/results.cov
lcd `dirname $1`
put `basename $1`
EOF
    elif [ x$VTE_NFS = "xyes" ];
    	then
	[ ! -d $VTE_DIR/results.cov ] && mkdir -p $VTE_DIR/results.cov
	cp $1 $VTE_DIR/results.cov
    else
        sz $1
    fi
}

init_cov()
{
    if [ ! -d /proc/gcov ];
        then
        if [ -f /lib/modules/`uname -r`/kernel/drivers/gcov/gcov-proc.ko ];
            then
            echo -n "=> [COV] Loading gcov-proc..."
            action insmod /lib/modules/`uname -r`/kernel/drivers/gcov/gcov-proc.ko
        else
            echo "ERROR: this kernel doesn't support lcov!"
        fi    
    fi
    if [ -d /proc/gcov ];
        then
        echo -n "=> [COV] Archiving kernel boot coverage..."
        action "cd /proc/gcov && tar -cf /tmp/KERNEL_BOOT.gcov.tar ."
        echo -n "=> [COV] Sending archive..."
        action send_file /tmp/KERNEL_BOOT.gcov.tar
        echo -n "=> [COV] Cleaning away..."
        action rm -f /tmp/KERNEL_BOOT.gcov.tar
    fi
}

init_cramfs()
{
    VTE_DIR=$HOME/vte
    echo -n "=> [VTE] Copying /vte to $VTE_DIR..."
    action cp -R /vte $VTE_DIR 
}



#
#
#
[ $# -gt 3 ] && usage
[ $# -lt 2 ] && usage

if [ $1 = "-C" -o $1 = "--coverage" ];
    then
    VTE_COV=yes
    shift
fi

case $1 in
    --nfs)
        export VTE_METHOD=NFS
        export VTE_NFS=yes
        case $2 in
            -n|--no-mount)
                shift
                nomount=yes
                ;;
            *)
                ;;
        esac
        ;;
    --ftp)
        export VTE_METHOD=FTP
        export VTE_FTP=yes
        ;;
    --cramfs)
        export VTE_METHOD=CRAMFS
        export VTE_CRAMFS=yes
        ;;
    --jffs2)
        export VTE_METHOD=JFFS2
        export VTE_JFFS2=yes
        ;;
    *)
        echo "Unknow method: $1"
        usage
        ;;
esac

case $2 in
    CGD|LDE|BGI|SVG|ODV)
        USER=$2
        ;;
    *)
        echo "Unknow user: $2"
        usage
        ;;
esac

dmesg -n 1
LOG=/tmp/${0}_${USER}_`date "+%Y%m%d%H%M%S"`.log
rm -f $LOG
touch $LOG
FTP=yes

eval_user

[ "x$nomount" != "xyes" ] && init_network
[ x$VTE_NFS = "xyes" ] && mount_nfs
[ x$VTE_CRAMFS = "xyes" ] && init_cramfs
[ x$VTE_FTP = "xyes" ] && init_cramfs
[ x$VTE_FTP = "xyes" ] && init_ftp
[ x$VTE_COV = "xyes" ] && init_cov



export PATH=$PATH:$VTE_DIR:$VTE_DIR/testcases/bin
cd $VTE_DIR
echo "** Welcome to VTE $VTE_VERSION"
#exec /bin/ash
