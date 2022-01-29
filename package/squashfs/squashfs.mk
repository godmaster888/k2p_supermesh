#############################################################
#
# squashfs4
#
#############################################################

HOST_SQUASHFS_SITE=${DLINK_GIT_STORAGE}/squashfs
HOST_SQUASHFS_LICENSE = GPL-2.0-or-later
HOST_SQUASHFS_LICENSE_FILES = README COPYING

SQUASHFS_VERSION=4.2

# no libattr in BR
HOST_SQUASHFS_DEPENDENCIES = host-zlib host-xz

# no libattr/xz in BR
HOST_SQUASHFS_MAKE_ARGS = \
	XATTR_SUPPORT=0 \
	XZ_SUPPORT=1    \
	GZIP_SUPPORT=1  \
	LZO_SUPPORT=0	\
	LZMA_XZ_SUPPORT=1

define HOST_SQUASHFS_BUILD_CMDS
 $(HOST_MAKE_ENV) $(MAKE) \
   CC="$(HOSTCC)" \
   EXTRA_CFLAGS="$(HOST_CFLAGS)"   \
   EXTRA_LDFLAGS="$(HOST_LDFLAGS)" \
   $(HOST_SQUASHFS_MAKE_ARGS) \
   -C $(@D)/squashfs-tools/
endef

define HOST_SQUASHFS_INSTALL_CMDS
 $(HOST_MAKE_ENV) $(MAKE) $(HOST_SQUASHFS_MAKE_ARGS) \
   -C $(@D)/squashfs-tools/ INSTALL_DIR=$(HOST_DIR)/usr/bin install
endef

$(eval $(call GENTARGETS,host))
