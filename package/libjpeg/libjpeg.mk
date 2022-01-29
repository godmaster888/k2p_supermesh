#############################################################
#
# libjpeg
#
#############################################################
LIBJPEG_VERSION = master
LIBJPEG_SITE = $(DLINK_GIT_STORAGE)/libjpeg
LIBJPEG_LICENSE = IJG
LIBJPEG_LICENSE_FILES = README
LIBJPEG_INSTALL_STAGING = YES
LIBJPEG_INSTALL_TARGET = YES

LIBJPEG_CONF_ENV = CFLAGS="-I$(STAGING_DIR)/include" DESTDIR="$(STAGING_DIR)"
LIBJPEG_CONF_OPT = --enable-shared

define LIBJPEG_INSTALL_STAGING_CMDS
	cp -P $(@D)/.libs/libjpeg.so* $(STAGING_DIR)/usr/lib/
	cp $(@D)/jpeglib.h $(STAGING_DIR)/include/
	cp $(@D)/jconfig.h $(STAGING_DIR)/include/
	cp $(@D)/jerror.h $(STAGING_DIR)/include/
	cp $(@D)/jmorecfg.h $(STAGING_DIR)/include/
endef

define LIBJPEG_INSTALL_TARGET_CMDS
	cp -P $(@D)/.libs/libjpeg.so* $(TARGET_DIR)/usr/lib/
endef

$(eval $(call AUTOTARGETS))
