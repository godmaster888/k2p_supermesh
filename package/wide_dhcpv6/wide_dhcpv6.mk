#############################################################
#
# dhcpv6
#
#############################################################
WIDE_DHCPV6_VERSION = master
WIDE_DHCPV6_SITE = $(DLINK_GIT_STORAGE)/dhcpv6
WIDE_DHCPV6_LICENSE = BSD-3-Clause
WIDE_DHCPV6_LICENSE_FILES = COPYRIGHT

WIDE_DHCPV6_CONF_OPT = \
	CC="$(TARGET_CC)" \
	LD="$(TARGET_LD)" \
	PREFIX="$(TARGET_DIR)" \
	STRIP=${TARGET_STRIP} \
	INSTALL_DIR=$(TARGET_DIR) \

define WIDE_DHCPV6_BUILD_CMDS
	$(MAKE1) ${WIDE_DHCPV6_CONF_OPT} -C $(@D) 
endef

define WIDE_DHCPV6_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/dhcp6c $(TARGET_DIR)/usr/bin
endef

define WIDE_DHCPV6_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(call GENTARGETS))
