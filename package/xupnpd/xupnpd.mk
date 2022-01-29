#############################################################
#
# xupnpd
#
#############################################################
XUPNPD_VERSION = master
XUPNPD_SITE = $(DLINK_GIT_STORAGE)/xupnpd
XUPNPD_LICENSE = GPL-2.0-or-later
XUPNPD_LICENSE_FILES =
XUPNPD_AUTORECONF = NO
XUPNPD_INSTALL_STAGING = YES
XUPNPD_INSTALL_TARGET = YES
XUPNPD_CFLAGS = $(TARGET_CFLAGS) $(TARGET_LDFLAGS)

ifeq ($(BR2_DSYSINIT),y)
XUPNPD_DEPENDENCIES = deuteron_framework
XUPNPD_CFLAGS += -ld_service_notify -Wl,-rpath-link,$(STAGING_DIR)/usr/lib
endif

XUPNPD_MAKE_OPTS = \
        CC="$(TARGET_CC)" \
        LD="$(TARGET_LD)" \
        CFLAGS="$(XUPNPD_CFLAGS)"

define XUPNPD_BUILD_CMDS
        $(MAKE) $(XUPNPD_MAKE_OPTS) -C $(@D) all
endef

define XUPNPD_CLEAN_CMDS
        $(MAKE) -C $(@D) clean
endef

$(eval $(call GENTARGETS))
