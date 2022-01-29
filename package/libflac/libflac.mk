#############################################################
#
# libflac
#
#############################################################
LIBFLAC_VERSION = master
LIBFLAC_SITE = $(DLINK_GIT_STORAGE)/libflac
LIBFLAC_LICENSE = LGPL-2.1-or-later
LIBFLAC_DEPENDENCIES = libogg
LIBFLAC_LICENSE_FILES = COPYING.LGPL
LIBFLAC_INSTALL_STAGING = YES
LIBFLAC_INSTALL_TARGET = YES
LIBFLAC_AUTORECONF = YES

LIBFLAC_CONF_ENV = CFLAGS="-I$(STAGING_DIR)/include"

$(eval $(call AUTOTARGETS))
