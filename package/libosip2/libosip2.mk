################################################################################
#
# libosip2
#
################################################################################

ifeq ($(BR2_PACKAGE_LIBOSIP2_V5_0),y)
LIBOSIP2_VERSION = 5.0.0
else ifeq ($(BR2_PACKAGE_LIBOSIP2_V5_1),y)
LIBOSIP2_VERSION = 5.1.0
endif

LIBOSIP2_SITE = $(DLINK_GIT_STORAGE)/libosip2
LIBOSIP2_INSTALL_STAGING = YES
LIBOSIP2_LICENSE = LGPL-2.1-or-later
LIBOSIP2_LICENSE_FILES = COPYING

LIBOSIP2_CONF_OPT += --prefix=/usr

ifeq ($(BR2_PACKAGE_LIBOSIP2_MULTITHREAD),y)
LIBOSIP2_CONF_OPT += --enable-mt=yes
else
LIBOSIP2_CONF_OPT += --enable-mt=no
endif

ifeq ($(BR2_PACKAGE_LIBOSIP2_MINIMIZE),y)
LIBOSIP2_CONF_OPT += --enable-minisize=yes
else
LIBOSIP2_CONF_OPT += --enable-minisize=no
endif

$(eval $(call AUTOTARGETS))
