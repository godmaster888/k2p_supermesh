#############################################################
#
# libev
#
#############################################################
LIBEV_VERSION = master
LIBEV_SITE = $(DLINK_GIT_STORAGE)/libev
LIBEV_INSTALL_STAGING = YES
LIBEV_LICENSE = GPL-2.0-or-later
LIBEV_LICENSE_FILES = LICENSE
LIBEV_AUTORECONF = YES
LIBEV_SUPPORTS_IN_SOURCE_BUILD = NO

LIBEV_CONF_OPT += --disable-static
LIBEV_CONF_OPT += --prefix=/usr


$(eval $(call AUTOTARGETS))
