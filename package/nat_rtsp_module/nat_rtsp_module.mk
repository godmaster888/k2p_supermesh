#############################################################
#
# nat-rtsp_module
#
#############################################################

NAT_RTSP_MODULE_VERSION = 0.7+4.18
NAT_RTSP_MODULE_SITE = $(DLINK_STORAGE)
#NAT_RTSP_MODULE_SITE = deb.debian.org/debian/pool/main/n/nat-rtsp
NAT_RTSP_MODULE_SOURCE = nat-rtsp_$(NAT_RTSP_MODULE_VERSION).orig.tar.xz
NAT_RTSP_MODULE_DEPENDENCIES = kernel
NAT_RTSP_MODULE_INSTALL_TARGET = YES


define NAT_RTSP_MODULE_BUILD_CMDS
	$(MAKE) V=99 CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
		ARCH=$(KERNEL_ARCH) \
		KERNELDIR=$(LINUX26_DIR) -C $(@D)
endef

define NAT_RTSP_MODULE_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define NAT_RTSP_MODULE_INSTALL_TARGET_CMDS
	$(INSTALL) -d $(LINUX_MODULE_DIR)
	$(INSTALL) -D -m 0644 $(@D)/nf_nat_rtsp.ko $(LINUX_MODULE_DIR)
	$(INSTALL) -D -m 0644 $(@D)/nf_conntrack_rtsp.ko $(LINUX_MODULE_DIR)
endef

$(eval $(call GENTARGETS))
