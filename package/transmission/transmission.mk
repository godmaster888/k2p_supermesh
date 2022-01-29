################################################################################
#
# transmission
#
################################################################################

TRANSMISSION_VERSION = 2.94
TRANSMISSION_SITE = $(DLINK_STORAGE)
TRANSMISSION_SOURCE = transmission-$(TRANSMISSION_VERSION).tar.xz
TRANSMISSION_DEPENDENCIES = \
	host-pkg-config \
	libcurl \
	libevent \
	openssl \
	zlib
TRANSMISSION_AUTORECONF = YES
TRANSMISSION_LICENSE = GPL-2.0 or GPL-3.0 with OpenSSL exception
TRANSMISSION_LICENSE_FILES = COPYING

TRANSMISSION_CONF_OPT = \
	--without-inotify \
	--enable-lightweight \
	--disable-external-natpmp \
	--without-gtk \
	--disable-cli \
	--enable-daemon \
	--without-systemd \
	--prefix="/usr" \
	--disable-static \
	--disable-dependency-tracking \
	--disable-inotify \
	--enable-utp \
	--enable-largefile

ifeq ($(BR2_DSYSINIT), y)
TRANSMISSION_DEPENDENCIES += deuteron_framework
TRANSMISSION_CONF_ENV += LIBS="-ld_service_notify -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib"
TRANSMISSION_CONF_ENV += CFLAGS="$(TARGET_CFLAGS) -DDLINK_CACHE_SIZE=$(BR2_TRANSMISSION_CACHE_SIZE)"
endif


$(eval $(call AUTOTARGETS))
