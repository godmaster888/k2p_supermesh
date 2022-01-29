#############################################################
#
# libnl-tiny
#
#############################################################

LIBNL_TINY_VERSION = svn39579
LIBNL_TINY_SITE = $(DLINK_GIT_STORAGE)/libnl-tiny
LIBNL_TINY_LICENSE = LGPL-2.1-only
LIBNL_TINY_LICENSE_FILES =
LIBNL_TINY_INSTALL_STAGING = YES

define LIBNL_TINY_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" CFLAGS="$(TARGET_CFLAGS) -fPIC" LDFLAGS="$(TARGET_LDFLAGS)" -C $(@D)
endef

define LIBNL_TINY_INSTALL_STAGING_CMDS
	$(INSTALL) -d $(STAGING_DIR)/usr/include/libnl-tiny
	cp -R $(@D)/include/* $(STAGING_DIR)/usr/include/libnl-tiny/
	$(INSTALL) -D -m 0755 $(@D)/libnl-tiny.so $(STAGING_DIR)/usr/lib/
endef

define LIBNL_TINY_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libnl-tiny.so $(TARGET_DIR)/usr/lib/
endef

$(eval $(call GENTARGETS))
