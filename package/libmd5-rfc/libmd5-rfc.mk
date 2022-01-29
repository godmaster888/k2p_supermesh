#############################################################
#
# libmd5-rfc
#
#############################################################
LIBMD5_RFC_VERSION = master
LIBMD5_RFC_SITE = $(DLINK_GIT_STORAGE)/libmd5-rfc
LIBMD5_RFC_LICENSE = Zlib
LIBMD5_RFC_LICENSE_FILES = md5-rfc1321.h md5.c
LIBMD5_RFC_AUTORECONF = NO
LIBMD5_RFC_INSTALL_STAGING = YES
LIBMD5_RFC_INSTALL_TARGET = YES
LIBMD5_RFC_MAKE_OPTS = CC="$(TARGET_CC)"

define LIBMD5_RFC_CONFIGURE_CMDS
	echo "No need to do it"
endef

define LIBMD5_RFC_BUILD_CMDS
	$(MAKE) $(TARGET_MAKE_ENV) $(LIBMD5_RFC_MAKE_OPTS) -C $(@D) all
endef

define HOST_LIBMD5_RFC_BUILD_CMDS
	$(HOST_MAKE_ENV) $(MAKE) -C $(@D) all
endef

define LIBMD5_RFC_CLEAN_CMDS
	$(MAKE) $(TARGET_MAKE_ENV) $(LIBMD5_RFC_MAKE_OPTS) -C $(@D) clean
endef

define LIBMD5_RFC_INSTALL_STAGING_CMDS
	cp $(@D)/md5-rfc1321.h $(STAGING_DIR)/usr/include/
	cp $(@D)/libmd5-rfc.so $(STAGING_DIR)/usr/lib/
endef

define HOST_LIBMD5_RFC_INSTALL_CMDS
	cp $(@D)/md5-rfc1321.h $(HOST_DIR)/usr/include/
	cp $(@D)/libmd5-rfc.so $(HOST_DIR)/usr/lib/
endef


define LIBMD5_RFC_INSTALL_TARGET_CMDS
	cp $(@D)/libmd5-rfc.so $(TARGET_DIR)/lib/
endef

$(eval $(call GENTARGETS))
$(eval $(call GENTARGETS,host))
