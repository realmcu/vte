#
# mandatory target:
# lbp_help lbp_ref2estr lbp_install_ref lbp_install lbp_clean lbp_mrproper
#
ifneq ($(LBP_VERSION),L2618-1)
$(error LBP version mismatch)
endif

lbp_help:
	@echo "* LBP/LNX targets:"
	@echo "  ================"
	@echo "  - lnx_install_ref: Install official kernel image"
	@echo "  - lnx_install: Install custom kernel image and modules (and config + build if needed)"
	@echo "  - lnx_defconfig: Configure custom linux kernel with $(LNX_DEFCONFIG)"
	@echo "  - lnx_menuconfig: Configure custom kernel via menu"
	@echo "  - lnx_oldconfig: Configure kernel with its current .config"
	@echo "  - lnx_build: Build custom kernel and modules"
	@echo "  - lnx_clean: FIXME"
	@echo "  - lnx_mrproper: FIXME"
	@echo ""
	@echo "* LBP/RFS targets:"
	@echo "  ================"
	@echo "  - rfs_install_ref: Install official RootDisk image."
	@echo "  - rfs_install: Install custom RootDisk image."
	@echo "  - rfs_clean: FIXME"
	@echo "  - rfs_mrproper: FIXME"
	@echo ""


lbp_ref2estr: lbp_mrproper
	@echo "$@: Not yet implemented"

lbp_install_ref: lnx_install_ref rfs_install_ref
ifeq ($(VTE_TARGET),Virtio)
	@echo "!!!"
	@echo "!!! Don't forget to install other Virtio files."
	@echo "!!!"
endif

lbp_install: lnx_install rfs_install
ifeq ($(VTE_TARGET),Virtio)
	@echo "!!!"
	@echo "!!! Don't forget to install otehr Virtio files."
	@echo "!!!"
endif

lbp_clean: lnx_clean rfs_clean

lbp_mrproper: lnx_mrproper rfs_mrproper


#
# Linux kernel stuff
#
KMAKE = make -C $(LNX_SRCDIR) $(LNX_MAKEFLAGS)

LNX_IMAGE_TARGET=$(LNX_IMAGE_DESTDIR)/zImage
RFS_IMAGE_TARGET=$(RFS_IMAGE_DESTDIR)/rootfs.cramfs

# Install official kernel image
lnx_install_ref:
	cp $(LNX_IMAGE_REF) $(LNX_IMAGE_TARGET)
	chmod a+r $(LNX_IMAGE_TARGET)
	du -sh $(LNX_IMAGE_TARGET)

# Install custom kernel image
# depend on custom image
lnx_install: $(LNX_IMAGE)
	sudo $(KMAKE) modules_install INSTALL_MOD_PATH=$(LNX_DESTDIR)
	cp $(LNX_IMAGE) $(LNX_IMAGE_TARGET)
	chmod a+r $(LNX_IMAGE_TARGET)
	du -sh $(LNX_IMAGE_TARGET)

# Custom Image depend on autoconf.h
$(LNX_IMAGE): $(LNX_BLDDIR)/include/linux/autoconf.h
	make lnx_build

# autoconf.h depend on .config
$(LNX_BLDDIR)/include/linux/autoconf.h: $(LNX_BLDDIR)/.config
	$(KMAKE) prepare-all

# If .config didn't exist, create the default one
$(LNX_BLDDIR)/.config:
	$(KMAKE) $(LNX_DEFCONFIG)

# Default config
lnx_defconfig:
	$(KMAKE) $(LNX_DEFCONFIG)

# Custom interactive config 
lnx_menuconfig:
	$(KMAKE) menuconfig

# Configure
lnx_oldconfig:
	$(KMAKE) oldconfig


lnx_build:
	$(KMAKE) zImage
#	#$(CROSS_COMPILE)objcopy -S -O binary $(LNX_BLDDIR)/vmlinux $(LNX_IMAGE)
	$(KMAKE) modules
	@echo -e "\n\n Execute \"make lnx_install\" To install $(LNX_IMAGE) as $(LNX_IMAGE_TARGET) and modules in $(LNX_DESTDIR)\n\n"

# 
lnx_clean:
	$(KMAKE) clean

lnx_mrproper: 
	$(KMAKE) mrproper

lnx_distclean: 
	$(KMAKE) distclean

#
# rootdisk stuff
#
ifeq ($(VTE_METHOD), cramfs)
# VTE by cramfs need to rebuild the rootdisk cramfs image
rfs_install_ref rfs_install: rfs_install_cramfs
endif
ifeq ($(VTE_METHOD), nfs)
# No rootdisk image
rfs_install_ref rfs_install: rfs_nothing_to_be_done
endif
ifeq ($(VTE_METHOD), jffs2)
$(error VTE_METHOD not supported: $VTE_METHOD)
endif
ifeq ($(VTE_METHOD), ftp)
rfs_install_ref: rfs_install_ref_cramfs
rfs_install: rfs_install_cramfs
endif


rfs_install_ref_cramfs:
	cp $(RFS_IMAGE_REF) $(RFS_IMAGE_TARGET)
	du -sh $(RFS_IMAGE_TARGET)

rfs_install_cramfs:
	tmp=`mktemp /tmp/rootfs.cramfs.XXXXXX`; sudo mkcramfs $(RFS_BLDDIR) $$tmp && mv $$tmp $(RFS_IMAGE)
	chmod ug+rw $(RFS_IMAGE)
	cp $(RFS_IMAGE) $(RFS_IMAGE_TARGET)
	chmod a+r $(RFS_IMAGE_TARGET)
	du -sh $(RFS_IMAGE_TARGET)

rfs_nothing_to_be_done:
	@echo "Nothing to perform when VTE_METHOD=$(VTE_METHOD)"


rfs_clean:
	@echo "$@: Not yet implemented"

rfs_mrproper: rfs_clean
	@echo "$@: Not yet implemented"



.PHONY: lbp_ref2estr lbp_install_ref lbp_install 
.PHONY: lbp_clean lbp_mrproper
.PHONY: lnx_install_ref lnx_install 
.PHONY: lnx_defconfig lnx_menuconfig lnx_oldconfig lnx_build 
.PHONY: lnx_clean lnx_mrproper
