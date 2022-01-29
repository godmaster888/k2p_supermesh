################################################################################
#
# libconfuse
#
################################################################################

LIBCONFUSE_VERSION = 3.2
LIBCONFUSE_SOURCE = confuse-$(LIBCONFUSE_VERSION).tar.xz
LIBCONFUSE_SITE = $(DLINK_STORAGE)
LIBCONFUSE_INSTALL_STAGING = YES
LIBCONFUSE_CONF_OPT = --disable-rpath
LIBCONFUSE_LICENSE = ISC
LIBCONFUSE_LICENSE_FILES = LICENSE
#LIBCONFUSE_DEPENDENCIES = $(TARGET_NLS_DEPENDENCIES)

$(eval $(call AUTOTARGETS))
