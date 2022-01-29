#############################################################
#
# udhcp
#
#############################################################

UDHCP_SITE=$(DLINK_GIT_STORAGE)/udhcp
UDHCP_VERSION = dsysinit
UDHCP_LICENSE = GPL-2.0-or-later
UDHCP_LICENSE_FILES = COPYING

ifeq (${BEELINE_RUS},y)
  TARGET_CFLAGS += -DBROADCAST_ONLY
endif

ifeq ($(strip $(PROFILE)), DIR_300NRU_MLD)
  TARGET_CFLAGS += -DOPT_58_59
endif

UDHCP_MAKE_ENV = CFLAGS="$(TARGET_CFLAGS) $(UDHCP_CFLAGS)"
UDHCP_MAKE_OPTS = \
	CC="$(TARGET_CC)" \
	LD="$(TARGET_CC)" \
	LDFLAGS="-Wl,-rpath,$(STAGING_DIR)/lib -Wl,-rpath,$(STAGING_DIR)/usr/lib -Wl,--sysroot=$(STAGING_DIR) -L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib -ljansson -ld_service_notify"


HOST_UDHCP_MAKE_OPTS = \
	$(HOST_MAKE_ENV) \
	CFLAGS="$(HOST_CFLAGS) $(UDHCP_CFLAGS)" \
	LDFLAGS="$(HOST_LDFLAGS)" \
	CC="$(CC)" \
	LD="$(CC)"

ifeq ($(UDHCP_RESTART_MIN_MAX_DELAY),y)
	UDHCP_MAKE_OPTS += DISCOVER_MIN_MAX_DELAY=y
endif

define UDHCP_BUILD_CMDS
	$(UDHCP_MAKE_ENV) $(MAKE) $(UDHCP_MAKE_OPTS) -C $(@D) all
endef

define UDHCP_INSTALL_TARGET_CMDS
	$(INSTALL) -pm0755 $(@D)/udhcpc $(TARGET_DIR)/usr/bin/
endef

define UDHCP_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define HOST_UDHCP_BUILD_CMDS
	$(HOST_UDHCP_MAKE_OPTS) $(MAKE) -C $(@D) all
endef

define HOST_UDHCP_INSTALL_CMDS
	$(INSTALL) -pm0755 $(@D)/udhcpc $(HOST_DIR)/usr/bin/
endef

define HOST_UDHCP_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(call GENTARGETS))
$(eval $(call GENTARGETS,host))
