#############################################################
#
# mini_upnp_rlt
#
#############################################################
MINI_UPNP_RLT_VERSION =$(call qstrip,$(BR2_MINI_UPNP_RLT_VERSION))
ifeq (${MINI_UPNP_RLT_VERSION},)
MINI_UPNP_RLT_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

MINI_UPNP_RLT_SITE = $(DLINK_GIT_STORAGE)/mini_upnp_rlt
MINI_UPNP_RLT_LICENSE = BSD-3-Clause
MINI_UPNP_RLT_LICENSE_FILES = minixml.h

MINI_UPNP_RLT_MAKE_OPTS = \
	CC=$(TARGET_CC) \
	LD=$(TARGET_LD) \
	STRIP=$(TARGET_STRIP) \
	INSTALL_DIR=$(TARGET_DIR)

WSCD_RLT_MAKE_OPTS += MINI_UPNP_RLT_VERSION=$(MINI_UPNP_RLT_VERSION)

define MINI_UPNP_RLT_BUILD_CMDS
	$(MAKE) $(MINI_UPNP_RLT_MAKE_OPTS) -C $(@D) all
endef

$(eval $(call GENTARGETS))

