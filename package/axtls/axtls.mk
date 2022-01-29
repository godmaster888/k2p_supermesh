################################################################################
#
# axtls
#
################################################################################

AXTLS_VERSION = 1.4.9
AXTLS_SITE_METHOD = git
AXTLS_SITE = $(DLINK_GIT_STORAGE)/axtls
AXTLS_LICENSE = BSD-3-Clause
AXTLS_LICENSE_FILES = www/index.html
AXTLS_CONFIG_FILE = package/axtls/config
AXTLS_BUILD_CONFIG = $(@D)/config/.config
AXTLS_INSTALL_STAGING = YES
AXTLS_CFLAGS = $(TARGET_CFLAGS)

ifeq ($(BR2_PACKAGE_CIVETWEB),y)
	AXTLS_CFLAGS += -DCIVETWEB
endif

define AXTLS_CONFIGURE_CMDS
	$(INSTALL) -D -m 0644 $(AXTLS_CONFIG_FILE) $(AXTLS_BUILD_CONFIG)
	$(MAKE) -C $(@D) oldconfig
endef

define AXTLS_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define AXTLS_BUILD_CMDS
	$(MAKE) -C $(@D) \
	           CC="$(TARGET_CC)" \
	           STRIP="/bin/true" \
	           OPT_CFLAGS="$(AXTLS_CFLAGS) -fPIC" OPT_LDFLAGS="" \
	           all
endef

define AXTLS_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) PREFIX="$(TARGET_DIR)/usr/" install
endef

define AXTLS_INSTALL_STAGING_CMDS
	$(MAKE) -C $(@D) PREFIX="$(STAGING_DIR)/usr/" install
endef

$(eval $(call GENTARGETS))
