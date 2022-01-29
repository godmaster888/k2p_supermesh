IMPROXY_SITE = $(DLINK_GIT_STORAGE)/mldproxy
IMPROXY_VERSION = improxy
IMPROXY_LICENSE = LGPL-3.0-or-later
IMPROXY_LICENSE_FILES =
IMPROXY_LDFLAGS =

ifeq ($(BR2_DSYSINIT),y)
	IMPROXY_LDFLAGS += -ljansson -ld_service_notify -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib
	IMPROXY_DEPENDENCIES += deuteron_framework
endif


define IMPROXY_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS) $(IMPROXY_LDFLAGS)" $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) improxy
endef

define IMPROXY_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define IMPROXY_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/improxy $(TARGET_DIR)/usr/bin/improxy
	$(TARGET_STRIP) $(TARGET_DIR)/usr/bin/improxy
endef

$(eval $(call GENTARGETS))
