#############################################################
#
# squashfs3
#
#############################################################

SQUASHFS3_VERSION=3.4
SQUASHFS3_SITE=${DLINK_GIT_STORAGE}/squashfs
SQUASHFS3_LICENSE = GPL-2.0-or-later
SQUASHFS3_LICENSE_FILES =

SQUASHFS3_DEPENDENCIES = zlib
HOST_SQUASHFS3_DEPENDENCIES = host-lzma

define SQUASHFS3_BUILD_CMDS
 $(TARGET_MAKE_ENV) $(MAKE)    \
   CC="$(TARGET_CC)"           \
   CFLAGS="$(TARGET_CFLAGS)"   \
   LDFLAGS="$(TARGET_LDFLAGS)" \
   -C $(@D)/squashfs-tools/
endef

define SQUASHFS3_INSTALL_TARGET_CMDS
 $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) INSTALL_DIR=$(TARGET_DIR)/usr/bin install
endef

#define HOST_SQUASHFS3_BUILD_CMDS
# $(HOST_MAKE_ENV) $(MAKE) CFLAGS="$(HOST_CFLAGS)" LDFLAGS="$(HOST_LDFLAGS)" -C $(@D)
#endef
define HOST_SQUASHFS3_BUILD_CMDS
 Sqlzma=$(BUILD_DIR)/host-lzma-$(LZMA_VERSION)/sqlzma $(HOST_MAKE_ENV) $(MAKE) \
   LzmaAlone=$(BUILD_DIR)/host-lzma-$(LZMA_VERSION)/CPP/7zip/Compress/LZMA_Alone \
   LzmaC=$(BUILD_DIR)/host-lzma-$(LZMA_VERSION)/C/Compress/Lzma \
   CC="$(HOSTCC)" \
   EXTRA_CFLAGS="$(HOST_CFLAGS)"   \
   EXTRA_LDFLAGS="$(HOST_LDFLAGS)" \
   $(HOST_SQUASHFS_MAKE_ARGS) \
   -C $(@D) mksquashfs
endef
#define HOST_SQUASHFS3_INSTALL_CMDS
# $(HOST_MAKE_ENV) $(MAKE) -C $(@D) INSTALL_DIR=$(HOST_DIR)/usr/bin install
#endef

define HOST_SQUASHFS3_INSTALL_CMDS
  mkdir -p $(HOST_DIR)/usr/bin
  install -pm0755 $(@D)/mksquashfs $(HOST_DIR)/usr/bin/
endef

$(eval $(call GENTARGETS))
$(eval $(call GENTARGETS,host))

