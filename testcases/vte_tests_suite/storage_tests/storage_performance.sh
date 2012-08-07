#!/bin/bash

echo "auto test all storage performance"
echo "need edit /etc/fstab as below"
cat << EOF
/dev/mtdblock10       /mnt/nand  jffs2   defaults          0       0
/dev/mmcblk0p1       /mnt/mmcblk0p1  ext3 noauto            0       0
/dev/mmcblk1p1       /mnt/mmcblk1p1  ext3 noauto            0       0
/dev/sda1       /mnt/sda1  ext3   defaults        0       0
/dev/sdb1       /mnt/sdb1  ext3   defaults        0       0
ubi0:test        /mnt/nand   ubifs no_bulk_read,sync,noauto,rw       0       0

EOF

while [ -e $1 ]
do
 echo "test $1"
 mkdir -p $1
 iozone -a -i 1 -i 0 -U $1 -s 1024m -f ${1}/iozone.test -R
 shift
done

