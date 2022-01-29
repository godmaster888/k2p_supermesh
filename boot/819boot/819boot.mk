819BOOT_SITE = $(DLINK_GIT_STORAGE)/$(BR2_BOOTLOADER_EXTERNAL_LOCATION)
819BOOT_VERSION = master

819BOOT_MAKE_OPTS += \
	BR2_PRIVATE_KEY=$(BR2_PRIVATE_KEY) \
	BR2_FW_UUID=$(BR2_FW_UUID) \
	BR2_FW_MAGIC_VERSION=$(BR2_FW_MAGIC_VERSION) \
	BR2_DUAL_SIGN_FW=$(BR2_DUAL_SIGN_FW) \
	BR2_FW_UUID_SEC=$(BR2_FW_UUID_SEC) \
	BR2_USE_CUSTOM_LABEL=$(BR2_USE_CUSTOM_LABEL) \
	BR2_CUSTOM_LABEL=$(BR2_CUSTOM_LABEL) \
	BR2_DUAL_BOOT_FW=$(BR2_DUAL_BOOT_FW)

define 819BOOT_CONFIGURE_CMDS
	@if [ -f "$(@D)/dlink/config_$(PROFILE_FULL)" ]; then \
		cp $(@D)/dlink/config_$(PROFILE_FULL) $(@D)/.config; \
	elif [ -f "$(@D)/dlink/config_$(BR2_DLINK_GENERIC_PROFILE)" ]; then \
		cp $(@D)/dlink/config_$(BR2_DLINK_GENERIC_PROFILE) $(@D)/.config; \
	else \
		$(call MESSAGE, "ERROR: 819boot config for $(PROFILE_FULL) does not exist"); \
		exit 1; \
	fi
	$(MAKE1) -C $(@D) $(819BOOT_MAKE_OPTS) clean configure
endef

define 819BOOT_BUILD_CMDS
	$(MAKE1) -C $(@D) $(819BOOT_MAKE_OPTS) all
endef

define 819BOOT_CLEAN_CMDS
	$(MAKE1) -C $(@D) $(819BOOT_MAKE_OPTS) clean
endef

define 819BOOT_INSTALL_TARGET_CMDS
	@if [ -L "$(PROFILE_DIR)/$(BR2_BOOTLOADER_INSTALL_FILE)" ]; then \
		echo "$(PROFILE_DIR) has a symlink to boot, don't update!"; \
	else \
		cp $(@D)/boot/Output/$(BR2_BOOTLOADER_INSTALL_FILE) $(PROFILE_DIR)/$(BR2_BOOTLOADER_INSTALL_FILE); \
	fi
endef

$(eval $(call GENTARGETS))

PHONY += 819boot-olddefconfig 819boot-menuconfig

819boot-olddefconfig:
	@if [ -f "$(819BOOT_DIR)/dlink/config_$(PROFILE_FULL)" ]; then \
		cp $(819BOOT_DIR)/dlink/config_$(PROFILE_FULL) $(819BOOT_DIR)/.config; \
	elif [ -f "$(819BOOT_DIR)/dlink/config_$(BR2_DLINK_GENERIC_PROFILE)" ]; then \
		cp $(819BOOT_DIR)/dlink/config_$(BR2_DLINK_GENERIC_PROFILE) $(819BOOT_DIR)/.config; \
	else \
		$(call MESSAGE, "ERROR: 819boot config for $(PROFILE_FULL) does not exist"); \
		exit 1; \
	fi

	$(MAKE1) -C $(819BOOT_DIR) $(819BOOT_MAKE_OPTS) configure

	@if [ -f "$(819BOOT_DIR)/dlink/config_$(PROFILE_FULL)" ]; then \
		cp $(819BOOT_DIR)/.config $(819BOOT_DIR)/dlink/config_$(PROFILE_FULL); \
	elif [ -f "$(819BOOT_DIR)/dlink/config_$(BR2_DLINK_GENERIC_PROFILE)" ]; then \
		cp $(819BOOT_DIR)/.config $(819BOOT_DIR)/dlink/config_$(BR2_DLINK_GENERIC_PROFILE); \
	fi

819boot-menuconfig:
	@if [ ! -f $(819BOOT_DIR)/.config ]; then \
		if [ -f "$(819BOOT_DIR)/dlink/config_$(PROFILE_FULL)" ]; then \
			cp $(819BOOT_DIR)/dlink/config_$(PROFILE_FULL) $(819BOOT_DIR)/.config; \
		elif [ -f "$(819BOOT_DIR)/dlink/config_$(BR2_DLINK_GENERIC_PROFILE)" ]; then \
			cp $(819BOOT_DIR)/dlink/config_$(BR2_DLINK_GENERIC_PROFILE) $(819BOOT_DIR)/.config; \
		else \
			$(call MESSAGE, "ERROR: 819boot config for $(PROFILE_FULL) does not exist"); \
			exit 1; \
		fi \
	fi
	$(MAKE1) -C $(819BOOT_DIR) $(819BOOT_MAKE_OPTS) menuconfig
