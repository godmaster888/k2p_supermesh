#############################################################
#
# libcyassl
#
#############################################################

LIBCYASSL_VERSION = master
LIBCYASSL_SITE = $(DLINK_GIT_STORAGE)/libcyassl
LIBCYASSL_LICENSE = GPL-2.0-or-later
LIBCYASSL_LICENSE_FILES = COPYING

LIBCYASSL_CONF_OPT = --enable-singlethreaded --disable-examples --enable-webserver

ifeq ($(BR2_ENDIAN),"BIG")
	LIBCYASSL_MAKE_OPT = CFLAGS='$(TARGET_CFLAGS) -DBIG_ENDIAN_ORDER'
endif

ifeq (${BR2_INET_IPV6},y)
	LIBCYASSL_CONF_OPT += --enable-ipv6
endif

ifeq (${BR2_CUSTOMIZATION_ORANGE2},y)
	LIBCYASSL_CONF_OPT += --disable-oldtls --disable-arc4
endif

LIBCYASSL_INSTALL_STAGING = YES
LIBCYASSL_INSTALL_TARGET = YES

$(eval $(call AUTOTARGETS))
