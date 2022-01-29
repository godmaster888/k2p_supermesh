#############################################################
#
# vrrpd
#
#############################################################
VRRPD_VERSION = 6f254eb752e7539bb5bd25385683a85d6e1e0f3c
VRRPD_SITE = $(DLINK_STORAGE)
VRRPD_SOURCE = vrrpd-$(VRRPD_VERSION).tar.gz
VRRPD_LICENSE = GPL-2
VRRPD_LICENSE_FILES = COPYING
VRRPD_INSTALL_TARGET = YES

ifeq ($(BR2_DSYSINIT),y)
VRRPD_DEPENDENCIES += deuteron_framework jansson
VRRPD_LDFLAGS += -ld_service_notify -ljansson -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib
endif

VRRPD_MAKE_OPTS = \
        CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)" \
        LD="$(TARGET_LD)" \
        LDFLAGS=" -L$(STAGING_DIR)/usr/lib $(VRRPD_LDFLAGS)"

define VRRPD_BUILD_CMDS
        CFLAGS="$(TARGET_CFLAGS)" $(MAKE) $(VRRPD_MAKE_OPTS) -C $(@D)
endef

define  VRRPD_INSTALL_TARGET_CMDS
	$(INSTALL) -pm0755 $(@D)/vrrpd $(TARGET_DIR)/usr/bin/

endef
define VRRPD_CLEAN_CMDS
        $(MAKE) -C $(@D) clean
       rm -f $(TARGET_DIR)/usr/bin/vrrpd
endef

$(eval $(call GENTARGETS))
