#############################################################
#
# libnl
#
#############################################################

LIBNL_VERSION = 3.0
LIBNL_SITE = ${DLINK_GIT_STORAGE}/libnl
LIBNL_LICENSE = LGPL-2.1-only
LIBNL_LICENSE_FILES = COPYING
LIBNL_INSTALL_STAGING = YES
LIBNL_DEPENDENCIES = host-bison
LIBNL_MAKE = $(MAKE1)

define LIBNL_UNINSTALL_TARGET_CMDS
	rm -r $(TARGET_DIR)/usr/lib/libnl.* $(TARGET_DIR)/usr/lib/libnl-*.*
	rm -rf $(TARGET_DIR)/usr/lib/libnl
endef

$(eval $(call AUTOTARGETS))
