MTK_BOOT_SITE = $(DLINK_GIT_STORAGE)/$(BR2_BOOTLOADER_EXTERNAL_LOCATION)
MTK_BOOT_VERSION = $(call qstrip,$(BR2_BOOTLOADER_EXTERNAL_BRANCH))

MTK_BOOT_MAKE_OPTS =

ifeq ($(BR2_OBFUSCATE_FW),y)
MTK_BOOT_MAKE_OPTS += OBFUSCATE_FW=y
endif

ifeq ($(BR2_BOOTLOADER_SIGN_FW_CHECK),y)
MTK_BOOT_MAKE_OPTS += FW_SIGN_CHECK=y
ifeq ($(BR2_DUAL_SIGN_FW),y)
MTK_BOOT_MAKE_OPTS += FW_DUAL_SIGN=y
endif
endif

ifneq (n,$(filter on,$(BR2_BOOTLOADER_SIGN_FW_CHECK)$(BR2_OBFUSCATE_FW)))
MTK_BOOT_MAKE_OPTS += FW_UUID='$(BR2_FW_UUID)'
ifeq ($(BR2_DUAL_SIGN_FW),y)
MTK_BOOT_MAKE_OPTS += FW_UUID_SEC='$(BR2_FW_UUID_SEC)'
endif
endif

define MTK_BOOT_CONFIGURE_CMDS
	cp $(@D)/$(BR2_BOOTLOADER_CONFIG_FILE) $(@D)/.config
	$(MAKE1) -C $(@D) $(MTK_BOOT_MAKE_OPTS) clean configure
endef

define MTK_BOOT_BUILD_CMDS
	$(MAKE1) -C $(@D) $(MTK_BOOT_MAKE_OPTS) all
endef

define MTK_BOOT_CLEAN_CMDS
	$(MAKE1) -C $(@D) $(MTK_BOOT_MAKE_OPTS) clean
endef

define MTK_BOOT_INSTALL_TARGET_CMDS
	@if [ -L "$(PROFILE_DIR)/$(BR2_BOOTLOADER_INSTALL_FILE)" ]; then \
		echo "$(PROFILE_DIR) has a symlink to boot, don't update!"; \
	else \
		cp $(@D)/$(BR2_BOOTLOADER_INSTALL_FILE) $(PROFILE_DIR)/$(BR2_BOOTLOADER_INSTALL_FILE); \
	fi
endef

$(eval $(call GENTARGETS))

PHONY += mtk_boot-menuconfig

mtk_boot-menuconfig:
	@if [ ! -f $(MTK_BOOT_DIR)/.config ]; then \
		cp $(@D)/$(BR2_BOOTLOADER_CONFIG_FILE) $(@D)/.config; \
	fi
	$(MAKE1) -C $(MTK_BOOT_DIR) $(MTK_BOOT_MAKE_OPTS) menuconfig
