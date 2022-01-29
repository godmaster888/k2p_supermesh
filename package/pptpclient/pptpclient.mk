#############################################################
#
# pptpclient
#
#############################################################

PPTPCLIENT_SITE = $(DLINK_GIT_STORAGE)/pptpclient
PPTPCLIENT_LICENSE = GPL-2.0-or-later
PPTPCLIENT_LICENSE_FILES = README COPYING
PPTPCLIENT_DEPENDENCIES = pppd


PPTPCLIENT_VERSION = $(call qstrip,$(BR2_PACKAGE_PPTPCLIENT_VERSION))
ifeq ($(call qstrip,$(BR2_PACKAGE_PPTPCLIENT_VERSION)), )
PPTPCLIENT_VERSION = $(call qstrip,$(BR2_DEFAULT_BRANCH))
endif

PPTPCLIENT_MAKE_OPTS = \
				  CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)" \
				  LD="$(TARGET_LD)" \
				  STRIP="$(TARGET_STRIP)" \
				  EXTRA_CFLAGS="$(EXTRA_CFLAGS) -DDLINK_SOFTWARE" \
				  EXTRA_LDFLAGS="$(EXTRA_LDFLAGS)"


define PPTPCLIENT_BUILD_CMDS
	make -C $(@D) $(PPTPCLIENT_MAKE_OPTS)
endef

define PPTPCLIENT_INSTALL_TARGET_CMDS
	install -pm0755 ${@D}/pptp $(TARGET_DIR)/bin/
endef

define PPTPCLIENT_CLEAN_CMDS
	make CC=$(TARGET_CC) -C $(@D) clean
endef

$(eval $(call GENTARGETS))
