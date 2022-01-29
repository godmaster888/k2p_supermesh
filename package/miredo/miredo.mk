MIREDO_SITE = $(DLINK_GIT_STORAGE)/miredo
MIREDO_VERSION = master
MIREDO_LICENSE = GPL-2.0-or-later
MIREDO_LICENSE_FILES = COPYING
MIREDO_CONF_OPT = --disable-shared --disable-binreloc --disable-assert
MIREDO_INSTALL_STAGING = NO
MIREDO_INSTALL_TARGET = YES

ifeq ($(BR2_DSYSINIT),y)
MIREDO_DEPENDENCIES += jansson deuteron_framework
MIREDO_LDFLAGS += -ld_service_notify -ljansson -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib
endif

MIREDO_MAKE_OPT = CC="$(TARGET_CC)" LD="$(TARGET_LD)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS) $(MIREDO_LDFLAGS)"

define MIREDO_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 $(@D)/src/miredo $(TARGET_DIR)/usr/sbin/miredo
	mkdir -p $(TARGET_DIR)/usr/libexec/miredo/
	$(INSTALL) -m 755 $(@D)/src/miredo-privproc $(TARGET_DIR)/usr/libexec/miredo/miredo-privproc
	$(TARGET_STRIP) $(TARGET_DIR)/usr/sbin/miredo $(TARGET_DIR)/usr/libexec/miredo/miredo-privproc
endef

$(eval $(call AUTOTARGETS))
