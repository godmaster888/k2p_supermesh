#############################################################
#
# ndisc6
#
#############################################################

NDISC6_VERSION = master
NDISC6_SITE = $(DLINK_GIT_STORAGE)/ndisc6
NDISC6_LICENSE = GPL-2.0-only
NDISC6_LICENSE_FILES = COPYING

ifeq ($(BR2_NDISC6_NAME2ADDR),y)
define NDISC6_INSTALL_NAME2ADDR
	$(INSTALL) -D -m 0755 $(@D)/src/name2addr $(TARGET_DIR)/usr/bin
endef
endif

ifeq ($(BR2_NDISC6_NDISC6),y)
define NDISC6_INSTALL_NDISC6
	$(INSTALL) -D -m 0755 $(@D)/src/ndisc6 $(TARGET_DIR)/usr/bin
endef
endif

ifeq ($(BR2_NDISC6_RDISC6),y)
define NDISC6_INSTALL_RDISC6
	$(INSTALL) -D -m 0755 $(@D)/src/rdisc6 $(TARGET_DIR)/usr/bin
endef
endif

ifeq ($(BR2_NDISC6_TCPSPRAY),y)
define NDISC6_INSTALL_TCPSPRAY
	$(INSTALL) -D -m 0755 $(@D)/src/tcpspray $(TARGET_DIR)/usr/bin
endef
endif

ifeq ($(BR2_NDISC6_TCPTRACEROUTE6),y)
define NDISC6_INSTALL_TCPTRACEROUTE6
	$(INSTALL) -D -m 0755 $(@D)/src/tcptraceroute6 $(TARGET_DIR)/usr/bin
endef
endif

ifeq ($(BR2_NDISC6_TRACEROUTE6),y)
define NDISC6_INSTALL_TRACEROUTE6
	$(INSTALL) -D -m 0755 $(@D)/src/rltraceroute6 $(TARGET_DIR)/usr/bin
endef
endif

define NDISC6_INSTALL_TARGET_CMDS
	$(NDISC6_INSTALL_NAME2ADDR)
	$(NDISC6_INSTALL_NDISC6)
	$(NDISC6_INSTALL_RDISC6)
	$(NDISC6_INSTALL_TCPSPRAY)
	$(NDISC6_INSTALL_TCPTRACEROUTE6)
	$(NDISC6_INSTALL_TRACEROUTE6)
endef

define NDISC6_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define NDISC6_UNINSTALL_STAGING_CMDS
	:
endef

define NDISC6_UNINSTALL_TARGET_CMDS
	:
endef

$(eval $(call AUTOTARGETS))
