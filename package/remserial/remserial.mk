################################################################################
#
# remserial
#
################################################################################
REMSERIAL_VERSION = 1.4
REMSERIAL_SOURCE = remserial-$(REMSERIAL_VERSION).tar.gz
# REMSERIAL_SITE = http://lpccomp.bc.ca/remserial
REMSERIAL_SITE = $(DLINK_STORAGE)
REMSERIAL_LICENSE = GPL-2.0-or-later
REMSERIAL_LICENSE_FILES =


define REMSERIAL_BUILD_CMDS
	$(MAKE) LDFLAGS="$(TARGET_LDFLAGS) -ld_service_notify" CFLAGS="$(TARGET_CFLAGS)" CC="$(TARGET_CC) --sysroot=$(STAGING_DIR)" LD="$(TARGET_LD)" -C $(@D) all
endef

define REMSERIAL_INSTALL_TARGET_CMDS
	cp $(@D)/remserial $(TARGET_DIR)/usr/bin
endef

$(eval $(call GENTARGETS))
