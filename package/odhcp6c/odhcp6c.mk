#############################################################
#
# odhcp6c
#
#############################################################

ODHCP6C_VERSION = dsysinit
ODHCP6C_SITE = $(DLINK_GIT_STORAGE)/odhcp6c
ODHCP6C_LICENSE = GPL-2.0-only
ODHCP6C_LICENSE_FILES = COPYING

ifeq ($(BR2_DSYSINIT),y)
ODHCP6C_DEPENDENCIES += jansson deuteron_framework
ODHCP6C_LIBS += -ld_service_notify -ljansson
endif

define ODHCP6C_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS) $(ODHCP6C_LIBS) -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib" $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define ODHCP6C_CLEAN_CMDS
	${MAKE} -C $(@D) clean
endef

define ODHCP6C_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/odhcp6c $(TARGET_DIR)/usr/bin
endef

$(eval $(call GENTARGETS))
