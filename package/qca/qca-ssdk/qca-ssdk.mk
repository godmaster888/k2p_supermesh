QCA_SSDK_VERSION = release/endive_preview_cc
QCA_SSDK_SITE = $(DLINK_GIT_STORAGE)/qca-ssdk
QCA_SSDK_LICENSE = ISC
QCA_SSDK_LICENSE_FILES =
QCA_SSDK_DEPENDENCIES = linux26
QCA_SSDK_INSTALL_STAGING = YES

$(warning qca ssdk build opts are ugly hardcoded)
QCA_SSDK_OPTS += \
		TOOL_PATH=$(TOOLCHAIN_DIR)/usr/bin \
		SYS_PATH=$(LINUX26_DIR) \
		TOOLPREFIX=$(BR2_TOOLCHAIN_EXTERNAL_PREFIX)- \
		KVER=4.4.7 \
		ARCH=$(TARGET_ARCH) \
		TARGET_SUFFIX="musl" \
		GCC_VERSION=6.3.0
#		 \
#		CFLAGS="-I$(STAGING_DIR)/usr/include -DBITS_PER_LONG=32"

QCA_SSDK_OPTS += HNAT_FEATURE=enable
#QCA_SSDK_OPTS += RFS_FEATURE=enable
# чтобы только ISISC собирался, см. про NONHK_CHIP в самом низу $(@D)/config
QCA_SSDK_OPTS += HK_CHIP=enable
QCA_SSDK_OPTS += SFE_FEATURE=enable
# наш код в хнате
QCA_SSDK_OPTS += DRU_FEATURE=enable

QCA_SSDK_OPTS += DRU_CROSS_COMPILE=$(TARGET_CROSS)



define QCA_SSDK_CONFIGURE_CMDS
	@echo restrict unused chips
	sed -ri 's/^CHIP_TYPE=ALL_CHIP/CHIP_TYPE=ISISC/1' $(@D)/config
	@echo avoid source list redefinition
	sed -ri 's/^IN_MALIBU_PHY=TRUE/IN_MALIBU_PHY=FALSE/1' $(@D)/config
endef

#$(TARGET_MAKE_ENV) ?
# MAKE1 из-за косых депендов у китайцев, валится обычно на цели kslib_o
define QCA_SSDK_BUILD_CMDS
	$(MAKE1) -C $(@D) $(strip $(QCA_SSDK_OPTS))
endef

# $(TARGET_MAKE_ENV) $(MAKE) $(LINUX26_MAKE_FLAGS) -C $(LINUX26_DIR) M=$(@D)/temp modules_install ?
define QCA_SSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -d $(LINUX_MODULE_DIR)
	$(INSTALL) -m 0644 $(@D)/build/bin/qca-ssdk.ko $(LINUX_MODULE_DIR)
endef

define QCA_SSDK_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(@D)/include/api/sw_ioctl.h $(STAGING_DIR)/usr/include/qca/api/sw_ioctl.h
	$(INSTALL) -D -m 0644 $(@D)/include/common/sw_error.h $(STAGING_DIR)/usr/include/qca/common/sw_error.h
endef

define QCA_SSDK_UNINSTALL_STAGING_CMDS
	rm -f $(STAGING_DIR)/usr/include/qca/api/sw_ioctl.h
	rm -f $(STAGING_DIR)/usr/include/qca/common/sw_error.h
endef

$(eval $(call GENTARGETS))

LINUX26_SYMVERS_DIRS += $(QCA_SSDK_DIR)/temp
