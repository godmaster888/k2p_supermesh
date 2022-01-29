#############################################################
#
# genext2fs
#
#############################################################

GENEXT2FS_VERSION = 1.4.1
GENEXT2FS_SOURCE = genext2fs-$(GENEXT2FS_VERSION).tar.gz
GENEXT2FS_SITE = $(DLINK_STORAGE)
GENEXT2FS_LICENSE = GPL-2.0-only
GENEXT2FS_LICENSE_FILES = COPYING

$(eval $(call AUTOTARGETS))
$(eval $(call AUTOTARGETS,host))
