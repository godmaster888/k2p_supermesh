#############################################################
#
# vconfig
#
#############################################################
VLAN_VERSION = master
VLAN_SITE = $(DLINK_GIT_STORAGE)/vlan
VLAN_LICENSE = GPL-2.0-or-later
VLAN_LICENSE_FILES =

define VLAN_BUILD_CMDS
	$(MAKE) -C $(@D) OPTIMIZE= DEBUG= \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) " \
		LDFLAGS="$(TARGET_LDFLAGS)" vconfig
endef

define VLAN_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/vconfig $(TARGET_DIR)/usr/sbin/vconfig
endef

$(eval $(call GENTARGETS,package,vlan))

