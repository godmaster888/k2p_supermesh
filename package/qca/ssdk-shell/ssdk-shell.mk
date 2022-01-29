SSDK_SHELL_VERSION = master
SSDK_SHELL_SITE = $(DLINK_GIT_STORAGE)/ssdk-shell
SSDK_SHELL_LICENSE = ISC
SSDK_SHELL_LICENSE_FILES =
# требует хидеры ядра
SSDK_SHELL_DEPENDENCIES = linux26

$(warning ssdk shell build opts are ugly hardcoded)
SSDK_SHELL_OPTS += \
		TOOL_PATH=$(TOOLCHAIN_DIR)/usr/bin \
		SYS_PATH=$(LINUX26_DIR) \
		TOOLPREFIX=$(BR2_TOOLCHAIN_EXTERNAL_PREFIX)- \
		KVER=4.4.7 \
		ARCH=$(TARGET_ARCH)


#$(TARGET_MAKE_ENV) ?
# тоже тухляк с депендами, как и в qca-ssdk. Сборка в один поток.
define SSDK_SHELL_BUILD_CMDS
	$(MAKE1) -C $(@D) $(strip $(SSDK_SHELL_OPTS))
endef

define SSDK_SHELL_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/build/bin/ssdk_sh $(TARGET_DIR)/sbin
endef

$(eval $(call GENTARGETS))
