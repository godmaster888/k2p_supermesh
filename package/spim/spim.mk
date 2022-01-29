#############################################################
#
# spim
#
#############################################################

SPIM_VERSION = $(call qstrip,$(BR2_PACKAGE_SPIM_BRANCH))
ifeq ($(SPIM_VERSION),)
SPIM_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

SPIM_SITE := $(DLINK_GIT_STORAGE)/spim
SPIM_LICENSE = BSD-4-Clause
SPIM_LICENSE_FILES = README
#SPIM_MAKE_OPTS += DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC)
SPIM_MAKE_OPTS += CC=$(TARGET_CC)

define SPIM_BUILD_CMDS
	$(SPIM_MAKE_OPTS) $(MAKE) -C $(@D)/spim
endef

define SPIM_INSTALL_TARGET_CMDS
	install -d $(TARGET_DIR)/usr/bin
	$(SPIM_MAKE_OPTS) DESTDIR=$(TARGET_DIR) $(MAKE) -C $(@D)/spim install
endef

$(eval $(call GENTARGETS))
