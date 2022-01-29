#############################################################
#
# lanauth
#
#############################################################
LANAUTH_VERSION = master
LANAUTH_SITE = $(DLINK_GIT_STORAGE)/lanauth
LANAUTH_LICENSE = Beerware
LANAUTH_LICENSE_FILES =
LANAUTH_DEPENDENCIES += libtomcrypt jansson deuteron_framework
LANAUTH_AUTORECONF = NO
LANAUTH_INSTALL_STAGING = YES
LANAUTH_INSTALL_TARGET = YES

LANAUTH_CONF_OPT = \
	CCFLAGS="$(TARGET_CFLAGS) -I$(@D)" \
	LDFLAGS="$(TARGET_LDLAGS) -L$(STAGING_DIR)/usr/lib -ltomcrypt -ljansson -ld_service_notify -Wl,-rpath-link,$(STAGING_DIR)/usr/lib" \
	CC="$(TARGET_CROSS)gcc" \
	LD="$(TARGET_CC)" \
	INSTALLDIR="$(TARGET_DIR)" \
	EXTRA_LDFLAGS="$(TARGET_LDFLAGS)" \
	STRIP="$(TARGET_STRIP)"


define LANAUTH_BUILD_CMDS
	$(MAKE) $(LANAUTH_CONF_OPT) -C $(@D) install
endef

define LANAUTH_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(call GENTARGETS))
