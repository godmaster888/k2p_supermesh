#############################################################
#
# libid3tag
#
#############################################################
LIBID3TAG_VERSION = master
LIBID3TAG_SITE = $(DLINK_GIT_STORAGE)/libid3tag
LIBID3TAG_LICENSE = GPL-2.0-or-later
LIBID3TAG_LICENSE_FILES = COPYING COPYRIGHT
LIBID3TAG_INSTALL_STAGING = YES
LIBID3TAG_INSTALL_TARGET = YES

LIBID3TAG_DEPENDENCIES = zlib
LIBID3TAG_CONF_ENV = CFLAGS="-I$(STAGING_DIR)/include" CPPFLAGS="-I$(STAGING_DIR)/usr/include"

$(eval $(call AUTOTARGETS))
