RTK_MS_UBOOT_SITE = $(DLINK_GIT_STORAGE)/$(BR2_BOOTLOADER_EXTERNAL_LOCATION)
RTK_MS_UBOOT_VERSION = $(call qstrip,$(BR2_BOOTLOADER_EXTERNAL_BRANCH))

ifeq ($(RTK_MS_UBOOT_VERSION),)
RTK_MS_UBOOT_VERSION := rtk-mswitch
endif

# need kernel config
RTK_MS_UBOOT_DEPENDENCIES += linux26
# .. and RTK MS SDK sources
RTK_MS_UBOOT_DEPENDENCIES += rtk_ms_sdk

RTK_MS_UBOOT_MAKE_OPTS += \
	CROSS_COMPILE=$(TARGET_CROSS)	\
	CONFIG_LINUXDIR=linux-3.18.24.x	\
	CONFIG_MODULES=y



define RTK_MS_UBOOT_CONFIGURE_CMDS
	$(Q)mkdir -p $(BASE_DIR)/kernel
	$(Q)ln -snf $(LINUX26_DIR) $(BASE_DIR)/kernel/uClinux
	$(Q)if [ -f $(PROFILE_DIR)/uboot.config ]; then			\
		cp -vf $(PROFILE_DIR)/uboot.config $(@D)/.config; 	\
	elif [ -f $(@D)/config_$(PROFILE) ]; then			\
		cp -vf $(@D)/config_$(PROFILE) $(@D)/.config; 		\
	else								\
		echo "Not found U-Boot config for $(PROFILE)";		\
		false;							\
	fi && true || false
	yes "" | $(RTK_MS_UBOOT_MAKE_OPTS) $(MAKE1) -C $(@D) rest_config
endef

define RTK_MS_UBOOT_BUILD_CMDS
	$(RTK_MS_UBOOT_MAKE_OPTS) $(MAKE) -C $(@D)
endef

define RTK_MS_UBOOT_CLEAN_CMDS
	$(RTK_MS_UBOOT_MAKE_OPTS) $(MAKE) -C $(@D) clean
endef

define RTK_MS_UBOOT_INSTALL_TARGET_CMDS
	@if [ -L "$(PROFILE_DIR)/$(BR2_BOOTLOADER_INSTALL_FILE)" ]; then \
		echo "$(PROFILE_DIR) has a symlink to boot, don't update!"; \
	else \
		cp -f $(@D)/u-boot.bin $(PROFILE_DIR)/$(BR2_BOOTLOADER_INSTALL_FILE); \
	fi
endef

$(eval $(call GENTARGETS))

rtk_ms_uboot-menuconfig: rtk_ms_uboot-configure
	rm -f $(RTK_MS_UBOOT_DIR)/.stamp_{built,target_installed}
	$(RTK_MS_UBOOT_MAKE_OPTS) $(MAKE1) -C $(RTK_MS_UBOOT_DIR) menuconfig

host-rtk_ms_mkimage: rtk_ms_uboot-configure
	$(RTK_MS_UBOOT_MAKE_OPTS) $(MAKE) -C $(RTK_MS_UBOOT_DIR) tools
	cp -f $(RTK_MS_UBOOT_DIR)/tools/mkimage $(HOST_DIR)/usr/bin/mkimage
