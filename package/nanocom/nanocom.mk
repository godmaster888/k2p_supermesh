#############################################################
#
# nanocom
#
#############################################################
NANOCOM_VERSION = master
NANOCOM_SITE = $(DLINK_GIT_STORAGE)/nanocom
NANOCOM_LICENSE = GPL-2.0-or-later
NANOCOM_LICENSE_FILES = nanocom.h
NANOCOM_AUTORECONF = NO
NANOCOM_INSTALL_STAGING = NO
NANOCOM_INSTALL_TARGET = YES

define NANOCOM_CONFIGURE_CMDS
	echo "No need to configure"
endef

define NANOCOM_BUILD_CMDS
	$(MAKE) CFLAGS="$(TARGET_CFLAGS)" CC=$(TARGET_CC) -C $(@D)
endef

define NANOCOM_INSTALL_TARGET_CMDS
	install -m 755 $(@D)/nanocom $(TARGET_DIR)/usr/bin
endef

define NANOCOM_CLEAN_CMDS
	rm $(TARGET_DIR)/usr/bin/nanocom
endef


$(eval $(call AUTOTARGETS,package,nanocom))
