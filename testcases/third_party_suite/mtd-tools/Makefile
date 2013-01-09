#git clone from  git://git.infradead.org/mtd-utils.git
SUBDIRS = mtd-utils

CROSS_COMPILE := arm-none-linux-gnueabi-

NFSROOT := /mnt/nfs_root/imx28_rootfs_r/rootfs
zlib_path = $(CURDIR)/../../../openlibs/zlib-1.2.3/
export CFLAGS += -I$(zlib_path) -I$(CURDIR) 
export LDFLAGS = -L$(NFSROOT)/usr/lib

MTD_BINS = \
	ftl_format flash_erase nanddump doc_loadbios \
	ftl_check mkfs.jffs2 flash_lock flash_unlock \
	flash_otp_info flash_otp_dump mtd_debug flashcp nandwrite nandtest \
	jffs2dump \
	nftldump nftl_format docfdisk \
	rfddump rfdformat \
	serve_image recv_image \
	sumtool jffs2reader
UBI_BINS = \
	ubiupdatevol ubimkvol ubirmvol ubicrc32 ubinfo ubiattach \
	ubidetach ubinize ubiformat ubirename mtdinfo ubirsvol


BINS = $(MTD_BINS)
BINS += mkfs.ubifs/mkfs.ubifs
BINS += $(addprefix ubi-utils/,$(UBI_BINS))
IBINS += $(addprefix $(SUBDIRS)/,$(BINS))

all: zlib
	$(MAKE) -C $(SUBDIRS) CC=$(CROSS_COMPILE)gcc WITHOUT_XATTR=1 WITHOUT_LZO=1

install:
	mkdir bin
	-ln -f $(IBINS) bin/

clean:
	$(MAKE) -C $(SUBDIRS) clean
	-rm -rf bin

zlib:
	cd $(zlib_path); ./myconfig
	
