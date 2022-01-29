#############################################################
#
# p0f
#
#############################################################
P0F_VERSION = 3.06b
P0F_SITE = ${DLINK_GIT_STORAGE}/p0f
P0F_LICENSE = LGPL-2.1-or-later
P0F_LICENSE_FILES = docs/COPYING
P0F_DEPENDENCIES = libpcap
P0F_INSTALL_STAGING = YES

define P0F_BUILD_CMDS
	$(MAKE) V=99 CC="$(TARGET_CC)" LD="$(TARGET_LD)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" -C $(@D)
endef

define P0F_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/p0f $(TARGET_DIR)/usr/bin/
	$(INSTALL) -D -m 0644 $(@D)/p0f.fp $(TARGET_DIR)/etc/
endef
define P0F_INSTALL_STAGING_CMDS
	$(INSTALL) -d $(STAGING_DIR)/usr/include/p0f/
	$(INSTALL) -D -m 0644 $(@D)/types.h $(STAGING_DIR)/usr/include/p0f/
	$(INSTALL) -D -m 0644 $(@D)/config.h $(STAGING_DIR)/usr/include/p0f/
	$(INSTALL) -D -m 0644 $(@D)/alloc-inl.h $(STAGING_DIR)/usr/include/p0f/
	$(INSTALL) -D -m 0644 $(@D)/debug.h $(STAGING_DIR)/usr/include/p0f/
	$(INSTALL) -D -m 0644 $(@D)/api.h $(STAGING_DIR)/usr/include/p0f/
endef


$(eval $(call GENTARGETS))
