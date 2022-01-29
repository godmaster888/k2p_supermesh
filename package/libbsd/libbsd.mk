################################################################################
#
# libbsd
#
################################################################################

LIBBSD_VERSION = 0.9.1
LIBBSD_SOURCE = libbsd-$(LIBBSD_VERSION).tar.xz
LIBBSD_SITE = $(DLINK_STORAGE)
LIBBSD_LICENSE = BSD-2-Clause, BSD-3-Clause, BSD-4-Clause, BSD-5-Clause, \
		MIT, ISC, Beerware
LIBBSD_LICENSE_FILES = COPYING
LIBBSD_INSTALL_STAGING = YES
LIBBSD_AUTORECONF = YES
HOST_LIBBSD_AUTORECONF = YES

LIBBSD_CONF_OPT += --disable-shared --enable-static --prefix=/usr

# Убираем все CFLAGS, т.к. libbsd не зависит от внешних хедеров
LIBBSD_CONF_OPT += CFLAGS=""

ifeq ($(BR2_PACKAGE_LIBBSD_STATIC),y)
define LIBBSD_REMOVE_STAGING_SO_LIB
	rm -fv $(STAGING_DIR)/usr/lib/libbsd.so*
endef
define LIBBSD_REMOVE_TARGET_SO_LIB
	rm -fv $(TARGET_DIR)/usr/lib/libbsd.so*
endef

LIBBSD_POST_INSTALL_STAGING_HOOKS += LIBBSD_REMOVE_STAGING_SO_LIB
LIBBSD_POST_INSTALL_TARGET_HOOKS  += LIBBSD_REMOVE_TARGET_SO_LIB
endif

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
