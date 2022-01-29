#############################################################
#
# wpa_supplicant
#
#############################################################
WPA_SUPPLICANT_VERSION =$(call qstrip,$(BR2_WPA_SUPPLICANT_VERSION))
ifeq (${WPA_SUPPLICANT_VERSION},)
WPA_SUPPLICANT_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

WPA_SUPPLICANT_SITE = $(DLINK_GIT_STORAGE)/wpa_supplicant
WPA_SUPPLICANT_LICENSE = GPL_2.0-only
WPA_SUPPLICANT_LICENSE_FILES = COPYING
WPA_SUPPLICANT_DEPENDENCIES = host-pkg-config openssl

WPA_SUPPLICANT_MAKE_OPTS = \
	TARGET_CFLAGS="$(TARGET_CFLAGS) -I${STAGING_DIR}/usr/include" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS) -L${TARGET_DIR}/usr/lib" \
	CC="$(TARGET_CC)" \
	LD="$(TARGET_LD)" \
	STRIP="$(TARGET_STRIP)" \
	INSTALL_DIR="$(TARGET_DIR)"

define WPA_SUPPLICANT_BUILD_CMDS
	$(MAKE) $(WPA_SUPPLICANT_MAKE_OPTS) -C $(@D) all
endef

define WPA_SUPPLICANT_INSTALL_TARGET_CMDS
	install $(@D)/wpa_supplicant ${TARGET_DIR}/sbin
	install $(@D)/wpa_passphrase ${TARGET_DIR}/sbin
	install $(@D)/wpa_cli ${TARGET_DIR}/sbin
	${TARGET_STRIP} ${TARGET_DIR}/sbin/wpa_supplicant
	${TARGET_STRIP} ${TARGET_DIR}/sbin/wpa_passphrase
	${TARGET_STRIP} ${TARGET_DIR}/sbin/wpa_cli
endef

$(eval $(call GENTARGETS))
