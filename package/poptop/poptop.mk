################################################################################
#
# PoPtOp
#
################################################################################
POPTOP_SITE_METHOD = git
POPTOP_SITE = $(DLINK_GIT_STORAGE)/poptop
POPTOP_VERSION := $(call qstrip, $(BR2_PACKAGE_POPTOP_VERSION))
POPTOP_LICENSE = GPL-2.0-or-later AND BSD-4-Clause
POPTOP_LICENSE_FILES = debian/copyright

POPTOP_CFLAGS = $(TARGET_CFLAGS) -O2 -fno-builtin -Wall
POPTOP_LDFLAGS = $(TARGET_LDFLAGS)


ifeq ($(BR2_DSYSINIT),y)
	POPTOP_DEPENDENCIES += deuteron_framework
	POPTOP_LDFLAGS += -ld_service_notify -ljansson
endif

POPTOP_MAKE_OPTS = \
				   CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)"  \
				   LD="$(TARGET_LD)" \
				   STRIP="$(TARGET_STRIP)" \
				   LINUX26_DIR=$(LINUX26_DIR) \
				   CFLAGS="$(POPTOP_CFLAGS)" \
				   LDFLAGS="$(POPTOP_LDFLAGS)"

POPTOP_CONFIGURE_OPTS = \
						CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)"  \
						LD="$(TARGET_LD)" \
						STRIP="$(TARGET_STRIP)" \
						LINUX26_DIR=$(LINUX26_DIR) \
						--prefix=$(TARGET_DIR)/usr \
						--sbindir=/sbin \
						--host=$(GNU_TARGET_NAME) \
						--disable-bcrelay


define POPTOP_CONFIGURE_CMDS
	cd $(@D) && ./configure $(POPTOP_CONFIGURE_OPTS)
endef

define POPTOP_BUILD_CMDS
	make -C $(@D) $(POPTOP_MAKE_OPTS)
endef

define POPTOP_INSTALL_TARGET_CMDS
	cp -f $(@D)/pptpd $(TARGET_DIR)/usr/sbin
	cp -f $(@D)/pptpctrl $(TARGET_DIR)/usr/sbin
endef

define POPTOP_CLEAN_CMDS
	make -C $(@D) clean
endef

define POPTOP_UNINSTALL_STAGING_CMDS
	rm -f $(STAGING_DIR)/usr/sbin/pptpd
	rm -f $(STAGING_DIR)/usr/sbin/pptpctrl
endef

define POPTOP_UNINSTALL_TARGET_CMDS
	rm -f $(TARGET_DIR)/usr/sbin/pptpd
	rm -f $(TARGET_DIR)/usr/sbin/pptpctrl
endef

$(eval $(call AUTOTARGETS))
