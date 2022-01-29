#############################################################
#
# libexif
#
#############################################################
LIBEXIF_VERSION = master
LIBEXIF_SITE = $(DLINK_GIT_STORAGE)/libexif
LIBEXIF_LICENSE = LGPL-2.1-or-later
LIBEXIF_LICENSE_FILES = COPYING
LIBEXIF_INSTALL_STAGING = YES
LIBEXIF_INSTALL_TARGET = YES
LIBEXIF_AUTORECONF = YES

LIBEXIF_CONF_ENV = CFLAGS="-I$(STAGING_DIR)/include"

$(eval $(call AUTOTARGETS))
