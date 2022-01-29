LIBRADIUS_SITE = $(DLINK_GIT_STORAGE)/libradius
LIBRADIUS_VERSION = master
LIBRADIUS_LICENSE = BSD-2-Clause
LIBRADIUS_LICENSE_FILES =
LIBRADIUS_DEPENDENCIES = libmd5-rfc
LIBRADIUS_INSTALL_STAGING = YES
LIBRADIUS_INSTALL_TARGET = YES

define LIBRADIUS_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define LIBRADIUS_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libradius.a $(STAGING_DIR)/lib/libradius.a
	$(INSTALL) -D -m 0644 $(@D)/radlib.h $(STAGING_DIR)/usr/include/radlib.h
	$(INSTALL) -D -m 0755 $(@D)/libradius.so $(STAGING_DIR)/lib/libradius.so
endef

define LIBRADIUS_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/libradius.so $(TARGET_DIR)/lib/libradius.so
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libradius.so
endef

$(eval $(call GENTARGETS,package,libradius))
