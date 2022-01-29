#############################################################
#
# patchelf
#
#############################################################

PATCHELF_VERSION = master
PATCHELF_SITE = $(DLINK_GIT_STORAGE)/patchelf
PATCHELF_LICENSE = GPL-3.0-or-later
PATCHELF_LICENSE_FILES = README COPYING

define host_patchelf_preconfigure_bootstrap
	cd $(@D); PATH=$(PATH):$(HOST_DIR)/usr/bin ./bootstrap.sh
endef

HOST_PATCHELF_PRE_CONFIGURE_HOOKS += host_patchelf_preconfigure_bootstrap
HOST_PATCHELF_DEPENDENCIES += host-autoconf

$(eval $(call AUTOTARGETS,host))
