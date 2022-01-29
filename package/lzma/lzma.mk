#############################################################
#
# lzma
#
#############################################################
LZMA_VERSION = 4.57
HOST_LZMA_SITE = ${DLINK_GIT_STORAGE}/lzma
LZMA_LICENSE = LGPL-2.1-only WITH LZMA-exception OR CPL-1.0 WITH LZMA-exception
LZMA_LICENSE_FILES = lzma.txt LGPL.txt

define HOST_LZMA_BUILD_CMDS
	Sqlzma=$(@D)/sqlzma $(MAKE) -C $(@D)/CPP/7zip/Compress/LZMA_Alone -f sqlzma.mk
	Sqlzma=$(@D)/sqlzma $(MAKE) -C $(@D)/C/Compress/Lzma -f sqlzma.mk
endef
define HOST_LZMA_INSTALL_CMDS
	mkdir -p $(HOST_DIR)/usr/bin
	cp $(@D)/CPP/7zip/Compress/LZMA_Alone/lzma $(HOST_DIR)/usr/bin/lzma
endef
$(eval $(call GENTARGETS,host))

LZMA=$(HOST_DIR)/usr/bin/lzma
