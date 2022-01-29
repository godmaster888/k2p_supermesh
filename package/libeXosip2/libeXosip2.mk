################################################################################
#
# libexosip2
#
################################################################################

ifeq ($(BR2_PACKAGE_LIBEXOSIP2_V5_0),y)
LIBEXOSIP2_VERSION = 5.0.0
else ifeq ($(BR2_PACKAGE_LIBEXOSIP2_V5_1),y)
LIBEXOSIP2_VERSION = 5.1.0
endif

LIBEXOSIP2_SITE = $(DLINK_GIT_STORAGE)/libexosip2
LIBEXOSIP2_INSTALL_STAGING = YES
LIBEXOSIP2_LICENSE = GPL-2.0-or-later WITH openvpn-openssl-exception
LIBEXOSIP2_LICENSE_FILES = COPYING LICENSE.OpenSSL

LIBEXOSIP2_DEPENDENCIES = host-pkg-config libosip2

LIBEXOSIP2_CONF_OPT += --prefix=/usr

ifeq ($(BR2_PACKAGE_LIBEXOSIP2_OPENSSL),y)
LIBEXOSIP2_CONF_OPT += --enable-openssl=yes
LIBEXOSIP2_DEPENDENCIES += openssl
else
LIBEXOSIP2_CONF_OPT += --enable-openssl=no
endif

ifeq ($(BR2_PACKAGE_LIBEXOSIP2_TOOLS),y)
LIBEXOSIP2_CONF_OPT += --enable-tools=yes
else
LIBEXOSIP2_CONF_OPT += --enable-tools=no
endif

ifeq ($(BR2_PACKAGE_LIBEXOSIP2_SRV_DNS),y)
LIBEXOSIP2_CONF_OPT += --enable-tools=yes
else
LIBEXOSIP2_CONF_OPT += --enable-tools=no
endif

ifeq ($(BR2_PACKAGE_LIBEXOSIP2_MULTITHREAD),y)
LIBEXOSIP2_CONF_OPT += --enable-mt=yes
else
LIBEXOSIP2_CONF_OPT += --enable-mt=no
endif

ifeq ($(BR2_PACKAGE_LIBEXOSIP2_MINIMIZE),y)
LIBEXOSIP2_CONF_OPT += --enable-minisize=yes
else
LIBEXOSIP2_CONF_OPT += --enable-minisize=no
endif

$(eval $(call AUTOTARGETS))
