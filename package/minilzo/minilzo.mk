#############################################################
#
# minilzo
#
#############################################################
MINILZO_VERSION:=2.06
MINILZO_SITE:=${DLINK_GIT_STORAGE}/minilzo
MINILZO_LICENSE = GPL-2.0-or-later
MINILZO_LICENSE_FLIES =

define MINILZO_BUILD_CMDS
	$(MAKE) -C $(@D) CC="$(TARGET_CC)"
endef

define MINILZO_INSTALL_TARGET_CMDS
	install $(@D)/libminilzo.so $(TARGET_DIR)/usr/lib/
	find $(@D) -name "*.h" | xargs -i cp {} ${STAGING_DIR}/usr/include/
endef

define MINILZO_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(call GENTARGETS))
