#############################################################
#
# ntfs-3g
#
#############################################################
NTFS_3G_SITE = $(DLINK_GIT_STORAGE)/ntfs-3g
NTFS_3G_VERSION = master
NTFS_3G_LICENSE = GPL-2.0-or-later AND LGPL-2.0-or-later
NTFS_3G_LICENSE_FILES = COPYING COPYING.LIB
NTFS_3G_CONF_OPT = --enable-shared=no --disable-mount-helper \
	--with-fuse=internal --sysconfdir=/tmp \
	--disable-ldconfig --disable-library

NTFS_3G_CFLAGS := "$(TARGET_CFLAGS) -O2"

define NTFS_3G_BUILD_CMDS
	$(MAKE) CFLAGS=$(NTFS_3G_CFLAGS) -C $(@D) all
endef

define NTFS_3G_INSTALL_TARGET_CMDS
	install -pm0755 $(@D)/src/ntfs-3g $(TARGET_DIR)/usr/sbin/ntfs-3g
	${TARGET_STRIP} $(TARGET_DIR)/usr/sbin/ntfs-3g
	ln -sf ../usr/sbin/ntfs-3g $(TARGET_DIR)/sbin/mount.ntfs-3g
endef

define NTFS_3G_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(call AUTOTARGETS))
