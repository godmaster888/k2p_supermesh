#############################################################
#
# fakeroot
#
#############################################################
FAKEROOT_VERSION:=1.9.5
FAKEROOT_SITE:=${DLINK_GIT_STORAGE}/fakeroot
FAKEROOT_LICENSE = GPL-2.0-or-later
FAKEROOT_LICENSE_FILES = COPYING
FAKEROOT_LIBTOOL_PATCH=NO

define FAKEROOT_PATCH_FAKEROOT_IN
	# If using busybox getopt, make it be quiet.
	$(SED) "s,getopt --version,getopt --version 2>/dev/null," \
		$(@D)/scripts/fakeroot.in
endef

FAKEROOT_POST_PATCH_HOOKS += FAKEROOT_PATCH_FAKEROOT_IN

define FAKEROOT_RENAME_TARGET_BINARIES
	-mv $(TARGET_DIR)/usr/bin/$(ARCH)-*-faked \
		$(TARGET_DIR)/usr/bin/faked
	-mv $(TARGET_DIR)/usr/bin/$(ARCH)-*-fakeroot \
		$(TARGET_DIR)/usr/bin/fakeroot
endef

FAKEROOT_POST_INSTALL_TARGET_HOOKS += FAKEROOT_RENAME_TARGET_BINARIES

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
