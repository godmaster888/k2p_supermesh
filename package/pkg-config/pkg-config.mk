################################################################################
#
# pkgconf
#
################################################################################

#PKGCONF_VERSION = 0.9.12
#PKGCONF_SITE = https://github.com/pkgconf/pkgconf/releases/download/pkgconf-$(PKGCONF_VERSION)
#PKGCONF_SOURCE = pkgconf-$(PKGCONF_VERSION).tar.bz2
PKG_CONFIG_VERSION = 0.9.12
PKG_CONFIG_SITE = $(DLINK_GIT_STORAGE)/pkg-config
PKGCONF_LICENSE = ISC
PKGCONF_LICENSE_FILES = COPYING


PKG_CONFIG_HOST_BINARY = $(HOST_DIR)/usr/bin/pkg-config

define PKG_CONFIG_LINK_PKGCONFIG
	ln -sf pkgconf $(TARGET_DIR)/usr/bin/pkg-config
endef

define HOST_PKG_CONFIG_INSTALL_WRAPPER
	$(INSTALL) -m 0755 -D package/pkg-config/pkg-config.in \
		$(HOST_DIR)/usr/bin/pkg-config
	$(SED) 's,@PKG_CONFIG_LIBDIR@,$(STAGING_DIR)/usr/lib/pkgconfig:$(STAGING_DIR)/usr/share/pkgconfig,' \
		-e 's,@STAGING_DIR@,$(STAGING_DIR),' \
		$(HOST_DIR)/usr/bin/pkg-config
endef

define HOST_PKG_CONFIG_STATIC
	$(SED) 's,@STATIC@,--static,' $(HOST_DIR)/usr/bin/pkg-config
endef

define HOST_PKG_CONFIG_SHARED
	$(SED) 's,@STATIC@,,' $(HOST_DIR)/usr/bin/pkg-config
endef

PKG_CONFIG_POST_INSTALL_TARGET_HOOKS += PKG_CONFIG_LINK_PKGCONFIG
HOST_PKG_CONFIG_POST_INSTALL_HOOKS += HOST_PKG_CONFIG_INSTALL_WRAPPER

ifeq ($(BR2_STATIC_LIBS),y)
HOST_PKG_CONFIG_POST_INSTALL_HOOKS += HOST_PKG_CONFIG_STATIC
else
HOST_PKG_CONFIG_POST_INSTALL_HOOKS += HOST_PKG_CONFIG_SHARED
endif

#$(eval $(autotools-package))
#$(eval $(host-autotools-package))
$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
