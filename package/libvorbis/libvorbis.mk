#############################################################
#
# libvorbis
#
#############################################################
LIBVORBIS_VERSION = master
LIBVORBIS_SITE = $(DLINK_GIT_STORAGE)/libvorbis
LIBVORBIS_LICENSE = BSD-3-Clause
LIBVORBIS_DEPENDENCIES = libogg
LIBVORBIS_LICENSE_FILES = COPYING
LIBVORBIS_INSTALL_STAGING = YES
LIBVORBIS_INSTALL_TARGET = YES
LIBVORBIS_AUTORECONF = YES

LIBVORBIS_CONF_ENV = CFLAGS="-I$(STAGING_DIR)/include"

$(eval $(call AUTOTARGETS))
