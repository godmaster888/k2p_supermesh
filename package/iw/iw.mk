#############################################################
#
# iw
#
#############################################################

IW_VERSION = $(call qstrip,$(BR2_IW_VERSION))
ifeq ($(IW_VERSION),)
IW_VERSION = 3.6
endif

IW_SITE = ${DLINK_GIT_STORAGE}/iw
IW_LICENSE = ISC
IW_LICENSE_FILES = COPYING
IW_DEPENDENCIES = libnl-tiny
IW_DIR = $(BUILD_DIR)/iw-$(IW_VERSION)

IW_CFLAGS:= \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-DCONFIG_LIBNL20 \
	-D_GNU_SOURCE

IW_MAKE_ENV += \
	CFLAGS="$(IW_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	NL1FOUND="" NL2FOUND=Y \
	NLLIBNAME="libnl-tiny" \
	LIBS="-lm -lnl-tiny" \
	V=99 \
	CC="${TARGET_CC}"

IW_INSTALL_ENV = \
	DESTDIR=$(TARGET_DIR) \
	NLLIBNAME="libnl-tiny"

define IW_CONFIGURE_CMDS
	echo "const char iw_version[] = \"$(IW_VERSION)\";" > $(IW_DIR)/version.c
	rm -f $(IW_DIR)/version.sh
	touch $(IW_DIR)/version.sh
	chmod +x $(IW_DIR)/version.sh
endef

define IW_BUILD_CMDS
	$(MAKE) $(IW_MAKE_ENV) -C $(IW_DIR)
endef

define IW_INSTALL_TARGET_CMDS
	$(IW_INSTALL_ENV) $(MAKE) install -C $(IW_DIR)
endef


$(eval $(call GENTARGETS))
