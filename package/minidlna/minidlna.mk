#############################################################
#
# minidlna
#
#############################################################

MINIDLNA_SITE = $(DLINK_GIT_STORAGE)/minidlna
MINIDLNA_LICENSE = GPL-2.0-only
MINIDLNA_LICENSE_FILES = COPYING
MINIDLNA_INSTALL_STAGING = NO
MINIDLNA_INSTALL_TARGET = YES
MINIDLNA_AUTORECONF = YES

MINIDLNA_DEPENDENCIES = deuteron_framework libexif libid3tag libjpeg zlib sqlite

ifeq ($(BR2_PACKAGE_MINIDLNA_VERSION_ADVANCED),y)
MINIDLNA_VERSION = advanced
else ifeq ($(BR2_PACKAGE_MINIDLNA_VERSION_1_2_1),y)
MINIDLNA_VERSION = 1.2.1
MINIDLNA_DEPENDENCIES += libflac libvorbis
endif

MINIDLNA_CONF_ENV = CFLAGS="-I$(STAGING_DIR)/include $(TARGET_CFLAGS) -D$(PROFILE)" LDFLAGS="$(TARGET_LDFLAGS) -ljansson -ld_service_notify -Wl,-rpath-link,$(STAGING_DIR)/lib -Wl,-rpath-link,$(STAGING_DIR)/usr/lib"
MINIDLNA_CONF_OPT = --program-transform-name=

$(eval $(call AUTOTARGETS))
