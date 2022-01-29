#############################################################
#
# BEARSSL
#
#############################################################
BEARSSL_VERSION = master
BEARSSL_LICENSE = MIT
BEARSSL_LICENSE_FILES = LICENSE.txt

BEARSSL_SITE = $(DLINK_GIT_STORAGE)/bearssl
BEARSSL_INSTALL_STAGING = YES

BEARSSL_MAKE_ENV = \
				   $(TARGET_MAKE_ENV) \
				   CC="$(TARGET_CC)" \
				   LD="$(TARGET_LD)"

define BEARSSL_BUILD_CMDS
	$(BEARSSL_MAKE_ENV) $(MAKE1) -C $(@D) all
endef

define BEARSSL_CLEAN_CMDS
	$(BEARSSL_MAKE_ENV) $(MAKE1) -C $(@D) clean
endef

define BEARSSL_INSTALL
	install -D -v -t $(1)/usr/lib             $(@D)/build/libbearssl.*
	install -D -v -t $(1)/usr/include/bearssl $(@D)/inc/*
endef
define BEARSSL_UNINSTALL
	rm -rf $(1)/usr/lib/libbearssl.*
	rm -rf $(1)/usr/include/bearssl
endef

define BEARSSL_INSTALL_STAGING_CMDS
	$(call BEARSSL_INSTALL,$(STAGING_DIR))
endef

define BEARSSL_UNINSTALL_STAGING_CMDS
	$(call BEARSSL_UNINSTALL,$(STAGING_DIR))
endef

define BEARSSL_INSTALL_TARGET_CMDS
	$(call BEARSSL_INSTALL,$(TARGET_DIR))
endef

define BEARSSL_UNINSTALL_TARGET_CMDS
	$(call BEARSSL_UNINSTALL,$(TARGET_DIR))
endef

$(eval $(call GENTARGETS))
